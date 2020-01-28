#!/bin/bash

DIR=`dirname $0`

# Update the system
/usr/bin/apt update
/usr/bin/apt -y upgrade

# Install the dependencies
/usr/bin/apt -y install libax25 libax25-dev ax25-apps ax25-tools python3-dev

# Install the Python module
${DIR}/setup.py build
${DIR}/setup.py install

# Remove the build
/bin/rm -rf "${DIR}/build"
