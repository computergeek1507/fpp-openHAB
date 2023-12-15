#!/bin/sh

echo "Running fpp-openHAB PreStart Script"

BASEDIR=$(dirname $0)
cd $BASEDIR
cd ..
make
