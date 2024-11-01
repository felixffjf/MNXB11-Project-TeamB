#!/bin/bash

# Memorize script name
FILTER_SCRIPTNAME=`basename $0`

###### Functions #######################################################

## usage
# this function takes no parameters and prints an error with the 
# information on how to run this script.
usage(){
	echo "----"
	echo -e "  To call this script please use"
	echo -e "   $0 '<path-to-datafile>'"
	echo -e "  Example:"
    echo -e "   $0 '../data/smhi-opendata_1_52240_20200905_163726_Lulea.csv'"
	echo "----"
}

## log functions
# Create log file with date
# Usage: 
#   createlog
createlog(){
  FILTER_DATE=`date +%F`
  FILTER_LOGFILE=${FILTER_DATE}_${FILTER_SCRIPTNAME}.log
  touch $FILTER_LOGFILE
  if [[ $? != 0 ]]; then
     echo "cannot write logfile, exiting" 1>&2
     exit 1
  fi
  echo "Redirecting filter logs to $FILTER_LOGFILE"
}

# logging utility
# Adds a timestamp to a log message and writes to file created with createlog
# Usage:
#   log "message"
# If logfile missing use default CLEANER_LOGFILE
log(){
  if [[ "x$FILTER_LOGFILE" == "x" ]]; then
    echo "Undefined variable FILTER_LOGFILE, please check code: createlog() missing. Exiting" 1>&2
    exit 1
  fi
  FILTER_LOGMESSAGE=$1
  FILTER_LOGTIMESTAMP=`date -Iseconds`
  # Create timestamped message
  FILTER_OUTMESSAGE="[${FILTER_LOGTIMESTAMP} Filter]: $FILTER_LOGMESSAGE"
  # Output to screen
  echo $FILTER_OUTMESSAGE
  # Output to file
  echo $FILTER_OUTMESSAGE >> ${FILTER_LOGFILE}
}

###### Functions END ###################################################

# Path to shmicleaner.sh

SMHICLEANER_PATH="/home/felixffjf/MNXB11-Project-TeamB/bin/CleanerAndFilter/smhicleaner.sh"

# Exit immediately if the shmicleaner.sh script is not found
if [ ! -f "$SMHICLEANER_PATH" ]; then
   echo "shmicleaner.sh script not found at $SMHICLEANER_PATH. Cannot continue. Exiting"
   exit 1
fi
 
# Create logfile
createlog

# Get the first parameter from the command line:
# and put it in the variable FILTER_SMHIINPUT
FILTER_SMHIINPUT=$1

# Input parameter validation:
# Check that the variable FILTER_SMHIINPUT is defined, if not, 
# inform the user, show the script usage by calling the usage() 
# function in the library above and exit with error
if [[ "x$FILTER_SMHIINPUT" == 'x' ]]; then
   echo "Missing input file parameter, exiting" 1>&2
   usage
   exit 1
fi

# Extract filename:
# Extract the name of the file using the "basename" command 
# then store it in a variable FILTER_DATAFILE
FILTER_DATAFILE=$(basename "$FILTER_SMHIINPUT")

# Extract the last part of the filename (e.g., "Lulea" from "smhi-opendata_1_162860_20231007_155220_Lulea.csv")
BASE_NAME=$(basename "$FILTER_DATAFILE" | sed 's/.*_\([^.]*\)\.csv/\1/')

# Validate extraction
if [[ -z "$BASE_NAME" ]]; then
    echo "Error: Could not extract base name from file. Ensure the file follows the expected pattern."
    exit 1
fi

# Call shmicleaner.sh with the full path
log "Calling shmicleaner.sh script"
"$SMHICLEANER_PATH" "$FILTER_SMHIINPUT"

if [[ $? != 0 ]]; then
   echo "shmicleaner.sh failed, exiting..." 1>&2
   exit 1
fi

# shmicleaner.sh generates a filename that starts with baredata_<datafilename>
# So storing it in a variable for convenience.
CLEANER_BAREDATAFILENAME="baredata_$FILTER_DATAFILE"

# Use only the base name as the output filename
FILTER_FILTEREDFILENAME="$BASE_NAME"

#############
# Only keeps measurements between 1950 and 2023
###############

log "Only saving measurements taken between 1950 and 2023, writing to $FILTER_FILTEREDFILENAME"
grep -E '^(195[0-9]|19[6-9][0-9]|20[0-2][0-3])' "$CLEANER_BAREDATAFILENAME" > "$FILTER_FILTEREDFILENAME"

echo "Filtered output saved to $FILTER_FILTEREDFILENAME"
