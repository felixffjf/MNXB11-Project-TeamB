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
    echo -e "   $0 '../data/smhi-opendata_1_52240_20200905_163726.csv'"
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

###### Functions END =##################################################

# Exit immediately if the smhicleaner.sh script is not found
if [ ! -f 'smhicleaner.sh' ]; then
   echo "shmicleaner.sh script not found in $PWD. Cannot continue. Exiting"
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
# See Tutorial 4 Slide 45-47 and exercises 4.14, 4.15
if [[ "x$FILTER_SMHIINPUT" == 'x' ]]; then
   echo "Missing input file parameter, exiting" 1>&2
   usage
   exit 1
fi

# Extract filename:
# Extract the name of the file using the "basename" command 
# basename examples: https://www.geeksforgeeks.org/basename-command-in-linux-with-examples/
# then store it in a variable FILTER_DATAFILE
FILTER_DATAFILE=$(basename $FILTER_SMHIINPUT)

# Call smhicleaner

log "Calling smhicleaner.sh script"
./smhicleaner.sh $FILTER_SMHIINPUT

if [[ $? != 0 ]]; then
   echo "smhicleaner.sh failed, exiting..." 1>&2
   exit 1
fi

# smhicleaner.sh generates a filename that starts with baredata_<datafilename>
# So storing it in a variable for convenience.
CLEANER_BAREDATAFILENAME="baredata_$FILTER_DATAFILE"

# base output filename for filtering. The name can be changed to something more relevant.
FILTER_FILTEREDFILENAME="filtered_${FILTER_DATAFILE}"


#############
   #Code that finds which years between 2004 and 2024 that had the highest and 
   #lowest lemperatures during august based on four time points during the day.
      # Author: Sofia Tolonen 
###############

#####################
#### keeps only data form august between 1990 and 1995
####################

FILTER_FILTERFILENAME_ONLYAUGUST90TO95="august_90_95_$FILTER_FILTEREDFILENAME"
log "Only saving measurements taken in August between 1990 and 1995, writing to $FILTER_FILTERFILENAME_ONLYAUGUST90TO95"
grep -E '199[0-5]-08' $CLEANER_BAREDATAFILENAME > $FILTER_FILTERFILENAME_ONLYAUGUST90TO95

####################
#average temp during august per year between 1990 and 1995
####################

FILTER_FILTERFILENAME_ONLYAT12="august90to95at12_$FILTER_FILTEREDFILENAME"
grep '12:00:00' $FILTER_FILTERFILENAME_ONLYAUGUST90TO95 | sort -t',' -n -k3 > $FILTER_FILTERFILENAME_ONLYAT12

for year in {1990..1995}; do
  grep "$year" "$FILTER_FILTERFILENAME_ONLYAT12" | awk -F ',' '{print $3}' > "temps_per_year/${year}temperatures"
  mean_value=$(awk '{sum+=$1} END {if (NR > 0) print sum/NR}' "temps_per_year/${year}temperatures")
  log "mean value of temperatures at 12.00.00 in $year: $mean_value"
done

##############################
# Get temperatures at specific times between 1990 and 1995
# Find the highest and lowest temperature for each time with respective date
##############################

for time in 00 06 12 18; do
  FILTER_FILTERFILENAME_ONLYATTIME="august90to95at${time}_$FILTER_FILTEREDFILENAME"
  grep "$time:00:00" "$FILTER_FILTERFILENAME_ONLYAUGUST90TO95" | sort -t',' -n -k3 > "$FILTER_FILTERFILENAME_ONLYATTIME"
  highest=$(tail -n 1 "$FILTER_FILTERFILENAME_ONLYATTIME")
  lowest=$(head -n 1 "$FILTER_FILTERFILENAME_ONLYATTIME")
  log "For time $time, the highest temperature was recorded: $highest and the lowest temperature was recorded: $lowest"
done

