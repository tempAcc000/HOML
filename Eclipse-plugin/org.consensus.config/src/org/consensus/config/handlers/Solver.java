package org.consensus.config.handlers;


import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Arrays;

import org.consensus.config.handlers.Config;

public class Solver {
    Config config=Config.getConfig();
    Runtime run =Runtime.getRuntime();
    String HOfile_linux="";
	String OutputPath_linux="";
	
	public Solver(){
		String HOfile=config.getHO_file();
		String OutputPath=config.getOutput_path();
		String separator = "/|\\\\";
		for(String str : Arrays.asList(HOfile.split(separator))){
        	if(str.endsWith(":")) {
        		 if(str.charAt(0) == 'C')
        			 HOfile_linux+="/mnt/c";
        		 else if(str.charAt(0) == 'D')
        			 HOfile_linux+="/mnt/d";
        		 else if(str.charAt(0) == 'E')
        			 HOfile_linux+="/mnt/e";
        		 else if(str.charAt(0) == 'F')
        			 HOfile_linux+="/mnt/f";
        		 else if(str.charAt(0) == 'G')
        			 HOfile_linux+="/mnt/g";
        		 else if(str.charAt(0) == 'H')
        			 HOfile_linux+="/mnt/h";
        		 else if(str.charAt(0) == 'I')
        			 HOfile_linux+="/mnt/i";
        		 else if(str.charAt(0) == 'J')
        			 HOfile_linux+="/mnt/j";
        	 }
        	else {
        		HOfile_linux+="/";
        		HOfile_linux+=str;
        	}
        }
        for(String str : Arrays.asList(OutputPath.split(separator))){
        	 if(str.endsWith(":")) {
        		 if(str.charAt(0) == 'C')
        			 OutputPath_linux+="/mnt/c";
        		 else if(str.charAt(0) == 'D')
        			 OutputPath_linux+="/mnt/d";
        		 else if(str.charAt(0) == 'E')
        			 OutputPath_linux+="/mnt/e";
        		 else if(str.charAt(0) == 'F')
        			 OutputPath_linux+="/mnt/f";
        		 else if(str.charAt(0) == 'G')
        			 OutputPath_linux+="/mnt/g";
        		 else if(str.charAt(0) == 'H')
        			 OutputPath_linux+="/mnt/h";
        		 else if(str.charAt(0) == 'I')
        			 OutputPath_linux+="/mnt/i";
        		 else if(str.charAt(0) == 'J')
        			 OutputPath_linux+="/mnt/j";
        	 }
        	 else {
        		 OutputPath_linux+="/";
        		 OutputPath_linux+=str;
        	 }
        }
        OutputPath_linux+="/";
	}
    
