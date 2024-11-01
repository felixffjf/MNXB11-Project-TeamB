#!/bin/bash

########################################################################
# 
# shmicleaner.sh - Production version
#
# Author: Florido Paganelli florido.paganelli@fysik.lu.se
#
# Description: this script manuipulates specific SMHI dataset and 
#              performs some cleaning actions on it, namely:
#              - Takes an SMHI datafile <filename> from a specified path in the filesystem
#              - Cleanses up unwanted information in the data file and
#                 extracts just the bare temperature data in
#                 - baredata_<filename>
#              - prepares the file to be read by standard C++ 
#                CSV libraries
#
# This script differs from the one in the case study in which it is 
# a streamlined version without comments and less file writing to
# optimize the cleanup operations.
#
# Examples:
#        ./smhicleaner.sh ../data/smhi-opendata_1_52240_20200905_163726.csv
#
# NOTE: the paths above are examples.
# So you should NOT assume the file is exactly in any of the above paths.
# The code must be able to process any possible file path.
#
#
########################################################################

# Memorize script name
CLEANER_SCRIPTNAME=`basename $0`

###### Functions #######################################################

## usage
# this function takes no parameters and prints an error with the 
# information on how to run this script.
usage(){
	echo "----"
	echo -e "  To call this script please use"
	echo -e "   $0 '<path-to-datafile>'"
	echo -e "  Example:"
    echo -e "   $0 '../data/smhi-opendata_1_52240_20200905_163726.csv'"
	echo "----"
}

## log functions
# Create log file with date
# Usage: 
#   createlog
createlog(){
  CLEANER_DATE=`date +%F`
  CLEANER_LOGFILE=${CLEANER_DATE}_${CLEANER_SCRIPTNAME}.log
  touch $CLEANER_LOGFILE
  if [[ $? != 0 ]]; then
     echo "cannot write logfile, exiting" 1>&2
     exit 1
  fi
  echo "Redirecting cleaner logs to $CLEANER_LOGFILE"
}

# logging utility
# Adds a timestamp to a log message and writes to file created with createlog
# Usage:
#   log "message"
# If logfile missing use default CLEANER_LOGFILE
log(){
  if [[ "x$CLEANER_LOGFILE" == "x" ]]; then
    echo "Undefined variable CLEANER_LOGFILE, please check code: createlog() missing. Exiting" 1>&2
    exit 1
  fi
  CLEANER_LOGMESSAGE=$1
  CLEANER_LOGTIMESTAMP=`date -Iseconds`
  # Create timestamped message
  CLEANER_OUTMESSAGE="[${CLEANER_LOGTIMESTAMP} Cleaner]: $CLEANER_LOGMESSAGE"
  # Output to screen
  echo $CLEANER_OUTMESSAGE
  # Output to file
  echo $CLEANER_OUTMESSAGE >> ${CLEANER_LOGFILE}
}

###### Functions END =##################################################

# Create logfile
createlog

# T1 Get the first parameter from the command line:
# (see Tutorial 4 slides 43,44)
# and put it in the variable CLEANER_SMHIINPUT
CLEANER_SMHIINPUT=$1

# T2 Input parameter validation:
# Check that the variable CLEANER_SMHIINPUT is defined, if not, 
# inform the user, show the script usage by calling the usage() 
# function in the library above and exit with error
# See Tutorial 4 Slide 45-47 and exercises 4.14, 4.15
if [[ "x$CLEANER_SMHIINPUT" == 'x' ]]; then
   echo "Missing input file parameter, exiting" 1>&2
   usage
   exit 1
fi

# T3 Extract filename:
# Extract the name of the file using the "basename" command 
# basename examples: https://www.geeksforgeeks.org/basename-command-in-linux-with-examples/
# then store it in a variable CLEANER_DATAFILE
CLEANER_DATAFILE=$(basename $CLEANER_SMHIINPUT)

# Pre-define names for output files in variables so that they can be
# used everywhere in the code
CLEANER_ORIGINALFILENAME="original_${CLEANER_DATAFILE}"
CLEANER_BAREDATAFILENAME="baredata_${CLEANER_DATAFILE}"

# T4 Analyze the input parameter and copy:

# T4.1 If $CLEANER_SMHIINPUT not empty
if [[ "x$CLEANER_SMHIINPUT" != "x" ]]; then
   # T4.2 check if the file is a directory, it should not be!
   if [[ -d $CLEANER_SMHIINPUT ]]; then
      echo -e "This script requires a data file and not a directory, exiting..." 1>&2
      exit 1
   fi
   # T4.3 Copy the file in the current directory as
   #    original_$CLEANER_DATAFILE
   log "Copying input file $CLEANER_SMHIINPUT to $CLEANER_ORIGINALFILENAME"
   cp -a $CLEANER_SMHIINPUT $CLEANER_ORIGINALFILENAME
   # Capture copy errors
   CLEANER_COPY_OUTCOME=$?
fi 

# T5 Check that the input file has been copied with no errors:
if [[ $CLEANER_COPY_OUTCOME != 0 ]]; then
   echo "Error downloading or copying file, check filename or command syntax. Exiting...." 1>&2
   usage
   exit 1
fi

# If we got here without errors, we can start cleaning up!

# T6 Identify the data starting line:
# Looking at the SHMI data, it seems that the line that contains the
# string "Datum" is the beginning of data.
# Find what line contains the string "Datum" using grep
# put the value in a variable called STARTLINE
log "Finding the first line containing 'Datum'..."
STARTLINE=$(grep -n 'Datum' $CLEANER_ORIGINALFILENAME | cut -d':' -f 1)
log "Found line $STARTLINE"

# T7 skip one more header line:
# Use arithmetic expansion to add a line, since the actual 
# data starts at the STARTLINE + 1 line, so to remove the header
# where Datum;... is contained
STARTLINE=$(( $STARTLINE + 1 ))

# T8, T9, T10 
# - Remove unnecessary lines at the top of the datafile (tail)
# - Fix format for the "strange" lines with comments (cut)
# - Convert format to spaces instead of commas (sed)
log "Perform cleanup in one line, result in $CLEANER_BAREDATAFILENAME"
tail -n +$STARTLINE $CLEANER_ORIGINALFILENAME | cut -d';' -f 1,2,3,4,5 | sed 's/;/,/g' > $CLEANER_BAREDATAFILENAME
