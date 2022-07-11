#include "translator.h"

static int curr_round = 0; // Current round number
static int curr_process = 0; // Current round number
static int decide_flag = 0; // Whether some decisions have been made or not
static int dv_assign_count = 0; // Number of assignments into dv

// Functions to generate formula
void gen_smt2_bv();

static void wrt_definitions(); // Function to output reserved variables
static void gen_v_dec();
static void gen_size();
static void gen_domain();
static void gen_round_rep();
static void gen_round();
static void gen_s_part();
static void gen_t_part();
static void gen_inv();
static void gen_uni();
static void gen_stmt_rep(char **buff, int mode);
static void gen_stmt(char **buff, int mode);
static void gen_expr(char **buff, int mode);
static void gen_and_expr(char **buff, int mode);
static void gen_equality_expr(char **buff, int mode);
static void gen_relational_expr(char **buff, int mode);
static void gen_simple_expr(char **buff, int mode);
static void gen_term(char **buff, int mode);
static void gen_factor(char **buff, int mode);

// Functions to calculate loop counter value
int calc_expr();
int calc_term();
int calc_factor();

// Functions Int -> BitVec
char* dec2hex(int x);
char* dec2hex_positive(int x);
void replace_proc(char *src,int proc, char **dst);
void replace_round(char *src, int round, char **dst);
void replace_proc_and_round(char *src,int proc, int round, char **dst);
void var_define();

//入口函数
void gen_smt2_bv() {
	int i = 0, j, k;
	VTYPE *p;

	asprintf(&guard, "true");
	asprintf(&assertion, "    true\n");
	asprintf(&assumption, "    true\n");
	str_append(&outbuf, "(set-logic QF_BV)\n");
	gettokn(); /* "Algorithm" */
	gettokn(); /* <Identifier> */
	gettokn(); /* ";" */
	wrt_definitions(); /* write reserved variables */
	gen_v_dec(); /* encode variable declaration */
	gen_size();
	gen_domain();
	gen_round_rep(); /* encode rounds */

	
	// X
	str_append(&round_outbuf, "; X\n");
	for (i = 1; i <= round_count; i++) {
		for (j = 1; j <= n; j++) {
			str_append(&round_outbuf, "(assert (round%d_p%d  (bvadd phase %s)))\n", i,
					j, dec2hex((i - 1) * phase / round_count));
		}
	}
	str_append(&round_outbuf, "\n");

	if (inv_mode && token.kind != SINVARIANT) {
		fprintf(stderr, "Error: Missing Invariant Block.\n");
		remove(outfilename);
		exit(1);
	}

	if (token.kind == SINVARIANT) {
		gen_inv(); /* Encode Invariant */
		gettokn();
		if (inv_mode) { 
			var_define();
			str_append(&outbuf,inv_outbuf);
			str_append(&outbuf, "(check-sat)\n");
			return; 
		}
	}

	if (token.kind != SUNIVALENCE) {
		fprintf(stderr, "Error: Missing Univalence Block.\n");
		remove(outfilename);
		exit(1);
	}

	gen_uni(); /* Encode Univalence */
	var_define();
	if(inv_outbuf!=NULL)
		str_append(&outbuf,inv_outbuf);
	if(uni_outbuf!=NULL)
		str_append(&outbuf,uni_outbuf);
	str_append(&outbuf, "(check-sat)\n");

}

static void gen_v_dec() {
	int i, j, k;
	VTYPE *p;

	while (token.kind != SROUND && token.kind != SDOMAIN)		//直接跳过变量声明
		gettokn();

}
static void gen_size(){
	int i, j, k;
	// |HO(p,r)|
	str_append(&sizebuf, "; |HO(p,r)|\n");
	for(i=1;i<=n;i++){
		for(j=1;j<=round_count+1;j++){
			str_append(&sizebuf, "(define-fun size_p%d_r%d () (_ BitVec 32)\n",i,j);
			str_append(&sizebuf, "  (bvadd\n");
			for (k = 1; k <= n; k++) {
				str_append(&sizebuf, "    (ite (= rcv_ack_p%d_r%d_%d true) #x00000001 #x00000000)\n", i,j,k);
			}
			str_append(&sizebuf, "  )\n");
			str_append(&sizebuf, ")\n");
		}
	}
	str_append(&sizebuf, "\n");

	// |SHO(p,r)|
	str_append(&sizebuf, "; |SHO(p,r)|\n");
	for(i=1;i<=n;i++){
		for(j=1;j<=round_count+1;j++){
			str_append(&sizebuf, "(define-fun ssize_p%d_r%d () (_ BitVec 32)\n",i,j);
			str_append(&sizebuf, "  (bvadd\n");
			for (k = 1; k <= n; k++) {
				str_append(&sizebuf, "    (ite (and (= rcv_ack_p%d_r%d_%d true) (= rcv_safe_p%d_r%d_%d true)) #x00000001 #x00000000)\n", i,j,k,i,j,k);
			}
			str_append(&sizebuf, "  )\n");
			str_append(&sizebuf, ")\n");
		}
	}
	str_append(&sizebuf, "\n");

	// |AHO(p,r)|
	str_append(&sizebuf, "; |AHO(p,r)|\n");
	for(i=1;i<=n;i++){
		for(j=1;j<=round_count+1;j++){
			str_append(&sizebuf, "(define-fun asize_p%d_r%d () (_ BitVec 32)\n",i,j);
			str_append(&sizebuf, "  (bvsub size_p%d_r%d ssize_p%d_r%d)\n",i,j,i,j);
			str_append(&sizebuf, ")\n");
		}
	}
	str_append(&sizebuf, "\n");
}
static void gen_domain(){
	int i, j, k;
	VTYPE *p;
	// Constraints to limit ranges of variables
	
	str_append(&domain, "(define-fun domain_PROCESSRPL_ROUNDRPL () Bool\n  (and\n");
	for (p = v_list; p != NULL; p = p->next) { 					// Process variables
		if (p->isHidden || p->type == TBOOL) continue;
			str_append(&domain, "    (%s %s_PROCESSRPL_ROUNDRPL_0 #x00000000)\n",(p->type == TV) ? "bvsgt" : "bvsge",p->name);
	}
	str_append(&domain, "    (bvsgt phase #x00000000)\n"); // Phase
	str_append(&domain, "    (bvslt phase #x7ffffffe)\n"); // Phase
	str_append(&domain, "    (bvsge E #x00000000)\n"); // E
	str_append(&domain, "    (bvsle E N)\n");
	str_append(&domain, "    (bvsge T #x00000000)\n"); // T
	str_append(&domain, "    (bvsle T N)\n");

	if (coord_flag) { // Coordinators
		str_append(&domain,"    (bvsge coord_PROCESSRPL #x00000001) (bvsle coord_PROCESSRPL N)\n");	
	} 
	str_append(&domain, "    (bvsge dv_PROCESSRPL_ROUNDRPL_0 #x00000000)\n");
	
	//Domain = "Domain" , "{" , statement-list , "}" , ";" ;
	if (token.kind == SDOMAIN) {
		str_append(&domain, "    ;user define domains\n");
		gettokn();					//{
		gen_stmt_rep(&domain,DOM_MODE);
		// }
		gettokn();					//;
		gettokn();					//Round
	}
	str_append(&domain, "  )\n");
	str_append(&domain, ")\n");

	for(i=1;i<=n;i++){
		for(j=1;j<=round_count+1;j++){
			replace_proc_and_round(domain,i,j,&domain_outbuf);
		}
	}
	str_append(&domain_outbuf,"\n");


	str_append(&domain_outbuf, "; domains assert\n");
	for(i=1;i<=n;i++){
		for(j=1;j<=round_count+1;j++){
			str_append(&domain_outbuf, "(assert domain_p%d_r%d)\n", i,j);
		}
	}
	str_append(&domain_outbuf,"\n");
}


