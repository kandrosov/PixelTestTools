#!/bin/bash
#
#  \file upload_bare_module_tests.sh
#  \brief Upload all bare module test results and moreweb outputs into the central database.
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

if [ $# -ne 3 ] ; then
    echo "Usage: test_results_path moreweb_output_path uploads_path"
    exit
fi

TEST_RESULTS_PATH=${1%/}
MOREWEB_OUTPUT_PATH=${2%/}
UPLOADS_PATH=${3%/}

if [ ! -d "$TEST_RESULTS_PATH" ] ; then
    echo "ERROR: test results path '$TEST_RESULTS_PATH' does not exist."
    exit
fi

if [ ! -d "$MOREWEB_OUTPUT_PATH" ] ; then
    echo "ERROR: moreweb output path '$MOREWEB_OUTPUT_PATH' does not exist."
    exit
fi

if [ ! -d "$UPLOADS_PATH" ] ; then
    echo "ERROR: uploads path '$UPLOADS_PATH' does not exist."
    exit
fi


SUMMARY_DIR="BareFinalResults-R001/QualificationGroup/BareModule_p17_1/Summary1"
SUMMARY_PATH="$MOREWEB_OUTPUT_PATH/$SUMMARY_DIR"

if [ ! -d "$SUMMARY_PATH" ] ; then
    echo "ERROR: summary path '$SUMMARY_PATH' does not exist."
    exit
fi

BUMP_BONDING_DIR=$( find "$SUMMARY_PATH" -maxdepth 1 -type d -name "*-BareModuleTestBumpBonding-*" -print0 \
                    | xargs -0 -n 1 basename )
BUMP_BONDING_TAR="$BUMP_BONDING_DIR.tar.gz"

if [ ! -d "$SUMMARY_PATH/$BUMP_BONDING_DIR" ] ; then
    echo "ERROR: bump bonding test summary is not found in '$SUMMARY_PATH'."
    exit
fi

if [ -d "$UPLOADS_PATH/$BUMP_BONDING_DIR" -o -f "$UPLOADS_PATH/$BUMP_BONDING_TAR" ] ; then
    echo "ERROR: bump bonding test summary for this module already exists in the uploads path."
    exit
fi

PIXEL_ALIVE_DIR=$( find "$SUMMARY_PATH" -maxdepth 1 -type d -name "*-BareModuleTestPixelAlive-*" -print0 \
                    | xargs -0 -n 1 basename )
PIXEL_ALIVE_TAR="$PIXEL_ALIVE_DIR.tar.gz"

if [ ! -d "$SUMMARY_PATH/$PIXEL_ALIVE_DIR" ] ; then
    echo "ERROR: pixel alive test summary is not found in '$SUMMARY_PATH'."
    exit
fi

if [ -d "$UPLOADS_PATH/$PIXEL_ALIVE_DIR" -o -f "$UPLOADS_PATH/$PIXEL_ALIVE_TAR" ] ; then
    echo "ERROR: pixel alive test summary for this module already exists in the uploads path."
    exit
fi

cp -r "$SUMMARY_PATH/$BUMP_BONDING_DIR" "$UPLOADS_PATH/"
cp -r "$SUMMARY_PATH/$PIXEL_ALIVE_DIR" "$UPLOADS_PATH/"

TEST_RESULTS_UPLOAD_PATH="$UPLOADS_PATH/$BUMP_BONDING_DIR/test_results"
cp -r $TEST_RESULTS_PATH $TEST_RESULTS_UPLOAD_PATH

MOREWEB_OUTPUT_UPLOAD_PATH="$UPLOADS_PATH/$BUMP_BONDING_DIR/moreweb_output"
cp -r $MOREWEB_OUTPUT_PATH $MOREWEB_OUTPUT_UPLOAD_PATH

tar czf "$UPLOADS_PATH/$BUMP_BONDING_TAR" "$UPLOADS_PATH/$BUMP_BONDING_DIR"
tar czf "$UPLOADS_PATH/$PIXEL_ALIVE_TAR" "$UPLOADS_PATH/$PIXEL_ALIVE_DIR"

rm -r "$UPLOADS_PATH/$BUMP_BONDING_DIR" "$UPLOADS_PATH/$PIXEL_ALIVE_DIR"

scp "$UPLOADS_PATH/$BUMP_BONDING_TAR" cmspixelprod.pi.infn.it:dropbox/
RESULT=$?
if [ $RESULT -ne 0 ] ; then
    echo "ERROR: unable to uplad the bump bonding results."
    exit
fi
echo "The bump bonding results are successfully uploaded."

scp "$UPLOADS_PATH/$PIXEL_ALIVE_TAR" cmspixelprod.pi.infn.it:dropbox/
RESULT=$?
if [ $RESULT -ne 0 ] ; then
    echo "ERROR: unable to uplad the pixel alive results."
    exit
fi
echo "The pixel alive results are successfully uploaded."
