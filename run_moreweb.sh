#!/bin/bash

if [ $# -lt 1 -o $# -gt 1 ] ; then
    echo "Usage: input_path"
    exit
fi

INPUT_PATH=${1%/}
MOREWEB_PATH=/soft/MoReWeb/Analyse

if ! [ -d $INPUT_PATH ] ; then
    echo "ERROR: Input path '$INPUT_PATH' does not exists"
    exit
fi

INPUT_PATH=$( cd $INPUT_PATH ; pwd )

cd $MOREWEB_PATH
python Controller.py --bareModuletest $INPUT_PATH
