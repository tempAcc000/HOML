package org.consensus.config.test;

import org.consensus.config.handlers.Solver;
import org.consensus.config.handlers.Translator;

public class SolveTest{
    public static void main(String[] args) {
        Solver s= new Solver();
        
        try {
			s.genScript();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
}