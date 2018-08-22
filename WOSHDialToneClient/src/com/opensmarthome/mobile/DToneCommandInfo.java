
package com.opensmarthome.mobile;

import javax.microedition.io.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import java.io.IOException;
import java.util.*;



public class DToneCommandInfo {

    private DToneClient parent;


	public DToneCommandInfo(DToneClient parent) {
        this.parent = parent;
	}
	

    public void Show(WOSHAction currCommand) {
        Alert alert = new Alert("About " + currCommand.label);
        alert.setTimeout(Alert.FOREVER);
		
		String info = "";
		info += "" + currCommand.label;
		info += "\n";
		info += "\nDMTF code: " + currCommand.dmtfcode;
		info += "\nDescription:\n" + currCommand.description;
		info += "\n";
		info += "\nMultiple Arg: ";
		if ( currCommand.allowMultipleArg ) 
			info += "Yes";
		else
			info += "No";

		info += "\nRequired Arg:\n" + currCommand.requiredArgTypes.toString();

        alert.setString(info);
        Display.getDisplay(this.parent).setCurrent(alert);
    }
	
}
