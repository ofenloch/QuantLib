#!/bin/bash

if [ -z ${workspaceFolder:+x} ]; then
    echo "Environment variable workspaceFolder is not set!"
    echo "Please do something like"
    echo "                          workspaceFolder=\$(pwd) ${0}"
    exit 1
fi

# https://www.quantlib.org/install/cmake.shtml#install tells us
#
# Installing
# The final build artifacts (the library, headers, etc.) can be installed into a 
# directory outside of the build tree for use from other projects. This separates 
# the build by-products from the final results and also allows packaging of the binaries.
#
# In CMake, you can select the installation prefix using the CMAKE_INSTALL_PREFIX 
# variable. That is, configuring the project using:
#
#     cd build
#     cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/install/dir
#     make install
#
# would install the binaries into the given folder. CMake generates a build target named 
# install (or INSTALL in Visual Studio projects) which accomplishes the task when built.


# we want to release / deploy the "Release" version
DIST_BUILD_TYPE="Release"

# the directory where we build out binaries:
DIST_BUID_DIR=${workspaceFolder}/build_dist

# the directory where we "install" out package:
DIST_DIR=${workspaceFolder}/dist/QuantLib


# during development we use the Debug versions:
DIST_BUILD_TYPE="Debug"
DIST_BUID_DIR=${workspaceFolder}/build
DIST_DIR=${workspaceFolder}/dist/QuantLib



export DIST_BUILD_TYPE
export DIST_BUID_DIR
export DIST_DIR

/bin/rm -rf ${DIST_DIR}
/usr/bin/mkdir -p ${DIST_DIR}

# cmake 
#   -S <path-to-source>          = Explicitly specify a source directory.
#   -B <path-to-build>           = Explicitly specify a build directory.
#   -C <initial-cache>           = Pre-load a script to populate the cache.
#   -D <var>[:<type>]=<value>    = Create or update a cmake cache entry.
#   -U <globbing_expr>           = Remove matching entries from CMake cache.
#   -G <generator-name>          = Specify a build system generator.
/usr/bin/cmake --no-warn-unused-cli \
               -DCMAKE_INSTALL_PREFIX=${DIST_DIR} \
               -DCMAKE_BUILD_TYPE:STRING="${DIST_BUILD_TYPE}" \
               -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
               -S${workspaceFolder} \
               -B${DIST_BUID_DIR} \
               -G "Unix Makefiles"
# /usr/bin/cmake --build ${DIST_BUID_DIR} --target install
cd ${DIST_BUID_DIR}
make install
