/*
		A basic implementation of the DeviceSetup class.
*/

import java.awt.*;

public class PELLETSetup extends DeviceSetup
{
	public PELLETSetup(Frame parent)
	{
		super(parent);
		
		// This code is automatically generated by Visual Cafe when you add
		// components to the visual environment. It instantiates and initializes
		// the components. To modify the code, only use code syntax that matches
		// what Visual Cafe can generate, or Visual Cafe may be unable to back
		// parse your Java file into its visual environment.
		//{{INIT_CONTROLS
		setDeviceTitle("Pellet device");
		getContentPane().setLayout(null);
		setSize(405,257);
		deviceField1.setNumCols(25);
		deviceField1.setTextOnly(true);
		deviceField1.setOffsetNid(2);
		deviceField1.setLabelString("Comment:");
		getContentPane().add(deviceField1);
		deviceField1.setBounds(6,19,365,30);
		deviceDispatch1.setBounds(249,51,131,31);
		deviceField3.setOffsetNid(25);
		deviceField3.setLabelString("Pellet T2");
		getContentPane().add(deviceField3);
		deviceField3.setBounds(25,116,169,23);
		deviceField4.setOffsetNid(39);
		deviceField4.setLabelString("Pellet T3");
		getContentPane().add(deviceField4);
		deviceField4.setBounds(25,142,169,23);
		deviceField5.setOffsetNid(53);
		deviceField5.setLabelString("Pellet T4");
		getContentPane().add(deviceField5);
		deviceField5.setBounds(25,168,169,23);
		deviceField6.setOffsetNid(67);
		deviceField6.setLabelString("Pellet T5");
		getContentPane().add(deviceField6);
		deviceField6.setBounds(205,90,169,23);
		deviceField7.setOffsetNid(81);
		deviceField7.setLabelString("Pellet T6");
		getContentPane().add(deviceField7);
		deviceField7.setBounds(205,116,169,23);
		deviceField8.setOffsetNid(11);
		deviceField8.setLabelString("Pellet T1");
		getContentPane().add(deviceField8);
		deviceField8.setBounds(25,90,169,23);
		deviceField9.setOffsetNid(95);
		deviceField9.setLabelString("Pellet T7");
		getContentPane().add(deviceField9);
		deviceField9.setBounds(205,142,169,23);
		deviceField10.setOffsetNid(109);
		deviceField10.setLabelString("Pellet T8");
		getContentPane().add(deviceField10);
		deviceField10.setBounds(205,168,169,23);
		{
			String[] tempString = new String[1];
			tempString[0] = "store";
			deviceButtons1.setMethods(tempString);
		}
		getContentPane().add(deviceButtons1);
		deviceButtons1.setBounds(71,207,281,40);
		deviceField2.setNumCols(15);
		deviceField2.setTextOnly(true);
		deviceField2.setOffsetNid(1);
		deviceField2.setLabelString("RS232: ");
		getContentPane().add(deviceField2);
		deviceField2.setBounds(10,51,236,30);
		getContentPane().add(deviceDispatch1);
		//}}
	}

	public PELLETSetup()
	{
		this((Frame)null);
	}

	public PELLETSetup(String sTitle)
	{
		this();
		setTitle(sTitle);
	}

	public void setVisible(boolean b)
	{
		if (b)
			setLocation(50, 50);
		super.setVisible(b);
	}

	static public void main(String args[])
	{
		(new PELLETSetup()).setVisible(true);
	}

	public void addNotify()
	{
		// Record the size of the window prior to calling parents addNotify.
		Dimension size = getSize();

		super.addNotify();

		if (frameSizeAdjusted)
			return;
		frameSizeAdjusted = true;

		// Adjust size of frame according to the insets
		Insets insets = getInsets();
		setSize(insets.left + insets.right + size.width, insets.top + insets.bottom + size.height);
	}

	// Used by addNotify
	boolean frameSizeAdjusted = false;

	//{{DECLARE_CONTROLS
	DeviceField deviceField1 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceField deviceField9 = new DeviceField();
	DeviceField deviceField10 = new DeviceField();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	//}}

}