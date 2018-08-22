/*
[General]
CallAction=heyu on j10
DialAction=hangup
LogFile=./waitcall.log

[AuthCallers]
Alex=123

*/
#include <QtGlobal>
#include <QtDebug>

#include <QCoreApplication>
#include <QSettings>
#include <QProcess>


#define _VERSION		"0.2"
#define _CONFIG_FILE	"waitcall.conf"

//--------------------------------------------------------------------------------
struct gn_statemachine *state = NULL;
int call_detected = -1;
QString call_number = "";

#include <gnokii.h>
gn_error busInit();
void busTerminate();
gn_error doDialAction(QString action);

gn_error answer(int lowlevel_id);
gn_error hangup(int lowlevel_id);
void notify_callback(gn_call_status call_status, gn_call_info *call_info, struct gn_statemachine *state, void *callback_data);

void incoming_call(int lowlevel_id, char *number);

//--------------------------------------------------------------------------------

#include <signal.h>

// ---

#include <qtextstream.h>
#include <qdatetime.h>
#include <qtextcodec.h>
#include <qfile.h>
#include <qdir.h>

#include <cstdio>

#include <fcntl.h>

#include <unistd.h>



/**
 * QT Core Application instance handle, used because of QT Threading vs. QT Events (slots and timers)
 */
QCoreApplication* sntApp = NULL;
QTextStream * logout = NULL;
bool running = true;
bool qsDaemonize = false;
bool verbose = false;

sig_atomic_t signal_received = 0;
/**
 * @return The last received signal or 0 if none.
 */
int signalReceived() {
	return signal_received;
}


void quit() {
	if ( running ) {
		running = false;
		return;
		}
	// handle second signal ..
	if ( sntApp != NULL)
		sntApp->quit();
}

/**
 * UNIX signal handler
 */
void signal_handler(int sig)
{
	switch(sig) {
	case SIGHUP:
		qWarning("Hangup signal caught");
		signal_received = sig;
		quit();
		break;
	case SIGTERM:
		qWarning("Terminate signal caught");
		signal_received = sig;
		quit();
		break;
	case SIGINT:
		qCritical("========================\n Interrupt signal caught\n");
		signal_received = sig;
		quit();
		break;
	}
}

/**
 * UNIX signals register
 */
void registerSignals()
{
	signal (SIGINT, signal_handler);
	signal (SIGTERM, signal_handler);
	signal (SIGHUP, signal_handler);
}


/**
 * Make the process a daemon
 */
void daemonize()
{
	if( getppid() == 1 )
		return; /* already a daemon */
	qDebug() << "Daemonized, PID: " << QString::number( getpid() );

	int i = fork();

	if ( i < 0 )
		exit( 1 ); /* fork error */
	else if ( i > 0 )
		exit( 0 ); /* parent exits */
	qDebug() << "Daemonized, FORK: " << QString::number( i );

	/* child (daemon) continues */
	setsid(); /* obtain a new process group */
/*
	for ( i=getdtablesize(); i>=0; --i) {
		close(i); // close all descriptors
	 }
*/

//	i=open("/dev/null",O_RDWR);
//	int dupRet;
//	dupRet = dup(i);
//	dupRet = dup(i); /* handle standart I/O */
//	umask(027); /* set newly created file permissions */
//	int chDirRet = chdir(RUNNING_DIR); /* change running directory */
//	(void)chDirRet;

	///// No lock
	//char str[10];
	//int lfp;
	//lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);
	//if (lfp<0) exit(1); /* can not open */
	//if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */
	///* first instance continues */
	//sprintf(str,"%d\n",getpid());
	//write(lfp,str,strlen(str)); /* record pid to lockfile */
}

/**
 * Prints the program name and version
 */
void printBanner()
{
	printf ("OpenSmartHome.com WaitCall - Gnokii Based\n");
	printf ("Version " _VERSION "\n");
	printf ("----------------------------------------------------------------\n");
}

/**
 * Prints the help.
 *
 * @param pn	Program name
 */
