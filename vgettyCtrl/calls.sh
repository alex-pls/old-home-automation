#! /usr/bin/vm shell

#################################################################
#                                                                                                                                    CONFIGURATION

VOICE_DIR=/var/spool/voice/messages
INCOMING_DIR=/var/spool/voice/incoming

MSG_DIR=/var/spool/voice/messages
MSG_GREETING=/var/spool/voice/messages/standard.rmd
MSG_CMD_EXECUTED=/var/spool/voice/messages/comando_eseguito.rmd
MSG_CMD_FAILED=/var/spool/voice/messages/comando_fallito.rmd
MSG_SESSION_CLOSED=/var/spool/voice/messages/connessione_chiusa.rmd

LOGFILE=/var/log/mgetty/calls.log

#################################################################
#                                                                                                                                  BASIC FUNCTIONS
# Define the function to receive an answer from the voice library
function receive
 {
     read -r INPUT <&$VOICE_INPUT;
     echo "$INPUT";
 }
#
# Define the function to send a command to the voice library
function send
 {
     echo $1 >&$VOICE_OUTPUT;
     kill -PIPE $VOICE_PID
 }
#
# Define the function send a beep
function beep
 {
     send "BEEP $1 $2"

     ANSWER=`receive`
     if [ "$ANSWER" != "BEEPING" ]; then
          log "Could not start beeping"
          kill -KILL $$
     fi

     ANSWER=`receive`
     if [ "$ANSWER" != "READY" ]; then
          log "Something went wrong on beeping"
          kill -KILL $$
     fi
 }
#
# Define the function to play a file
function play
 {
     send "PLAY $1"

     ANSWER=`receive`
     if [ "$ANSWER" != "PLAYING" ]; then
          log "Could not start playing"
          kill -KILL $$
     fi

     ANSWER=`receive`
     if [ "$ANSWER" != "READY" ]; then
          log "Something went wrong on playing"
          kill -KILL $$
     fi
 }
#
# Define the function to record voice
function record
 {
     send "RECORD $1"

     ANSWER=`receive`
     if [ "$ANSWER" != "RECORDING" ]; then
          log "could not start recording"
          kill -KILL $$
     fi

     ANSWER=`receive`
     if [ "$ANSWER" != "READY" ]; then
          log "something went wrong on recording"
          kill -KILL $$
     fi
 }
#
# Define the function to record voice as a new (last) message
function record_last
 {
	 nowtime=`date "+%Y-%m-%d_%k-%M"`
	 MSG_NAME=$INCOMING_DIR/$nowtime
     if [ "$CALLER_ID" != "none" ] && [ "$CALLER_ID" != "" ]; then
          MSG_NAME=$MSG_NAME_$CALLER_ID;
     fi

     send "RECORD $MSG_NAME.rmd"

     ANSWER=`receive`
     if [ "$ANSWER" != "RECORDING" ]; then
          log "could not start recording"
          kill -KILL $$
     fi
 }

#
# Define the function to enable/disable AutoStop (reduce time for voice menu)
function autostop
 {
     send "AUTOSTOP $1"

     ANSWER=`receive`
     if [ "$ANSWER" != "READY" ]; then
          log "AutoStop ON Failed"
          kill -KILL $$
     fi
 }
#
# Define the function to enable/disable Events (PLAYING, RECORDING, WAITING or DIALING)
function events
 {
     if [ "$1" == "ON" ]; then
		send "ENABLE EVENTS"
     else
		send "ENABLE EVENTS"
     fi	 

     ANSWER=`receive`
     if [ "$ANSWER" != "READY" ]; then
          log "EVENTS $1 Failed"
          kill -KILL $$
     fi
 }
#
# Define the function to enable WAIT mode
function waitfor
 {
     send "WAIT $1"

     ANSWER=`receive`
     if [ "$ANSWER" != "WAITING" ]; then
          log "Could not start waiting ($1)"
          kill -KILL $$
     fi
 }
