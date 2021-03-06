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

public class Translator {
    Config config=Config.getConfig();
    Runtime run =Runtime.getRuntime();
    
    public void gen(boolean isAgreement,boolean isUnivalence, boolean isInvariant, boolean isSMT, boolean isSMTBV, boolean isSAT,
    		int min, int max, String HOfile, String OutputPath, String Config_path) throws Exception {
    	String separator = "/|\\\\";
    	String HOfile_linux="";
    	String OutputPath_linux="";
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
         
         
    	File file = new File(Config_path+"/gen.sh");
		if (file.exists()) file.delete();
		file.createNewFile();
		

		BufferedWriter output = new BufferedWriter(new FileWriter(file));
		output.write("#!/bin/bash\n\n");
		
    	if(isSMT) {
    		File SMT_path = new File(OutputPath+"\\\\SMT");
            if (!SMT_path.exists()) {
                SMT_path.mkdirs();
            }
    		for(int i=min;i<=max;i++) {
    			if(isAgreement)
    				output.write("translator -s -a -n "+String.valueOf(i)+" "+HOfile_linux+" "+OutputPath_linux+"SMT/a"+String.valueOf(i)+".smt2\n");
    			if(isUnivalence)
    				output.write("translator -s -u -n "+String.valueOf(i)+" "+HOfile_linux+" "+OutputPath_linux+"SMT/u"+String.valueOf(i)+".smt2\n");
    			if(isInvariant)
    				output.write("translator -s -i -n "+String.valueOf(i)+" "+HOfile_linux+" "+OutputPath_linux+"SMT/i"+String.valueOf(i)+".smt2\n");
    		}
    	}
    	if(isSMTBV && !isSAT) {
    		File SMT_BV_path = new File(OutputPath+"\\\\SMT_BV");
            if (!SMT_BV_path.exists()) {
            	SMT_BV_path.mkdirs();
            }
    		for(int i=min;i<=max;i++) {
    			if(isAgreement)
    				output.write("translator -b -a -n "+String.valueOf(i)+" "+HOfile_linux+" "+OutputPath_linux+"SMT_BV/a"+String.valueOf(i)+".smt2\n");
    			if(isUnivalence)
    				output.write("translator -b -u -n "+String.valueOf(i)+" "+HOfile_linux+" "+OutputPath_linux+"SMT_BV/u"+String.valueOf(i)+".smt2\n");
    			if(isInvariant)
    				output.write("translator -b -i -n "+String.valueOf(i)+" "+HOfile_linux+" "+OutputPath_linux+"SMT_BV/i"+String.valueOf(i)+".smt2\n");
    		}
    	}
    	if(isSAT) {
    		File SMT_BV_path = new File(OutputPath+"\\\\SMT_BV");
            if (!SMT_BV_path.exists()) {
            	SMT_BV_path.mkdirs();
            }
            File SAT_path = new File(OutputPath+"\\\\SAT");
            if (!SAT_path.exists()) {
            	SAT_path.mkdirs();
            }
    		for(int i=min;i<=max;i++) {
    			if(isAgreement) {
    				output.write("translator -b -a -n "+String.valueOf(i)+" "+HOfile_linux+" "+OutputPath_linux+"SMT_BV/a"+String.valueOf(i)+".smt2\n");
    				output.write("yices-smt2 --dimacs="+OutputPath_linux+"SAT/a"+String.valueOf(i)+".cnf "+OutputPath_linux+"SMT_BV/a"+String.valueOf(i)+".smt2\n");
    			}
    			if(isUnivalence) {
    				output.write("translator -b -u -n "+String.valueOf(i)+" "+HOfile_linux+" "+OutputPath_linux+"SMT_BV/u"+String.valueOf(i)+".smt2\n");
    				output.write("yices-smt2 --dimacs="+OutputPath_linux+"SAT/u"+String.valueOf(i)+".cnf "+OutputPath_linux+"SMT_BV/u"+String.valueOf(i)+".smt2\n");
    			}
    			if(isInvariant) {
    				output.write("translator -b -i -n "+String.valueOf(i)+" "+HOfile_linux+" "+OutputPath_linux+"SMT_BV/i"+String.valueOf(i)+".smt2\n");
    				output.write("yices-smt2 --dimacs="+OutputPath_linux+"SAT/i"+String.valueOf(i)+".cnf "+OutputPath_linux+"SMT_BV/i"+String.valueOf(i)+".smt2\n");
    			}
    		}
    	}
    	output.close();
    	
    }
    public void genScript() {
    	
    	try {
    		gen(config.isGen_agreement(),config.isGen_univalence(),config.isGen_invariant(),config.isGen_SMT(),config.isGen_SMT_bv(),
    			config.isGen_SAT(),config.getNode_num_MIN(),config.getNode_num_MAX(),
    			config.getHO_file(),config.getOutput_path(),config.getConfig_path());
    	} catch (Exception e) {
            e.printStackTrace();
    	}
    }

    public void run(){
    	String config_path=config.getConfig_path()+"\\\\gen.sh";
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