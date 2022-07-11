package org.consensus.config.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;

public class CompileHandler extends AbstractHandler {
	
	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		CompileShell compileshell= CompileShell.getCompileShell();
		Translator t=new Translator();
		
		t.genScript();
		t.run();
		compileshell.init();
		while (!compileshell.shell.isDisposed()) {
            if (!compileshell.display.readAndDispatch()) {
            	compileshell.display.sleep();
            }
        }
        compileshell.display.dispose();
        return null;
	}
}  