#
# Define the function to STOP current command
function stop_command
 {
     send "STOP"

     ANSWER=`receive`
     if [ "$ANSWER" != "READY" ]; then
          log "Could not stop current command"
#          kill -KILL $$
     fi
 }
#
# Define the function to initiate DIALING
function dial
 {
     send "DIAL $1"

     ANSWER=`receive`
     if [ "$ANSWER" != "DIALING" ]; then
          log "Could not start dialing"
          kill -KILL $$
     fi
 }
#################################################################
#                                                                                                                              UTILITY FUNCTIONS
# Define the function to say a text
function say_text
 {
	FILE_TMP=/tmp/vgetty.voicedata.wav
	echo "$1" | text2wave -o $FILE_TMP;

	playfile=`convert_wav2rmd $FILE_TMP`
	play "$playfile"
 }
#
# Define the function to convert RMD to WAV
function convert_rmd2wav
 {
	filename=`echo $1|sed 's/\..\{3\}$//'`

	rmdtopvf $1 | pvftowav > $filename.wav
	
	echo $filename.wav;
 }
#
# Define the function to convert WAV to RMD
function convert_wav2rmd
 {
	SPEED=8000
	MODEM_TYPE=V253modem
	COMPRESSION=8
	filename=`echo $1|sed 's/\..\{3\}$//'`

	wavtopvf $1 | pvfspeed -s $SPEED > $filename.pvf
	pvftormd $MODEM_TYPE $COMPRESSION $filename.pvf $filename.rmd &> /dev/null

	echo $filename.rmd;
 }
#
# Define the function to convert WAV to RMD
function log
 {
	TIME=`date`
	echo "$TIME $1" >> $LOGFILE;
	#logger -p local0.crit -f $LOGFILE $1
 }

#################################################################
#                                                                                                              DTMF CONTROLLER FUNCTIONS
# Define the function to read one dtmf code string
function getcode_orig
 {
     send "WAIT 30"

     ANSWER=`receive`
     if [ "$ANSWER" != "WAITING" ]; then
          log "Could not start waiting"
          kill -KILL $$
     fi

     ANSWER=""
     while [ "$ANSWER" != "READY" ]
     do
          ANSWER=`receive`

          if [ "$ANSWER" = "RECEIVED_DTMF" ]; then
               ANSWER=`receive`

               if [ "$ANSWER" = "*" ]; then
                    RECEIVED=""
               else

                    case $ANSWER in
                    "#")
                         send "STOP"
                                  ;;
                    0|1|2|3|4|5|6|7|8|9)
                         RECEIVED=$RECEIVED$ANSWER
                                  ;;
                    *)
                         log "Ignoring DTMF $ANSWER"
                                  ;;
                    esac

               fi

          else

               if [ "$ANSWER" = "SILENCE_DETECTED" ] || [ "$ANSWER" = "NO_VOICE_ENERGY" ]; then
                    send "STOP"
               else

                    if [ "$ANSWER" != "READY" ]; then
                         logger -t "calls.sh[$$]" "Ignoring $ANSWER"
                    fi

               fi

          fi

     done

     echo "$RECEIVED";
 }
#
# Define the function to read one dtmf code string WHEN DTFM sequence has started!
function getcode
 {
     ANSWER=""
     while [ "$ANSWER" != "READY" ]
     do
          ANSWER=`receive`

		   if [ "$ANSWER" = "*" ]; then
				RECEIVED=""
		   elif [ "$ANSWER" = "SILENCE_DETECTED" ] || [ "$ANSWER" = "NO_VOICE_ENERGY" ]; then
				break;
		   else
				case $ANSWER in
				"#")
					 break;
							  ;;
				0|1|2|3|4|5|6|7|8|9)
					 RECEIVED=$RECEIVED$ANSWER
							  ;;
				*)
					 log "Ignoring DTMF $ANSWER"
							  ;;
				esac
		   fi
     done

     echo "$RECEIVED";
 }
#   
# Report Last messages
function msg_report
 {
	say_text "hello how are you!!"
	
 }