static void gen_round_rep() {
	int i,j;
	curr_round=0;
	do {
		roundbuf=NULL;
		varbuf=NULL;
		curr_round++;
		gen_round();
		
		for(i=1;i<=n;i++){
			replace_proc_and_round(roundbuf,i,curr_round,&round_outbuf);
			replace_proc_and_round(varbuf,i,curr_round,&var_outbuf);
		}
		
	} while (gettokn() && token.kind == SROUND);		//Round
	
}

static void gen_round() {
	int i,j;
	VTYPE *temp;

	/* "Round" */
	gettokn(); /* "{" */
	gettokn(); /* "SendPart" */
	str_append(&roundbuf, "; round%d_PROCESSRPL definition\n", curr_round);
	str_append(&roundbuf,"(define-fun round%d_PROCESSRPL ((_phase (_ BitVec 32))) Bool\n",curr_round);
	str_append(&roundbuf,
			"  (and\n");
	str_append(&roundbuf, "    ;; send part\n");
	gen_s_part(); /* Encode message sending part */
	/* ";" */
	gettokn(); /* "TransitionPart" */
	str_append(&roundbuf, "    ;; transition part\n");
	gen_t_part(); /* Encode state transition part */
	/* "}" */

	// Preparation to proceed next round
	str_append(&varbuf,"; PROCESSRPL var used in ROUNDRPL\n");
	for (temp = v_list; temp != NULL; temp = temp->next) {
		if (!temp->isHidden) {
			str_append(&roundbuf,
					"    (= %s_PROCESSRPL_ROUNDINCRPL_0 %s_PROCESSRPL_ROUNDRPL_%d)\n", temp->name, temp->name, temp->assign_count);
			
			switch (temp->type) {
				case TBOOL:
					for (i = 0; i <= temp->assign_count; i++) 
						str_append(&varbuf, "(declare-const %s_PROCESSRPL_ROUNDRPL_%d Bool)\n",temp->name,i);
					break;
				case TVARRAY:
				case TINTARRAY:
				case TTSARRAY:
					semantic_error("array cannot be unhidden.");
					break;
				default:
					for (i = 0; i <= temp->assign_count; i++) 
						str_append(&varbuf, "(declare-const %s_PROCESSRPL_ROUNDRPL_%d (_ BitVec 32))\n",temp->name,i);
					break;
			}

			temp->assign_count = 0;
		}
		temp->isMessage = 0;
	}

	str_append(&varbuf, "(declare-const dv_PROCESSRPL_ROUNDRPL_n1 (_ BitVec 32))\n");
	str_append(&varbuf, "(declare-const dv_PROCESSRPL_ROUNDRPL_0 (_ BitVec 32))\n");
	if (decide_flag == 0)
		str_append(&roundbuf, "    (= dv_PROCESSRPL_ROUNDRPL_n1 #x00000000)\n", curr_round);
	else {
		for (i = 1; i <= dv_assign_count; i++) 
			str_append(&varbuf, "(declare-const dv_PROCESSRPL_ROUNDRPL_%d (_ BitVec 32))\n", i);

		str_append(&roundbuf, "    (= dv_PROCESSRPL_ROUNDRPL_n1 dv_PROCESSRPL_ROUNDRPL_%d)\n",dv_assign_count);
		dv_assign_count = 0;
		decide_flag = 0;
	}
	str_append(&varbuf,"\n");
	str_append(&roundbuf, "  )\n)\n\n");
	gettokn(); /* ";" */
}
/*
send-part =  "if" , "(" expression , ")" , "send" , "(" , message , "," , destination , ")" ,
             [ "else" , "send" , "(" , message , "," , destination , ")" ] |
             "send" , "(" , message , "," , destination , ")" ;
message = "<" , variable-name | "Coord" , { "," , variable-name | "Coord" } , ">" |
        	"<" "ack" ">" ;
*/
static void gen_s_part() {
	int pid, isTowardCoord, else_flag = 0, if_flag = 0, send_coord = 0;
	VTYPE *v, *temp;
	char *condition = NULL;

	gettokn();
	if (token.kind == SIF) {				//if
		if_flag = 1;
		gettokn(); /* "(" */
		gen_expr(&condition, SEND_MODE); /* Encode expression */
		/* ")" */
		gettokn();
	};
	/* "send" */
	gettokn(); /* "(" */
	gettokn(); /* "<" */
	gettokn();
	if (token.kind != SACK) {			//发送的是ack,coord,还是var
		do {
			if (token.kind == SCOORD) {			//coord
				send_coord = 1;
			} else if (token.kind == SIDENTIFIER && (v = search_variable(	//var
					token.id)) != NULL) {
				v->isMessage = 1;
			}
			if (!gettokn() || token.kind != SCOMMA) break;
		} while (gettokn());
	} else gettokn();
	/* ">" */
	gettokn(); /* "," */
	gettokn();
	isTowardCoord = (token.kind == SALL) ? 0 : 1;			//destination是All还是coord
	gettokn(); /* ")" */
	gettokn();
	if (if_flag && token.kind == SELSE) {
		else_flag = 1;
		gettokn(); /* "send" */
		gettokn(); /* "(" */
		gettokn(); /* "<" */
		gettokn();
		if (token.kind != SACK) {
			do {
				if (token.kind == SCOORD) {
					send_coord = 2;
				} else if (token.kind == SIDENTIFIER && (v = search_variable(
						token.id)) != NULL) {
					v->isMessage = 2;
				}
				if (!gettokn() || token.kind != SCOMMA) break;
			} while (gettokn());
		} else gettokn();
		/* ">" */
		gettokn(); /* "," */
		gettokn();
		gettokn(); /* ")" */
		gettokn();
	};

	// formulas to set rcv
	for (pid = 1; pid <= n; pid++) {
		str_append(&roundbuf, "    ;;; from p%d\n", pid);
		str_append(&roundbuf, "    (ite ");
		if ((if_flag && !else_flag) || (!if_flag && isTowardCoord)) { // Send msgs with 'if' guard condition or Send msgs to coordinator without guard
			if (isTowardCoord) {
				str_append(&roundbuf, "%s(= coord_p%d PROCESSIDRPL)%s",
						if_flag ? "(and " : "", pid, if_flag ? " " : "");
			}
			if (if_flag) replace_proc(condition,pid,&roundbuf);  //replaceCondition(condition, &roundbuf, pid);
			str_append(&roundbuf, "%s\n", if_flag && isTowardCoord ? ")" : "");
		} else if (if_flag && isTowardCoord && else_flag) { // Send msgs to coordinator with 'if' and 'else' guard
			str_append(&roundbuf, "(= coord_p%d PROCESSIDRPL)\n", pid);
		} else { // Send msgs to all processes without guard
			str_append(&roundbuf, "true\n");
		}
		str_append(&roundbuf, "      true\n");
		str_append(&roundbuf, "      (= rcv_ack_PROCESSRPL_ROUNDRPL_%d false)\n", pid);
		str_append(&roundbuf, "    )\n");

		if (!else_flag) { // Without 'else'
			if (coord_flag) {
				if (send_coord == 1) {
					str_append(&roundbuf,"    ( ite (= rcv_ack_PROCESSRPL_ROUNDRPL_%d false) ", pid);
					str_append(&roundbuf,"(= rcv_coord_PROCESSRPL_ROUNDRPL_%d NULL) true) ; coordinator\n", pid);
					str_append(&roundbuf,"    ( ite (and (= rcv_ack_PROCESSRPL_ROUNDRPL_%d true) (= rcv_safe_PROCESSRPL_ROUNDRPL_%d true) )  ",pid,pid);
					str_append(&roundbuf,"(= rcv_coord_PROCESSRPL_ROUNDRPL_%d coord_p%d) true)\n",pid,pid);
					str_append(&roundbuf,"    ( ite (and (= rcv_ack_PROCESSRPL_ROUNDRPL_%d true) (= rcv_safe_PROCESSRPL_ROUNDRPL_%d false) )  ",pid,pid);
					str_append(&roundbuf,"(and (not (= rcv_coord_PROCESSRPL_ROUNDRPL_%d coord_p%d))(bvsgt rcv_coord_PROCESSRPL_ROUNDRPL_%d #x00000000)) true)\n",pid,pid,pid);
				} else if (send_coord == 0) {
					str_append(&roundbuf,
							"    (= rcv_coord_PROCESSRPL_ROUNDRPL_%d NULL) ; coordinator\n", pid);
				}
			}
			for (temp = v_list; temp != NULL; temp = temp->next) {
				if (temp->isMessage) {
					str_append(&roundbuf,"    ( ite (= rcv_ack_PROCESSRPL_ROUNDRPL_%d false) ",pid);
					str_append(&roundbuf,"(= rcv_%s_PROCESSRPL_ROUNDRPL_%d NULL) true)\n",temp->name,pid);
					str_append(&roundbuf,"    ( ite (and (= rcv_ack_PROCESSRPL_ROUNDRPL_%d true) (= rcv_safe_PROCESSRPL_ROUNDRPL_%d true) )  ",pid,pid);
					str_append(&roundbuf,"(= rcv_%s_PROCESSRPL_ROUNDRPL_%d %s_p%d_ROUNDRPL_0) true)\n",temp->name,pid,temp->name,pid);
					str_append(&roundbuf,"    ( ite (and (= rcv_ack_PROCESSRPL_ROUNDRPL_%d true) (= rcv_safe_PROCESSRPL_ROUNDRPL_%d false) )  ",pid,pid);
					str_append(&roundbuf,"(and (not (= rcv_%s_PROCESSRPL_ROUNDRPL_%d %s_p%d_ROUNDRPL_0)) (bvsgt rcv_%s_PROCESSRPL_ROUNDRPL_%d #x00000000)) true)\n",temp->name,pid,temp->name,pid,temp->name,pid);
				} else if (!temp->isHidden && temp->type != TBOOL) {
					str_append(&roundbuf,"    (= rcv_%s_PROCESSRPL_ROUNDRPL_%d NULL) ; %s\n",
							temp->name, pid, temp->name);
				}
			}
		} else { // With 'else'
			str_append(&roundbuf, "    (ite ");
			replace_proc(condition,pid,&roundbuf);
			//replaceCondition(condition, &roundbuf, pid);
			str_append(&roundbuf, " (and\n");
			if (coord_flag) {
				if (send_coord == 1) {
					str_append(&roundbuf,"    ( ite (= rcv_ack_PROCESSRPL_ROUNDRPL_%d false) ",pid);
					str_append(&roundbuf,"(= rcv_coord_PROCESSRPL_ROUNDRPL_%d NULL) true) ; coordinator\n", pid);
					str_append(&roundbuf,"    ( ite (and (= rcv_ack_PROCESSRPL_ROUNDRPL_%d true) (= rcv_safe_PROCESSRPL_ROUNDRPL_%d true) )  ",pid,pid);
					str_append(&roundbuf,"(= rcv_coord_PROCESSRPL_ROUNDRPL_%d coord_p%d) true)\n",pid,pid);
					str_append(&roundbuf,"    ( ite (and (= rcv_ack_PROCESSRPL_ROUNDRPL_%d true) (= rcv_safe_PROCESSRPL_ROUNDRPL_%d false) )  ",pid,pid);
					str_append(&roundbuf,"(and (not (= rcv_coord_PROCESSRPL_ROUNDRPL_%d coord_p%d))(bvsgt rcv_coord_PROCESSRPL_ROUNDRPL_%d #x00000000)) true)\n",pid,pid,pid);
				} else if (send_coord == 0) {
					str_append(&roundbuf,"    (= rcv_coord_PROCESSRPL_ROUNDRPL_%d NULL) ; coordinator\n", pid);
				}
			}
			for (temp = v_list; temp != NULL; temp = temp->next) {
				if (temp->isMessage == 1) {
					str_append(&roundbuf,"    ( ite (= rcv_ack_PROCESSRPL_ROUNDRPL_%d false) ",pid);
					str_append(&roundbuf,"(= rcv_%s_PROCESSRPL_ROUNDRPL_%d NULL) true)\n",temp->name,pid);
					str_append(&roundbuf,"    ( ite (and (= rcv_ack_PROCESSRPL_ROUNDRPL_%d true) (= rcv_safe_PROCESSRPL_ROUNDRPL_%d true) )  ",pid,pid);
					str_append(&roundbuf,"(= rcv_%s_PROCESSRPL_ROUNDRPL_%d %s_p%d_ROUNDRPL_0) true)\n",temp->name,pid,temp->name,pid);
					str_append(&roundbuf,"    ( ite (and (= rcv_ack_PROCESSRPL_ROUNDRPL_%d true) (= rcv_safe_PROCESSRPL_ROUNDRPL_%d false) )  ",pid,pid);
					str_append(&roundbuf,"(and (not (= rcv_%s_PROCESSRPL_ROUNDRPL_%d %s_p%d_ROUNDRPL_0)) (bvsgt rcv_%s_PROCESSRPL_ROUNDRPL_%d #x00000000)) true)\n",temp->name,pid,temp->name,pid,temp->name,pid);
				} else if (!temp->isHidden && temp->type != TBOOL) {
					str_append(&roundbuf,
							"    (= rcv_%s_PROCESSRPL_ROUNDRPL_%d NULL) ; %s\n"
							,temp->name, pid , temp->name);
				}
			}
			str_append(&roundbuf, "    ) (and\n");
			if (send_coord == 2) {
				str_append(&roundbuf,"    ( ite (= rcv_ack_PROCESSRPL_ROUNDRPL_%d false) ",pid);
				str_append(&roundbuf,"(= rcv_coord_PROCESSRPL_ROUNDRPL_%d NULL) true) ; coordinator\n", pid);
				str_append(&roundbuf,"    ( ite (and (= rcv_ack_PROCESSRPL_ROUNDRPL_%d true) (= rcv_safe_PROCESSRPL_ROUNDRPL_%d true) )  ",pid,pid);
				str_append(&roundbuf,"(= rcv_coord_PROCESSRPL_ROUNDRPL_%d coord_p%d) true)\n",pid,pid);
				str_append(&roundbuf,"    ( ite (and (= rcv_ack_PROCESSRPL_ROUNDRPL_%d true) (= rcv_safe_PROCESSRPL_ROUNDRPL_%d false) )  ",pid,pid);
				str_append(&roundbuf,"(and (not (= rcv_coord_PROCESSRPL_ROUNDRPL_%d coord_p%d))(bvsgt rcv_coord_PROCESSRPL_ROUNDRPL_%d #x00000000)) true)\n",pid,pid,pid);
			} else if (send_coord == 0) {
				str_append(&roundbuf,
						"    (= rcv_coord_PROCESSRPL_ROUNDRPL_%d NULL) ; coordinator\n", pid);
			}
			for (temp = v_list; temp != NULL; temp = temp->next) {
				if (temp->isMessage == 2) {
					str_append(&roundbuf,"    ( ite (= rcv_ack_PROCESSRPL_ROUNDRPL_%d false) ",pid);
					str_append(&roundbuf,"(= rcv_%s_PROCESSRPL_ROUNDRPL_%d NULL) true)\n",temp->name,pid);
					str_append(&roundbuf,"    ( ite (and (= rcv_ack_PROCESSRPL_ROUNDRPL_%d true) (= rcv_safe_PROCESSRPL_ROUNDRPL_%d true) )  ",pid,pid);
					str_append(&roundbuf,"(= rcv_%s_PROCESSRPL_ROUNDRPL_%d %s_p%d_ROUNDRPL_0) true)\n",temp->name,pid,temp->name,pid);
					str_append(&roundbuf,"    ( ite (and (= rcv_ack_PROCESSRPL_ROUNDRPL_%d true) (= rcv_safe_PROCESSRPL_ROUNDRPL_%d false) )  ",pid,pid);
					str_append(&roundbuf,"(and (not (= rcv_%s_PROCESSRPL_ROUNDRPL_%d %s_p%d_ROUNDRPL_0)) (bvsgt rcv_%s_PROCESSRPL_ROUNDRPL_%d #x00000000)) true)\n",temp->name,pid,temp->name,pid,temp->name,pid);
				} else if (!temp->isHidden && temp->type != TBOOL) {
					str_append(
							&roundbuf,
							"    (= %s_PROCESSRPL_ROUNDRPL_%d NULL) ; %s\n",
							temp->name, pid, temp->name);
				}
			}
			str_append(&roundbuf, "))\n");
		}

	}
	if (condition != NULL) free(condition);
}