void printHelp()
{
	printf("USE: waitcall [options]");
	printf("Available options:\n");
	printf("\t-h (--help)                Print this Help\n");
	printf("\t-v (--version)             Print Version number\n");
	printf("\t-d (--daemon)              Deamonize SW\n");
	printf("\t-b (--verbose)             Verbose mode\n");
	printf("\t-s (--simulation)          Similation mode\n");

}

//@}


/** 
 * logfile log messages writing
 */
void logOutput(QtMsgType type, const char *msg)
{
	QString debugdate = QDateTime::currentDateTime().toString(QLatin1String("[dd.MM.yy hh:mm:ss.zzz]"));
	switch (type) {
		case QtDebugMsg:
			debugdate += QLatin1String("[D]");
			break;

		case QtWarningMsg:
			debugdate += QLatin1String("[W]");
			break;

		case QtCriticalMsg:
			debugdate += QLatin1String("[C]");
			break;

		case QtFatalMsg:
			debugdate += QLatin1String("[F]");
	 }

	if ( logout != NULL ) {
		(*logout) << debugdate << QLatin1Char(' ') << msg << endl;
		logout->flush();
		}

	if ( verbose )
		fprintf(stderr, "[D] %s\n", msg);
}


int checkConfigDefaults(QSettings &settings)
{
	if ( !settings.contains("LogFile") )
		settings.setValue("LogFile", "/var/log/waitcall.log");

	if ( !settings.contains("AuthCallers") )
		settings.setValue("AuthCallers", "123; 456");

	if ( !settings.contains("DialAction") )
		settings.setValue("DialAction", "hangup");

	if ( !settings.contains("CallAction") )
		settings.setValue("CallAction", "heyu on j10");

	return 0;
}


bool evalCall( QHash<QString, QString>& authNumbers )
{
	if ( authNumbers.contains(call_number) )
	{
		qDebug() << "Number: " << call_number << authNumbers.value(call_number) << " Authorized";
		return true;
	}
	else
	{
		qDebug() << "Number: " << call_number << " Denied";
	}
	return false;
}

/**
 * Application entry point
 */