#################################################################
#                                                                                                         ANSWERING MACHINE FUNCTIONS
TIMESTAMP=$VOICE_DIR/.timestamp

# Report Last messages
function msg_report
 {
     if [ ! -f $TIMESTAMP ]; then
          MSGS=`find $INCOMING_DIR/ -type f -name 'v*.rmd' -print`
     else
          MSGS=`find $INCOMING_DIR/ -type f -name 'v*.rmd' -newer $TIMESTAMP -print`
          if [ -z "$MSGS" ]; then
               BASENAME=`basename $TIMESTAMP`
               NEWSTAMP=`find $VOICE_DIR -name $BASENAME -cmin -10 -print`
               if [ "$NEWSTAMP" = "$TIMESTAMP" ]; then
                    MSGS=`find $INCOMING_DIR -type f -name 'v*.rmd' -print`
               fi
          fi
     fi
     touch $TIMESTAMP-n

     if [ -z "$MSGS" ]; then
		 say_text "No New Messages"
#          play "$NO_NEW_MESSAGES"
     else
		 say_text "There are $j Messages"
		 j=0
	     for i in $MSGS
	     do
	          beep 1320 200
			  #msgtime=`stat -c %Y $i`
			  #msgtime=`date -r $msgtime "+%R %A %d`
			  msgtime=`date -r $i "+%R %A %d"`
			  say_text "Message $j at $msgtime"
	          beep 1320 100
	          play $i
			  $j=$j+1
	     done
     fi
 }
# Report Last messages
function msg_read_last
 {
	say_text "hello how are you!!"
	
 }

#################################################################
#                                                                                                                                            INITIALIZE
log "Starting.. CALLER ID: $CALLER_ID"

# Let's see if the voice library is talking to us
ANSWER=`receive`
if [ "$ANSWER" != "HELLO SHELL" ]; then
     log "Voice library not answering"
     kill -KILL $$
fi

# Initialize voice library
send "HELLO VOICE PROGRAM"

ANSWER=`receive`
if [ "$ANSWER" != "READY" ]; then
     log "Initialization failed"
     kill -KILL $$
fi

#################################################################
#                                                                                                                                                      MAIN

log "Playing Welcome message $MSG_GREETING"
play "$MSG_GREETING"
# play "$INCOMING_DIR/2008-09-30_20-00.rmd"

log "Enabling Events"
events "ON"

beep

log "Recording"
record_last

ANSWER=`receive`
log "Received Action $ANSWER"

case $ANSWER in
    "FAX_CALLING_TONE")
		send "STOP";
		receive;
		send "GOODBYE";
		receive;
		exit 2 # try to get the fax
		;;

	"RECEIVED_DTMF")
		DTMF=`getcode`
		log "DTMF Sequence: $DTMF"
		stop_command
		beep 1320 100

		if [ "$DTMF" = "78" ]; then
			msg_report
		else
			log "Forwarding DTMF Command to dtmfsrv"
			RESPONSE=`/storage/storage/dtmf/dtmfsrv $DTMF`
			EXITCODE=$?
			log "dtmfsrv returned $EXITCODE, RESPONSE=$RESPONSE"

			if [ $EXITCODE -eq 0 ]; then
				beep 1320 100
				play "$MSG_CMD_EXECUTED"
			else
				beep 150 200
				play "$MSG_CMD_FAILED"
			fi
		fi
		beep
		play "$MSG_SESSION_CLOSED"
		break
		;;
	"VOICE_DETECTED"|"LOOP_BREAK"|"SILENCE_DETECTED"|"NO_VOICE_ENERGY")
		break;
		;;
esac

stop_command
		
log "Exiting.."


#################################################################
#                                                                                                                                             SHUTDOWN

send "GOODBYE"

ANSWER=`receive`
if [ "$ANSWER" != "GOODBYE SHELL" ]; then
     log "Could not say goodbye to voice library"
     kill -KILL $$
fi

exit 0
