#!/bin/bash
# run_pxar.sh: run pxar using an environment organized for the bare module tests.


bold=$(tput bold)
red=$(tput setaf 1)
reset=$(tput sgr0)

echo "${bold}run_pxar.sh${reset}: run pxar using an environment organized for the bare module tests."
echo "${bold}Usage${reset}: run_pxar.sh center_name operator_name test_parameters label module_id sensor_id roc_id"
echo

PARAMETERS_PATH_PREFIX="digi_params"
RESULTS_PATH_PREFIX="digi_results"
ROOT_FILE_NAME="Test.root"
LOG_FILE_NAME="Test.log"

if [[ $# -ne 7 ]] ; then
	echo "${red}${bold}Invalid number of arguments.${reset}"
	exit 1
fi

CENTER_NAME="$1"
OPERATOR_NAME="$2"
PARAMETERS_PATH="$PARAMETERS_PATH_PREFIX/$3"
LABEL="$4"
MODULE_ID="$5"
SENSOR_ID="$6"
ROC_ID="$7"

TEST_DATE=`date +"%Y-%m-%d_%H_%M"`
RESULTS_PATH="$RESULTS_PATH_PREFIX/$MODULE_ID/${SENSOR_ID}_${LABEL}_${ROC_ID}_${OPERATOR_NAME}_$TEST_DATE"

if [[ -d $RESULTS_PATH ]] ; then
    echo "${red}${bold}ERROR:${reset} working directory '$RESULTS_PATH' already exists!"
    exit
fi

printf "Creating working directory '$RESULTS_PATH'... "
mkdir -p "$RESULTS_PATH"
RESULT=$?
if [[ $RESULT -ne 0 ]] ; then
    echo "${red}${bold}ERROR${reset} while creating working directory!"
fi
printf "done.\n"

printf "Copying parameter files to the working directory... "
cp "$PARAMETERS_PATH"/* "$RESULTS_PATH"/
RESULT=$?
if [[ $RESULT -ne 0 ]] ; then
    echo "${red}${bold}ERROR${reset} while copying parameter files to the working directory!"
fi
printf "done.\n"

echo "Starting test procedure..."
pXar -d "$RESULTS_PATH" -v DEBUG -g
