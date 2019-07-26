g++ -I ${ROOTSYS}/include -fPIC -std=c++14 -shared Reconstructor.cxx -o libReconstructor.so
g++ -I ${ROOTSYS}/include -fPIC -std=c++14 -shared PixelLoader.h -o libPixelLoader.so