static void gen_t_part() {
	gen_stmt_rep(&roundbuf, NORMAL_MODE); /* Encode statements */
}

static void gen_inv() {
	/* "Invariant" */
	gettokn(); /* "{" */

	str_append(&invbuf, "; Constraints for INV\n");
	str_append(&invbuf,"(define-fun inv_PROCESSRPL_ROUNDRPL ((_phase (_ BitVec 32))) Bool (and\n");
	str_append(&invbuf, "    true\n");
	gen_stmt_rep(&invbuf, INV_MODE); /* Encode statements */
	str_append(&invbuf, "  )\n");
	str_append(&invbuf, ")\n");
	str_append(&invbuf, ";; Assumptions for INV\n");
	str_append(&invbuf,"(define-fun inv_assumptions_PROCESSRPL_ROUNDRPL ((_phase (_ BitVec 32))) Bool (and\n");
	str_append(&invbuf, assumption);
	str_append(&invbuf, "  )\n");
	str_append(&invbuf, ")\n\n");

	replace_proc_and_round(invbuf,1,1,&inv_outbuf);
	if(inv_mode){
		replace_proc_and_round(invbuf,2,round_count + 1,&inv_outbuf);
	}

	str_append(&inv_outbuf, "; INV\n");
	str_append(&inv_outbuf, "(assert (and\n");
	str_append(&inv_outbuf, "  (inv_p1_r1 phase)\n");
	str_append(&inv_outbuf, "  (inv_assumptions_p1_r1 phase)\n");
	str_append(&inv_outbuf, "))\n\n");
	if (inv_mode) {
		str_append(&inv_outbuf, "; INV'\n");
		str_append(&inv_outbuf, "(assert (and\n");
		str_append(&inv_outbuf, "  (inv_p2_r%d (bvadd phase #x00000001))\n", round_count + 1);
		str_append(&inv_outbuf, "  (not (inv_assumptions_p2_r%d (bvadd phase #x00000001)))\n",
				round_count + 1);
		str_append(&inv_outbuf, "))\n\n");
	}
	/* "}" */
	gettokn(); /* ";" */
}

