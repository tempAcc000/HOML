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
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.consensus.config.handlers.Config;

public class ConfigShell {
	private static ConfigShell instance; 
	public Display display;
	public Shell shell;
	Config config;

	
	//Basic configuration
	Group basic_group;
	Label HO_label;
	Label Config_label;
	Label Output_label;
	Text HO_input;
	Text Config_input;Text 
	Output_input;
	Button HO_button;
	Button Config_button;
	Button Output_button;
	
	
	//Compiler configuration
	Group Compiler_config_group;
	Group node_group;
	Group format_group;
	Group property_group;
	Label textLabel1;
	Label textLabel2;
	Text min_input;
	Text max_input;
	Button SMT_button;
	Button SMT_bv_button;
	Button SAT_button;
	Button Invariant_button;
	Button Univalence_button;
	Button Agreement_button;
	
	
	//Solver configuration
	Group Solver_config_group;
	Group SMT_solver_group;
	Group SAT_solver_group;
	Group mode_group;
	Label horizontalLabel;
	Composite mode_comp;
	Button SMT_MODE_button;
	Button SAT_Serial_MODE_button;
	Button SAT_Parallel_MODE_button;
	Button Z3_button;
	Button CVC4_button;
	Button Alt_button;
	Button Glucose_button;
	Button Minisat_button;
	Label level_label;
	Label conquer_label;
	Text level_input;
	Text conquer_input;
	
	
	//bottom button
	Button save_button;
	Button cancel_button;
	
	public void init() {
		display = Display.getDefault();
		shell = new Shell(display);
		config=Config.getConfig();
		//shell.setImage(new Image(display, "icons/config2.png"));
		
		//Basic configuration
		basic_group = new Group(shell, SWT.NONE);
		HO_label = new Label(basic_group, SWT.NONE);
		Config_label = new Label(basic_group, SWT.NONE);
		Output_label = new Label(basic_group, SWT.NONE);
		HO_input = new Text(basic_group, SWT.BORDER);
		Config_input = new Text(basic_group, SWT.BORDER);
		Output_input = new Text(basic_group, SWT.BORDER);
		HO_button = new Button(basic_group, SWT.PUSH);
		Config_button = new Button(basic_group, SWT.PUSH);
		Output_button = new Button(basic_group, SWT.PUSH);
		
		
		//Compiler configuration
		Compiler_config_group = new Group(shell, SWT.NONE);
		node_group = new Group(Compiler_config_group, SWT.NONE);
		format_group = new Group(Compiler_config_group, SWT.NO_RADIO_GROUP);
		property_group = new Group(Compiler_config_group, SWT.NO_RADIO_GROUP);
		textLabel1 = new Label(node_group, SWT.NONE);
		textLabel2 = new Label(node_group, SWT.NONE);
		min_input = new Text(node_group, SWT.BORDER);
		max_input = new Text(node_group, SWT.BORDER);
		SMT_button = new Button(format_group, SWT.RADIO);
		SMT_bv_button = new Button(format_group, SWT.RADIO);
		SAT_button = new Button(format_group, SWT.RADIO);
		Invariant_button = new Button(property_group, SWT.RADIO);
		Univalence_button = new Button(property_group, SWT.RADIO);
		Agreement_button = new Button(property_group, SWT.RADIO);
		
		//Solver configuration
		Solver_config_group=new Group(shell, SWT.NONE);
		SMT_solver_group= new Group(Solver_config_group, SWT.NONE);
		SAT_solver_group= new Group(Solver_config_group, SWT.NONE);
		mode_group= new Group(Solver_config_group, SWT.NONE);
		horizontalLabel = new Label(Solver_config_group,SWT.HORIZONTAL | SWT.SEPARATOR);
		mode_comp=new Composite(Solver_config_group, SWT.NONE);
		SMT_MODE_button= new Button(mode_comp, SWT.RADIO);
		SAT_Serial_MODE_button= new Button(mode_comp, SWT.RADIO);
		SAT_Parallel_MODE_button= new Button(mode_comp, SWT.RADIO);
		Z3_button= new Button(SMT_solver_group, SWT.RADIO);
		CVC4_button= new Button(SMT_solver_group, SWT.RADIO);
		Alt_button= new Button(SMT_solver_group, SWT.RADIO);
		Glucose_button = new Button(SAT_solver_group, SWT.RADIO);
		Minisat_button= new Button(SAT_solver_group, SWT.RADIO);
		level_label= new Label(mode_group, SWT.NONE);
		conquer_label= new Label(mode_group, SWT.NONE);
		level_input= new Text(mode_group, SWT.BORDER);
		conquer_input= new Text(mode_group, SWT.BORDER);
		
		//bottom button
		save_button=new Button(shell,SWT.PUSH);
		cancel_button=new Button(shell,SWT.PUSH);
		
		shell.setText("HO-Ext Configuration");
        shell.setSize(640,720);
	}
	
