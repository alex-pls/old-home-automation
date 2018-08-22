
package com.opensmarthome.mobile;

import javax.microedition.io.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

import java.util.*;
import java.io.*;
import javax.xml.parsers.*;
import org.xml.sax.*;
import org.xml.sax.helpers.*;


public final class WOSHItemGroup {

	public static final String UNKNOWN = "UNKNOWN";
	public static final String LIGHT = "LIGHT";
	public static final String APPLIANCE = "APPLIANCE";
	public static final String REPORTS = "REPORTS";
	public static final String TIMESPAN = "TIMESPAN";

	private ChoiceGroup choiceItems = null;

	public String label = "";
	public String type = WOSHItemGroup.UNKNOWN;
	public Vector items = null;


	WOSHItemGroup() {
		this.items = new Vector();
	}

	WOSHItemGroup( String lbl ) {
		this.label = lbl;
		this.items = new Vector();
	}

	WOSHItemGroup( String lbl, String type ) {
		this.label = lbl;
		this.type = type;
		this.items = new Vector();
	}

	
	public ChoiceGroup getChoiceGroup() {
		if ( this.choiceItems == null ) {
			this.choiceItems = new ChoiceGroup(this.label, Choice.MULTIPLE);
			for(int j=0; j<this.items.size(); j++)
				this.choiceItems.append( ((WOSHItem)this.items.elementAt(j)).label, null);
			this.choiceItems.setSelectedIndex(0, false);
		}
		return this.choiceItems;
	}
	
	public static void loadItemGroups(Vector groups)
	{
		try {
			SAXParserFactory factory = SAXParserFactory.newInstance();
			SAXParser saxParser = factory.newSAXParser();
			InputStream is = factory.getClass().getResourceAsStream("/com/opensmarthome/mobile/items.xml");
			InputSource inputSource = new InputSource(is);
			saxParser.parse(is,new itemXmlHandler(groups));
		}
		catch (Exception ex) { 
			System.out.println("exception" + ex.toString() );
		}
	}

};


class itemXmlHandler extends DefaultHandler
{
	private Vector data;
	private Stack tagStack = new Stack();

	public itemXmlHandler (Vector data) {
		this.data = data;
	}

	public void startDocument() throws SAXException {
		System.out.println("starting read");
	}

	public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException
	{
		if( qName.equals("ItemsGroup") ) {
			WOSHItemGroup itemsGroup = new WOSHItemGroup();
			this.data.addElement(itemsGroup);
//System.out.println("itemgroup found");
		}
		else if( qName.equals("Item") ) {
			WOSHItem item = new WOSHItem();
			((WOSHItemGroup)this.data.lastElement()).items.addElement(item);
//System.out.println("item found");
		}
		tagStack.push(qName);
	}

	public void characters(char[] ch, int start, int length) throws SAXException
	{
		String chars = new String(ch, start, length).trim();

		if ( chars.length() > 0 ) {
			String qName = (String)tagStack.peek();
			
			WOSHItemGroup currGroup = (WOSHItemGroup)this.data.lastElement();

			if ( qName.equals("title") )
				currGroup.label = chars;
			else if ( qName.equals("type") )
				currGroup.type = chars;
			else {
					WOSHItem currItem = (WOSHItem)currGroup.items.lastElement();

					if ( qName.equals("label") ) {
						currItem.label = chars;
//System.out.println("item found : " + currItem.label );
					}
					else if( qName.equals("dmtfcode") )
						currItem.dmtfcode = chars;
					else if( qName.equals("value") )
						currItem.value = chars;

			}
			
		}
	}

	public void endElement(String uri, String localName, String qName, Attributes attributes) throws SAXException
	{
		tagStack.pop();
	}

	public void endDocument() throws SAXException
	{
		System.out.println("doc compl");
	}
};




