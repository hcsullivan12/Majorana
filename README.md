# Majorana
This project holds the Geant4 simulation code for the SiPM wheel.

## Physics
An isotropic TPB emission generator simulates a slightly diffuse point-like source incident on an acrylic disk. The photons undergo total internal reflection until they are either detected by a surrounding SiPM or refracted. 

## Prerequisites
   * [GEANT4](https://geant4.web.cern.ch/support/download)

## Installation Help for Prerequisites
#### GEANT4 (in GEANT4 directory)
 Run this before installing GEANT4
 ```
 sudo apt-get install libx11-dev libxmu-dev
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
#### Boost
``` 
sudo apt-get install libboost-all-dev
```
	
## Installation
```
 git clone https://github.com/hcsullivan12/Majorana
 git branch yourname
 git checkout yourname
 git add .
 cd Majorana
 ```
 Adjust the file paths in the `envsetup.sh` script to fit your environment.
 ```
 source envsetup.sh 
 mkdir build
 cd build
 cmake ..
 make
```		
## Configuration
The simulation uses the `LightSourceSteering.txt` file for placement of the emission generator. 
There are two simulation modes which require different formats for the `LightSourceSteering.txt`.
### Point
This mode place the center of the emission generator at the points listed in `LightSourceSteering.txt`, smearing the position using the sourcePosSigma parameter in `config/Configuration.ini`, e.g.
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
## Running the code
Change the filepaths listed in `config/Configuration.ini` to fit your needs. Paths are relative. 
``` 
 ./simulate -c <path_to_config.ini> --vis ON/OFF (visualization flag, default is OFF)
```
## Reconstruction
### Using 'raw' reference table
Reconstruction uses a maximum likelihood method to reconstruct the light source position. The algorithm requires the probability that a photon leaving any position will be detected by any SiPM. 

You must generate an optical lookup table from the `simulateOutput.root` ntuple that was produced using a particular pixelization scheme in pixel mode. There is a sample script in `scripts/makeOpRefTable/` that will produce the reference table. Simply make a class using the `scripts/makeClass.C` script that points to the ntuple, and copy over the code in the sample `makeOpRefTable.C`. You will need to edit the parameters in the script to fit your application. The script also allows you to generate a reference table using fewer SiPMs than simulated. For example, if my ntuple was the result of a pixel simulation with 128 SiPMs and I wanted to generate a reference table of only 32 SiPMs (mocking a 32 SiPM setup), then I could set:
```
nSiPMs = 128 
factor = 2 
```
of course this only works if the number of SiPMs is a power of 2, e.g. `nSiPMs = 2^n`.

Running this script will generate a text file named `NPIXELSp_NSIPMSs_opReferenceTable.txt`. With the reconstruct variable set to true, the code will parse the reference table into a data structure that is fed into the reconstruction algorithm. 

### Generating other lookup tables
Also provided are scripts that allow one to improve the accuracy of the reference table. These scripts also allow you to quickly generate lookup tables for different pixelizations and number of SiPMs. The idea is to create a 2D light detection probability profile by averaging the response from several sipms. See the ``README.md` files in the relavent directories in `scripts` for further details.

## Event display
The Majorana event display allows one to view the reconstruction results in real time on data or monte carlo. To run the event display on simulation, it's useful to generate some events from `scripts/generators`. Running
```
root -l 'scripts/generators/rndPositionsGen.C(1000)'
```
will generate a `LightSourceSteering.txt` file containing 1000 events uniformly distributed across the disk. Now point your `Configuration.ini` to this file and run the simulation as before except with the `-E ON` flag. Once this has started, open another terminal and run the event display script in `evd`
```
python SIPM_WHEEL_EVD.py
```
Make sure the reconstruction parameters match your simulation parameters, e.g. number of sipms. Click the start button and voila!
