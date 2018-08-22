
package com.opensmarthome.mobile;

import java.io.IOException;
import java.util.*;



public final class WOSHItem {

	public String label = "";
	public String dmtfcode = "";
	public String value = "";

	WOSHItem()	{ }

	WOSHItem( String lbl, String cod)
	{
		this.label = lbl;
		this.dmtfcode = cod;
	}

}
