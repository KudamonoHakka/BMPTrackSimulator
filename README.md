# BMPTrackSimulator

<p>This project inputs a 24 bit BMP file and creates labeled training data based on dot-marching algorithm (yes I made up this algorithm name).</p>

 ``Usage: %s [track.bmp]``

<p><b>To create a track, </b> you can use a variety of different software, including MSPaint; reference the following example: </p> 
<br>

![image](https://github.com/KudamonoHakka/BMPTrackSimulator/assets/71854345/51049a03-6427-4d92-b0c2-df404cebd795)

<br>
<p>The solid track will be interpreted as road (grey also works) while single pixels of different colors will be interpreted as the following: </p>
<ul>
 <li><b>Blue (0, 0, 255):</b> The initial checkpoint the algorithm will start at.</li>
 <li><b>Green (0, 255, 0) and red (255, 0, 0):</b> The path the algorithm shall transverse. It's important to lay them out like Christmas lights</li>
</ul>

<p>Please reference the following image: </p>
<br>

![image](https://github.com/KudamonoHakka/BMPTrackSimulator/assets/71854345/da8602b3-a6f8-4fa9-b691-4d438e4545c8)

<br>

<h2><b>Export this as a 24 bit BMP image</b></h2>

<p>If done successfuly, the img folder should populate with simulated data that should look like the following (-1.0 means to turn left, 1.0 turn right, 0.0 move forward)</p>

![image](https://github.com/KudamonoHakka/BMPTrackSimulator/assets/71854345/249151b3-0866-446e-90fb-bc43997d1906)
