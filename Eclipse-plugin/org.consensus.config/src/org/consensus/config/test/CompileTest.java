package org.consensus.config.test;

import org.consensus.config.handlers.CompileShell;
import org.consensus.config.handlers.ConfigShell;

public class CompileTest{
	public static void main(String[] args) {
		CompileShell compileshell= CompileShell.getCompileShell();
        
        
        while (!compileshell.shell.isDisposed()) {
            if (!compileshell.display.readAndDispatch()) {
            	compileshell.display.sleep();
            }
        }
        compileshell.display.dispose();
	}
}