static void gen_uni() {
	int i, j;

	/* "Univalence" */
	gettokn(); /* "{" */

	str_append(&unibuf, "; Constraints for Agr\n");

	str_append(&unibuf,"(define-fun univ_PROCESSRPL_ROUNDRPL () Bool (and\n");
	str_append(&unibuf, "    true\n");
	gen_stmt_rep(&unibuf, UNI_MODE); /* Encode statements */
	str_append(&unibuf, "  )\n");
	str_append(&unibuf, ")\n");
	str_append(&unibuf, ";; Assertions for Agr\n");

	str_append(&unibuf,"(define-fun univ_assertions_PROCESSRPL_ROUNDRPL () Bool (and\n");
	str_append(&unibuf, assertion);
	str_append(&unibuf, "  )\n");
	str_append(&unibuf, ")\n\n");


	if (agr_mode) { // Agreement
		replace_proc_and_round(unibuf,1,round_count + 1,&uni_outbuf);

		str_append(&uni_outbuf, ";; Choose for _v\n");
		str_append(&uni_outbuf, "(assert\n");
		str_append(&uni_outbuf, "  (ite\n");
		str_append(&uni_outbuf, "    (or\n");
		for (i = 1; i <= n; i++) {
			for (j = 1; j <= round_count; j++) {
				str_append(&uni_outbuf, "      (not (= dv_p%d_r%d_n1 #x00000000))\n", i, j);
			}
		}
		str_append(&uni_outbuf, "    )\n");
		str_append(&uni_outbuf, "    (or\n");
		for (i = 1; i <= n; i++) {
			for (j = 1; j <= round_count; j++) {
				str_append(
						&uni_outbuf,
						"      (and (not (= dv_p%d_r%d_n1 #x00000000)) (= dv_p%d_r%d_n1 _v))\n",
						i, j, i, j);
			}
		}
		str_append(&uni_outbuf, "    )\n");
		str_append(&uni_outbuf, "    (= _v #x00000000)\n");
		str_append(&uni_outbuf, "  )\n");
		str_append(&uni_outbuf, ")\n");
		str_append(&uni_outbuf,
				";; if some process decided, then all processes must obey \"univ\"\n");

		str_append(&uni_outbuf, "(assert (=> (not (= _v #x00000000)) univ_p1_r%d))\n\n",round_count + 1);
		str_append(&uni_outbuf, "; Not Agr\n");
		str_append(&uni_outbuf, "(assert (not (or\n");
		str_append(&uni_outbuf, "  (= _v #x00000000)\n");
		str_append(&uni_outbuf, "  (and\n");

		str_append(&uni_outbuf, "    (not (= _v #x00000000))\n");
		for (i = 1; i <= n; i++) {
			for (j = 1; j <= round_count; j++) {
				str_append(
						&uni_outbuf,
						"    (or (= dv_p%d_r%d_n1 #x00000000) (= dv_p%d_r%d_n1 _v))\n",
						i, j, i, j);
			}
		}
		str_append(&uni_outbuf, "    univ_assertions_p1_r%d\n", round_count + 1);
		str_append(&uni_outbuf, "  )\n");
		str_append(&uni_outbuf, ")))\n\n");
	} else if (uni_mode) { // Univalence
		replace_proc_and_round(unibuf,1,1,&uni_outbuf);
		replace_proc_and_round(unibuf,2,round_count + 1,&uni_outbuf);

		str_append(&uni_outbuf, "; Univ\n");
		str_append(&uni_outbuf, "(assert (and\n");
		str_append(&uni_outbuf, "  (and\n");
		str_append(&uni_outbuf, "    univ_p1_r1\n");
		str_append(&uni_outbuf, "    univ_assertions_p1_r1\n");
		str_append(&uni_outbuf, "  )\n");
		str_append(&uni_outbuf, "  (or\n");
		str_append(&uni_outbuf, "    (not (and\n");
		for (i = 1; i <= n; i++) {
			for (j = 1; j <= round_count; j++) {
				str_append(
						&uni_outbuf,
						"      (or (= dv_p%d_r%d_n1 #x00000000) (= dv_p%d_r%d_n1 _v))\n",
						i, j, i, j);
			}
		}
		str_append(&uni_outbuf, "    ))\n");
		str_append(&uni_outbuf, "    (and\n");
		str_append(&uni_outbuf, "      univ_p2_r%d\n", round_count + 1);
		str_append(&uni_outbuf, "      (not univ_assertions_p2_r%d)\n",round_count + 1);
		str_append(&uni_outbuf, "    )\n");
		str_append(&uni_outbuf, "  )\n");
		str_append(&uni_outbuf, "))\n\n");
	}
	/* "}" */
	gettokn(); /* ";" */
}
//statement-list = statement , ";" , { statement , ";" } 

