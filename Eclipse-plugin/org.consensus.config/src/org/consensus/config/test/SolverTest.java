package org.consensus.config.test;

import org.consensus.config.handlers.CompileShell;
import org.consensus.config.handlers.ConfigShell;
import org.consensus.config.handlers.SolverShell;

public class SolverTest{
	public static void main(String[] args) {
		SolverShell solvershell= SolverShell.getSolverShell();

		solvershell.init();
		while (!solvershell.shell.isDisposed()) {
            if (!solvershell.display.readAndDispatch()) {
            	solvershell.display.sleep();
            }
        }
		solvershell.display.dispose();
	}
}