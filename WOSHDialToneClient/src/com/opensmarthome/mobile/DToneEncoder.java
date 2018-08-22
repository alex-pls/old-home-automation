
package com.opensmarthome.mobile;

import java.io.IOException;
import java.util.*;


public final class DToneEncoder {


	private String genPassCode()
	{
		String value = "";
		
		
		return value;
	}
	

	DToneEncoder() { }

	public static String encodeAction2Digits(WOSHAction action) {
		if ( action == null )
			return "00";

		String stringValue = DToneSettings.tonesHeader + action.dmtfcode + DToneSettings.tonesCmdSep;

		if ( action.currentArg != null ) {
			for(int h=0; h<action.currentArg.size(); h++) {
				if ( action.currentArg.elementAt(h) == null )
					continue;
				WOSHItem item = (WOSHItem)action.currentArg.elementAt(h);

				stringValue += item.dmtfcode;
				if ( h < action.currentArg.size()-1 )
					stringValue += DToneSettings.tonesArgSep;
			}
		}

		stringValue += DToneSettings.tonesFooter;
		return String2Digits(stringValue);
	}
	
	public static String String2Digits(String stringValue) {
		String digitValue;
/*
		bytes[] b = stringValue.getBytes();

		for (int j=0; j<b.length; j++) {
			for (int i=0; i<8; i++) {
				res[i] = (b[j] & (0x01 << i)) >> i;
			}
		}
*/		
		digitValue = stringValue;

		return digitValue;
	}
	
	
	public static String Digits2String(String digitValue) {
		String stringValue;

		stringValue = digitValue;

		return stringValue;
	}



}

