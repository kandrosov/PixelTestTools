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

FIX_TOOL_PATH="fix_bare_chip_id"
OLD_CHIP_ID=0
MODULE_INFO_FILE="bareModuleInfo.txt"

if [ ! -d "$INPUT_PATH" ] ; then
    echo "ERROR: input path '$INPUT_PATH' does not exist."
    exit
fi

if [ ! -d "$OUTPUT_PATH" ] ; then
    echo "ERROR: output path '$OUTPUT_PATH' does not exist."
    exit
fi

if [ ! -f "$INPUT_PATH/$MODULE_INFO_FILE" ] ; then
    echo "ERROR: $MODULE_INFO_FILE not found."
    exit
fi

MODULE_ID=$( basename $INPUT_PATH )

CHIPS=$( find "$INPUT_PATH" -maxdepth 1 -type d ! -path "$INPUT_PATH" -print0 | xargs -0 -n 1 basename \
    | sed 's/\([0-9]*\).*/\1/' | sort )

DUPLICATES=$( echo "$CHIPS" | uniq -d )

if [ ! "$DUPLICATES" = "" ] ; then
    echo "ERROR: there more then one test results for the following chips:" $DUPLICATES
    exit
fi

N_CHIPS=$( echo "$CHIPS" | wc -l | tr -d " " )

echo "Total numer of chips: $N_CHIPS"

TEST_DATE=
TEST_TIMESTAMP=0

for CHIP in $CHIPS ; do
    CHIP_DIR=$( find $INPUT_PATH -path "$INPUT_PATH/$CHIP*" -type d )
    CHIP_TEST_DATE=$( echo $CHIP_DIR | sed 's/.*\(_.*_.*_[^_]*$\)/\1/' \
                                     | sed 's/^_\([^_]*\)_\([^_]*\)_\(.*\)/\1 \2:\3/' )
    if [ $OSTYPE = "linux-gnu" ] ; then
        CHIP_TEST_TIMESTAMP=$( date --date="$CHIP_TEST_DATE:00" "+%s" )
    else
        CHIP_TEST_TIMESTAMP=$( date -j -f "%Y-%m-%d %H:%M:%S" "$CHIP_TEST_DATE:00" "+%s" )
    fi

    if (( $TEST_TIMESTAMP < CHIP_TEST_TIMESTAMP )) ; then
        TEST_DATE="$CHIP_TEST_DATE"
        TEST_TIMESTAMP=$CHIP_TEST_TIMESTAMP
    fi
done

echo "Test date will be set to '$TEST_DATE', which corresponds to an unix timestamp $TEST_TIMESTAMP."

TEST_DATE=$( echo $TEST_DATE | sed 's/\(^[^ ]*\) \([^:]*\):\(.*\)/\1_\2h\3m/' )
OUTPUT_DIR="$OUTPUT_PATH/${MODULE_ID}_${TEST_DATE}_${TEST_TIMESTAMP}"
FINAL_OUTPUT="$OUTPUT_DIR/commander_BareModuletest.root"
mkdir -p "$OUTPUT_DIR"
rm -f $FINAL_OUTPUT $OUTPUT_DIR/pxar_*.root

DAC_FILE_PREFIX="dacParameters_C"
ORIGINAL_DAC_FILE="${DAC_FILE_PREFIX}0.dat"

for CHIP in $CHIPS ; do
    CHIP_NUMBER=$( echo $CHIP | sed 's/^0//' )
    CHIP_DIR=$( find $INPUT_PATH -path "$INPUT_PATH/$CHIP*" -type d )
    INPUT_FILE=$( find $INPUT_PATH -path "$INPUT_PATH/$CHIP*/pxar.root" -type f )
    OUTPUT_FILE="$OUTPUT_DIR/pxar_$CHIP.root"
    $FIX_TOOL_PATH --input="$INPUT_FILE" --output="$OUTPUT_FILE" --old-chip-id=$OLD_CHIP_ID --new-chip-id=$CHIP
    RESULT=$?
    if [ $RESULT -ne 0 ] ; then
        echo "ERROR: fix tool finished unsuccessfully. Aborting."
        exit
    fi


    if [ ! -f "$CHIP_DIR/$ORIGINAL_DAC_FILE" ] ; then
        echo "ERROR: $ORIGINAL_DAC_FILE not found for chip $CHIP_NUMBER."
        exit
    fi

    CHIP_DAC_FILE="${DAC_FILE_PREFIX}${CHIP_NUMBER}.dat"
    cp "$CHIP_DIR/$ORIGINAL_DAC_FILE" "$OUTPUT_DIR/$CHIP_DAC_FILE"
done

hadd -f9 $FINAL_OUTPUT $OUTPUT_DIR/pxar_*.root > /dev/null
rm -f $OUTPUT_DIR/pxar_*.root

CONFIG_FILE="$OUTPUT_PATH/configParameters.dat"

PXAR_FILE="$OUTPUT_DIR/pxar.cfg"
cp "$CONFIG_FILE" "$OUTPUT_DIR"
touch "$PXAR_FILE"
cp "$INPUT_PATH/$MODULE_INFO_FILE" "$OUTPUT_DIR"

echo "Bare module $MODULE_ID test results are successfully merged into $FINAL_OUTPUT"
