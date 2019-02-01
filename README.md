# Majorana
This project holds the Geant4 simulation code for the SiPM wheel.

## Physics
An isotropic TPB emission generator simulates a slightly diffuse point-like source incident on an acrylic disk. The photons undergo total internal reflection until they are either detected by a surrounding SiPM or refracted. 

# Prerequisits
   * [GEANT4](https://geant4.web.cern.ch/support/download)
   * [RapidJason](https://github.com/Tencent/rapidjson.git)

# Installation Help for Prerequisits
  ## -GEANT4 (in GEANT4 directory)
 Run this before instaling GEANT4
 ```
 sudo apt-get install libx11-dev libxmu-dev
```
```
 mkdir build
 cd build
 cmake -DGEANT4_USE_OPENGL_X11=ON -DGEANT_INSTAL_DATA=ON ..
 make
 sudo make install
```

   Add enviroment variables to your profile for GEANT4
```
 sudo vim ~/.bashrc
 source geant4.sh // Add this line to bash file
```

		
		
  ## -RapidJason
   if you dont have install [Doxygen](https://github.com/doxygen/doxygen) 
``` 
 mkdir build
 cd build
 cmake ..
 make
 sudo make install
```
	
# Installation

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
  ### Adjust the file paths and mode configuration in config/Configuration.json
  ### Run the code
``` 
 ./simulate <path_to_config.json> --vis ON
```
		
## Configuration
The simulation uses the LightSourceSteering.txt file for placement of the emission generator. 
There are two simulation modes which require different formats for the LightSourceSteering.txt.
### Point
This mode place the center of the emission generator at the points listed in the LightSourceSteeringFile, smearing the position using the sourcePosSigma parameter in config/Configuration.json, e.g.
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

### Voxel 
This mode requires an input voxelization.txt file listing the voxelIDs and positions, e.g. assuming a 1 cm2 voxel size
```
voxelID x y
1 0 0
2 1 0
3 2 0
.
.
.
```
Then the LightSourceSteering.txt file might read
```
voxelID n
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




	

