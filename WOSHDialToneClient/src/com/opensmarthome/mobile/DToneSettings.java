
package com.opensmarthome.mobile;

import javax.microedition.io.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import javax.microedition.rms.*;

import java.util.*;


public class DToneSettings implements CommandListener {

    public DToneClient parent;

	// ------------------------------------

	public static String phoneNumber = "";
	public static String tonesTag = ";postd=";
	public static boolean closeOnDial = true;
	public static boolean showTones = false;
	public static String tonesHeader = "";
	public static String tonesFooter = "";
	public static String tonesTerminator = "#";

	public static final String tonesCmdSep = "0";
	public static final String tonesArgSep = "0";

    public Vector actions = null;
    public Vector arguments = null;
	
	// ------------------------------------
	
	Form formOpt;

	TextField txtPhoneNumber;
	Spacer spacer1;
	TextField txtPauseTag;
	Spacer spacer2;
	TextField txtHeaderTag;
	TextField txtFooterTag;
	Spacer spacer3;
	ChoiceGroup choiceBox;
	
	// ------------------------------------
	
    private final Command cmdApply = new Command("Apply", Command.ITEM, 0);
    private final Command cmdCancel = new Command("Cancel", Command.ITEM, 1);
    private final Command cmdHelp = new Command("Help", Command.HELP, 4);

	// ------------------------------------
	
    DToneSettings(DToneClient parent) {
        this.parent = parent;
        this.actions = new Vector();
        this.arguments = new Vector();

		this.formOpt = new Form("WOSH DTone Settings");

		this.txtPhoneNumber = new TextField("WOSH Phone Number", null, 32, TextField.PHONENUMBER);
		this.spacer1 = new Spacer(16, 1);
		this.txtPauseTag = new TextField("Dialing pause tag", null, 32, TextField.ANY & TextField.NON_PREDICTIVE);
		this.spacer2 = new Spacer(16, 1);
		this.txtHeaderTag = new TextField("DTMF Header", null, 32, TextField.NUMERIC);
		this.txtFooterTag = new TextField("DTMF Footer", null, 32, TextField.NUMERIC);
		this.spacer3 = new Spacer(16, 1);

	
		this.choiceBox = new ChoiceGroup( "", Choice.MULTIPLE);
		this.choiceBox.append( "Close After Dial", null );
		this.choiceBox.append( "Show DTMF Tones", null );
		
		this.formOpt.append( this.txtPhoneNumber );
		this.formOpt.append( this.spacer1 );
		this.formOpt.append( this.txtPauseTag );
		this.formOpt.append( this.spacer2 );
		this.formOpt.append( this.choiceBox );
		this.formOpt.append( this.spacer3 );
		this.formOpt.append( this.txtHeaderTag );
		this.formOpt.append( this.txtFooterTag );


		this.formOpt.addCommand( this.cmdApply );
		this.formOpt.addCommand( this.cmdCancel );
		this.formOpt.addCommand( this.cmdHelp );

		this.formOpt.setCommandListener(this);
    }
	

    public void Show() {
		this.txtPhoneNumber.setString( DToneSettings.phoneNumber );
		this.txtPauseTag.setString( DToneSettings.tonesTag );
		this.choiceBox.setSelectedIndex( 0, DToneSettings.closeOnDial );
		this.choiceBox.setSelectedIndex( 1, DToneSettings.showTones );
		this.txtHeaderTag.setString( DToneSettings.tonesHeader );
		this.txtFooterTag.setString( DToneSettings.tonesFooter );
		
        Display.getDisplay(this.parent).setCurrent(this.formOpt);
    }


    public void commandAction(Command c, Displayable s) {

		if ((c == cmdCancel)) {
			this.parent.ShowPrevious();
		}
		else if (c == cmdHelp) {
			this.parent.showHelp();
		}
		else if (c == cmdApply) {

			boolean edited = ( this.txtPhoneNumber.getString() != DToneSettings.phoneNumber ) | ( this.txtPauseTag.getString() != DToneSettings.tonesTag );
			
			this.parent.ShowPrevious();
			
			if ( edited )
				applyChanges();
		}


    }


