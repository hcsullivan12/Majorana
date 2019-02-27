# Majorana
This project holds the Geant4 simulation code for the SiPM wheel.

## Physics
An isotropic TPB emission generator simulates a slightly diffuse point-like source incident on an acrylic disk. The photons undergo total internal reflection until they are either detected by a surrounding SiPM or refracted. 

## Prerequisites
   * [GEANT4](https://geant4.web.cern.ch/support/download)
   * [RapidJason](https://github.com/Tencent/rapidjson.git)

## Installation Help for Prerequisites
### -GEANT4 (in GEANT4 directory)
 Run this before installing GEANT4
 ```
 sudo apt-get install libx11-dev libxmu-dev
```
```
 mkdir build
 cd build
 cmake -DGEANT4_USE_OPENGL_X11=ON -DGEANT4_INSTALL_DATA=ON ..
 make
 sudo make install
```

   Add enviroment variables to your profile for GEANT4
```
 sudo vim ~/.bashrc
 source geant4.sh // Add this line to bash file
```

### -RapidJason
   if you dont have install [Doxygen](https://github.com/doxygen/doxygen) 
``` 
 mkdir build
 cd build
 cmake ..
 make
 sudo make install
```
	
## Installation
```
 git clone https://github.com/hcsullivan12/Majorana
 git branch yourname
 git checkout yourname
 git add .
 cd Majorana
 mkdir build
 cd build
 cmake ..
 make
```
Adjust the file paths and mode configuration in `Configuration.json`
## Run the code
``` 
 ./simulate <path_to_config.json> --vis OFF // if you want visuals type ON instead of OFF
```
		
## Configuration
The simulation uses the `LightSourceSteering.txt` file for placement of the emission generator. 
There are two simulation modes which require different formats for the `LightSourceSteering.txt`.
### Point
This mode place the center of the emission generator at the points listed in `LightSourceSteering.txt`, smearing the position using the sourcePosSigma parameter in `Configuration.json`, e.g.
```
x y n
0 0 5000
0 9 4500
-4 0 6000
.
.
.
```
or 
```
r theta n
0 0 5000
9 90 4500
4 180 6000
```
where n is the number of photons to fire for the event. 

### Pixel 
This mode requires an input pixelization.txt file listing the pixelIDs and positions, e.g. assuming a 1cm x 1cm pixel size
```
pixelID x y
1 0 0
2 1 0
3 2 0
.
.
.
```

Then the `LightSourceSteering.txt` file might read

```
pixelID n
1 5000
1 4500
3 6000
.
.
.
```

## Visualization
Supply the following command at runtime
```
./simulate <path_to_config.json> --vis ON
```

## Reconstruction
Reconstruction uses a maximum likelihood method to reconstruct the light source position. The algorithm requires the probability that a photon leaving any position will be detected by any SiPM. 

You must generate an optical reference table from the `simulateOutput.root` ntuple that was produced using a particular pixelization scheme in pixel mode. There is a sample script in `script/makeOpRefTable/` that will produce the reference table. Simply make a class using the `scripts/makeClass.C` script that points to the ntuple, and copy over the code in the sample `makeOpRefTable.C`. You will need to edit the parameters in the script to fit your application. The script also allows you to generate a reference table using fewer SiPMs than simulated. For example, if my ntuple was the result of a pixel simulation (iterating over all pixels) with 128 SiPMs and I wanted to generate a reference table of only 32 SiPMs (mocking a 32 SiPM setup), then I could set:
```
nSiPMs = 128 
factor = 2 
```
of course this only works if the number of SiPMs is a power of 2, e.g. `nSiPMs = 2^n`.

Running this script will generate a text file named `<number_of_pixels>p_<number_of_sipms>s_opReferenceTable.txt`. With the reconstruct variable set to true, the code will parse the reference table into a data structure that is fed into the reconstruction algorithm. 
