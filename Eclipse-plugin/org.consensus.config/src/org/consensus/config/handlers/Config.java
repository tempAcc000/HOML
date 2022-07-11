package org.consensus.config.handlers;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.HashMap;
import java.util.Map;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;



public class Config {
	private static Config instance; 
	
	private String HO_file="";
	private String Config_path="";
	private String Output_path="";
	
	private boolean gen_SMT=false;
	private boolean gen_SMT_bv=false;
	private boolean gen_SAT=false;
	
	private boolean gen_agreement=false;
	private boolean gen_univalence=false;
	private boolean gen_invariant=false;
	
	private int node_num_MIN=3;
	private int node_num_MAX=8;
	
	private String Solver_mode="SMT";
	private String SMT_solver="Z3";
	private String SAT_solver="Glucose";
	private int Divide_level=4;
	private int Conquer_num=16;
	
	public String to_gson() throws Exception{
		Map map = new HashMap();
		Map compiler_map=new HashMap();
		Map solver_map=new HashMap();
		
		map.put("HO_file",HO_file);
		map.put("Config_path",Config_path);
		map.put("Output_path",Output_path);
		
		compiler_map.put("node_num_MIN",node_num_MIN);
		compiler_map.put("node_num_MAX",node_num_MAX);
		compiler_map.put("gen_SMT",gen_SMT);
		compiler_map.put("gen_SMT_bv",gen_SMT_bv);
		compiler_map.put("gen_SAT",gen_SAT);
		compiler_map.put("gen_agreement",gen_agreement);
		compiler_map.put("gen_univalence",gen_univalence);
		compiler_map.put("gen_invariant",gen_invariant);
		map.put("Compiler", compiler_map);
		
		solver_map.put("Solver_mode",Solver_mode);
		solver_map.put("SMT_solver",SMT_solver);
		solver_map.put("SAT_solver",SAT_solver);
		solver_map.put("Divide_level",Divide_level);
		solver_map.put("Conquer_num",Conquer_num);
		map.put("Solver",solver_map);
		
		Gson gson = new GsonBuilder().setPrettyPrinting().create();
	    String gson_string = gson.toJson(map);
		return gson_string;
	}
	public void save_config() throws Exception{
		File file = new File(Config_path+"/config.json");
		if (file.exists()) file.delete();
		file.createNewFile();

		BufferedWriter output = new BufferedWriter(new FileWriter(file));
		output.write(to_gson());
		output.close();
	}
	
	public void load_config( )throws Exception {
		Config config=Config.getConfig();
		File file = new File(Config_path+"/config.json");
		if (!file.exists()) throw new Exception("File not Exists");
		
		BufferedReader input = new BufferedReader(new FileReader(file));
		String content;
		StringBuilder builder = new StringBuilder();
		while ((content = input.readLine()) != null) 
            builder.append(content);
		
		Gson gson = new GsonBuilder().setPrettyPrinting().create();
		Map<String, Object> map=gson.fromJson(builder.toString(),new TypeToken<Map<String , Object>>() { }.getType());
		Map<String, Object> compiler_map=(Map<String, Object>) map.get("Compiler");
		Map<String, Object> solver_map=(Map<String, Object>) map.get("Solver");

		setAll((String)map.get("HO_file"),(String)map.get("Config_path"),(String)map.get("Output_path"),
				((Double)compiler_map.get("node_num_MIN")).intValue(),((Double)compiler_map.get("node_num_MAX")).intValue(),
				(boolean)compiler_map.get("gen_SMT"),(boolean)compiler_map.get("gen_SMT_bv"),(boolean)compiler_map.get("gen_SAT"),
				(boolean)compiler_map.get("gen_agreement"),(boolean)compiler_map.get("gen_univalence"),(boolean)compiler_map.get("gen_invariant"),
				(String)solver_map.get("Solver_mode"),(String)solver_map.get("SMT_solver"),(String)solver_map.get("SAT_solver"),
				((Double)solver_map.get("Divide_level")).intValue(),((Double)solver_map.get("Conquer_num")).intValue());
		System.out.println(toString());
		
		
	}
	
	public void setAll(String HO_file,String Config_path,String Output_path,int node_num_MIN, int node_num_MAX,
			boolean gen_SMT,boolean gen_SMT_bv,boolean gen_SAT,boolean gen_agreement,boolean gen_univalence,boolean gen_invariant,
			String Solver_mode,String SMT_solver,String SAT_solver,int Divide_level,int Conquer_num) {
		setHO_file(HO_file);
		setConfig_path(Config_path);
		setOutput_path(Output_path);
		setNode_num_MIN(node_num_MIN);
		setNode_num_MAX(node_num_MAX);
		setGen_SMT(gen_SMT);
		setGen_SMT_bv(gen_SMT_bv);
		setGen_SAT(gen_SAT);
		setGen_agreement(gen_agreement);
		setGen_univalence(gen_univalence);
		setGen_invariant(gen_invariant);
		setSolver_mode(Solver_mode);
		setSMT_solver(SMT_solver);
		setSAT_solver(SAT_solver);
		setDivide_level(Divide_level);
		setConquer_num(Conquer_num);
	}
	
	