    public void applyChanges() {

		DToneSettings.phoneNumber = this.txtPhoneNumber.getString();
		DToneSettings.tonesTag = this.txtPauseTag.getString();
		DToneSettings.closeOnDial = this.choiceBox.isSelected( 0 );
		DToneSettings.showTones = this.choiceBox.isSelected( 1 );
		DToneSettings.tonesHeader = this.txtHeaderTag.getString();
		DToneSettings.tonesFooter = this.txtFooterTag.getString();

		saveCurrentState();
		
		this.parent.appliedChanges();
	}


	public boolean saveCurrentState()
	{
		RecordStore myStore = null;
		try {
			byte[] phoneNum = DToneSettings.phoneNumber.getBytes();
			byte[] toneTag = DToneSettings.tonesTag.getBytes();
			byte[] closeTag = (DToneSettings.closeOnDial?"true":"false").getBytes();
			byte[] showTag = (DToneSettings.showTones?"true":"false").getBytes();
			byte[] headerTag = DToneSettings.tonesHeader.getBytes();
			byte[] footerTag = DToneSettings.tonesFooter.getBytes();

			myStore = RecordStore.openRecordStore("WOSHDialToneClient", true);
			int n = myStore.getNumRecords();
			for(int h=1; h<n+1; h++)
				myStore.deleteRecord(h);

//			if (n != 5) {

				myStore.addRecord(phoneNum, 0, phoneNum.length);
				myStore.addRecord(toneTag, 0, toneTag.length);
				myStore.addRecord(closeTag, 0, closeTag.length);
				myStore.addRecord(showTag, 0, showTag.length);
				myStore.addRecord(headerTag, 0, headerTag.length);
				myStore.addRecord(footerTag, 0, footerTag.length);
/*			}
			else
			{
				myStore.setRecord(1, phoneNum, 0, phoneNum.length);
				myStore.setRecord(2, toneTag, 0, toneTag.length);
				myStore.setRecord(3, closeTag, 0, closeTag.length);
				myStore.setRecord(4, headerTag, 0, headerTag.length);
				myStore.setRecord(5, footerTag, 0, footerTag.length);
			}
*/
		}
		catch(Exception ex) {
			System.out.println("exception" + ex.toString() );
			return false;
		}
		finally {
			if (myStore != null) {
				try { myStore.closeRecordStore(); }
				catch(Exception ex) {	// ignore
					System.out.println("exception" + ex.toString() );
					}
			}
		}
		return true;
	}

	public boolean loadCurrentState()
	{
		RecordStore myStore = null;
		try {
			myStore = RecordStore.openRecordStore("WOSHDialToneClient", true);
			int n = myStore.getNumRecords();
			if (n == 0) {
				myStore.closeRecordStore();
				return saveCurrentState();
			}
			else {
				byte b1[] = myStore.getRecord(1);
				byte b2[] = myStore.getRecord(2);
				byte b3[] = myStore.getRecord(3);
				byte b4[] = myStore.getRecord(4);
				byte b5[] = myStore.getRecord(5);
				byte b6[] = myStore.getRecord(6);

				DToneSettings.phoneNumber = new String(b1,0,b1.length);
				DToneSettings.tonesTag = new String(b2,0,b2.length);
				DToneSettings.closeOnDial = (boolean)( (new String(b3,0,b3.length) == "true") ?true:false);
				DToneSettings.showTones = (boolean)( (new String(b4,0,b4.length) == "true") ?true:false);
				DToneSettings.tonesHeader = new String(b5,0,b5.length);
				DToneSettings.tonesFooter = new String(b6,0,b6.length);

System.out.println("DToneSettings.phoneNumber = " + DToneSettings.phoneNumber );
System.out.println("DToneSettings.tonesTag = " + DToneSettings.tonesTag );
//System.out.println(" = " +  );
System.out.println("DToneSettings.tonesHeader = " + DToneSettings.tonesHeader );
System.out.println("DToneSettings.tonesFooter = " + DToneSettings.tonesFooter );

			}
		}
		catch(Exception ex) {
			System.out.println("exception" + ex.toString() );
			return false;
		}
		finally {
			if (myStore != null) {
				try { myStore.closeRecordStore(); }
				catch(Exception ex) { // ignore
					System.out.println("exception" + ex.toString() );
					}
			}
		}
		return true;
	}

	public void loadData() {

		WOSHAction.loadActions(this.actions);
		WOSHItemGroup.loadItemGroups(this.arguments);

	}


};
