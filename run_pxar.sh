#!/bin/bash
# run_pxar.sh: run pxar using an environment organized for the bare module tests.
# Original authors: CMS Pisa Group
# Modified by Konstantin Androsov <konstantin.androsov@cern.ch>.
# Change log:
#   2014-07-11 Modified script to run with pxar. (KA)
#   2014-02-17 Added argument to specify a test center. (KA)
#   2013-07-16 Added argument to specify comments. (KA)
#   2012-12-12 Reviewing script design and adapting it to run on a new environment. (KA)

echo "run_pxar.sh: run pxar using an environment organized for the bare module tests."
echo "Usage: run_pxar.sh [center_name] [operator_name] [module_serial_number] [roc_id] [parameters_path] [comments]"
echo

PARAMETERS_PATH_PREFIX="digi_params"
RESULTS_PATH_PREFIX="digi_results"
ROOT_FILE_NAME="Test.root"
LOG_FILE_NAME="Test.log"
DEFAULT_PARAMETER_SET="defaultParametersRocPSI46digV21"

function read_parameter
{
    if [[ $# -lt 1 || $# -gt 2 ]] ; then
        echo "read_parameter: read a parameter from the standard input"
        echo "Usage: read_parameter parameter_name [default_value]"
        return
    fi
    PARAMETER_NAME="$1"
    if [[ $# -ge 2 ]] ; then
        RESULT="$2"
    else
        RESULT=""
    fi

    ANSWER="no"
    while [[ $ANSWER != "y" && $ANSWER != "yes" ]] ; do
        read -e -p "$PARAMETER_NAME: " -i "$RESULT" RESULT
        printf "$PARAMETER_NAME is '$RESULT'. Are you sure? (y/n) "
        read ANSWER
    done
}

if [[ $# -ge 1 ]] ; then
    CENTER_NAME="$1"
else
    read_parameter "Test center name"
    CENTER_NAME="$RESULT"
fi

if [[ $# -ge 2 ]] ; then
    OPERATOR_NAME="$2"
else
    read_parameter "Operator name"
    OPERATOR_NAME="$RESULT"
fi

if [[ $# -ge 3 ]] ; then
    MODULE_SERIAL_NUMBER="$3"
else
    read_parameter "Module serial number"
    MODULE_SERIAL_NUMBER="$RESULT"
fi

if [[ $# -ge 4 ]] ; then
    ROC_ID="$4"
else
    read_parameter "ROC ID"
    ROC_ID="$RESULT"
fi

if [[ $# -ge 5 ]] ; then
    PARAMETERS_PATH="$PARAMETERS_PATH_PREFIX/$5"
else
    PARAMETERS_PATH=""
    while [[ 1 ]] ; do
        read_parameter "Parameters set" "$DEFAULT_PARAMETER_SET"
        PARAMETERS_PATH="$PARAMETERS_PATH_PREFIX/$RESULT"
        if [[ -d $PARAMETERS_PATH ]] ; then
            break
        else
            echo "ERROR: parameters set '$RESULT' is not found!"
        fi
    done
fi

if [[ $# -ge 6 ]] ; then
    COMMENTS="$6"
else
    COMMENTS=""
fi

TEST_DATE=`date +"%Y-%m-%d_%H_%M"`
RESULTS_PATH="$RESULTS_PATH_PREFIX/$MODULE_SERIAL_NUMBER/${ROC_ID}_${COMMENTS}_${OPERATOR_NAME}_$TEST_DATE"

if [[ -d $RESULTS_PATH ]] ; then
    echo "ERROR: working directory '$RESULTS_PATH' is already exists!"
    exit
fi

printf "Creating working directory '$RESULTS_PATH'... "
mkdir -p "$RESULTS_PATH"
RESULT=$?
if [[ $RESULT -ne 0 ]] ; then
    echo "ERROR while creating working directory!"
fi
printf "done.\n"

printf "Copying parameter files to the working directory... "
cp "$PARAMETERS_PATH"/* "$RESULTS_PATH"/
RESULT=$?
if [[ $RESULT -ne 0 ]] ; then
    echo "ERROR while copying parameter files to the working directory!"
fi
printf "done.\n"

echo "Starting test procedure..."
#psi46expert -V -90 -dir "$RESULTS_PATH" -r "$ROOT_FILE_NAME" -log "$LOG_FILE_NAME"
#psi46expert --center "$CENTER_NAME" --operator "$OPERATOR_NAME" --detector "${MODULE_SERIAL_NUMBER} ${ROC_ID}" --dir "$RESULTS_PATH" --root "$ROOT_FILE_NAME" --log "$LOG_FILE_NAME"
#pXar -d "$RESULTS_PATH" -g
pXar -d "$RESULTS_PATH" -v DEBUG -g
