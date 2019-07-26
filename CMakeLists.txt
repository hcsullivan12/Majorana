cmake_minimum_required(VERSION 3.5)
project(Majorana)

add_definitions(-DVERSION="v02_05_07")

set(CMAKE_CXX_STANDARD 14)

# Geant4 dependencies
list(APPEND CMAKE_MODULE_PATH $ENV{GEANTDIR})
option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
include_directories(${Geant4_INCLUDE_DIRS})
add_definitions(${Geant4_DEFINITIONS})

if (DEFINED ENV{MAJORANADIR})
  add_definitions(-DPROJECTDIR="$ENV{MAJORANADIR}")
else()
  message(FATAL_ERROR "Could not find MAJORANADIR environment variable")
endif(DEFINED ENV{MAJORANADIR})

# ROOT dependencies 
list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake)
find_package(ROOT COMPONENTS REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
add_definitions(${ROOT_CXX_FLAGS})

# Project includes
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE utilsources ${PROJECT_SOURCE_DIR}/src/majutil/*.cxx)
file(GLOB_RECURSE utilheaders ${PROJECT_SOURCE_DIR}/include/majutil/*.h)

file(GLOB_RECURSE simsources ${PROJECT_SOURCE_DIR}/src/majsim/*.cxx)
file(GLOB_RECURSE simheaders ${PROJECT_SOURCE_DIR}/include/majsim/*.h)
file(GLOB_RECURSE recosources ${PROJECT_SOURCE_DIR}/src/majreco/*.cxx)
file(GLOB_RECURSE recoheaders ${PROJECT_SOURCE_DIR}/include/majreco/*.h)

add_executable(simulate ${PROJECT_SOURCE_DIR}/simulate.cxx ${simsources} ${simheaders} ${utilsources} ${utilheaders})
add_executable(reconstruct ${PROJECT_SOURCE_DIR}/reconstruct.cxx ${recosources} ${recoheaders} ${utilsources} ${utilheaders})

target_link_libraries(simulate ${ROOT_LIBRARIES})
target_link_libraries(simulate ${Geant4_LIBRARIES})
target_link_libraries(reconstruct ${ROOT_LIBRARIES})

# build tests
add_custom_target(tests ALL COMMAND ${CMAKE_COMMAND} -E make_directory tests)

enable_testing()
add_executable(simtest ${PROJECT_SOURCE_DIR}/tests/simtest.cxx ${simsources} ${simheaders} ${utilsources} ${utilheaders})
add_executable(recotest ${PROJECT_SOURCE_DIR}/tests/recotest.cxx ${recosources} ${recoheaders} ${utilsources} ${utilheaders})

set_target_properties(simtest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
set_target_properties(recotest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)

target_link_libraries(simtest ${ROOT_LIBRARIES})
target_link_libraries(simtest ${Geant4_LIBRARIES})
target_link_libraries(recotest ${ROOT_LIBRARIES})

add_test(NAME simtest COMMAND simtest)
add_test(NAME recoTest COMMAND recotest)

file(COPY ${PROJECT_SOURCE_DIR}/tests/SimConfiguration.ini
     DESTINATION ${PROJECT_BINARY_DIR}/tests/)
file(COPY ${PROJECT_SOURCE_DIR}/tests/RecoConfiguration.ini
     DESTINATION ${PROJECT_BINARY_DIR}/tests/)