	public void open() {
		shell.open();
	}
	
	public void close() {
		shell.close();
	}
	
	public void refresh() {
		//Basic
		HO_input.setText(config.getHO_file());
		Config_input.setText(config.getConfig_path());
		Output_input.setText(config.getOutput_path());
		
		//Compiler
		min_input.setText(String.valueOf(config.getNode_num_MIN()));
		max_input.setText(String.valueOf(config.getNode_num_MAX()));
		if(config.isGen_SMT())
			SMT_button.setSelection(true);
		else
			SMT_button.setSelection(false);
		if(config.isGen_SMT_bv())
			SMT_bv_button.setSelection(true);
		else
			SMT_bv_button.setSelection(false);
		if(config.isGen_SAT())
			SAT_button.setSelection(true);
		else
			SAT_button.setSelection(false);

		if(config.isGen_invariant())
			Invariant_button.setSelection(true);
		else
			Invariant_button.setSelection(false);
		if(config.isGen_univalence())
			Univalence_button.setSelection(true);
		else
			Univalence_button.setSelection(false);
		if(config.isGen_agreement())
			Agreement_button.setSelection(true);
		else
			Agreement_button.setSelection(false);
		
		//Solver
		if(config.getSolver_mode().equals("SMT")) {
			SMT_MODE_button.setSelection(true);
			SAT_Serial_MODE_button.setSelection(false);
			SAT_Parallel_MODE_button.setSelection(false);
		}
		else if(config.getSolver_mode().equals("SAT-Serial")){
			SMT_MODE_button.setSelection(false);
			SAT_Serial_MODE_button.setSelection(true);
			SAT_Parallel_MODE_button.setSelection(false);
		}
		else {
			SMT_MODE_button.setSelection(false);
			SAT_Serial_MODE_button.setSelection(false);
			SAT_Parallel_MODE_button.setSelection(true);
		}
			
		
		if(config.getSMT_solver().equals("Z3"))
			Z3_button.setSelection(true);
		else if(config.getSMT_solver().equals("CVC4"))
			CVC4_button.setSelection(true);
		else
			Alt_button.setSelection(true);
				
		if(config.getSAT_solver().equals("Glucose"))
			Glucose_button.setSelection(true);
		else
			Minisat_button.setSelection(true);
		
		level_input.setText(String.valueOf(config.getDivide_level()));
		conquer_input.setText(String.valueOf(config.getConquer_num()));
		
		
		//button enable
		if(config.getSolver_mode().equals("SMT")) {
			SMT_MODE_button.setSelection(true);
			SAT_Serial_MODE_button.setSelection(false);
			SAT_Parallel_MODE_button.setSelection(false);
			Z3_button.setEnabled(true);
			CVC4_button.setEnabled(true);
			Alt_button.setEnabled(true);
			Glucose_button.setEnabled(false);
			Minisat_button.setEnabled(false);
			level_label.setEnabled(false);
			conquer_label.setEnabled(false);
			level_input.setEnabled(false);
			conquer_input.setEnabled(false);
		}
		else if(config.getSolver_mode().equals("SAT-Serial")) {
			SMT_MODE_button.setSelection(false);
			SAT_Serial_MODE_button.setSelection(true);
			SAT_Parallel_MODE_button.setSelection(false);
			Z3_button.setEnabled(false);
			CVC4_button.setEnabled(false);
			Alt_button.setEnabled(false);
			Glucose_button.setEnabled(true);
			Minisat_button.setEnabled(true);
			level_label.setEnabled(false);
			conquer_label.setEnabled(false);
			level_input.setEnabled(false);
			conquer_input.setEnabled(false);
		}
		else {
			SMT_MODE_button.setSelection(false);
			SAT_Serial_MODE_button.setSelection(false);
			SAT_Parallel_MODE_button.setSelection(true);
			Z3_button.setEnabled(false);
			CVC4_button.setEnabled(false);
			Alt_button.setEnabled(false);
			Glucose_button.setEnabled(true);
			Minisat_button.setEnabled(true);
			level_label.setEnabled(true);
			conquer_label.setEnabled(true);
			level_input.setEnabled(true);
			conquer_input.setEnabled(true);
		}
				
	}
	
