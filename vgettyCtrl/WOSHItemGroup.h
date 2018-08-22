


#ifndef __WOSH_WOSHItemGroup_H__
 #define __WOSH_WOSHItemGroup_H__

#include "WOSHItem.h"

#include <QList>
#include <QString>

#include <QDomDocument>
#include <QFile>



class WOSHItemGroup {

	public:

		QList<WOSHItem*> items;

		QString title;
		QString type;
			
		WOSHItemGroup()
		{
			this->title = "";
			this->type = "UNKNOWN";
		}

	// ------------------------------------------------------------------------------------

	static WOSHItemGroup* loadGroup( QDomNode& child )
	{
		WOSHItemGroup * action = new WOSHItemGroup();
		action->title = child.namedItem("title").toElement().text();
		action->type = child.namedItem("type").toElement().text();

		return action;
	}


	static QList<WOSHItem*>* loadItems( QString filename, QString dmtfcode )
	{
		QList<WOSHItem*>* allitems = new QList<WOSHItem*>();

		QDomDocument doc("items");
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
		
		QDomNodeList items = docElem.elementsByTagName( "Item" );
		int itemsCount = items.length();

		for(int j=0; j<itemsCount; j++)
		{
			QDomNode child = items.item(j);
			if ( child.namedItem("dmtfcode").toElement().text() != dmtfcode )
				continue;

			WOSHItem * item = new WOSHItem();
			item->label = child.namedItem("label").toElement().text();
			item->dmtfcode = child.namedItem("dmtfcode").toElement().text();
			item->value = child.namedItem("value").toElement().text();

			allitems->append(item);
			
		}

		return allitems;
	}

	static QList<WOSHItemGroup*>* loadItems( QString filename )
	{
		QList<WOSHItemGroup*>* allgroups = new QList<WOSHItemGroup*>();

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
		
		QDomNodeList groups = docElem.elementsByTagName( "ItemsGroup" );
		int groupsCount = groups.length();

		for(int j=0; j<groupsCount; j++)
		{
			QDomNode child = groups.item(j);
			WOSHItemGroup * group = WOSHItemGroup::loadGroup(child);

			QDomNodeList items = child.toElement().elementsByTagName( "Item" );
			int itemsCount = items.length();
			for(int i=0; i<itemsCount; i++)
			{
				QDomNode child = items.item(i);
				WOSHItem * item = new WOSHItem();
				item->label = child.namedItem("label").nodeValue();
				item->dmtfcode = child.namedItem("dmtfcode").nodeValue();
				group->items.append(item);
			}
			
			allgroups->append(group);
		}

		return allgroups;
	}

};

#endif //__WOSH_WOSHItemGroup_H__

