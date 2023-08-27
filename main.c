#include "main.h"

int main() {

    // This whole first bit reads 24-bit BMP file and extracts pixel information
    FILE *fp = fopen("road.bmp", "rb");
    if (fp == NULL)
    {
        printf("Error: Could not open file.\n");
        return 1;
    }

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    if (fileHeader.bfType != 0x4D42)
    {
        printf("Error: Not a BMP file.\n");
        fclose(fp);
        return 1;
    }

    fseek(fp, fileHeader.bfOffBits, SEEK_SET);

    int width = infoHeader.biWidth;
    int height = infoHeader.biHeight;

    unsigned char *pixelData = malloc(3 * width * height);
    if (pixelData == NULL)
    {
        printf("Error: Could not allocate memory.\n");
        fclose(fp);
        return 1;
    }

    // Create a buffer with all of the pixel information
    fread(pixelData, 3 * width, height, fp);


    // Filter the image to be an average grey scale; spare the map points and add them to unsorted list
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {

            // Calculate index of the pixel
            int index = (y * width + x) * 3;

            // Extract original RBG values
            unsigned char blue = pixelData[index];
            unsigned char green = pixelData[index + 1];
            unsigned char red = pixelData[index + 2];

            // Computate original RGB value
            unsigned char avg = (blue + green + red) / 3;

            // Print out values if they're a landmark; else set to average RGB values
            if (red + green + blue == 255)
              printf("Pixel at (%d, %d): R=%u, G=%u, B=%u\n", x, y, red, green, blue);
            else
              for (int i = 0; i < 3; i++)
                pixelData[index + i] = avg;

        }
    }

    // Do a final cleanup
    free(pixelData);
    fclose(fp);

    return 0;
}