static void gen_stmt_rep(char **buff, int mode) {
	gettokn();
	do {
		gen_stmt(buff, mode); /* Encode statement */
		/* ";" */
		gettokn();
	} while (token.kind != SRBRACE);
}
/*
statement = assignment |
            decision |
            "for" , "[" , variable , "=" , expression , "to" , expression , "]" , statement |
            "if" , "(" , expression , ")" , statement , [ "else" , statement ] |
            "assume" , "(" , expression , ")" |
            "assert" , "(" , expression , ")" |
            "{" , statement-list , "}" ;
			assignment = variable , "=" , expression ; 
*/
static void gen_stmt(char **buff, int mode) {
	VTYPE *v;
	int idx;
	char *new_guard = NULL, *org_guard = NULL;
	switch (token.kind) {
	case SIDENTIFIER: /* assignment */
		v = search_variable(token.id);
		if (v->type != TINTARRAY && v->type != TVARRAY && v->type != TTSARRAY) { // Non-array variable
			if (!v->isHidden) { // Process variable	x,ack...
				str_append(buff, "    (= %s_PROCESSRPL_ROUNDRPL_%d (ite (and %s) ",
						v->name, (v->assign_count) + 1, guard);
			} else if (v->other_field == 0) { // Hidden variable except loop counter
				str_append(buff, "    (= %s_PROCESSRPL_%d (ite (and %s) ",
						v->name, (v->assign_count) + 1, guard);
			} else { // Loop counter
				gettokn(); /* "=" */
				v->value = calc_expr(); /* Evaluate */
				break;
			}
		} else { // Array variable
			gettokn(); /* "[" */
			str_append(buff, "    (= %s_PROCESSRPL", v->name);
			idx = calc_expr(); /* Evaluate */
			if (idx < 1 || n < idx) semantic_error("index is out of range."); // Check range
			str_append(buff, "_%d_%d (ite (and %s) ", idx,
					(v->assign_count_elem[idx - 1]) + 1, guard);
			/* "]" */
		}
		gettokn(); /* "=" */
		gen_expr(buff, mode); /* Encode expression */
		if (v->type != TINTARRAY && v->type != TVARRAY && v->type != TTSARRAY) { // Non-array variable
			if (!v->isHidden) { // Process variable
				str_append(buff, " %s_PROCESSRPL_ROUNDRPL_%d))\n", v->name, (v->assign_count)++);
			} else { // Hidden variable
				str_append(buff, " %s_PROCESSRPL_%d))\n", v->name,
						(v->assign_count)++);
			}
		} else { // Array variable
			str_append(buff, " %s_PROCESSRPL_%d_%d))\n", v->name, idx,
					(v->assign_count_elem[idx - 1])++);
		}
		break;
	case SDECIDE: /* Decide statement */
		decide_flag = 1;
		str_append(buff, "    (= dv_PROCESSRPL_ROUNDRPL_%d (ite (and %s) ", ++dv_assign_count, guard);
		gettokn(); /* "(" */
		gen_expr(buff, mode); /* Encode expression */
		/* ")" */
		gettokn();
		if (dv_assign_count == 1) str_append(buff, " #x00000000))\n");
		else str_append(buff, " dv_PROCESSRPL_ROUNDRPL_%d))\n", dv_assign_count - 1);
		break;
		//"for" , "[" , variable , "=" , expression , "to" , expression , "]" , statement
	case SFOR: /* for loop */
		gettokn(); /* "（" */
		gettokn(); /* <Identifier> */
		if ((v = search_variable(token.id)) == NULL) error(
				"undeclared variable.");
		v->other_field = -1;
		gettokn(); /* "）" */
		gettokn();
		gen_stmt(buff, mode); /* Encode statement */
		v->other_field = 0;
		break;
		//"if" , "(" , expression , ")" , statement , [ "else" , statement ]
	case SIF: /* if statement */
		gettokn(); /* "(" */
		gen_expr(&new_guard, mode); /* Encode expression */
		/* ")" */
		str_append(&org_guard, guard); // org_guard := guard
		str_append(&guard, " %s", new_guard); // guard := guard ∧ new condition
		gettokn();
		gen_stmt(buff, mode); /* Encode statement */
		free(guard);
		guard = NULL;
		str_append(&guard, org_guard); // guard := org_guard
		free(org_guard);
		org_guard = NULL;
		if (token.kind == SELSE) { // else
			str_append(&org_guard, guard); // org_guard := guard
			str_append(&guard, " (not %s)", new_guard); // guard := guard ∧ not (new condition)
			gettokn();
			gen_stmt(buff, mode); /* Encode statement */
			free(guard);
			guard = NULL;
			str_append(&guard, org_guard); // guard := org_guard
			free(org_guard);
		}
		free(new_guard);
		break;
		//"assume" , "(" , expression , ")" | "assert" , "(" , expression , ")" |
	case SASSUME: /* assume statement */
		gettokn(); /* "(" */
		str_append(&assumption, "    (=> (and %s) ", guard);
		gen_expr(&assumption, mode); /* Encode expression */
		/* ")" */
		str_append(&assumption, ")\n");
		gettokn();
		break;
	case SASSERT: /* assert statement */
		if(mode==UNI_MODE){
			gettokn(); /* "(" */
			str_append(&assertion, "    (=> (and %s) ", guard);
			gen_expr(&assertion, mode); /* Encode statement */
			/* ")" */
			str_append(&assertion, ")\n");
			gettokn();
		}
		else if(mode==DOM_MODE){
			gettokn(); /* "(" */
			str_append(&domain, "    ", guard);
			gen_expr(&domain, mode); /* Encode statement */
			/* ")" */
			str_append(&domain, "\n");
			gettokn();
		}
		break;
	case SLBRACE: /* code block */
		gen_stmt_rep(buff, mode); /* Encode statements */
		/* ")" */
		gettokn();
		break;
	default:
		fprintf(stdout,"%d-%s-%d\n",token.kind,token.id,token.line);
		error("parse error.");
	}
}
//expression  =  conjunctive-expression , { "||" , conjunctive-expression } ;
static void gen_expr(char **buff, int mode) {
	int flag;
	char *op1 = NULL, *op2 = NULL;

	gen_and_expr(&op1, mode);
	if (token.kind != SOR) {		// ||
		str_append(buff, op1);		
		flag = 0;
	} else {
		str_append(buff, "(%s %s", token.id, op1);
		flag = 1;
	}
	while (token.kind == SOR) {
		str_append(buff, " ");
		gen_and_expr(&op2, mode);
		str_append(buff, op2);
		free(op2);
		op2 = NULL;
	};
	if (flag) str_append(buff, ")");
	free(op1);
}
//conjunctive-expression  =  equality-expression { "&&" equality-expression }
static void gen_and_expr(char **buff, int mode) {
	int flag;
	char *op1 = NULL, *op2 = NULL;

	gen_equality_expr(&op1, mode);
	if (token.kind != SAND) {		//&&
		str_append(buff, op1);
		flag = 0;
	} else {
		str_append(buff, "(%s %s", token.id, op1);
		flag = 1;
	}
	while (token.kind == SAND) {
		str_append(buff, " ");
		gen_equality_expr(&op2, mode);
		str_append(buff, op2);
		free(op2);
		op2 = NULL;
	};
	if (flag) str_append(buff, ")");
	free(op1);
}
//equality-expression = relational-expression , [ equality-operator , relational-expression ] ;
static void gen_equality_expr(char **buff, int mode) {
	char *op1 = NULL;

	gen_relational_expr(&op1, mode);
	if (!isEqualityOperator(token.kind)) {			// ==, !=
		str_append(buff, op1);
	} else {
		int flag=0;
		if(token.kind==SEQUAL)
			str_append(buff, "(%s %s ", token.id, op1);			// (= op1 op2)
		else if(token.kind==SNOTEQUAL){
			str_append(buff, "(not (= %s ", op1);			//	(not (= op1 op2))
			flag=1;
		}
		gen_relational_expr(buff, mode);						//(= op1 op2)
			str_append(buff, ")");
		if(flag==1)
			str_append(buff, ")");							//	(not (= op1 op2))
	};
	free(op1);
}
//relational-expression = simple-expression , [ relational-operator , simple-expression ] ;
static void gen_relational_expr(char **buff, int mode) {
	char *op1 = NULL;

	gen_simple_expr(&op1, mode);
	if (!isRelationalOperator(token.kind)) {
		str_append(buff, op1);
	} else {
		switch (token.kind) {
			case SLESS:
				str_append(buff, "(bvslt %s ", op1);
				break;
			case SLESSEQUAL:
				str_append(buff, "(bvsle %s ", op1);
				break;
			case SGREAT:
				str_append(buff, "(bvsgt %s ", op1);
				break;
			case SGREATEQUAL:
				str_append(buff, "(bvsge %s ", op1);
				break;
			default:
				semantic_error("invalid operator.");
		}
		gen_simple_expr(buff, mode);
		str_append(buff, ")");
	};
	free(op1);
}
//simple-expression = term , { additive-operator , term } ;
static void gen_simple_expr(char **buff, int mode) {
	int flag;
	char *op1 = NULL, *op2 = NULL;

	gen_term(&op1, mode);
	if (!isAdditiveOperator(token.kind)) {
		str_append(buff, op1);
		flag = 1;
	} else flag = 0;
	while (isAdditiveOperator(token.kind)) {
		op2 = NULL;
		switch (token.kind) {
			case SPLUS:
				str_append(&op2, "(bvadd %s ", op1);
				break;
			case SMINUS:
				str_append(&op2, "(bvsub %s ", op1);
				break;
			default:
				semantic_error("invalid operator.");
		}
		gen_term(&op2, mode);
		str_append(&op2, ")");
		free(op1);
		op1 = op2;
	};
	if (!flag) {
		str_append(buff, op2);
		free(op2);
	} else free(op1);
}
//term = factor , { multiplicative-operator , factor } ;
static void gen_term(char **buff, int mode) {
	int flag;
	char *op1 = NULL, *op2 = NULL;

	gen_factor(&op1, mode);
	gettokn();
	if (!isMultiplicativeOperator(token.kind)) {
		str_append(buff, op1);
		flag = 1;
	} else flag = 0;
	while (isMultiplicativeOperator(token.kind)) {
		op2 = NULL;
		switch (token.kind) {
			case SMULT:
				str_append(&op2, "(bvmul %s ", op1);
				break;
			case SDIV:
				str_append(&op2, "(bvsdiv %s ", op1);
				break;
			default:
				semantic_error("invalid operator.");
		}
		gen_factor(&op2, mode);
		str_append(&op2, ")");
		free(op1);
		op1 = op2;
		gettokn();
	};
	if (!flag) {
		str_append(buff, op2);
		free(op2);
	} else free(op1);
}
/*
factor = variable | constant | "isCoord" | "Coord" |
         "rcv" , "[" , expression , "]" , "." , variable | "(" , expression , ")" ;
constant = "N" | "SIZE"  | "NULL" | "phase" | "DECISION" | simple-constant ;
variable = normal-variable | array-variable ;
normal-variable = variable-name ;
array-variable = variable-name , "[" , expression , "]" ;
*/
static void gen_factor(char **buff, int mode) {
	VTYPE *v;
	int idx;
	char *expression = NULL;
	static int is_index=0;
	static int is_identifier=0;

	gettokn();
	switch (token.kind) {
	case SIDENTIFIER:									//variable | constant
		is_identifier=1;
		v = search_variable(token.id);
		if (v->type != TINTARRAY && v->type != TVARRAY && v->type != TTSARRAY) { // Non-array variable
			if (mode == NORMAL_MODE || mode == SEND_MODE) { // In round
				if (mode == NORMAL_MODE && v->isHidden && v->other_field >= 0) { // Hidden variable except loop counter
					str_append(buff, "%s_PROCESSRPL_%d", v->name,v->assign_count);
				} else if (mode == NORMAL_MODE && v->other_field < 0) { // Loop counter
					is_identifier=0;
					if(is_index)
						str_append(buff, "%d", v->value);
					else
						str_append(buff, "%s", dec2hex(v->value));
				} else if (!v->isHidden) { // Process variable
					str_append(buff, "%s_PROCESSRPL_ROUNDRPL_%d", v->name, v->assign_count);
				}
			} else if (mode == INV_MODE || mode == UNI_MODE) { // In Invariant，Univalence
				if (!v->isHidden) { // Process variable
					gettokn(); /* "[" */
					idx = calc_expr(); /* Evaluate */
					if (idx < 1 || idx > n) semantic_error(
							"index is out of range.");
					/* "]" */
					str_append(buff, "%s_p%d_ROUNDRPL_0", v->name, idx);
				} else { // Hidden variable
					if (v->other_field < 0){ // Loop counter
						is_identifier=0;
						if(is_index)
							str_append(buff, "%d", v->value);
						else
							str_append(buff, "%s", dec2hex(v->value));
					}
					else 
						str_append(buff, "%s_PROCESSRPL_%d", v->name,v->assign_count); // Others
				}
			}
		} else { // Array variable
			gettokn(); /* "[" */
			idx = calc_expr(); /* Evaluate */
			if (idx < 1 || n < idx) semantic_error("index is out of range."); // Check range
			str_append(buff, "%s_PROCESSRPL_%d_%d", v->name,idx,v->assign_count_elem[idx - 1]);
			/* "]" */
		}
		break;
	case SQUESTION:						//?
		str_append(buff, "#x00000000");
		break;
	case SFALSE:						//false
		str_append(buff, "false");
		break;
	case STRUE:							//true
		str_append(buff, "true");
		break;
	case SNUM:							//num
		if(is_index)
			str_append(buff, "%d", atoi(token.id));
		else
			str_append(buff, "%s", dec2hex(atoi(token.id)));
		break;
	case SPROC_COUNT:					//N
		str_append(buff, "N");
		break;
	case SSIZE:							//SIZE
		if(mode == DOM_MODE)
			str_append(buff, "size_PROCESSRPL_ROUNDRPL");
		else
			str_append(buff, "size_PROCESSRPL_ROUNDRPL");
		break;
	case STHRESHOLD:
		str_append(buff, "T");
		break;
	case SENOUGH:
		str_append(buff, "E");
		break;
	case SA:
		str_append(buff, "a");
		break;
	case SSSIZE:
		str_append(buff, "ssize_PROCESSRPL_ROUNDRPL");
		break;
	case SASIZE:
		str_append(buff, "asize_PROCESSRPL_ROUNDRPL");
		break;
	case SNULL:							//NULL
		str_append(buff, "NULL");
		break;
	case SPHASE:						//phase
		str_append(buff, "_phase");
		break;
	case SISCOORD:						//iscord
		str_append(buff, "(= coord_PROCESSRPL PROCESSIDRPL)");
		break;
	case SCOORD:						//coord
		str_append(buff, "coord_PROCESSRPL");
		break;
	case SPID:							//pid
		str_append(buff, "PROCESSIDRPL");
		break;
	case SVVAL:							//val
		str_append(buff, "_v");
		break;
	case SRCV:							//rcv
		is_identifier=0;
		is_index=1;
		gettokn(); /* "[" */
		gen_expr(&expression, mode); /* Encode expression */
		/* "]" */
		is_index=0;
		gettokn(); /* "." */
		gettokn();
		if (token.kind == SCOORD) {
			str_append(buff, "(rcv_coord (rcv _proc r%d %s) 2)", curr_round,expression);
		} else if (token.kind == SIDENTIFIER && (v = search_variable(token.id))
				!= NULL && v->isMessage) {

			if(!is_identifier)
				str_append(buff, "rcv_%s_PROCESSRPL_ROUNDRPL_%s", v->name,expression);
			else{
				str_append(buff, "(ite (= %s #x00000001) rcv_%s_PROCESSRPL_ROUNDRPL_1 ",expression, v->name);
				for(idx=2;idx<n;idx++){
					str_append(buff, "(ite (= %s %s) rcv_%s_PROCESSRPL_ROUNDRPL_%d ",expression, dec2hex(idx), v->name, idx);
				}
				str_append(buff, "rcv_%s_PROCESSRPL_ROUNDRPL_%d", v->name,idx);
				for(idx=1;idx<n;idx++){
					str_append(buff, ")");
				}
			}
		}
		free(expression);
		break;
	case SLPAREN:						//()
		gen_expr(buff, mode); /* Encode expression */
		/* ")" */
		break;
	default:
		fprintf(stdout,"%d-%s-%d\n",token.kind,token.id,token.line);
		error("parse error.");
	};
}