    void gen_Z3(boolean isAgreement,boolean isUnivalence, boolean isInvariant, boolean isSMT, boolean isSMTBV, boolean isSAT,
    		int min, int max, String HOfile, String OutputPath, String Config_path) throws IOException {
        
    	File file = new File(Config_path+"\\\\solve.sh");
		if (file.exists()) file.delete();
		file.createNewFile();
		BufferedWriter output = new BufferedWriter(new FileWriter(file));
		output.write("#!/bin/bash\n\n");
		
		if(isSMT) {
			File a_file = new File(OutputPath+"\\\\SMT\\\\z3_a_result.txt");
			if (a_file.exists()) a_file.delete();
			a_file.createNewFile();
			File u_file = new File(OutputPath+"\\\\SMT\\\\z3_u_result.txt");
			if (u_file.exists()) u_file.delete();
			u_file.createNewFile();
			File i_file = new File(OutputPath+"\\\\SMT\\\\z3_i_result.txt");
			if (i_file.exists()) i_file.delete();
			i_file.createNewFile();
			for(int i=min;i<=max;i++) {
				if(isAgreement)
					output.write("z3 -T:1800 -st "+OutputPath_linux+"SMT/a"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT/z3_a_result.txt >> "+OutputPath_linux+"SMT/z3_a_result.txt\n");
				if(isUnivalence)
					output.write("z3 -T:1800 -st "+OutputPath_linux+"SMT/u"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT/z3_u_result.txt >> "+OutputPath_linux+"SMT/z3_u_result.txt\n");
				if(isInvariant)
					output.write("z3 -T:1800 -st "+OutputPath_linux+"SMT/i"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT/z3_i_result.txt >> "+OutputPath_linux+"SMT/z3_i_result.txt\n");
			}
		}
		else if(isSMTBV) {
			File a_file = new File(OutputPath+"\\\\SMT_BV\\\\z3_a_result.txt");
			if (a_file.exists()) a_file.delete();
			a_file.createNewFile();
			File u_file = new File(OutputPath+"\\\\SMT_BV\\\\z3_u_result.txt");
			if (u_file.exists()) u_file.delete();
			u_file.createNewFile();
			File i_file = new File(OutputPath+"\\\\SMT_BV\\\\z3_i_result.txt");
			if (i_file.exists()) i_file.delete();
			i_file.createNewFile();
			for(int i=min;i<=max;i++) {
				if(isAgreement)
					output.write("z3 -T:1800 -st "+OutputPath_linux+"SMT_BV/a"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT_BV/z3_a_result.txt >> "+OutputPath_linux+"SMT_BV/z3_a_result.txt\n");
				if(isUnivalence)
					output.write("z3 -T:1800 -st "+OutputPath_linux+"SMT_BV/u"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT_BV/z3_u_result.txt >> "+OutputPath_linux+"SMT_BV/z3_u_result.txt\n");
				if(isInvariant)
					output.write("z3 -T:1800 -st "+OutputPath_linux+"SMT_BV/i"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT_BV/z3_i_result.txt >> "+OutputPath_linux+"SMT_BV/z3_i_result.txt\n");
			}
		}
		output.close();
    }
    void gen_CVC4(boolean isAgreement,boolean isUnivalence, boolean isInvariant, boolean isSMT, boolean isSMTBV, boolean isSAT,
    		int min, int max, String HOfile, String OutputPath, String Config_path) throws IOException {
        
    	File file = new File(Config_path+"\\\\solve.sh");
		if (file.exists()) file.delete();
		file.createNewFile();
		BufferedWriter output = new BufferedWriter(new FileWriter(file));
		output.write("#!/bin/bash\n\n");
		
		if(isSMT) {
			File a_file = new File(OutputPath+"\\\\SMT\\\\cvc4_a_result.txt");
			if (a_file.exists()) a_file.delete();
			a_file.createNewFile();
			File u_file = new File(OutputPath+"\\\\SMT\\\\cvc4_u_result.txt");
			if (u_file.exists()) u_file.delete();
			u_file.createNewFile();
			File i_file = new File(OutputPath+"\\\\SMT\\\\cvc4_i_result.txt");
			if (i_file.exists()) i_file.delete();
			i_file.createNewFile();
			for(int i=min;i<=max;i++) {
				if(isAgreement)
					output.write("cvc4 --tlimit=1800000 --stats "+OutputPath_linux+"SMT/a"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT/cvc4_a_result.txt >> "+OutputPath_linux+"SMT/cvc4_a_result.txt\n");
				if(isUnivalence)
					output.write("cvc4 --tlimit=1800000 --stats "+OutputPath_linux+"SMT/u"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT/cvc4_u_result.txt >> "+OutputPath_linux+"SMT/cvc4_u_result.txt\n");
				if(isInvariant)
					output.write("cvc4 --tlimit=1800000 --stats "+OutputPath_linux+"SMT/i"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT/cvc4_i_result.txt >> "+OutputPath_linux+"SMT/cvc4_i_result.txt\n");
			}
		}
		else if(isSMTBV) {
			File a_file = new File(OutputPath+"\\\\SMT_BV\\\\cvc4_a_result.txt");
			if (a_file.exists()) a_file.delete();
			a_file.createNewFile();
			File u_file = new File(OutputPath+"\\\\SMT_BV\\\\cvc4_u_result.txt");
			if (u_file.exists()) u_file.delete();
			u_file.createNewFile();
			File i_file = new File(OutputPath+"\\\\SMT_BV\\\\cvc4_i_result.txt");
			if (i_file.exists()) i_file.delete();
			i_file.createNewFile();
			for(int i=min;i<=max;i++) {
				if(isAgreement)
					output.write("cvc4 --tlimit=1800000 --stats "+OutputPath_linux+"SMT_BV/a"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT_BV/cvc4_a_result.txt >> "+OutputPath_linux+"SMT_BV/cvc4_a_result.txt\n");
				if(isUnivalence)
					output.write("cvc4 --tlimit=1800000 --stats "+OutputPath_linux+"SMT_BV/u"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT_BV/cvc4_u_result.txt >> "+OutputPath_linux+"SMT_BV/cvc4_u_result.txt\n");
				if(isInvariant)
					output.write("cvc4 --tlimit=1800000 --stats "+OutputPath_linux+"SMT_BV/i"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT_BV/cvc4_i_result.txt >> "+OutputPath_linux+"SMT_BV/cvc4_i_result.txt\n");
			}
		}
		output.close();
    }
    
    void gen_Glu_seq(boolean isAgreement,boolean isUnivalence, boolean isInvariant, boolean isSMT, boolean isSMTBV, boolean isSAT,
    		int min, int max, String HOfile, String OutputPath, String Config_path) throws IOException {
        
    	File file = new File(Config_path+"\\\\solve.sh");
		if (file.exists()) file.delete();
		file.createNewFile();
		BufferedWriter output = new BufferedWriter(new FileWriter(file));
		output.write("#!/bin/bash\n\n");
		
		if(isSAT) {
			File a_file = new File(OutputPath+"\\\\SAT\\\\glucose_a_result.txt");
			if (a_file.exists()) a_file.delete();
			a_file.createNewFile();
			File u_file = new File(OutputPath+"\\\\SAT\\\\glucose_u_result.txt");
			if (u_file.exists()) u_file.delete();
			u_file.createNewFile();
			File i_file = new File(OutputPath+"\\\\SAT\\\\glucose_i_result.txt");
			if (i_file.exists()) i_file.delete();
			i_file.createNewFile();
			for(int i=min;i<=max;i++) {
				if(isAgreement)
					output.write("glucose -cpu-lim=1800 "+OutputPath_linux+"SAT/a"+String.valueOf(i)+".cnf 2>> "+OutputPath_linux+"SAT/glucose_a_result.txt >> "+OutputPath_linux+"SAT/glucose_a_result.txt\n");
				if(isUnivalence)
					output.write("glucose -cpu-lim=1800 "+OutputPath_linux+"SAT/u"+String.valueOf(i)+".cnf 2>> "+OutputPath_linux+"SAT/glucose_u_result.txt >> "+OutputPath_linux+"SAT/glucose_u_result.txt\n");
				if(isInvariant)
					output.write("glucose -cpu-lim=1800 "+OutputPath_linux+"SAT/i"+String.valueOf(i)+".cnf 2>> "+OutputPath_linux+"SAT/glucose_i_result.txt >> "+OutputPath_linux+"SAT/glucose_i_result.txt\n");
			}
		}
		output.close();
    }
    void gen_Mini_seq(boolean isAgreement,boolean isUnivalence, boolean isInvariant, boolean isSMT, boolean isSMTBV, boolean isSAT,
    		int min, int max, String HOfile, String OutputPath, String Config_path) throws IOException {
		
    	File file = new File(Config_path+"\\\\solve.sh");
		if (file.exists()) file.delete();
		file.createNewFile();	
		BufferedWriter output = new BufferedWriter(new FileWriter(file));
		output.write("#!/bin/bash\n\n");
		
		if(isSAT) {
			File a_file = new File(OutputPath+"\\\\SAT\\\\minisat_a_result.txt");
			if (a_file.exists()) a_file.delete();
			a_file.createNewFile();
			File u_file = new File(OutputPath+"\\\\SAT\\\\minisat_u_result.txt");
			if (u_file.exists()) u_file.delete();
			u_file.createNewFile();
			File i_file = new File(OutputPath+"\\\\SAT\\\\minisat_i_result.txt");
			if (i_file.exists()) i_file.delete();
			i_file.createNewFile();
			for(int i=min;i<=max;i++) {
				if(isAgreement)
					output.write("minisat -cpu-lim=1800 "+OutputPath_linux+"SAT/a"+String.valueOf(i)+".cnf 2>> "+OutputPath_linux+"SAT/minisat_a_result.txt >> "+OutputPath_linux+"SAT/minisat_a_result.txt\n");
				if(isUnivalence)
					output.write("minisat -cpu-lim=1800 "+OutputPath_linux+"SAT/u"+String.valueOf(i)+".cnf 2>> "+OutputPath_linux+"SAT/minisat_u_result.txt >> "+OutputPath_linux+"SAT/minisat_u_result.txt\n");
				if(isInvariant)
					output.write("minisat -cpu-lim=1800 "+OutputPath_linux+"SAT/i"+String.valueOf(i)+".cnf 2>> "+OutputPath_linux+"SAT/minisat_i_result.txt >> "+OutputPath_linux+"SAT/minisat_i_result.txt\n");
			}
		}
		output.close();
    }
    
    void gen_Alt(boolean isAgreement,boolean isUnivalence, boolean isInvariant, boolean isSMT, boolean isSMTBV, boolean isSAT,
    		int min, int max, String HOfile, String OutputPath, String Config_path) throws IOException {
        
    	File file = new File(Config_path+"\\\\solve.sh");
		if (file.exists()) file.delete();
		file.createNewFile();
		BufferedWriter output = new BufferedWriter(new FileWriter(file));
		output.write("#!/bin/bash\n\n");
		
		if(isSMT) {
			File a_file = new File(OutputPath+"\\\\SMT\\\\alt_a_result.txt");
			if (a_file.exists()) a_file.delete();
			a_file.createNewFile();
			File u_file = new File(OutputPath+"\\\\SMT\\\\alt_u_result.txt");
			if (u_file.exists()) u_file.delete();
			u_file.createNewFile();
			File i_file = new File(OutputPath+"\\\\SMT\\\\alt_i_result.txt");
			if (i_file.exists()) i_file.delete();
			i_file.createNewFile();
			for(int i=min;i<=max;i++) {
				if(isAgreement)
					output.write("alt-ergo -t 1800 "+OutputPath_linux+"SMT/a"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT/alt_a_result.txt >> "+OutputPath_linux+"SMT/alt_a_result.txt\n");
				if(isUnivalence)
					output.write("alt-ergo -t 1800 "+OutputPath_linux+"SMT/u"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT/alt_u_result.txt >> "+OutputPath_linux+"SMT/alt_u_result.txt\n");
				if(isInvariant)
					output.write("alt-ergo -t 1800 "+OutputPath_linux+"SMT/i"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT/alt_i_result.txt >> "+OutputPath_linux+"SMT/alt_i_result.txt\n");
			}
		}
		else if(isSMTBV) {
			File a_file = new File(OutputPath+"\\\\SMT_BV\\\\alt_a_result.txt");
			if (a_file.exists()) a_file.delete();
			a_file.createNewFile();
			File u_file = new File(OutputPath+"\\\\SMT_BV\\\\alt_u_result.txt");
			if (u_file.exists()) u_file.delete();
			u_file.createNewFile();
			File i_file = new File(OutputPath+"\\\\SMT_BV\\\\alt_i_result.txt");
			if (i_file.exists()) i_file.delete();
			i_file.createNewFile();
			for(int i=min;i<=max;i++) {
				if(isAgreement)
					output.write("alt-ergo -t 1800 "+OutputPath_linux+"SMT_BV/a"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT_BV/alt_a_result.txt >> "+OutputPath_linux+"SMT_BV/alt_a_result.txt\n");
				if(isUnivalence)
					output.write("alt-ergo -t 1800 "+OutputPath_linux+"SMT_BV/u"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT_BV/alt_u_result.txt >> "+OutputPath_linux+"SMT_BV/alt_u_result.txt\n");
				if(isInvariant)
					output.write("alt-ergo -t 1800 "+OutputPath_linux+"SMT_BV/i"+String.valueOf(i)+".smt2 2>> "+OutputPath_linux+"SMT_BV/alt_i_result.txt >> "+OutputPath_linux+"SMT_BV/alt_i_result.txt\n");
			}
		}
		output.close();
    }
    
    
    public void genScript() {
    	String mode=config.getSolver_mode();
    	String SMT_solver=config.getSMT_solver();
    	String SAT_solver=config.getSAT_solver();
    	try {
    		if(mode.equals("SMT")) {
    			if(SMT_solver.equals("Z3"))
    				gen_Z3(config.isGen_agreement(),config.isGen_univalence(),config.isGen_invariant(),config.isGen_SMT(),config.isGen_SMT_bv(),
    						config.isGen_SAT(),config.getNode_num_MIN(),config.getNode_num_MAX(),
    						config.getHO_file(),config.getOutput_path(),config.getConfig_path());
    			else if(SMT_solver.equals("CVC4"))
    				gen_CVC4(config.isGen_agreement(),config.isGen_univalence(),config.isGen_invariant(),config.isGen_SMT(),config.isGen_SMT_bv(),
    						config.isGen_SAT(),config.getNode_num_MIN(),config.getNode_num_MAX(),
    						config.getHO_file(),config.getOutput_path(),config.getConfig_path());
    			else
    				gen_Alt(config.isGen_agreement(),config.isGen_univalence(),config.isGen_invariant(),config.isGen_SMT(),config.isGen_SMT_bv(),
    						config.isGen_SAT(),config.getNode_num_MIN(),config.getNode_num_MAX(),
    						config.getHO_file(),config.getOutput_path(),config.getConfig_path());
    		}
    		else if(mode.equals("SAT-Serial")) {
    			if(SAT_solver.equals("Glucose"))
    				gen_Glu_seq(config.isGen_agreement(),config.isGen_univalence(),config.isGen_invariant(),config.isGen_SMT(),config.isGen_SMT_bv(),
    						config.isGen_SAT(),config.getNode_num_MIN(),config.getNode_num_MAX(),
    						config.getHO_file(),config.getOutput_path(),config.getConfig_path());
    			else if(SAT_solver.equals("Minisat"))
    				gen_Mini_seq(config.isGen_agreement(),config.isGen_univalence(),config.isGen_invariant(),config.isGen_SMT(),config.isGen_SMT_bv(),
    						config.isGen_SAT(),config.getNode_num_MIN(),config.getNode_num_MAX(),
    						config.getHO_file(),config.getOutput_path(),config.getConfig_path());
    		}
    			
    	}catch (Exception e) {
            e.printStackTrace();
    	}
    }
    public void run() {
    	String config_path=config.getConfig_path()+"\\\\solve.sh";
    	String script_path="";
    	for(String str : Arrays.asList(config_path.split("/|\\\\"))){
       	 if(str.endsWith(":")) {
       		 if(str.charAt(0) == 'C')
       			script_path+="/mnt/c";
       		 else if(str.charAt(0) == 'D')
       			script_path+="/mnt/d";
       		 else if(str.charAt(0) == 'E')
       			script_path+="/mnt/e";
       		 else if(str.charAt(0) == 'F')
       			script_path+="/mnt/f";
       		 else if(str.charAt(0) == 'G')
       			script_path+="/mnt/g";
       		 else if(str.charAt(0) == 'H')
       			script_path+="/mnt/h";
       		 else if(str.charAt(0) == 'I')
       			script_path+="/mnt/i";
       		 else if(str.charAt(0) == 'J')
       			script_path+="/mnt/j";
       	 }
       	 else {
       		script_path+="/";
       		script_path+=str;
       	 }
       }
    	try {
            Process p = run.exec("wsl bash "+script_path);
            InputStream ins= p.getInputStream();
            new Thread(new inputStreamThread(ins)).start();
            p.waitFor();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
    class inputStreamThread implements Runnable{
        private InputStream ins = null;
        private BufferedReader bfr = null;
        public inputStreamThread(InputStream ins){
            this.ins = ins;
            this.bfr = new BufferedReader(new InputStreamReader(ins));
        }
        @Override
        public void run() {
            String line = null;
            try {
                while((line = bfr.readLine()) != null){
                    System.out.println(line);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}