



#ifndef __WOSH_WOSHAction_H__
 #define __WOSH_WOSHAction_H__

 
#include <QList>
#include <QString>
#include <QProcess>

#include <QDomDocument>
#include <QFile>

#include "WOSHItem.h"


class WOSHAction {

	public:
	
		QString name;
		QString label;

		QString dmtfcode;
		static const int codeLength = 2;

		QString command;
		QString description;

		QString echo;
		int exitcode;
		
		QList<int> requiredArgs;
		QList<WOSHItem*> currArgs;


		WOSHAction() {
			this->name = "";
			this->label = "";
			this->dmtfcode = "";
			this->command = "";
			this->description = "";
			
			this->exitcode = 0;
		}

		~WOSHAction() {
			for(int j=0; j<this->currArgs.size(); j++)
				delete this->currArgs.at(j);
		}

		void clearCurrArgs() {
			this->currArgs.clear();
		}


		int execute() {
			if ( this->command == "" )
				return -1001;

			QProcess myProcess;
			for(int j=0; j<this->currArgs.size(); j++)
			{
				QString program = command + " " + this->currArgs.at(j)->value;
				qDebug() << "Running " << program;
				myProcess.start( program );

				if ( !myProcess.waitForFinished(10000) )
					myProcess.terminate();
			}
			if ( this->echo != "" )
				printf(this->echo.toAscii().constData());

			return this->exitcode;
		}


	// ------------------------------------------------------------------------------------


	static WOSHAction* loadAction( QDomNode& child )
	{
		WOSHAction * action = new WOSHAction();
		action->name = child.namedItem("name").toElement().text();
		action->label = child.namedItem("label").toElement().text();
		action->command = child.namedItem("command").toElement().text();
		action->dmtfcode = child.namedItem("dmtfcode").toElement().text();
		action->description = child.namedItem("description").toElement().text();

		QDomNodeList args = child.namedItem("arguments").toElement().elementsByTagName( "argument" );
		int argsCount = args.length();

		for(int j=0; j<argsCount; j++) {
			action->requiredArgs.append( args.item(j).namedItem("argument").toElement().text().toInt()  );
		}

		action->exitcode = child.namedItem("return").toElement().namedItem("exitcode").toElement().text().toInt();
		action->echo = child.namedItem("return").toElement().namedItem("echo").toElement().text();

		return action;
	}


	static WOSHAction* loadAction( QString filename, QString dmtfcode )
	{
		QDomDocument doc("actions");
		QFile file(filename);
		if (!file.open(QIODevice::ReadOnly)) {
			qDebug() << "Cant open file: " << filename;
			return NULL;
		}
		if (!doc.setContent(&file)) {
			file.close();
			qDebug() << "Cant set contents from " << filename;
			return NULL;
		}
		file.close();

		qDebug() << "Parsing document.." << filename << ", searching dmtfcode" << dmtfcode;
		QDomElement docElem = doc.documentElement();

		QDomNodeList acts = docElem.elementsByTagName( "Action" );
		int actsCount = acts.length();

		for(int j=0; j<actsCount; j++)
		{
			QDomNode child = acts.item(j);
//			qDebug() << "Found Action dmtfcode#" << child.namedItem("dmtfcode").toElement().text() << " label:" << child.namedItem("label").toElement().text();
			if ( child.namedItem("dmtfcode").toElement().text() == dmtfcode )
				return WOSHAction::loadAction(child);
		}

		return NULL;
	}

	static QList<WOSHAction*>* loadActions( QString filename )
	{
		QList<WOSHAction*>* actions = new QList<WOSHAction*>();

		QDomDocument doc("actions");
		QFile file(filename);
		if (!file.open(QIODevice::ReadOnly)) {
			qDebug() << "Cant open file: " << filename;
			return NULL;
		}
		if (!doc.setContent(&file)) {
			file.close();
			qDebug() << "Cant set contents from " << filename;
			return NULL;
		}
		file.close();

		qDebug() << "Parsing document.." << filename;
		QDomElement docElem = doc.documentElement();
		
		QDomNodeList acts = docElem.elementsByTagName( "Action" );
		int actsCount = acts.length();

		for(int j=0; j<actsCount; j++)
		{
			QDomNode child = acts.item(j);

			WOSHAction * action = WOSHAction::loadAction(child);

			actions->append(action);
		}

/*
    <Action>
      <label>Turn ON</label>
      <name>x10.turn.on</name>
      <command>11</command>
      <description>Turn on lights o appliances</description>
	  <arguments>
        <argument>1</argument>
        <argument>2</argument>
	  </arguments>
    </Action>
*/

		return actions;
	}









};


#endif //__WOSH_WOSHAction_H__

