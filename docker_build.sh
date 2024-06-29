#!/bin/bash

# Script to compile RS41FOX on any platform using Docker
# This script is copied into the Docker image when it is built.
# If you plan to change this script, please make sure to rebuild the Docker image.

# This script was copied from the RS41ng repository by mikaelnousiainen

set -e # Exit if failed

SOURCE_PATH="/usr/local/src/RS41FOX"

# Sanity check
if [ ! -d "${SOURCE_PATH}" ]
then
    echo "Source directory does not exist, please run the Docker command given in README to mount the source directory"
    exit 1
fi

# Build RS41FOX
cd "${SOURCE_PATH}"
rm -rf build
mkdir -p build
cd build
cmake ..
make -j$(nproc)

echo "RS41FOX build complete"