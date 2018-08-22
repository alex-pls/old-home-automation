
package com.opensmarthome.mobile;

import javax.microedition.io.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import java.io.IOException;
import java.util.*;



public class DToneAbout {

    private static String copyright =
        "Copyright © 2008 www.OpenSmartHome.com  All rights reserved.\n" +
        "WOSH DialTone (DTFM) Client - part WOSH Project\n" +
        "Developed by Alessandro - Polo http://www.alessandropolo.name\n" +
        " TBD \n";


    private DToneClient parent;


	public DToneAbout(DToneClient parent) {
        this.parent = parent;
	}
	

    public void Show() {
        Alert alert = new Alert("About WOSH DialTone Client");
        alert.setTimeout(Alert.FOREVER);

        if (Display.getDisplay(this.parent).numColors() > 2) {
            String icon = (Display.getDisplay(this.parent).isColor()) ? "/com/opensmarthome/mobile/logo.jpg" : "/com/opensmarthome/mobile/logo.jpg";

            try {
                Image image = Image.createImage(icon);
                alert.setImage(image);
            } catch (java.io.IOException x) { }
        }

        alert.setString(copyright);
        Display.getDisplay(this.parent).setCurrent(alert);
    }
	
}
