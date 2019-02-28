# Setup the needed environment
export MAJORANADIR=$( cd $(dirname ${BASH_SOURCE[0]}) && pwd )

# Export local root 
export ROOTSYS=/home/hunter/Software/root-6.14.04-install
source $ROOTSYS/bin/thisroot.sh

# Export local geant4 
export GEANTDIR=/home/hunter/Software/geant4.10.03-install/release
source $GEANTDIR/bin/geant4.sh
