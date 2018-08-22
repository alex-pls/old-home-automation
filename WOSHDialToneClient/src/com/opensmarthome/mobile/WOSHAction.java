
package com.opensmarthome.mobile;

import javax.microedition.io.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

import java.util.*;
import java.io.*;
import javax.xml.parsers.*;
import org.xml.sax.*;
import org.xml.sax.helpers.*;




final class WOSHAction {

	public String name = "";
	public String label = "";
	public String dmtfcode = "";
	public String description = "";

	public Vector requiredArgTypes = new Vector();
	public boolean allowMultipleArg = false;

	public Vector currentArg = new Vector();


	WOSHAction() { }

	WOSHAction( String lbl, String cmd)
	{
		this.label = lbl;
		this.dmtfcode = cmd;
	}

	WOSHAction( String lbl, String cmd, Vector requiredTypes )
	{
		this.label = lbl;
		this.dmtfcode = cmd;

		this.requiredArgTypes = null;
		this.requiredArgTypes = requiredTypes;
	}

	WOSHAction( String lbl, String cmd, Vector requiredTypes, String desc)
	{
		this.label = lbl;
		this.dmtfcode = cmd;
		this.requiredArgTypes = null;
		this.requiredArgTypes = requiredTypes;
		this.description = desc;
	}

	public static void loadActions(Vector actions)
	{
		try {
			SAXParserFactory factory = SAXParserFactory.newInstance();
			SAXParser saxParser = factory.newSAXParser();
			InputStream is = factory.getClass().getResourceAsStream("/com/opensmarthome/mobile/actions.xml");
			InputSource inputSource = new InputSource(is);
			saxParser.parse(is,new actionXmlHandler(actions));
		}
		catch (Exception ex) { 
			System.out.println("exception" + ex.toString() );
		}
	}

};





class actionXmlHandler extends DefaultHandler
{
	private Vector data;
	private Stack tagStack = new Stack();

	public actionXmlHandler (Vector data) {
		this.data = data;
	}

	public void startDocument() throws SAXException {	}

	public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException
	{
		if( qName.equals("Action") ) {
			WOSHAction action = new WOSHAction();
			this.data.addElement(action);
		}

		tagStack.push(qName);
	}

	public void characters(char[] ch, int start, int length) throws SAXException
	{
		String chars = new String(ch, start, length).trim();

		if ( chars.length() > 0 ) {
			String qName = (String)tagStack.peek();
			WOSHAction currAction = (WOSHAction)data.lastElement();
			if ( qName.equals("label") )
				currAction.label = chars;
			else if( qName.equals("name") )
				currAction.name = chars;
			else if( qName.equals("dmtfcode") )
				currAction.dmtfcode = chars;
			else if( qName.equals("description") )
				currAction.description = chars;
			else if( qName.equals("argtype") )
				currAction.requiredArgTypes.addElement( chars );
			else if( qName.equals("argmultiple") ) {
				if ( chars == "true" )
					currAction.allowMultipleArg = true;
				else
					currAction.allowMultipleArg = false;
			}
		}
	}

	public void endElement(String uri, String localName, String qName, Attributes attributes) throws SAXException
	{
		tagStack.pop();
	}

	public void endDocument() throws SAXException {	}

	
};


