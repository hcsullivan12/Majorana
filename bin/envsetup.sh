# Setup the needed environment
packagesPath="/home/hunter/Software/packages"

MAJORANA_DIR=$( cd $(dirname ${BASH_SOURCE[0]}) && pwd )

# Export local root 
export ROOTSYS=/home/hunter/Software/root-6.14.00-install
source $ROOTSYS/bin/thisroot.sh

# Export local geant4 
export GEANTDIR=/home/hunter/Software/geant4.10.03-install/release
source $GEANTDIR/bin/geant4.sh

# Export local rapidjson
export RapidJSON_CMAKEDIR="${packagesPath}/lib/cmake/RapidJSON"
