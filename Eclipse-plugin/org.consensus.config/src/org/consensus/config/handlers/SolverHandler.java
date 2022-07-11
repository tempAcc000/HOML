package org.consensus.config.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;

public class SolverHandler extends AbstractHandler {
	
	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		SolverShell solvershell= SolverShell.getSolverShell();
		Solver s= new Solver();
		
		s.genScript();
		s.run();
		solvershell.init();
		while (!solvershell.shell.isDisposed()) {
            if (!solvershell.display.readAndDispatch()) {
            	solvershell.display.sleep();
            }
        }
		solvershell.display.dispose();
        return null;
	}
}  