int main(int argc, char **argv)
{
	QCoreApplication app( argc, argv );
	sntApp = &app;
	bool simulation = false;
	gn_error error = GN_ERR_NONE;
// is global:	bool qsDaemonize

	QStringList arguments = app.arguments();
	if ( arguments.size() > 1 ) {
		if ( arguments.at(1) == "-h" || arguments.at(1) == "--help" ) {
			printHelp();
			exit(0);
			}
		if ( arguments.at(1) == "-v" || arguments.at(1) == "--version" ) {
			printBanner();
			exit(0);
			}

		if ( arguments.contains("-b") || arguments.contains("--verbose") )
			verbose = true;

		if ( arguments.contains("-s") || arguments.contains("--simulation") )
			simulation = true;

		if ( arguments.contains("-d") || arguments.contains("--daemon") )
			qsDaemonize = true;
	}

	try {
		QSettings settings(_CONFIG_FILE, QSettings::IniFormat, &app);
		checkConfigDefaults( settings );

		QFile logFile( settings.value("LogFile").toString() );
		if ( logFile.open(QIODevice::WriteOnly | QIODevice::Append) ) {
			logout = new QTextStream(&logFile);
			qInstallMsgHandler(logOutput);
			}
		else {
			qWarning() << "Can't open log file [" << settings.value("LogFile").toString() << "], all message will be output to debugger and console";
			}

		qDebug() << "Loading Settings..";

		QHash<QString, QString> authNumbers;
		settings.beginGroup("AuthCallers");
		QStringList numNames = settings.childKeys();
		for (int i = 0; i < numNames.size(); ++i)
			authNumbers.insert( settings.value(numNames.at(i)).toString(), QString(numNames.at(i)) );
		settings.endGroup();

		QString program = settings.value("CallAction").toString();
		QProcess myActionProcess( &app );

		qDebug() << "Registering System Signals..";
		registerSignals();

		if ( simulation ) {
			qDebug() << "Skipping Gnokii setup..";
			}
		else {
			qDebug() << "Initializing Bus.. [please wait, when fails there is a timeout]";
			busInit();
			if ( error != GN_ERR_NONE ) {
				qFatal("FATAL: Gnokii Initialization Failed");
				return 1;
				}

			qDebug() << "Initializing Gnokii [callback]..";
	        /* Set up so that we get notified of incoming calls, if supported by the driver */
			gn_data	data;
			// state is global
			gn_data_clear(&data);
			data.call_notification = notify_callback;
			error = gn_sm_functions(GN_OP_SetCallNotification, &data, state);
			// --------------

			}

		// Daemonize
		if ( qsDaemonize ) {
			qDebug() << "OSH Waitcall switching to Daemon Mode..";
			daemonize();
			}
		else {
			qDebug() << "OSH Waitcall running as Console Application..";
			printBanner();
			}

		qDebug() << "\nOSH Waitcall started.. [Waiting for incoming calls]\n";

		while(running)
		 {
			sleep(2);
			call_detected = -1;
			call_number = "";

			if ( simulation ) {
				QTextStream in(stdin);
				call_number = in.readLine();
				call_detected = -2;
				}
			else {
				error = gn_call_check_active(state);
				if ( error != GN_ERR_NONE ) {
						qCritical() << "\nOSH Gnokii Interface error:" << gn_error_print(error); 
						running = false;
						call_number = "";
						continue;
					}
				else {

					if ( call_detected == -1 ) 
						continue;
					}
				}

			if ( evalCall( authNumbers ) ) {
				qDebug() << "Running " << program;
				myActionProcess.start( program );
				}

			if ( simulation )
				qDebug() << "Skipping DialAction: " <<  settings.value("DialAction").toString() ;
			else
				doDialAction( settings.value("DialAction").toString() );

		 }

		qDebug() << "OSH Waitcall killing.. [called app.quit] Exit on signal " << signalReceived();
	}

      catch(const char *err) {
		qCritical() << "Exception in main: " << err;
	} catch(...) {
		qFatal("Exception in main");
	}

	qDebug ("MAIN: Terminating");
	qInstallMsgHandler(0);
	delete logout;
	logout = NULL;

	return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

gn_error busInit()
{
	gn_error error;

	atexit(busTerminate);

	error = gn_lib_phoneprofile_load(NULL, &state);
	if (GN_ERR_NONE == error) {
		error = gn_lib_phone_open(state);
	}

	return error;
}

void busTerminate()
{
	gn_lib_phone_close(state);
	gn_lib_phoneprofile_free(&state);
	gn_lib_library_free();
}

gn_error doDialAction(QString action)
{
	gn_error result = GN_ERR_NONE;
	
	if ( action == "hangup" )
		result = hangup(call_detected);
	else if ( action == "answer" )
		result = answer(call_detected);
	else
		qWarning() << "DialAction Ignored (exit mode): " << action;
	
	if (result == GN_ERR_NONE)
		qDebug() << "DialAction Succeded: " << action;
	else
		qCritical() << "DialAction Failed: " << action << gn_error_print(result);

	call_detected = -1;
	
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

gn_error hangup(int lowlevel_id)
{
	gn_call_info	callinfo;
	gn_data		data;
	
	memset(&callinfo, 0, sizeof(callinfo));
	callinfo.call_id = lowlevel_id;

	gn_data_clear(&data);
	data.call_info = &callinfo;

	return gn_sm_functions(GN_OP_CancelCall, &data, state);
}

gn_error answer(int lowlevel_id)
{
	gn_call_info	callinfo;
	gn_data		data;
	
	memset(&callinfo, 0, sizeof(callinfo));
	callinfo.call_id = lowlevel_id;

	gn_data_clear(&data);
	data.call_info = &callinfo;

	return gn_sm_functions(GN_OP_AnswerCall, &data, state);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* in this callback function you can't use those libgnokii functions that send a packet */
void notify_callback(gn_call_status call_status, gn_call_info *call_info, struct gn_statemachine *state, void *callback_data)
{
	(void)state; (void)callback_data;
	
	if (call_status != GN_CALL_Incoming )
		return;

	incoming_call(call_info->call_id, call_info->number);

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void incoming_call(int lowlevel_id, char *number)
 {
	/* sometimes the callback function gets called twice for the same call */
	if (call_detected == lowlevel_id) return;
	call_number = QString(number);

//	printf("%s\n", number);
	call_detected = lowlevel_id;

}
