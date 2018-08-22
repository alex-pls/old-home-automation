
package com.opensmarthome.mobile;

import javax.microedition.io.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import javax.microedition.rms.*;

import java.util.*;


public class DToneManual implements CommandListener {

    public DToneClient parent;

	// ------------------------------------
	
	// ------------------------------------
	
	Form formMan;

	TextField txtTones;
	Spacer spacer1;
	ChoiceGroup choiceBox;
	Spacer spacer2;
	TextField txtHeaderTag;
	TextField txtFooterTag;
	TextField txtTerminator;

	// ------------------------------------
	
    private final Command cmdApply = new Command("Apply", Command.OK, 0);
    private final Command cmdCancel = new Command("Cancel", Command.EXIT, 1);

	// ------------------------------------
	
    DToneManual(DToneClient parent) {
        this.parent = parent;

		this.formMan = new Form("WOSH DTone Manual");

		this.txtTones = new TextField("DTMF Sequence", null, 32, TextField.NUMERIC);
		this.spacer1 = new Spacer(16, 1);
		this.spacer2 = new Spacer(16, 1);
		this.txtHeaderTag = new TextField("DTMF Header", null, 32, TextField.NUMERIC);
		this.txtFooterTag = new TextField("DTMF Footer", null, 32, TextField.NUMERIC);
		this.txtTerminator = new TextField("DTMF Terminator", null, 32, TextField.ANY);

		this.choiceBox = new ChoiceGroup( "", Choice.MULTIPLE);
		this.choiceBox.append( "Send Header/Footer", null );
		this.choiceBox.append( "Send Terminator", null );
		this.choiceBox.setSelectedIndex( 0, true );
		this.choiceBox.setSelectedIndex( 1, true );

		this.formMan.append( this.txtTones );
		this.formMan.append( this.spacer1 );
		this.formMan.append( this.choiceBox );
		this.formMan.append( this.spacer2 );
		this.formMan.append( this.txtHeaderTag );
		this.formMan.append( this.txtFooterTag );
		this.formMan.append( this.txtTerminator );

		this.formMan.addCommand( this.cmdApply );
		this.formMan.addCommand( this.cmdCancel );

		this.formMan.setCommandListener(this);
    }
	

    public void Show() {
		this.txtHeaderTag.setString( DToneSettings.tonesHeader );
		this.txtFooterTag.setString( DToneSettings.tonesFooter );
		this.txtTerminator.setString( DToneSettings.tonesTerminator );

        Display.getDisplay(this.parent).setCurrent(this.formMan);
    }


    public void commandAction(Command c, Displayable s) {

		if ((c == cmdCancel)) {
			this.parent.ShowPrevious();
		}
		else if (c == cmdApply) {
			String url = DToneSettings.phoneNumber + DToneSettings.tonesTag;
			if ( this.choiceBox.isSelected( 0 ) )
				url += this.txtHeaderTag.getString();
			if ( this.choiceBox.isSelected( 0 ) )
				url += this.txtFooterTag.getString();

			url += this.txtTones.getString();

			if ( this.choiceBox.isSelected( 1 ) )
				url += this.txtTerminator.getString();

			this.parent.doDial(url);
		
		}


    }



};