	public void set_basic() {		
		basic_group.setBounds(20, 20, 580, 130);
		basic_group.setText("Basic Configuration");
		
		
		HO_label.setText("HO-Ext file:");
		HO_label.setBounds(20, 30, 100, 30);		
		
		Config_label.setText("Config path:");
		Config_label.setBounds(20, 60, 100, 30);		
		
		Output_label.setText("Output path:");
		Output_label.setBounds(20, 90, 100, 30);
		
		HO_input.setBounds(120, 30, 340, 25);
		Config_input.setBounds(120, 60, 340, 25);
		Output_input.setBounds(120, 90, 340, 25);
		
		HO_button.setText("Browse...");
		HO_button.setBounds(480, 30, 80, 25);
		HO_button.addSelectionListener(new SelectionAdapter() {
	    	public void widgetSelected(SelectionEvent e) {
	    	FileDialog dialog = new FileDialog(shell, SWT.OPEN);
	    	dialog.setFilterPath("D:\\");
	    	String result = dialog.open();
	    	HO_input.setText(result);
	    	config.setHO_file(result);
	    	System.out.println("HO file:"+result);
	    	}
	    });
		
		Config_button.setText("Browse...");
		Config_button.setBounds(480, 60, 80, 25);
		Config_button.addSelectionListener(new SelectionAdapter() {
	    	public void widgetSelected(SelectionEvent e) {
	    		DirectoryDialog  dialog = new DirectoryDialog (shell);
	    		dialog.setFilterPath("D:\\");
	    		String result = dialog.open();
	    		Config_input.setText(result);
	    		config.setConfig_path(result);
	    		System.out.println("Config path:"+result);
	    		try {
	    			config.load_config();
	    			System.out.println("Loading Configuration");
	    			refresh();
	    		}catch(Exception ex) {
	    			ex.printStackTrace();
	    		}
	    	}
	    });
		
		Output_button.setText("Browse...");
		Output_button.setBounds(480, 90, 80, 25);
		Output_button.addSelectionListener(new SelectionAdapter() {
	    	public void widgetSelected(SelectionEvent e) {
	    	DirectoryDialog  dialog = new DirectoryDialog (shell);
	    	dialog.setFilterPath("D:\\");
	    	String result = dialog.open();
	    	Output_input.setText(result);
	    	config.setOutput_path(result);
	    	System.out.println("Output path:"+result);
	    	}
	    });
	}
	
