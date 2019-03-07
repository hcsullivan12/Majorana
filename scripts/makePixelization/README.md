# makePixelization
The scripts here can be used to generate a new pixelization scheme and a `LightSourceSteering.txt` for which walks along each pixel. Example:
```
python makePixelization.py [-h] -r DISKRADIUS -s SPACING [-n NEVENTS] [-p NPRIMARIES]
```
or to mass produce
```
python massProduce.py [-h] -s START -e END -i INCREMENT -d DISKRADIUS
```