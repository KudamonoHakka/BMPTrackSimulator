import os
import matplotlib.pyplot as plt
from PIL import Image, ImageOps
import pennylane as qml
from pennylane import numpy as np
import tensorflow as tf
from tensorflow import keras
import random

dir_path = '../data/3232'
data_len = len(os.listdir(dir_path))
classical_weight_path = '../models/classical/classical3232_weights'
hybrid_weight_path = '../models/hybrid/hybrid3232_weights'

# General constants
n_epochs = 10   # Number of optimization epochs
n_layers = 1    # Number of random layers
n_train = int(data_len * 0.8)    # Size of the train dataset
n_test = data_len - n_train     # Size of the test dataset
input_shape = (32, 32, 1)

# Quantum circuit constants
num_qlayers = 1
num_qubits = 5+1
qinput_shape = (32)
qweights_shape = (num_qlayers, num_qubits-1, 3)

num_qlayers = 1
num_qubits = 5+1
qinput_shape = (32)

PREPROCESS = True           # If False, skip quantum processing and load data from SAVE_PATH
np.random.seed(0)           # Seed for NumPy random number generator
tf.random.set_seed(0)       # Seed for TensorFlow random number generator

#n_train, n_test

def process_img(filepath):
    img = np.array(ImageOps.grayscale(Image.open(filepath))) / 255

    return img

def binary_data():
    x_train, x_test, y_train, y_test = [], [], [], []
    extracts = []

    for file in os.listdir(dir_path):
        filename = os.fsdecode(file)
        y = float(filename.split(' ')[0])

        filepath = os.path.join(dir_path, filename)
        img = process_img(filepath)

        extracts.append((img, y))

    random.shuffle(extracts)

    for i in range(data_len):
        if i < n_train:
            x_train.append(extracts[i][0])
            y_train.append(extracts[i][1])
        else:
            x_test.append(extracts[i][0])
            y_test.append(extracts[i][1])

    x_train = np.array(x_train).reshape(n_train, *input_shape)
    x_test = np.array(x_test).reshape(n_test, *input_shape)

    return x_train, x_test, np.array(y_train), np.array(y_test)

def conv_model():
    model = keras.models.Sequential([
        keras.layers.Conv2D(filters=4,
                            kernel_size=5,
                            activation=keras.activations.relu,
                            input_shape=input_shape,
                            padding="same",
                            dtype=tf.float32,
                            name="conv1"),
        keras.layers.AveragePooling2D(3, padding="same", dtype=tf.float32, name="pool1"),
        keras.layers.Conv2D(filters=8,
                            kernel_size=5,
                            activation=keras.activations.relu,
                            padding="same",
                            dtype=tf.float32,
                            name="conv2"),
        keras.layers.AveragePooling2D(2, padding="same", dtype=tf.float32, name="pool2"),
        keras.layers.Conv2D(filters=16,
                            kernel_size=5,
                            activation=keras.activations.relu,
                            padding="same",
                            dtype=tf.float32,
                            name="conv3"),
        keras.layers.AveragePooling2D(2, padding="same", dtype=tf.float32, name="pool3"),
        keras.layers.Flatten(name="flatten", dtype=tf.float32)
    ])

    return model

def conv_model_min():
    model = keras.models.Sequential([
        keras.layers.Conv2D(filters=2,
                            kernel_size=5,
                            activation=keras.activations.relu,
                            input_shape=input_shape,
                            padding="same",
                            dtype=tf.float32,
                            name="conv1"),
        keras.layers.AveragePooling2D(4, padding="same", dtype=tf.float32, name="pool1"),
        keras.layers.Conv2D(filters=4,
                            kernel_size=5,
                            activation=keras.activations.relu,
                            padding="same",
                            dtype=tf.float32,
                            name="conv2"),
        keras.layers.AveragePooling2D(2, padding="same", dtype=tf.float32, name="pool2"),
        keras.layers.Conv2D(filters=8,
                            kernel_size=5,
                            activation=keras.activations.relu,
                            padding="same",
                            dtype=tf.float32,
                            name="conv3"),
        keras.layers.AveragePooling2D(2, padding="same", dtype=tf.float32, name="pool3"),
        keras.layers.Flatten(name="flatten", dtype=tf.float32)
    ])

    return model

# Classical
def c_model():
    model = keras.models.Sequential([
        conv_model(),
        keras.layers.Dense(1, activation="sigmoid", name="dense1"),
    ])

    model.compile(
        optimizer=keras.optimizers.legacy.SGD(),
        loss="mse",
        metrics=["mse", "mae", "acc"],
    )

    return model

# qml device/circuit
dev = qml.device("default.qubit", wires=num_qubits)

@qml.qnode(dev)
def circuit(inputs, weights):
    unbactched = tf.cast(tf.squeeze(inputs), dtype=tf.float32)

    qml.AmplitudeEmbedding(unbactched, wires=range(num_qubits)[1:], normalize=True)
    qml.templates.StronglyEntanglingLayers(weights, wires=range(num_qubits)[1:])

    qml.QFT(wires=range(num_qubits)[1:])
    qml.broadcast(unitary=qml.Hadamard, pattern="single", wires=range(num_qubits))
    qml.broadcast(unitary=qml.CNOT, pattern=[[0,1], [0, 2], [0, 3], [0, 4], [0, 5]], wires=range(num_qubits))
    qml.broadcast(unitary=qml.Hadamard, pattern="single", wires=range(num_qubits))

    return qml.expval(qml.PauliZ(0))


def load_model(model_type='hybrid'):
    print('Model loading in progress...')

    if model_type == 'hybrid':
        conv_min = conv_model_min()
        quant = qml.qnn.KerasLayer(circuit, {"weights": qweights_shape }, output_dim=1, dtype=tf.float32, name="quantum_layer")
        # Load the model
        model = keras.Sequential([conv_min, quant])
        model.load_weights(hybrid_weight_path).expect_partial()

    else:
        model = c_model()
        model.load_weights(classical_weight_path).expect_partial()

    print('Model loading successful!')

    return model

def predict_classical(x):
    model = load_model(model_type='classical')
    y = model.predict(x.reshape(1, *input_shape))

    return y

def predict_hybrid(x):
    model = load_model(model_type='hybrid')
    y = model.predict(x.reshape(1, *input_shape))

    return y

# Measuring Performance
# x_train, x_test, y_train, y_test = binary_data()
#
# hybrid_aes = []
# classical_aes = []
# for i in range(30):
#     random_number = random.randint(0, n_test-1)
#
#     x = x_test[random_number]
#     y = y_test[random_number]
#     plt.imshow(tf.cast(x_test[random_number], tf.float32))
#     hybrid_y = predict_hybrid(x)
#     classical_y = predict_classical(x)
#
#     hybrid_aes.append(abs(hybrid_y - y))
#     classical_aes.append(abs(classical_y - y))
#
# print('Hybrid MAE: ', np.mean(hybrid_aes))
# print('Classical MAE: ', np.mean(classical_aes))
import socket

def start_server(host, port):
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((host, port))
    server.listen(1)
    print(f"Server listening on {host}:{port}")

    conn, addr = server.accept()
    print(f"Connected by {addr}")

    while True:
        data = conn.recv(1024)
        if not data:
            break
        print(f"Received from C client: {data.decode()}")
        conn.sendall(b"Hello from Python server!")

    conn.close()

if __name__ == "__main__":
    start_server('127.0.0.1', 31337)
