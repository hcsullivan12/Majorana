# Majorana
Majorana is a package that holds the simulation, reconstruction, visualization, and analysis code along with other tools for the SiPM Wheel. The SiPM Wheel is a photodetector concept to be used for noble-liquid neutrino experiments. The photodetector is a TPB coated acrylic disk that captures scintillation light produced in particle interactions and acts as a waveguide in propagating wavelength-shifted light to a collection of silicon photomultipliers located around the perimeter of the disk. 

## Physics
An isotropic TPB emission generator simulates a slightly diffuse, point-like light source. The emitted photons undergo total internal reflection until they are either detected by a surrounding SiPM or lost to the sorrounding medium. Currently, the variables in the simulation include the disk/SiPM geometries, number of SiPMs, and various material properties (e.g. surface roughness, internal/external mediums). Majorana uses a modified version of [Geant4](https://geant4.web.cern.ch/support/download)'s OpBoundaryProcess library for handling the photon propagation.

## Prerequisites
1. [ROOT](https://root.cern.ch/)
2. [Geant4](https://geant4.web.cern.ch/support/download)
```
$ sudo apt-get install libx11-dev libxmu-dev
$ mkdir build
$ cd build
$ cmake -DGEANT4_USE_OPENGL_X11=ON -DGEANT4_INSTALL_DATA=ON ..
$ make
$ sudo make install
```
3. Boost
``` 
$ sudo apt-get install libboost-all-dev
```
	
## Installation
```
$ git clone https://github.com/hcsullivan12/Majorana
```
Adjust the paths in the `envsetup.sh` script to fit your environment.
```
$ source envsetup.sh 
$ mkdir build
$ cd build
$ cmake ..
$ make
```
## Running the simulation
After updating the configuration file `config/Configuration.ini`, run a simulation with
``` 
 $ ./simulate -c ../config/Configuration.ini 
```

See these pages for further information on the configuration, reconstruction, and visualization:
 1. [Configuration](https://github.com/hcsullivan12/Majorana/README/CONFIGURATION.md)
 2. [Reconstruction](https://github.com/hcsullivan12/Majorana/README/RECONSTRUCTION.md)
 3. [Visualization](https://github.com/hcsullivan12/Majorana/README/VISUALIZATION.md)

 ![et](https://github.com/hcsullivan12/Majorana/README/et.png)
