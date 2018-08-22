
package com.opensmarthome.mobile;

import javax.microedition.io.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

import java.io.IOException;


public class DToneClient extends MIDlet {


	public WOSHAction currCommand = null;

	// ------------------------------------
		
	private DToneSelector ctrlCommand = null;
	private DToneAbout ctrlAbout = null;
	private DToneManual ctrlManual = null;
	private DToneCommandInfo ctrlCmdInfo = null;

	public DToneSettings ctrlOptions = null;
	
	Displayable previousDisplayable = null;
	
//	private DToneSettings ctrlOptions = null;
	
	// ------------------------------------
	

    public DToneClient() {

		DToneSettings.phoneNumber = WOSHSecrets.phoneNumber;
		DToneSettings.tonesHeader = WOSHSecrets.tonesHeader;
		DToneSettings.tonesFooter = WOSHSecrets.tonesFooter;

//		DToneSettings.phoneNumber = getAppProperty("WOSH-PhoneNumber");
//		DToneSettings.tonesTag = getAppProperty("Dial-PauseString");

	}



	// ------------------------------------
	
    public void startApp() {
		this.ctrlOptions = new DToneSettings(this);
		this.ctrlOptions.loadData();
		this.ctrlOptions.loadCurrentState();
		if ( this.ctrlCommand == null )
			this.ctrlCommand = new DToneSelector(this);

		showCommands();
    }

    public void pauseApp() {
		this.ctrlOptions.saveCurrentState();
    }

    public void destroyApp(boolean unconditional) {
		this.ctrlOptions.saveCurrentState();
		notifyDestroyed();
    }

	// ------------------------------------
		
	public void ShowPrevious() {
		if ( this.previousDisplayable == null )
			showCommands();
		else {
			Display.getDisplay(this).setCurrent( this.previousDisplayable );
		}
	}

	// ------------------------------------
	public void showCmdInfo(WOSHAction currCommand) {
		if ( this.ctrlCmdInfo == null )
			this.ctrlCmdInfo = new DToneCommandInfo(this);
		this.ctrlCmdInfo.Show(currCommand);
	}

	public void showHelp() {
		if ( this.ctrlAbout == null )
			this.ctrlAbout = new DToneAbout(this);
		this.ctrlAbout.Show();
	}

	public void showManual() {
		if ( this.ctrlManual == null )
			this.ctrlManual = new DToneManual(this);
		this.ctrlManual.Show();
	}

	public void showCommands() {
		if ( this.ctrlCommand == null )
			this.ctrlCommand = new DToneSelector(this);
		this.ctrlCommand.Show();
	}

	public void showOptions() {
		if ( this.ctrlOptions == null )
			this.ctrlOptions = new DToneSettings(this);
		this.ctrlOptions.Show();
	}

    public void appliedChanges() {
		showCommands();
		Alert alert = new Alert("Setting applied..", null, null, AlertType.INFO);
		alert.setTimeout(1000);
		Display.getDisplay(this).setCurrent(alert);
	}

	// ------------------------------------
	// ------------------------------------

	public void doAction() {

		String codedValue = DToneEncoder.encodeAction2Digits( this.currCommand );

		System.out.println("DTMF Sequence: " + codedValue );
		if ( doDial(DToneSettings.phoneNumber + DToneSettings.tonesTag + codedValue + DToneSettings.tonesTerminator ) ) {
			if ( DToneSettings.closeOnDial )
				destroyApp(true);
			else
				pauseApp();
		}
	}


	public void doDialTest() {
		
		String url = DToneSettings.phoneNumber + DToneSettings.tonesTag + "123456789123456789123456789" + DToneSettings.tonesTerminator;
		
		if ( doDial(url) ) {
			
			Alert alert = new Alert("Dialing Initiated..", url, null, AlertType.INFO);
			alert.setTimeout(1000);
			Display.getDisplay(this).setCurrent(alert);
			
			showCommands();
		}
	}



	public boolean doDial(String url) {
		try {
			platformRequest("tel:" + url );
			resumeRequest(); 
			return true;
		} catch (Exception ex) {
			ex.printStackTrace();
			Alert alert = new Alert("Dialing Failed", url, null, AlertType.ERROR);
			alert.setTimeout(5000);
			Display.getDisplay(this).setCurrent(alert);
		}
		return false;
	}

}

