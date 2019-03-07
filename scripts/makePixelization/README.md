# makePixelization
The scripts here can be used to generate a new pixelization scheme and a `LightSourceSteering.txt` for which walks along each pixel. Example:
```
python makePixelization.py -r diskradius -s pixelspacing -n neventsperpixel -p --nprimariesperevent
```
or to mass produce
```
python massProduce.py -s spacingstart -e spacingend -i increment -d diskradius
```