#! /usr/bin/vm shell

logger -t "dtmf.vm.sh[$$]" "Starting.. CALLER ID: $CALLER_ID - SEQUENCE: $1"

echo "CALLER ID: $CALLER_ID - SEQUENCE: $1" >> /var/log/mgetty/dmtf2.log
 
#
# Define the function to receive an answer from the voice library
#
function receive
     {
     read -r INPUT <&$VOICE_INPUT;
     echo "$INPUT";
     }

#
# Define the function to send a command to the voice library
#
function send
     {
     echo $1 >&$VOICE_OUTPUT;
     kill -PIPE $VOICE_PID
     }

#
# Define the function send a beep
#
function beep
     {
     send "BEEP $1 $2"
     ANSWER=`receive`

     if [ "$ANSWER" != "BEEPING" ]; then
          logger -t "dtmf.vm.sh[$$]" "Could not start beeping"
          kill -KILL $$
     fi

     ANSWER=`receive`

     if [ "$ANSWER" != "READY" ]; then
          logger -t "dtmf.vm.sh[$$]" "Something went wrong on beeping"
          kill -KILL $$
     fi

     }

#
# Define the function to play a file
#
function play
     {
     send "PLAY $1"
     ANSWER=`receive`

     if [ "$ANSWER" != "PLAYING" ]; then
          logger -t "dtmf.vm.sh[$$]" "Could not start playing"
          kill -KILL $$
     fi

     ANSWER=`receive`

     if [ "$ANSWER" != "READY" ]; then
          logger -t "dtmf.vm.sh[$$]" "Something went wrong on playing"
          kill -KILL $$
     fi

     }


#---------------------------------------------------


#
# Let's see if the voice library is talking to us
#

ANSWER=`receive`

if [ "$ANSWER" != "HELLO SHELL" ]; then
     logger -t "dtmf.vm.sh[$$]" "Voice library not answering"
     kill -KILL $$
fi

send "HELLO VOICE PROGRAM"

ANSWER=`receive`
if [ "$ANSWER" != "READY" ]; then
     logger -t "dtmf.vm.sh[$$]" "Initialization failed"
     kill -KILL $$
fi

#
# Enable events
#
send "ENABLE EVENTS"
ANSWER=`receive`
if [ "$ANSWER" != "READY" ]; then
     logger -t "dtmf.vm.sh[$$]" "Initialization failed"
     kill -KILL $$
fi


/storage/storage/dtmf/dtmfsrv $1
if [ $? -eq 0 ]; then
	logger -t "dtmf.vm.sh[$$]" "dtmfsrv returned 0"
	beep 1320 100
          echo "dtmf.vm.sh[$$]" "dtmfsrv returned 0" >> /var/log/mgetty/dmtf2.log
	play "/var/spool/voice/messages/comando_eseguito.rmd"
else
	logger -t "dtmf.vm.sh[$$]" "dtmfsrv returned $?"
	beep 1320 100
          echo "dtmfsrv returned $?" >> /var/log/mgetty/dmtf2.log
	play "/var/spool/voice/messages/standard.rmd"
fi


#
# Let's say goodbye
#

send "GOODBYE"

#
# Let's see if the voice library got it
#

ANSWER=`receive`

if [ "$ANSWER" != "GOODBYE SHELL" ]; then
     logger -t "dtmf.vm.sh[$$]" "Could not say goodbye to voice library"
     kill -KILL $$
fi

exit 0







