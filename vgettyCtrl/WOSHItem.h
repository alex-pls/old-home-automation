

#ifndef __WOSH_WOSHItem_H__
 #define __WOSH_WOSHItem_H__

#include <QList>
#include <QString>




class WOSHItem {

	public:

		QString label;
		QString dmtfcode;
		QString value;

		static const int codeLength = 3;


		WOSHItem()
		{
			this->label = "";
			this->dmtfcode = "";
			this->value = "";
		}


};

#endif //__WOSH_WOSHItem_H__

