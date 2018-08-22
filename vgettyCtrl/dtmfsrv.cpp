
#include <QtGlobal>
#include <QtDebug>

#include <QCoreApplication>
#include <QSettings>


#define _VERSION		"0.2"
#define _CONFIG_FILE	"dtmfsrv.conf"
#define _LOG_FILE		"/var/log/mgetty/dtmfsrv.log"

#define _XML_ACTIONS	"/storage/storage/dtmf/actions.xml"
#define _XML_ITEMS		"/storage/storage/dtmf/items.xml"


//--------------------------------------------------------------------------------
#include "WOSHSecrets.h"
#include "WOSHAction.h"
#include "WOSHItemGroup.h"
#include "WOSHItem.h"

#include <QDomDocument>

//--------------------------------------------------------------------------------

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
bool verbose = false;



/**
 * Prints the program name and version
 */
void printBanner()
{
	printf ("OpenSmartHome.com DTMF  - Server\n");
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
	printf("USE: DTMF Server [options]");
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


/**
 * Application entry point
 */
int main(int argc, char **argv)
{
	QCoreApplication app( argc, argv );
	sntApp = &app;

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

	}

	QFile logFile( _LOG_FILE );
	int retCode = 0;
	
	try {
		QSettings settings(_CONFIG_FILE, QSettings::IniFormat, &app);
//		checkConfigDefaults( settings );

		if ( logFile.open(QIODevice::WriteOnly | QIODevice::Append) ) {
			logout = new QTextStream(&logFile);
			qInstallMsgHandler(logOutput);
		}
		else {
			qWarning() << "Can't open log file [" << _LOG_FILE << "], all message will be output to debugger and console";
		}

		QString header = _SECRET_header;
		QString footer = _SECRET_footer;
		QString cmdSep = _SECRET_cmdSep;
		QString argSep = _SECRET_argSep;

		QString dmtf = arguments.last();
		qDebug () << "DTMF Sequence:" << dmtf;

		if ( !dmtf.startsWith(header) ) {
			qWarning() << "Missing DMTF Header! " << header;
			exit(-1);
		}
		if ( !dmtf.endsWith(footer) ) {
			qWarning() << "Missing DMTF Footer! " << footer;
			exit(-1);
		}
		if ( cmdSep.size() > 0 && dmtf.mid(header.size()+WOSHAction::codeLength, 1) != cmdSep ) {
			qWarning() << "Missing DMTF Command/Arg Separator! " << cmdSep;
			exit(-1);
		}

		// CHECK AUTH
		// DECODE DATA

		qDebug () << "Parsing DTMF Sequence:" << dmtf;
		QString cCmd = "";
		QString cArgs = "";

		cCmd = dmtf.mid(header.size(), WOSHAction::codeLength);

		int argpos = header.size() + cCmd.size() + cmdSep.size();
		cArgs = dmtf.mid(argpos, dmtf.size() - argpos - footer.size());

		qDebug () << "Command: " << cCmd;
		qDebug () << "Arguments: " << cArgs;

		WOSHAction* action = WOSHAction::loadAction( _XML_ACTIONS, cCmd );
		if ( action == NULL ) {
			qCritical() << "Action not found/loading error" << cCmd;
			exit(-1);
		}

		qDebug () << "Action: " << action->label << " (command:" << action->command << ")";

		if ( action->requiredArgs.size() > 0 ) {
			qDebug () << "Parsing Arguments.. " << action->requiredArgs.size();

			QStringList argList = cArgs.split(argSep);
			for(int j=0; j<argList.size(); j++)
			{
				QList<WOSHItem*>* items = WOSHItemGroup::loadItems( _XML_ITEMS, argList.at(j) );
				qDebug () << "Arg" << j << ":: Item: " << items->first()->label << " (code:" <<  items->first()->dmtfcode << ")";
				action->currArgs.append( items->first() );

				delete items;
			}
		
			if ( action->currArgs.size() == 0 ) {
				qCritical() << "Argument/Item not found/loading error" << cArgs;
				exit(-2);
			}
		}
		// 

		qDebug () << "Executing Action.. - STARTED";
		retCode = action->execute();
		qDebug () << "Executing Action.. - COMPLETED = " << retCode;

		delete action;
	}

      catch(const char *err) {
		qCritical() << "Exception in main: " << err;
	} catch(...) {
		qFatal("Exception in main");
	}

	qDebug ("MAIN: Terminating");
	qInstallMsgHandler(0);
	delete logout;
	
	//logout = NULL;

	return retCode;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

