package org.consensus.config.test;

import org.consensus.config.handlers.ConfigShell;

public class ConfigurationTest{
	public static void main(String[] args) {
		ConfigShell configshell= ConfigShell.getConfigShell();
		
		configshell.init();
		configshell.set_basic();
		configshell.set_compiler();
		configshell.set_solver();
		configshell.set_button();
		configshell.refresh();
        
        
		configshell.open();
        
        
        while (!configshell.shell.isDisposed()) {
            if (!configshell.display.readAndDispatch()) {
            	configshell.display.sleep();
            }
        }
        configshell.display.dispose();
	}
}