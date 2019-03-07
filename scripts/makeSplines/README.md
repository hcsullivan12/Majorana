# makeSplines
The scripts here are used for conditioning and constructing the 2D light detection probability profile for the sipm. Besides eliminating any noise in the 'raw' optical reference tables, the purpose here is to take a single simulation with relatively high granularity, and interpolate using a 2D spline fit onto a configurable pixelization scheme. This reduces the computational charge needed for quick performance optimization.

## What is needed
    * 'Raw' optical reference table produced from relatively high granular simulations (production/2617Pixels_v1 holds reference table for 5mm spacing) 
    * Pixelization scheme used in simulation (pixelization.txt)

## Making the spline    
Once you have what is needed above, you can run the `makeSpline.py' script to produce the conditioned 2D light detection probability profile. This script creates a table holding the spline knots for the east sipm (sipm1). 
```
python makeSpline.py [-h] -orf OPREFTABLE -p PIXELIZATION -o OUTPUT
```
where OPREFTABLE is the 'raw' optical reference table.

## Making new optical reference table
Now you can create a new optical reference table using a different pixelization scheme. Runtime arguments include the `spline.txt` file from above, the new pixelization scheme, the output new optical reference table, and the number of SiPMs.
```
python makeOpRefTableFromSpline.py [-h] -s SPLINE -p NEWPIXELIZATION -o OUTPUT -n NSIPMS
```