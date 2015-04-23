#!/bin/bash
#
#  \file merge_bare_module_tests.sh
#  \brief Merge test output files for each bare module chip into single file.
#  \author Konstantin Androsov (University of Siena, INFN Pisa)
#
#  Copyright 2015 Konstantin Androsov <konstantin.androsov@gmail.com>
#
#  This file is part of PixelTestTools.
#
#  PixelTestTools is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  PixelTestTools is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with PixelTestTools.  If not, see <http://www.gnu.org/licenses/>.

if [ $# -lt 2 -o $# -gt 2 ] ; then
    echo "Usage: input_path output_path"
    exit
fi

INPUT_PATH=${1%/}
OUTPUT_PATH=${2%/}

FIX_TOOL_PATH="./bin/fix_bare_chip_id"
OLD_CHIP_ID=0

if [ ! -d "$INPUT_PATH" ] ; then
    echo "ERROR: input path '$INPUT_PATH' does not exist."
    exit
fi

if [ ! -d "$OUTPUT_PATH" ] ; then
    echo "ERROR: output path '$OUTPUT_PATH' does not exist."
    exit
fi

MODULE_ID=$( basename $INPUT_PATH )

CHIPS=$( find "$INPUT_PATH" -type d -maxdepth 1 ! -path "$INPUT_PATH" -print0 | xargs -0 -n 1 basename \
    | sed 's/\([0-9]*\).*/\1/' | sort )

DUPLICATES=$( echo "$CHIPS" | uniq -d )

if [ ! "$DUPLICATES" = "" ] ; then
    echo "ERROR: there more then one test results for the following chips:" $DUPLICATES
    exit
fi

N_CHIPS=$( echo "$CHIPS" | wc -l | tr -d " " )

echo "Total numer of chips: $N_CHIPS"

OUTPUT_DIR="$OUTPUT_PATH/$MODULE_ID"
FINAL_OUTPUT="$OUTPUT_DIR/commander_BareModuletest.root"
mkdir -p "$OUTPUT_DIR"
rm -f $FINAL_OUTPUT $OUTPUT_DIR/pxar_*.root

for CHIP in $CHIPS ; do
    INPUT_FILE=$( find $INPUT_PATH -path "$INPUT_PATH/$CHIP*/pxar.root" -type f )
    OUTPUT_FILE="$OUTPUT_DIR/pxar_$CHIP.root"
    $FIX_TOOL_PATH --input="$INPUT_FILE" --output="$OUTPUT_FILE" --old-chip-id=$OLD_CHIP_ID --new-chip-id=$CHIP
    RESULT=$?
    if [ $RESULT -ne 0 ] ; then
        echo "ERROR: fix tool finished unsuccessfully. Aborting."
        exit
    fi
done

hadd -f9 $FINAL_OUTPUT $OUTPUT_DIR/pxar_*.root > /dev/null
rm -f $OUTPUT_DIR/pxar_*.root

CONFIG_FILE="$OUTPUT_PATH/configParameters.dat"
PXAR_FILE="$OUTPUT_DIR/pxar.cfg"
cp $CONFIG_FILE $OUTPUT_DIR
touch $PXAR_FILE

echo "Bare module $MODULE_ID test results are successfully merged into $FINAL_OUTPUT"