//转16进制
char* dec2hex(int x){
	static char ret[35];
	if(x<0){
		strcpy(ret,"(bvsub #x00000000 ");
		strcat(ret,dec2hex_positive(x));
		strcat(ret,")");
		return ret;
	}
	else{
		return dec2hex_positive(x);
	}
}
char* dec2hex_positive(int x){
	static char ret[11];
	int idx=10;
	ret[idx--]='\0';
	int remainder,quotient;
	remainder=x%16;
	quotient=x/16;
	while(remainder!=0 || quotient!=0){
		if(remainder<10)
			ret[idx--]=remainder+'0';
		else
			ret[idx--]=remainder-10+'a';
		remainder=quotient%16;
		quotient=quotient/16;
	}
	for(;idx>=2;idx--)
		ret[idx]='0';
	ret[idx--]='x';
	ret[idx--]='#';
	return ret;
}


void replace_proc(char *src,int proc, char **dst){
	int len = strlen(src)+500;
	char* temp=(char*)malloc(sizeof(char)*len);
	char proc_str[10]="";
	char pid_str[35]="";

	sprintf(proc_str,"p%d",proc);
	sprintf(pid_str,"%s",dec2hex(proc));

	strcpy(temp,src);
	replaceStr(temp,"PROCESSIDRPL",pid_str);
	replaceStr(temp,"PROCESSRPL",proc_str);
	str_append(dst,temp);

	free(temp);
}
void replace_round(char *src, int round, char **dst){
	int len = strlen(src)+500;
	char* temp=(char*)malloc(sizeof(char)*len);
	char round_str[10]="";
	char round_inc_str[10]="";

	sprintf(round_str,"r%d",round);
	sprintf(round_inc_str,"r%d",round+1);

	strcpy(temp,src);
	replaceStr(temp,"ROUNDRPL",round_str);
	replaceStr(temp,"ROUNDINCRPL",round_inc_str);
	str_append(dst,temp);

	free(temp);
}
void replace_proc_and_round(char *src,int proc, int round, char **dst){
	int len = strlen(src)+500;
	char* temp=(char*)malloc(sizeof(char)*len);
	char proc_str[10]="";
	char round_str[10]="";
	char round_inc_str[10]="";
	char pid_str[35]="";

	sprintf(proc_str,"p%d",proc);
	sprintf(round_str,"r%d",round);
	sprintf(round_inc_str,"r%d",round+1);
	sprintf(pid_str,"%s",dec2hex(proc));

	strcpy(temp,src);
	replaceStr(temp,"PROCESSIDRPL",pid_str);
	replaceStr(temp,"PROCESSRPL",proc_str);
	replaceStr(temp,"ROUNDRPL",round_str);
	replaceStr(temp,"ROUNDINCRPL",round_inc_str);
	str_append(dst,temp);

	free(temp);
}

