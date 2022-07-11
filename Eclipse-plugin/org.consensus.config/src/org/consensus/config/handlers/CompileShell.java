package org.consensus.config.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.events.VerifyEvent;
import org.eclipse.swt.events.VerifyListener;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.consensus.config.handlers.Config;

public class CompileShell {
	private static CompileShell instance; 
	public Display display;
	public Shell shell;
	public MessageBox success_message;
	Config config;

	
	
	public void init() {
		display = Display.getDefault();
		shell = new Shell(display);
		config=Config.getConfig();
		success_message= new MessageBox(shell, SWT.ICON_SEARCH);
		success_message.setText("Success");
		success_message.setMessage("Generate SMT/SAT Success!");
		success_message.open();
		
	}
	
	public static synchronized CompileShell getCompileShell () {
		if(instance == null)
			instance= new CompileShell();
		return instance;
	}

	public void open() {
		shell.open();
		
	}
}  