	public void set_compiler() {		
		Compiler_config_group.setBounds(20, 160, 580, 165);
		Compiler_config_group.setText("Translator Configuration");
		
		node_group.setText("Node Number");
		node_group.setBounds(40, 20, 140, 130);

		format_group.setText("Output Format");
		format_group.setBounds(220, 20, 140, 130);
		
		textLabel1.setText("MIN:");
		textLabel1.setBounds(20, 45, 40, 30);
		min_input.setText(String.valueOf(config.getNode_num_MIN()));
		min_input.setBounds(70, 45, 45, 25);
		min_input.addVerifyListener(new VerifyListener() {
		    @Override
		    public void verifyText(VerifyEvent e) {
		    	for(char c:e.text.toCharArray()) {
		    		if (!Character.isDigit(c)) { // 0-9��ֹ����
		                e.doit = false;
		            }
		    	}
		    }
		});
		min_input.addModifyListener(new ModifyListener() {
		    @Override
		    public void modifyText(ModifyEvent e) {
		        try {
		        	if(min_input.getText().length()==0)
		        		min_input.setText("0");
		        	config.setNode_num_MIN(Integer.parseInt(min_input.getText()));
		        	System.out.println("Node_num_MIN:"+min_input.getText());
		        }
		        catch (NumberFormatException err) {
		        	err.printStackTrace();
		        }
		    }
		});
		
		
		textLabel2.setText("MAX:");
		textLabel2.setBounds(20, 82, 40, 30);
		max_input.setText(String.valueOf(config.getNode_num_MAX()));
		max_input.setBounds(70, 82, 45, 25);
		max_input.addVerifyListener(new VerifyListener() {
		    @Override
		    public void verifyText(VerifyEvent e) {
		    	for(char c:e.text.toCharArray()) {
		    		if (!Character.isDigit(c)) {
		                e.doit = false;
		            }
		    	}
		    }
		});
		max_input.addModifyListener(new ModifyListener() {
		    @Override
		    public void modifyText(ModifyEvent e) {
		        try {
		        	if(max_input.getText().length()==0)
		        		max_input.setText("0");
		        	config.setNode_num_MAX(Integer.parseInt(max_input.getText()));
		        	System.out.println("Node_num_MAX:"+max_input.getText());
		        }
		        catch (NumberFormatException err) {
		        	err.printStackTrace();
		        }
		    }
		});
		
		
		
		SMT_button.setText("SMT");
		SMT_button.setBounds(30, 30, 105, 30);
		SMT_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(SMT_button.getSelection()) {
					config.setGen_SMT(true);
					System.out.println("Gen SMT: true");
				}
				else {
					config.setGen_SMT(false);
					System.out.println("Gen SMT: false");
				}
			}
		});
		SMT_bv_button.setText("SMT-BV");
		SMT_bv_button.setBounds(30, 60, 105, 30);
		SMT_bv_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(SMT_bv_button.getSelection()) {
					config.setGen_SMT_bv(true);
					System.out.println("Gen SMT-BV: true");
				}
				else {
					config.setGen_SMT_bv(false);
					System.out.println("Gen SMT-BV: false");
				}
			}
		});
		SAT_button.setText("SAT");
		SAT_button.setBounds(30, 90, 105, 30);
		SAT_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(SAT_button.getSelection()) {
					config.setGen_SAT(true);
					System.out.println("Gen SAT: true");
				}
				else {
					config.setGen_SAT(false);
					System.out.println("Gen SAT: false");
				}
			}
		});
		
		property_group.setText("Property");
		property_group.setBounds(400, 20, 140, 130);
		
		Invariant_button.setText("Invariant");
		Invariant_button.setBounds(20, 30, 110, 30);
		Invariant_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(Invariant_button.getSelection()) {
					config.setGen_invariant(true);
					System.out.println("Check Invariant: true");
				}
				else {
					config.setGen_invariant(false);
					System.out.println("Check Invariant: false");
				}
			}
		});
		Univalence_button.setText("Univalence");
		Univalence_button.setBounds(20, 60, 110, 30);
		Univalence_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(Univalence_button.getSelection()) {
					config.setGen_univalence(true);
					System.out.println("Check Univalence: true");
				}
				else {
					config.setGen_univalence(false);
					System.out.println("Check Univalence: false");
				}
			}
		});
		Agreement_button.setText("Agreement");
		Agreement_button.setBounds(20, 90, 110, 30);
		Agreement_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(Agreement_button.getSelection()) {
					config.setGen_agreement(true);
					System.out.println("Check Agreement: true");
				}
				else {
					config.setGen_agreement(false);
					System.out.println("Check Agreement: false");
				}
			}
		});
		
		
	}
	
	public void set_solver() {
		Solver_config_group.setBounds(20, 335, 580, 290);
		Solver_config_group.setText("Solver Configuration");
		horizontalLabel.setBounds(20, 165, 540, 2);
		
		
		mode_comp.setBounds(40, 30, 140, 240);
		SMT_MODE_button.setText("SMT");
		SMT_MODE_button.setBounds(10, 55, 120, 20);
		SMT_MODE_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(SMT_MODE_button.getSelection()) {
					config.setSolver_mode("SMT");
					System.out.println("Solver Mode: SMT");
					Z3_button.setEnabled(true);
					CVC4_button.setEnabled(true);
					Alt_button.setEnabled(true);
					Glucose_button.setEnabled(false);
					Minisat_button.setEnabled(false);
					level_label.setEnabled(false);
					conquer_label.setEnabled(false);
					level_input.setEnabled(false);
					conquer_input.setEnabled(false);
				}
			}
		});
		SAT_Serial_MODE_button.setText("SAT Sequential");
		SAT_Serial_MODE_button.setBounds(10, 180, 130, 20);
		SAT_Serial_MODE_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(SAT_Serial_MODE_button.getSelection()) {
					config.setSolver_mode("SAT-Serial");
					System.out.println("Solver Mode: SAT-Serial");
					Z3_button.setEnabled(false);
					CVC4_button.setEnabled(false);
					Alt_button.setEnabled(false);
					Glucose_button.setEnabled(true);
					Minisat_button.setEnabled(true);
					level_label.setEnabled(false);
					conquer_label.setEnabled(false);
					level_input.setEnabled(false);
					conquer_input.setEnabled(false);
				}
			}
		});
		
		SAT_Parallel_MODE_button.setText("SAT Parallel");
		SAT_Parallel_MODE_button.setBounds(10, 210, 120, 20);
		SAT_Parallel_MODE_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(SAT_Parallel_MODE_button.getSelection()) {
					config.setSolver_mode("SAT-Parallel");
					System.out.println("Solver Mode: SAT-Parallel");
					Z3_button.setEnabled(false);
					CVC4_button.setEnabled(false);
					Alt_button.setEnabled(false);
					Glucose_button.setEnabled(true);
					Minisat_button.setEnabled(true);
					level_label.setEnabled(true);
					conquer_label.setEnabled(true);
					level_input.setEnabled(true);
					conquer_input.setEnabled(true);
				}
			}
		});
		
		
		//SMT MODE
		SMT_solver_group.setText("SMT Solver");
		SMT_solver_group.setBounds(220, 20, 140, 130);
		
		Z3_button.setText("Z3");
		Z3_button.setBounds(30, 30, 105, 30);
		Z3_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(Z3_button.getSelection()) {
					config.setSMT_solver("Z3");
					System.out.println("SMT Solver: Z3");
				}
			}
		});
		CVC4_button.setText("CVC4");
		CVC4_button.setBounds(30, 60, 105, 30);
		CVC4_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(CVC4_button.getSelection()) {
					config.setSMT_solver("CVC4");
					System.out.println("SMT Solver: CVC4");
				}
			}
		});
		Alt_button.setText("Alt-ergo");
		Alt_button.setBounds(30, 90, 105, 30);
		Alt_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(Alt_button.getSelection()) {
					config.setSMT_solver("Alt-ergo");
					System.out.println("SMT Solver: Alt-ergo");
				}
			}
		});
		
		
		//SAT MODE
		SAT_solver_group.setText("SAT Solver");
		SAT_solver_group.setBounds(220, 175, 140, 100);
		
		Glucose_button.setText("Glucose");
		Glucose_button.setBounds(30, 30, 105, 30);
		Glucose_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(Glucose_button.getSelection()) {
					config.setSAT_solver("Glucose");
					System.out.println("SAT Solver: Glucose");
				}
			}
		});
		Minisat_button.setText("Minisat");
		Minisat_button.setBounds(30, 60, 105, 30);
		Minisat_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				if(Minisat_button.getSelection()) {
					config.setSAT_solver("Minisat");
					System.out.println("SAT Solver: Minisat");
				}
			}
		});
		
		mode_group.setText("Parallel");
		mode_group.setBounds(400, 175, 140, 100);
		
		
		level_label.setText("Level:");
		level_label.setBounds(10, 30, 70, 30);
		level_input.setBounds(85, 30, 45, 25);
		level_input.addVerifyListener(new VerifyListener() {
		    @Override
		    public void verifyText(VerifyEvent e) {
		    	for(char c:e.text.toCharArray()) {
		    		if (!Character.isDigit(c)) {
		                e.doit = false;
		            }
		    	}
		    }
		});
		level_input.addModifyListener(new ModifyListener() {
		    @Override
		    public void modifyText(ModifyEvent e) {
		        try {
		        	if(level_input.getText().length()==0)
		        		level_input.setText("0");
		        	config.setDivide_level(Integer.parseInt(level_input.getText()));
		        	System.out.println("Divide Level:"+level_input.getText());
		        }
		        catch (NumberFormatException err) {
		        	err.printStackTrace();
		        }
		    }
		});
		
		conquer_label.setText("Conquer:");
		conquer_label.setBounds(10, 60, 70, 30);
		conquer_input.setBounds(85, 60, 45, 25);
		conquer_input.addVerifyListener(new VerifyListener() {
		    @Override
		    public void verifyText(VerifyEvent e) {
		    	for(char c:e.text.toCharArray()) {
		    		if (!Character.isDigit(c)) {
		                e.doit = false;
		            }
		    	}
		    }
		});
		conquer_input.addModifyListener(new ModifyListener() {
		    @Override
		    public void modifyText(ModifyEvent e) {
		        try {
		        	if(conquer_input.getText().length()==0)
		        		conquer_input.setText("0");
		        	config.setConquer_num(Integer.parseInt(conquer_input.getText()));
		        	System.out.println("Conquer Number:"+conquer_input.getText());
		        }
		        catch (NumberFormatException err) {
		        	err.printStackTrace();
		        }
		    }
		});
		
	}
	
	public void set_button() {
		save_button.setText("Save");
		save_button.setBounds(400, 633, 80, 30);
		save_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				try {
					config.save_config();
					System.out.println("Configuration Saved.");
				}
				catch(Exception ex){
					ex.printStackTrace();
					System.out.println("Configuration Abort.");
				}
			}
		});
		
		
		cancel_button.setText("Cancel");
		cancel_button.setBounds(510, 633, 80, 30);
		cancel_button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e){
				shell.dispose();
				System.out.println("Configuration Abort.");
			}
		});
	}
	
	private ConfigShell() {
		init();
		set_basic();
		set_compiler();
		set_solver();
		set_button();
	}
	public static synchronized ConfigShell getConfigShell () {
		if(instance == null)
			instance= new ConfigShell();
		return instance;
	}
}  