	/**
	 * @return the hO_file
	 */
	public String getHO_file() {
		return HO_file;
	}
	/**
	 * @return the config_path
	 */
	public String getConfig_path() {
		return Config_path;
	}
	/**
	 * @return the output_path
	 */
	public String getOutput_path() {
		return Output_path;
	}
	/**
	 * @return the gen_SMT
	 */
	public boolean isGen_SMT() {
		return gen_SMT;
	}
	/**
	 * @return the gen_SMT_bv
	 */
	public boolean isGen_SMT_bv() {
		return gen_SMT_bv;
	}
	/**
	 * @return the gen_SAT
	 */
	public boolean isGen_SAT() {
		return gen_SAT;
	}
	/**
	 * @return the gen_agreement
	 */
	public boolean isGen_agreement() {
		return gen_agreement;
	}
	/**
	 * @return the gen_univalence
	 */
	public boolean isGen_univalence() {
		return gen_univalence;
	}
	/**
	 * @return the gen_invariant
	 */
	public boolean isGen_invariant() {
		return gen_invariant;
	}
	/**
	 * @return the node_num_MIN
	 */
	public int getNode_num_MIN() {
		return node_num_MIN;
	}
	/**
	 * @return the node_num_MAX
	 */
	public int getNode_num_MAX() {
		return node_num_MAX;
	}
	/**
	 * @return the solver_mode
	 */
	public String getSolver_mode() {
		return Solver_mode;
	}
	/**
	 * @return the sMT_solver
	 */
	public String getSMT_solver() {
		return SMT_solver;
	}
	/**
	 * @return the sAT_solver
	 */
	public String getSAT_solver() {
		return SAT_solver;
	}
	/**
	 * @return the divide_level
	 */
	public int getDivide_level() {
		return Divide_level;
	}
	/**
	 * @return the conquer_num
	 */
	public int getConquer_num() {
		return Conquer_num;
	}
	/**
	 * @param hO_file the hO_file to set
	 */
	public void setHO_file(String hO_file) {
		HO_file = hO_file;
	}
	/**
	 * @param config_path the config_path to set
	 */
	public void setConfig_path(String config_path) {
		Config_path = config_path;
	}
	/**
	 * @param output_path the output_path to set
	 */
	public void setOutput_path(String output_path) {
		Output_path = output_path;
	}
	/**
	 * @param gen_SMT the gen_SMT to set
	 */
	public void setGen_SMT(boolean gen_SMT) {
		this.gen_SMT = gen_SMT;
	}
	/**
	 * @param gen_SMT_bv the gen_SMT_bv to set
	 */
	public void setGen_SMT_bv(boolean gen_SMT_bv) {
		this.gen_SMT_bv = gen_SMT_bv;
	}
	/**
	 * @param gen_SAT the gen_SAT to set
	 */
	public void setGen_SAT(boolean gen_SAT) {
		this.gen_SAT = gen_SAT;
	}
	/**
	 * @param gen_agreement the gen_agreement to set
	 */
	public void setGen_agreement(boolean gen_agreement) {
		this.gen_agreement = gen_agreement;
	}
	/**
	 * @param gen_univalence the gen_univalence to set
	 */
	public void setGen_univalence(boolean gen_univalence) {
		this.gen_univalence = gen_univalence;
	}
	/**
	 * @param gen_invariant the gen_invariant to set
	 */
	public void setGen_invariant(boolean gen_invariant) {
		this.gen_invariant = gen_invariant;
	}
	/**
	 * @param node_num_MIN the node_num_MIN to set
	 */
	public void setNode_num_MIN(int node_num_MIN) {
		this.node_num_MIN = node_num_MIN;
	}
	/**
	 * @param node_num_MAX the node_num_MAX to set
	 */
	public void setNode_num_MAX(int node_num_MAX) {
		this.node_num_MAX = node_num_MAX;
	}
	/**
	 * @param solver_mode the solver_mode to set
	 */
	public void setSolver_mode(String solver_mode) {
		Solver_mode = solver_mode;
	}
	/**
	 * @param sMT_solver the sMT_solver to set
	 */
	public void setSMT_solver(String sMT_solver) {
		SMT_solver = sMT_solver;
	}
	/**
	 * @param sAT_solver the sAT_solver to set
	 */
	public void setSAT_solver(String sAT_solver) {
		SAT_solver = sAT_solver;
	}
	/**
	 * @param divide_level the divide_level to set
	 */
	public void setDivide_level(int divide_level) {
		Divide_level = divide_level;
	}
	/**
	 * @param conquer_num the conquer_num to set
	 */
	public void setConquer_num(int conquer_num) {
		Conquer_num = conquer_num;
	}
	private Config() {}
	
	public static synchronized Config getConfig () {
		if(instance == null)
			instance= new Config();
		return instance;
	}

	@Override
	public String toString() {
		return "Config [HO_file=" + HO_file + ", Config_path=" + Config_path + ", Output_path=" + Output_path
				+ ", gen_SMT=" + gen_SMT + ", gen_SMT_bv=" + gen_SMT_bv + ", gen_SAT=" + gen_SAT + ", gen_agreement="
				+ gen_agreement + ", gen_univalence=" + gen_univalence + ", gen_invariant=" + gen_invariant
				+ ", node_num_MIN=" + node_num_MIN + ", node_num_MAX=" + node_num_MAX + ", Solver_mode=" + Solver_mode
				+ ", SMT_solver=" + SMT_solver + ", SAT_solver=" + SAT_solver + ", Divide_level=" + Divide_level
				+ ", Conquer_num=" + Conquer_num + "]";
	}

}
