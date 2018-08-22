
package com.opensmarthome.mobile;

import javax.microedition.io.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

import java.io.IOException;
import java.util.*;



public class DToneSelector implements CommandListener {

    private DToneClient parent = null;

	// ------------------------------------

//	private Vector commands = null;
//    private Vector arguments = null;

	// ------------------------------------

	public int lastCommandIndex = 0;
	public WOSHAction runningCommand = null; // != null when selecting its argument!
//	public int lastCommandIndex = 0;

	// ------------------------------------

	List listCmd = null;
	Ticker tickerCmd = null;

	// ------------------------------------

	Form formArg = null;
	Ticker tickerArg = null;

	// ------------------------------------
	
    private final Command cmdNext = new Command("Next", Command.ITEM, 0);
    private final Command cmdOptions = new Command("Options", Command.SCREEN, 3);
    private final Command cmdDialManual = new Command("Manual", Command.SCREEN, 5);
    private final Command cmdDialTest = new Command("Dial Test", Command.SCREEN, 6);
    private final Command cmdAbout = new Command("About", Command.SCREEN, 4);
    private final Command cmdExit = new Command("Exit", Command.EXIT, 2);

    private final Command cmdExec = new Command("Execute", Command.OK, 0);
    private final Command cmdBack = new Command("Back", Command.EXIT, 0);
    private final Command cmdCmdInfo = new Command("Info", Command.SCREEN, 0);

	// ------------------------------------
	
    DToneSelector(DToneClient parent) {
        this.parent = parent;

//		this.arguments = parent.ctrlOptions.arguments;

		this.listCmd = new List("WOSH Action", List.IMPLICIT);
		this.listCmd.setSelectCommand( this.cmdNext );
		this.listCmd.addCommand( this.cmdOptions );
		this.listCmd.addCommand( this.cmdAbout );
		this.listCmd.addCommand( this.cmdDialTest );
		this.listCmd.addCommand( this.cmdDialManual );
		this.listCmd.addCommand( this.cmdExit );
		this.listCmd.setCommandListener(this);

		this.formArg = new Form("WOSH Action");
		this.formArg.addCommand( this.cmdExec );
		this.formArg.addCommand( this.cmdBack );
		this.formArg.addCommand( this.cmdCmdInfo );
		this.formArg.setCommandListener(this);

		// ------------------------------------
		for(int i=0;i<this.parent.ctrlOptions.actions.size();i++) {
			WOSHAction act = (WOSHAction)this.parent.ctrlOptions.actions.elementAt(i);
			Image image = null;
			try {
				image = Image.createImage("/icons/icon."+act.name+".png");
			} catch (java.io.IOException ex) { 
				System.out.println("exception" + ex.toString() );
			}

			this.listCmd.append( act.label, image );

		}
		
		if ( this.lastCommandIndex < this.listCmd.size() )
			this.listCmd.setSelectedIndex( this.lastCommandIndex, true);
    }
	

    public void Show() {
		this.runningCommand = null;
        Display.getDisplay(this.parent).setCurrent(this.listCmd);
    }


    public void commandAction(Command c, Displayable s) {

		if ((c == cmdExit)) {
			this.parent.destroyApp(true);
		}
		else if (c == cmdOptions) {
			this.parent.showOptions();
		}
		else if (c == cmdAbout) {
			this.parent.showHelp();
		}
		else if (c == cmdDialTest) {
			this.parent.doDialTest();
		}
		else if (c == cmdDialManual) {
			this.parent.showManual();
		}
		else if (c == cmdCmdInfo) {
			this.parent.showCmdInfo(this.runningCommand);
		}
		else if (c == cmdBack) {
			this.runningCommand = null;
			Show();
		}
		else if ( (c == cmdExec) || (c == cmdNext) ) {
			this.lastCommandIndex = this.listCmd.getSelectedIndex();
			if ( this.lastCommandIndex == -1 ) {
		        Alert alert = new Alert("Please select a command");
				alert.setTimeout(3000);
				Display.getDisplay(this.parent).setCurrent(alert);
			}
			else {
				if ( this.runningCommand == null ) {
					this.runningCommand = (WOSHAction)this.parent.ctrlOptions.actions.elementAt(this.lastCommandIndex);
					if ( this.runningCommand.requiredArgTypes.size() == 0 ) {
						this.parent.currCommand = this.runningCommand;
						parent.doAction();
					}
					else
						promptArguments();
				}
				else {
					int selCount = 0;
					try {
						this.runningCommand.currentArg.removeAllElements();
						for(int h=0; h<this.formArg.size(); h++) {
							ChoiceGroup cg = (ChoiceGroup) this.formArg.get(h);
							WOSHItemGroup wig = (WOSHItemGroup)this.parent.ctrlOptions.arguments.elementAt(h);
//System.out.println("h" + h + " WIG=" + wig.label + " || CG=" + cg.getLabel() );
							for(int k=0; k<cg.size(); k++) {
//System.out.println("k" + k + " Witem=" + ((WOSHItem)(wig.items.elementAt(k))).label  );
								if ( cg.isSelected(k) ) {
									this.runningCommand.currentArg.addElement( (WOSHItem)(wig.items.elementAt(k)) );
									selCount++;
								}
							}
						}
					} catch (Exception ex) { 
						System.out.println("exception" + ex.toString() );
					}

for(int h=0; h<this.runningCommand.currentArg.size(); h++) 
	System.out.println("=>h" + h + " item=" + ((WOSHItem)this.runningCommand.currentArg.elementAt(h)).label  );

					if ( selCount == 0 ) {
						Alert alert = new Alert("Please select an item");
						alert.setTimeout(2000);
						Display.getDisplay(this.parent).setCurrent(alert);
						return;
					}
					
					this.parent.currCommand = this.runningCommand;
					parent.doAction();

				}
			}
		}

    }


	

	private void promptArguments() {

		if ( this.runningCommand.description != "" )
			this.formArg.setTicker( new Ticker(this.runningCommand.description) );
		else
			this.formArg.setTicker( null );

		this.formArg.setTitle("WOSH Action:" + this.runningCommand.label );
		this.formArg.deleteAll();
		
		for(int i=0; i<this.parent.ctrlOptions.arguments.size(); i++) { // seach right type
			for(int h=0; h<this.runningCommand.requiredArgTypes.size(); h++) { // for each group
				if ( ((WOSHItemGroup)this.parent.ctrlOptions.arguments.elementAt(i)).type.equals( (String)this.runningCommand.requiredArgTypes.elementAt(h) ) ) {
					this.formArg.append( ((WOSHItemGroup)this.parent.ctrlOptions.arguments.elementAt(i)).getChoiceGroup() );
				}
			}
		}
/*		List listCmd = new List("", List.IMPLICIT);
		listCmd.setSelectCommand( this.cmdCmdInfo );
		listCmd.addCommand( this.cmdCmdInfo );
		listCmd.setCommandListener(this);
		this.formArg.append(listCmd);
*/

		Display.getDisplay(this.parent).setCurrent(this.formArg);
	}


	
};