//常量定义
void wrt_definitions() {
	int i,j,k;
	VTYPE *p;

	// Constants
	str_append(&outbuf, "; constant\n");
	str_append(&outbuf,"(declare-const N (_ BitVec 32)) ; Number of processes\n");
	str_append(&outbuf,"(assert (= N %s))\n",dec2hex(n));
	str_append(&outbuf,"(declare-const NULL (_ BitVec 32)) ; Failure\n");
	str_append(&outbuf,"(assert (= NULL (bvsub #x00000000 #x00000001)))\n");
	str_append(&outbuf,"(declare-const T (_ BitVec 32)) ; Therehold\n");
	str_append(&outbuf,"(declare-const E (_ BitVec 32)) ; Enough\n");
	str_append(&outbuf,"(declare-const a (_ BitVec 32)) ; α\n");
	str_append(&outbuf, "\n");



	// System-defined variables
	str_append(&outbuf, ";; system define var\n");
	if (coord_flag)
		for(i=1;i<=n;i++)
			str_append(&outbuf, "(declare-const coord_p%d (_ BitVec 32))  ; coordinator\n",i);
	str_append(&outbuf, "(declare-const phase (_ BitVec 32))  ; phase number\n");
	str_append(&outbuf, "(declare-const _v (_ BitVec 32))\n");


	str_append(&outbuf, "; rcv ack\n");
	for(i=1;i<=n;i++)
		for(j=1;j<=round_count+1;j++)
			for(k=1;k<=n;k++)
				str_append(&outbuf, "(declare-const rcv_ack_p%d_r%d_%d Bool)\n",i,j,k);
	str_append(&outbuf, "\n");


	str_append(&outbuf, "; rcv safe\n");
	for(i=1;i<=n;i++)
		for(j=1;j<=round_count+1;j++)
			for(k=1;k<=n;k++)
				str_append(&outbuf, "(declare-const rcv_safe_p%d_r%d_%d Bool)\n",i,j,k);
	str_append(&outbuf, "\n");

	str_append(&outbuf, "; rcv coord\n");
	if (coord_flag) 
		for(i=1;i<=n;i++)
			for(j=1;j<=round_count+1;j++)
				for(k=1;k<=n;k++)
					str_append(&outbuf, "(declare-const rcv_coord_p%d_r%d_%d (_ BitVec 32))\n",i,j,k);
	str_append(&outbuf, "\n");

	for (p = v_list; p != NULL; p = p->next) {
		if (!p->isHidden && p->type != TBOOL){
			str_append(&outbuf, "; rcv %s\n",p->name);
			for(i=1;i<=n;i++)
				for(j=1;j<=round_count+1;j++)
					for(k=1;k<=n;k++)
						str_append(&outbuf, "(declare-const rcv_%s_p%d_r%d_%d (_ BitVec 32))\n",p->name,i,j,k);
			str_append(&outbuf, "\n");
		}
	}

}

