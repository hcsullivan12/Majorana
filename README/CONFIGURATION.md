# Configuration
The configuration files are located in `config/`. This allows certain parameters to be changed without having to recompile the code. Some sample configuration files are provided with comments for the parameters.

The simulation uses the `LightSourceSteering.txt` file for placement of the emission generator. 
There are two simulation modes which require different formats for the `LightSourceSteering.txt`.

## Point Mode
This mode places the center of the emission generator at the points listed in `LightSourceSteering.txt`, smearing the position using the *sourcePosSigma* parameter, e.g.
```
x y n
0 0 5000
0 9 4500
-4 0 6000
...
```
or 
```
r theta n
0 0 5000
9 90 4500
4 180 6000
...
```
where *n* is the number of photons to fire for the event. 

## Pixel Mode
This mode requires an input `pixelization.txt` file listing the pixelIDs and positions, e.g. assuming a 1cm x 1cm pixel size
```
pixelID x y
1 0 0
2 1 0
3 2 0
...
```
Then the `LightSourceSteering.txt` file might read
```
pixelID n
1 5000
1 4500
3 6000
...
```
Various scripts are provided in `scripts/` to generate a `LightSourceSteering.txt` file either at random or based on a `pixelization.txt` file.