void var_define(){
	int i,j,k;
	VTYPE *p;
	for (p = v_list; p != NULL; p = p->next) {
		if (p->isHidden) {
			switch (p->type) {
				case TBOOL:
					str_append(&outbuf, "; %s\n",p->name);
					for (i = 1; i <= n; i++) 
						for (j = 0; j <= p->assign_count; j++) 
							str_append(&outbuf, "(declare-const %s_p%d_%d Bool)\n",p->name,i,j);
					str_append(&outbuf, "\n");
					break;
				case TVARRAY:
				case TINTARRAY:
				case TTSARRAY:
					str_append(&outbuf, "; %s\n",p->name);
					for (i = 1; i <= n; i++) 
						for (j = 1; j <= n; j++) 
							for(k=0; k<=p->assign_count_elem[j-1];k++)
								str_append(&outbuf, "(declare-const %s_p%d_%d_%d (_ BitVec 32))\n",p->name,i,j,k);
					str_append(&outbuf, "\n");
					break;
				default:
					str_append(&outbuf, "; %s\n",p->name);
					for (i = 1; i <= n; i++) 
						for (j = 0; j <= p->assign_count; j++) 
							str_append(&outbuf, "(declare-const %s_p%d_%d (_ BitVec 32))\n",p->name,i,j);
					str_append(&outbuf, "\n");
					break;
			}
		}
	}
	str_append(&outbuf, "; var used in r%d\n",round_count+1);
	for (p = v_list; p != NULL; p = p->next) {
		if (!p->isHidden) {
			switch (p->type) {
				case TBOOL:
					for (i = 1; i <= n; i++) 
						str_append(&outbuf, "(declare-const %s_p%d_r%d_0 Bool)\n",p->name,i,round_count+1);
					break;
				case TVARRAY:
				case TINTARRAY:
				case TTSARRAY:
					semantic_error("array cannot be unhidden.");
					break;
				default:
					for (i = 1; i <= n; i++) 
						str_append(&outbuf, "(declare-const %s_p%d_r%d_0 (_ BitVec 32))\n",p->name,i,round_count+1);
					break;
			}
		}
	}
	for(i=1;i<=n;i++)
		str_append(&outbuf, "(declare-const dv_p%d_r%d_0 (_ BitVec 32))\n",i,round_count+1);
	str_append(&outbuf, "\n");

	str_append(&outbuf,var_outbuf);
	
	//fprintf(stdout,"%s",sizebuf);
	str_append(&outbuf,sizebuf);
	str_append(&outbuf,domain_outbuf);
	str_append(&outbuf,round_outbuf);
}