#include "translator.h"

static int curr_round = 0; // Current round number
static int decide_flag = 0; // Whether some decisions have been made or not
static int dv_assign_count = 0; // Number of assignments into dv

// Functions to generate formula
void gen_yices();
static void wrt_definitions(); // Function to output reserved variables
static void gen_v_dec();
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


void gen_yices() {
	int i = 0, j;
	asprintf(&guard, "true");
	asprintf(&domain, "  ;user define domains\n");
	asprintf(&assertion, "    true\n");
	asprintf(&assumption, "    true\n");
	gettokn(); /* "Algorithm" */
	gettokn(); /* <Identifier> */
	gettokn(); /* ";" */
	wrt_definitions(); /* write reserved variables */
	gen_v_dec(); /* encode variable declaration */
	gen_domain();

	gen_round_rep(); /* encode rounds */

	// X
	str_append(&outbuf, "; X\n");
	for (i = 1; i <= round_count; i++) {
		for (j = 1; j <= n; j++) {
			str_append(&outbuf, "(assert (round%d p%d %d (+ phase %d)))\n", i,
					j, j, (i - 1) * phase / round_count);
		}
	}
	str_append(&outbuf, "\n");

	if (inv_mode && token.kind != SINVARIANT) {
		fprintf(stderr, "Error: Missing Invariant Block.\n");
		remove(outfilename);
		exit(1);
	}

	if (token.kind == SINVARIANT) {
		gen_inv(); /* Encode Invariant */
		gettokn();
		if (inv_mode) { 
			str_append(&outbuf, "(check)\n");
			str_append(&outbuf, "(show-stats)\n");	
			return;
			}
	}

	if (token.kind != SUNIVALENCE) {
		fprintf(stderr, "Error: Missing Univalence Block.\n");
		remove(outfilename);
		exit(1);
	}
	gen_uni(); /* Encode Univalence */
	str_append(&outbuf, "(check)\n");
	str_append(&outbuf, "(show-stats)\n");	
}

static void gen_v_dec() {
	int i, j;
	VTYPE *p;

	while (token.kind != SROUND && token.kind != SDOMAIN)		//直接跳过变量声明
		gettokn();

	// User-defined variables
	str_append(&outbuf, "; variable dec\n");
	str_append(&outbuf, ";; user define\n");
	for (p = v_list; p != NULL; p = p->next) {
		switch (p->type) {
		case TBOOL:
			str_append(&outbuf, "(define %s::(-> process %sint bool))\n",
					p->name, !p->isHidden ? "round " : "");
			break;
		case TVARRAY:
		case TINTARRAY:
		case TTSARRAY:
			str_append(&outbuf, "(define %s::(-> process int int int))\n",
					p->name);
			break;
		default:
			str_append(&outbuf, "(define %s::(-> process %sint int))\n",
					p->name, !p->isHidden ? "round " : "");
			break;
		}
	}

	// System-defined variables
	str_append(&outbuf, ";; system define\n");
	str_append(&outbuf,
			"(define dv::(-> process round int int))  ; decision value\n"); // Decision values
	if (coord_flag) str_append(&outbuf,
			"(define coord::(-> process%s int))  ; coordinator\n",
			phase > 1 ? " int" : ""); // Coordinators
	str_append(&outbuf, "(define phase::int)  ; phase number\n"); // Phase number
	str_append(&outbuf,
			"(define rcv::(-> process round int data)) ; received data\n"); // Received messages

	str_append(&outbuf, "; |HO(p,r)|\n");
	str_append(&outbuf, "(define size::(-> process round int)\n"); // Number of received messages
	str_append(&outbuf, "  (lambda (_proc::process _r::round) (+\n");
	for (i = 1; i <= n; i++) {
		str_append(&outbuf, "    (ite (= (select (rcv _proc _r %d) 1) true) 1 0)\n", i);
	}
	str_append(&outbuf, "  ))\n");
	str_append(&outbuf, ")\n");
	str_append(&outbuf, "\n");

	str_append(&outbuf, "; |SHO(p,r)|\n");
	str_append(&outbuf, "(define ssize::(-> process round int)\n"); // Number of received messages
	str_append(&outbuf, "  (lambda (_proc::process _r::round) (+\n");
	for (i = 1; i <= n; i++) {
		str_append(&outbuf, "    (ite (and (= (select (rcv _proc _r %d) 1) true) (= (select (rcv _proc _r %d) 2) true)) 1 0)\n", i,i);
	}
	str_append(&outbuf, "  ))\n");
	str_append(&outbuf, ")\n");
	str_append(&outbuf, "\n");

	str_append(&outbuf, "; |AHO(p,r)|\n");
	str_append(&outbuf, "(define asize::(-> process round int)\n"); // Number of received messages
	str_append(&outbuf, "  (lambda (_proc::process _r::round)\n");
	str_append(&outbuf, "    (- (size _proc _r) (ssize _proc _r))\n", i,i);
	str_append(&outbuf, "  )\n");
	str_append(&outbuf, ")\n");
}
static void gen_domain(){
	int i, j;
	VTYPE *p;
	// Constraints to limit ranges of variables
	str_append(&outbuf, "(define domain::(-> process round bool)\n"); // Number of received messages
	str_append(&outbuf, "  (lambda (_proc::process _r::round) (and\n");
	
	for (p = v_list; p != NULL; p = p->next) { 					// Process variables
		if (p->isHidden || p->type == TBOOL) continue;
			str_append(&outbuf, "  (%s (%s _proc _r 0) 0)\n",(p->type == TV) ? ">" : ">=",p->name);
		}
	str_append(&outbuf, "  (> phase 0)\n"); // Phase
	 
	if (coord_flag) { // Coordinators
		if(phase==1)
			str_append(&outbuf,"  (>= (coord _proc) 1) (<= (coord _proc) N)\n");
		else
			for(j=1 ; j<=phase ; j++)
				str_append(&outbuf,"  (>= (coord _proc %d) 1) (<= (coord _proc %d) N)\n",j,j);	
	} 
	str_append(&outbuf, "  (>= (dv _proc _r 0) 0)\n");

	//Domain = "Domain" , "{" , statement-list , "}" , ";" ;
	if (token.kind == SDOMAIN) {
		gettokn();					//{
		gen_stmt_rep(&outbuf,DOM_MODE);
		str_append(&outbuf, domain);
		// }
		gettokn();					//;
		gettokn();					//Round
	}
	str_append(&outbuf, "  ))\n");
	str_append(&outbuf, ")\n");
	str_append(&outbuf, "; domains\n");
	for (i = 1; i <= n; i++) {
		for (j = 1; j <= round_count+1; j++) {
			str_append(&outbuf, "(assert (domain p%d r%d))\n", i,j);
		}
	}
	str_append(&outbuf, "\n");

	if (phase == 1) return;

	// Initialization of process variables
	str_append(&outbuf, "; initialization\n");
	for (i = 1; i <= n; i++) {
		//str_append(&outbuf, "(define _value%d::int)\n", i);
		str_append(&outbuf, "(declare-const _value%d Int)\n", i);
	}
	str_append(&outbuf, "(assert (and\n");
	str_append(&outbuf, "  (= phase 1)\n");
	for (i = 1; i <= n; i++) {
		str_append(&outbuf, "  (> _value%d 0)\n", i);
	}
	for (p = v_list; p != NULL; p = p->next) {
		if (p->isHidden || p->value == UNINI) continue;
		for (i = 1; i <= n; i++) {
			str_append(&outbuf, "  (= (%s p%d r1 0) ", p->name, i);
			switch (p->value) {
			case ANY_V:
				str_append(&outbuf, "_value%d", i);
				break;
			case TRUE:
				str_append(&outbuf, "true");
				break;
			case FALSE:
				str_append(&outbuf, "false");
				break;
			default:
				str_append(&outbuf, "%d", p->value);
				break;
			}
			str_append(&outbuf, ")\n");
		}
	}
	str_append(&outbuf, "))\n\n");
}

static void gen_round_rep() {
	do {
		curr_round++;
		gen_round(); /* Encode round */
	} while (gettokn() && token.kind == SROUND);
}

static void gen_round() {
	VTYPE *temp;

	/* "Round" */
	gettokn(); /* "{" */
	gettokn(); /* "SendPart" */
	str_append(&outbuf, "; round%d definition\n", curr_round);
	str_append(&outbuf, "(define round%d::(-> process int int bool)\n",
			curr_round);
	str_append(&outbuf,
			"  (lambda (_proc::process _pid::int _phase::int) (and\n");
	str_append(&outbuf, "    ;; send part\n");
	gen_s_part(); /* Encode message sending part */
	/* ";" */
	gettokn(); /* "TransitionPart" */
	str_append(&outbuf, "    ;; transition part\n");
	gen_t_part(); /* Encode state transition part */
	/* "}" */

	// Preparation to proceed next round
	for (temp = v_list; temp != NULL; temp = temp->next) {
		if (!temp->isHidden) {
			str_append(&outbuf,
					"    (= (%s _proc r%d 0) (%s _proc r%d %d))\n", temp->name,
					curr_round + 1, temp->name, curr_round, temp->assign_count);
			temp->assign_count = 0;
		}
		temp->isMessage = 0;
	}
	if (decide_flag == 0)
	str_append(&outbuf, "    (= (dv _proc r%d -1) 0)\n", curr_round);
	else {
		str_append(&outbuf, "    (= (dv _proc r%d -1) (dv _proc r%d %d))\n",
				curr_round, curr_round, dv_assign_count);
		dv_assign_count = 0;
		decide_flag = 0;
	}
	str_append(&outbuf, "  ))\n)\n\n");
	gettokn(); /* ";" */
}

static void gen_s_part() {
	int pid, isTowardCoord, else_flag = 0, if_flag = 0, send_coord = 0;
	VTYPE *v, *temp;
	char *condition = NULL;

	gettokn();
	if (token.kind == SIF) {
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
	if (token.kind != SACK) {
		do {
			if (token.kind == SCOORD) {
				send_coord = 1;
			} else if (token.kind == SIDENTIFIER && (v = search_variable(
					token.id)) != NULL) {
				v->isMessage = 1;
			}
			if (!gettokn() || token.kind != SCOMMA) break;
		} while (gettokn());
	} else gettokn();
	/* ">" */
	gettokn(); /* "," */
	gettokn();
	isTowardCoord = (token.kind == SALL) ? 0 : 1;
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
		str_append(&outbuf, "    ;;; from p%d\n", pid);
		str_append(&outbuf, "    (if ");
		if ((if_flag && !else_flag) || (!if_flag && isTowardCoord)) { // Send msgs with 'if' guard condition or Send msgs to coordinator without guard
			if (isTowardCoord) {
				str_append(&outbuf, "%s(= (coord p%d%s) _pid)%s",
						if_flag ? "(and " : "", pid,
						phase > 1 ? " _phase" : "", if_flag ? " " : "");
			}
			if (if_flag) replaceCondition(condition, &outbuf, pid);
			str_append(&outbuf, "%s\n", if_flag && isTowardCoord ? ")" : "");
		} else if (if_flag && isTowardCoord && else_flag) { // Send msgs to coordinator with 'if' and 'else' guard
			str_append(&outbuf, "(= (coord p%d%s) _pid)\n", pid,
					phase > 1 ? " _phase" : "");
		} else { // Send msgs to all processes without guard
			str_append(&outbuf, "true\n");
		}
		str_append(&outbuf, "      true\n");
		str_append(&outbuf, "      (= (select (rcv _proc r%d %d) 1) false)\n",
				curr_round, pid);
		str_append(&outbuf, "    )\n");

		if (!else_flag) { // Without 'else'
			if (coord_flag) {
				if (send_coord == 1) {
					str_append(&outbuf,"    (ite (= (select (rcv _proc r%d %d) 1) false) ",curr_round,pid);
					str_append(&outbuf,"(= (select (rcv _proc r%d %d) 3) NULL) true) ; coordinator\n",curr_round, pid);
					str_append(&outbuf,"    (ite (and (= (select (rcv _proc r%d %d) 1) true) (= (select (rcv _proc r%d %d) 2) true) )  ",curr_round,pid,curr_round,pid);
					str_append(&outbuf,"(= (select (rcv _proc r%d %d) 3) (coord p%d%s)) true)\n",curr_round,pid,pid,phase > 1 ? " _phase" : "");
					str_append(&outbuf,"    (ite (and (= (select (rcv _proc r%d %d) 1) true) (= (select (rcv _proc r%d %d) 2) false) )  ",curr_round,pid,curr_round,pid);
					str_append(&outbuf,"(and (not (= (select (rcv _proc r%d %d) 3) (coord p%d%s))) (> (select (rcv _proc r%d %d) 3) 0)) true)\n",curr_round,pid,pid,phase > 1 ? " _phase" : "",curr_round,pid);
				} else if (send_coord == 0) {
					str_append(
							&outbuf,
							"    (= (select (rcv _proc r%d %d) 3) NULL) ; coordinator\n",
							curr_round, pid);
				}
			}
			for (temp = v_list; temp != NULL; temp = temp->next) {
				if (temp->isMessage) {
					str_append(&outbuf,"    (ite (= (select (rcv _proc r%d %d) 1) false) ",curr_round,pid);
					str_append(&outbuf,"(= (select (rcv _proc r%d %d) %d) NULL) true) ; %s\n",curr_round, pid,temp->other_field+1,temp->name);
					str_append(&outbuf,"    (ite (and (= (select (rcv _proc r%d %d) 1) true) (= (select (rcv _proc r%d %d) 2) true) )  ",curr_round,pid,curr_round,pid);
					str_append(&outbuf,"(= (select (rcv _proc r%d %d) %d) (%s p%d r%d 0)) true)\n",curr_round,pid,temp->other_field+1,temp->name,pid,curr_round);
					str_append(&outbuf,"    (ite (and (= (select (rcv _proc r%d %d) 1) true) (= (select (rcv _proc r%d %d) 2) false) )  ",curr_round,pid,curr_round,pid);
					str_append(&outbuf,"(and (not (= (select (rcv _proc r%d %d) %d) (%s p%d r%d 0))) (> (select (rcv _proc r%d %d) %d) 0)) true)\n",curr_round,pid,temp->other_field+1,temp->name,pid,curr_round,curr_round,pid,temp->other_field+1);
				} else if (!temp->isHidden && temp->type != TBOOL) {
					str_append(
							&outbuf,
							"    (= (select (rcv _proc r%d %d) %d) NULL) ; %s\n",
							curr_round, pid, temp->other_field+1, temp->name);
				}
			}
		} else { // With 'else'
			str_append(&outbuf, "    (if ");
			replaceCondition(condition, &outbuf, pid);
			str_append(&outbuf, " (and\n");
			if (coord_flag) {
				if (send_coord == 1) {
					str_append(&outbuf,"    (ite (= (select (rcv _proc r%d %d) 1) false) ",curr_round,pid);
					str_append(&outbuf,"(= (select (rcv _proc r%d %d) 3) NULL) true) ; coordinator\n",curr_round, pid);
					str_append(&outbuf,"    (ite (and (= (select (rcv _proc r%d %d) 1) true) (= (select (rcv _proc r%d %d) 2) true) )  ",curr_round,pid,curr_round,pid);
					str_append(&outbuf,"(= (select (rcv _proc r%d %d) 3) (coord p%d%s)) true)\n",curr_round,pid,pid,phase > 1 ? " _phase" : "");
					str_append(&outbuf,"    (ite (and (= (select (rcv _proc r%d %d) 1) true) (= (select (rcv _proc r%d %d) 2) false) )  ",curr_round,pid,curr_round,pid);
					str_append(&outbuf,"(and (not (= (select (rcv _proc r%d %d) 3) (coord p%d%s))) (> (select (rcv _proc r%d %d) 3) 0)) true)\n",curr_round,pid,pid,phase > 1 ? " _phase" : "",curr_round,pid);
				} else if (send_coord == 0) {
					str_append(
							&outbuf,
							"    (= (select (rcv _proc r%d %d) 3) NULL) ; coordinator\n",
							curr_round, pid);
				}
			}
			for (temp = v_list; temp != NULL; temp = temp->next) {
				if (temp->isMessage == 1) {
					str_append(&outbuf,"    (ite (= (select (rcv _proc r%d %d) 1) false) ",curr_round,pid);
					str_append(&outbuf,"(= (select (rcv _proc r%d %d) %d) NULL) true) ; %s\n",curr_round, pid,temp->other_field+1,temp->name);
					str_append(&outbuf,"    (ite (and (= (select (rcv _proc r%d %d) 1) true) (= (select (rcv _proc r%d %d) 2) true) )  ",curr_round,pid,curr_round,pid);
					str_append(&outbuf,"(= (select (rcv _proc r%d %d) %d) (%s p%d r%d 0)) true)\n",curr_round,pid,temp->other_field+1,temp->name,pid,curr_round);
					str_append(&outbuf,"    (ite (and (= (select (rcv _proc r%d %d) 1) true) (= (select (rcv _proc r%d %d) 2) false) )  ",curr_round,pid,curr_round,pid);
					str_append(&outbuf,"(and (not (= (select (rcv _proc r%d %d) %d) (%s p%d r%d 0))) (> (select (rcv _proc r%d %d) %d) 0)) true)\n",curr_round,pid,temp->other_field+1,temp->name,pid,curr_round,curr_round,pid,temp->other_field+1);
				} else if (!temp->isHidden && temp->type != TBOOL) {
					str_append(
							&outbuf,
							"    (= (select (rcv _proc r%d %d) %d) NULL) ; %s\n",
							curr_round, pid, temp->other_field+1, temp->name);
				}
			}
			str_append(&outbuf, "    ) (and\n");
			if (send_coord == 2) {
				str_append(&outbuf,"    (ite (= (select (rcv _proc r%d %d) 1) false) ",curr_round,pid);
				str_append(&outbuf,"(= (select (rcv _proc r%d %d) 3) NULL) true) ; coordinator\n",curr_round, pid);
				str_append(&outbuf,"    (ite (and (= (select (rcv _proc r%d %d) 1) true) (= (select (rcv _proc r%d %d) 2) true) )  ",curr_round,pid,curr_round,pid);
				str_append(&outbuf,"(= (select (rcv _proc r%d %d) 3) (coord p%d%s)) true)\n",curr_round,pid,pid,phase > 1 ? " _phase" : "");
				str_append(&outbuf,"    (ite (and (= (select (rcv _proc r%d %d) 1) true) (= (select (rcv _proc r%d %d) 2) false) )  ",curr_round,pid,curr_round,pid);
				str_append(&outbuf,"(and (not (= (select (rcv _proc r%d %d) 3) (coord p%d%s))) (> (select (rcv _proc r%d %d) 3) 0)) true)\n",curr_round,pid,pid,phase > 1 ? " _phase" : "",curr_round,pid);
			} else if (send_coord == 0) {
				str_append(
						&outbuf,
						"    (= (select (rcv _proc r%d %d) 3) NULL) ; coordinator\n",
						curr_round, pid);
			}
			for (temp = v_list; temp != NULL; temp = temp->next) {
				if (temp->isMessage == 2) {
					str_append(&outbuf,"    (ite (= (select (rcv _proc r%d %d) 1) false) ",curr_round,pid);
					str_append(&outbuf,"(= (select (rcv _proc r%d %d) %d) NULL) true) ; %s\n",curr_round, pid,temp->other_field+1,temp->name);
					str_append(&outbuf,"    (ite (and (= (select (rcv _proc r%d %d) 1) true) (= (select (rcv _proc r%d %d) 2) true) )  ",curr_round,pid,curr_round,pid);
					str_append(&outbuf,"(= (select (rcv _proc r%d %d) %d) (%s p%d r%d 0)) true)\n",curr_round,pid,temp->other_field+1,temp->name,pid,curr_round);
					str_append(&outbuf,"    (ite (and (= (select (rcv _proc r%d %d) 1) true) (= (select (rcv _proc r%d %d) 2) false) )  ",curr_round,pid,curr_round,pid);
					str_append(&outbuf,"(and (not (= (select (rcv _proc r%d %d) %d) (%s p%d r%d 0))) (> (select (rcv _proc r%d %d) %d) 0)) true)\n",curr_round,pid,temp->other_field+1,temp->name,pid,curr_round,curr_round,pid,temp->other_field+1);
				} else if (!temp->isHidden && temp->type != TBOOL) {
					str_append(
							&outbuf,
							"    (= (select (rcv _proc r%d %d) %d) NULL) ; %s\n",
							curr_round, pid, temp->other_field+1, temp->name);
				}
			}
			str_append(&outbuf, "))\n");
		}

	}
	if (condition != NULL) free(condition);
}

static void gen_t_part() {
	gen_stmt_rep(&outbuf, NORMAL_MODE); /* Encode statements */
}

static void gen_inv() {
	/* "Invariant" */
	gettokn(); /* "{" */

	str_append(&outbuf, "; Constraints for INV\n");
	str_append(&outbuf, "(define inv::(-> process round int bool)\n");
	str_append(&outbuf,
			"  (lambda (_proc::process _r::round _phase::int) (and\n");
	str_append(&outbuf, "    true\n");
	gen_stmt_rep(&outbuf, INV_MODE); /* Encode statements */
	str_append(&outbuf, "  ))\n");
	str_append(&outbuf, ")\n");
	str_append(&outbuf, ";; Assumptions for INV\n");
	str_append(&outbuf,
			"(define inv_assumptions::(-> process round int bool)\n");
	str_append(&outbuf,
			"  (lambda (_proc::process _r::round _phase::int) (and\n");
	str_append(&outbuf, assumption);
	str_append(&outbuf, "  ))\n");
	str_append(&outbuf, ")\n\n");
	str_append(&outbuf, "; INV\n");
	str_append(&outbuf, "(assert (and\n");
	str_append(&outbuf, "  (inv p1 r1 phase)\n");
	str_append(&outbuf, "  (inv_assumptions p1 r1 phase)\n");
	str_append(&outbuf, "))\n\n");
	if (inv_mode) {
		str_append(&outbuf, "; INV'\n");
		str_append(&outbuf, "(assert (and\n");
		str_append(&outbuf, "  (inv p2 r%d (+ phase 1))\n", round_count + 1);
		str_append(&outbuf, "  (not (inv_assumptions p2 r%d (+ phase 1)))\n",
				round_count + 1);
		str_append(&outbuf, "))\n\n");
	}
	/* "}" */
	gettokn(); /* ";" */
}

static void gen_uni() {
	int i, j;

	/* "Univalence" */
	gettokn(); /* "{" */

	str_append(&outbuf, "; Constraints for Agr\n");
	str_append(&outbuf, "(define _v::int)\n");
	str_append(&outbuf, "(define univ::(-> process round bool)\n");
	str_append(&outbuf, "  (lambda (_proc::process _r::round) (and\n");
	str_append(&outbuf, "    true\n");
	gen_stmt_rep(&outbuf, UNI_MODE); /* Encode statements */
	str_append(&outbuf, "  ))\n");
	str_append(&outbuf, ")\n");
	str_append(&outbuf, ";; Assertions for Agr\n");
	str_append(&outbuf, "(define univ_assertions::(-> process round bool)\n");
	str_append(&outbuf, "  (lambda (_proc::process _r::round) (and\n");
	str_append(&outbuf, assertion);
	str_append(&outbuf, "  ))\n");
	str_append(&outbuf, ")\n\n");
	if (agr_mode) { // Agreement
		str_append(&outbuf, ";; Choose for _v\n");
		str_append(&outbuf, "(assert\n");
		str_append(&outbuf, "  (ite\n");
		str_append(&outbuf, "    (or\n");
		for (i = 1; i <= n; i++) {
			for (j = 1; j <= round_count; j++) {
				str_append(&outbuf, "      (/= (dv p%d r%d -1) 0)\n", i, j);
			}
		}
		str_append(&outbuf, "    )\n");
		str_append(&outbuf, "    (or\n");
		for (i = 1; i <= n; i++) {
			for (j = 1; j <= round_count; j++) {
				str_append(
						&outbuf,
						"      (and (/= (dv p%d r%d -1) 0) (= (dv p%d r%d -1) _v))\n",
						i, j, i, j);
			}
		}
		str_append(&outbuf, "    )\n");
		str_append(&outbuf, "    (= _v 0)\n");
		str_append(&outbuf, "  )\n");
		str_append(&outbuf, ")\n");
		str_append(&outbuf,
				";; if some process decided, then all processes must obey \"univ\"\n");
		str_append(&outbuf, "(assert (=> (/= _v 0) (univ p1 r%d)))\n\n",
				round_count + 1);
		str_append(&outbuf, "; Not Agr\n");
		str_append(&outbuf, "(assert (not (or\n");
		str_append(&outbuf, "  (= _v 0)\n");
		str_append(&outbuf, "  (and\n");
		str_append(&outbuf, "    (/= _v 0)\n");
		for (i = 1; i <= n; i++) {
			for (j = 1; j <= round_count; j++) {
				str_append(
						&outbuf,
						"    (or (= (dv p%d r%d -1) 0) (= (dv p%d r%d -1) _v))\n",
						i, j, i, j);
			}
		}
		str_append(&outbuf, "    (univ_assertions p1 r%d)\n", round_count + 1);
		str_append(&outbuf, "  )\n");
		str_append(&outbuf, ")))\n\n");
	} else if (uni_mode) { // Univalence
		str_append(&outbuf, "; Univ\n");
		str_append(&outbuf, "(assert (and\n");
		str_append(&outbuf, "  (and\n");
		str_append(&outbuf, "    (univ p1 r1)\n");
		str_append(&outbuf, "    (univ_assertions p1 r1)\n");
		str_append(&outbuf, "  )\n");
		str_append(&outbuf, "  (or\n");
		str_append(&outbuf, "    (not (and\n");
		for (i = 1; i <= n; i++) {
			for (j = 1; j <= round_count; j++) {
				str_append(
						&outbuf,
						"      (or (= (dv p%d r%d -1) 0) (= (dv p%d r%d -1) _v))\n",
						i, j, i, j);
			}
		}
		str_append(&outbuf, "    ))\n");
		str_append(&outbuf, "    (and\n");
		str_append(&outbuf, "      (univ p2 r%d)\n", round_count + 1);
		str_append(&outbuf, "      (not (univ_assertions p2 r%d))\n",
				round_count + 1);
		str_append(&outbuf, "    )\n");
		str_append(&outbuf, "  )\n");
		str_append(&outbuf, "))\n\n");
	}
	/* "}" */
	gettokn(); /* ";" */
}

static void gen_stmt_rep(char **buff, int mode) {
	gettokn();
	do {
		gen_stmt(buff, mode); /* Encode statement */
		/* ";" */
		gettokn();
	} while (token.kind != SRBRACE);
}

static void gen_stmt(char **buff, int mode) {
	VTYPE *v;
	int idx;
	char *new_guard = NULL, *org_guard = NULL;
	switch (token.kind) {
	case SIDENTIFIER: /* assignment */
		v = search_variable(token.id);
		if (v->type != TINTARRAY && v->type != TVARRAY && v->type != TTSARRAY) { // Non-array variable
			if (!v->isHidden) { // Process variable	x,ack...
				str_append(buff, "    (= (%s _proc r%d %d) (ite (and %s) ",
						v->name, curr_round, (v->assign_count) + 1, guard);
			} else if (v->other_field == 0) { // Hidden variable except loop counter
				str_append(buff, "    (= (%s _proc %d) (ite (and %s) ",
						v->name, (v->assign_count) + 1, guard);
			} else { // Loop counter
				gettokn(); /* "=" */
				v->value = calc_expr(); /* Evaluate */
				break;
			}
		} else { // Array variable
			gettokn(); /* "[" */
			str_append(buff, "    (= (%s _proc ", v->name);
			idx = calc_expr(); /* Evaluate */
			if (idx < 1 || n < idx) semantic_error("index is out of range."); // Check range
			str_append(buff, "%d %d) (ite (and %s) ", idx,
					(v->assign_count_elem[idx - 1]) + 1, guard);
			/* "]" */
		}
		gettokn(); /* "=" */
		gen_expr(buff, mode); /* Encode expression */
		if (v->type != TINTARRAY && v->type != TVARRAY && v->type != TTSARRAY) { // Non-array variable
			if (!v->isHidden) { // Process variable
				str_append(buff, " (%s _proc r%d %d)))\n", v->name,
						curr_round, (v->assign_count)++);
			} else { // Hidden variable
				str_append(buff, " (%s _proc %d)))\n", v->name,
						(v->assign_count)++);
			}
		} else { // Array variable
			str_append(buff, " (%s _proc %d %d)))\n", v->name, idx,
					(v->assign_count_elem[idx - 1])++);
		}
		break;
	case SDECIDE: /* Decide statement */
		decide_flag = 1;
		str_append(buff, "    (= (dv _proc r%d %d) (ite (and %s) ",
				curr_round, ++dv_assign_count, guard);
		gettokn(); /* "(" */
		gen_expr(buff, mode); /* Encode expression */
		/* ")" */
		gettokn();
		if (dv_assign_count == 1) str_append(buff, " 0))\n");
		else str_append(buff, " (dv _proc r%d %d)))\n", curr_round,
				dv_assign_count - 1);
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
			str_append(&domain, "  ", guard);
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
		error("parse error.");
	}
}

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

static void gen_equality_expr(char **buff, int mode) {
	char *op1 = NULL;

	gen_relational_expr(&op1, mode);
	if (!isEqualityOperator(token.kind)) {
		str_append(buff, op1);
	} else {
		str_append(buff, "(%s %s ", token.id, op1);
		gen_relational_expr(buff, mode);
		str_append(buff, ")");
	};
	free(op1);
}

static void gen_relational_expr(char **buff, int mode) {
	char *op1 = NULL;

	gen_simple_expr(&op1, mode);
	if (!isRelationalOperator(token.kind)) {
		str_append(buff, op1);
	} else {
		str_append(buff, "(%s %s ", token.id, op1);
		gen_simple_expr(buff, mode);
		str_append(buff, ")");
	};
	free(op1);
}

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
		str_append(&op2, "(%s %s ", token.id, op1);
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
		str_append(&op2, "(%s %s ", token.id, op1);
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

static void gen_factor(char **buff, int mode) {
	VTYPE *v;
	int idx;
	char *expression = NULL;

	gettokn();
	switch (token.kind) {
	case SIDENTIFIER:
		v = search_variable(token.id);
		if (v->type != TINTARRAY && v->type != TVARRAY && v->type != TTSARRAY) { // Non-array variable
			if (mode == NORMAL_MODE || mode == SEND_MODE) { // In round
				if (mode == NORMAL_MODE && v->isHidden && v->other_field >= 0) { // Hidden variable except loop counter
					str_append(buff, "(%s _proc %d)", v->name,
							v->assign_count);
				} else if (mode == NORMAL_MODE && v->other_field < 0) { // Loop counter
					str_append(buff, "%d", v->value);
				} else if (!v->isHidden) { // Process variable
					str_append(buff, "(%s _proc r%d %d)", v->name,
							curr_round, v->assign_count);
				}
			} else if (mode == INV_MODE || mode == UNI_MODE) { // In Invariant，Univalence
				if (!v->isHidden) { // Process variable
					gettokn(); /* "[" */
					idx = calc_expr(); /* Evaluate */
					if (idx < 1 || idx > n) semantic_error(
							"index is out of range.");
					/* "]" */
					str_append(buff, "(%s p%d _r 0)", v->name, idx);
				} else { // Hidden variable
					if (v->other_field < 0) str_append(buff, "%d", v->value); // Loop counter
					else str_append(buff, "(%s _proc %d)", v->name,
							v->assign_count); // Others
				}
			}
		} else { // Array variable
			gettokn(); /* "[" */
			idx = calc_expr(); /* Evaluate */
			if (idx < 1 || n < idx) semantic_error("index is out of range."); // Check range
			str_append(buff, "(%s _proc %d %d)", v->name, idx,
					v->assign_count_elem[idx - 1]);
			/* "]" */
		}
		break;
	case SQUESTION:
		str_append(buff, "0");
		break;
	case SFALSE:
		str_append(buff, "false");
		break;
	case STRUE:
		str_append(buff, "true");
		break;
	case SNUM:
		str_append(buff, "%d", atoi(token.id));
		break;
	case SPROC_COUNT:
		str_append(buff, "N");
		break;
	case SSIZE:							//SIZE
		if(mode == DOM_MODE)
			str_append(buff, "(size _proc _r)");
		else
			str_append(buff, "(size _proc r%d)", curr_round);
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
		str_append(buff, "(ssize _proc _r)");
		break;
	case SASIZE:
		str_append(buff, "(asize _proc _r)");
		break;
	case SNULL:
		str_append(buff, "NULL");
		break;
	case SPHASE:
		str_append(buff, "_phase");
		break;
	case SISCOORD:
		str_append(buff, "(= (coord _proc%s) _pid)", phase > 1 ? " _phase"
				: "");
		break;
	case SCOORD:
		str_append(buff, "(coord _proc%s)", phase > 1 ? " _phase" : "");
		break;
	case SPID:
		str_append(buff, "_pid");
		break;
	case SVVAL:
		str_append(buff, "_v");
		break;
	case SRCV:
		gettokn(); /* "[" */
		gen_expr(&expression, mode); /* Encode expression */
		/* "]" */
		gettokn(); /* "." */
		gettokn();
		if (token.kind == SCOORD) {
			str_append(buff, "(select (rcv _proc r%d %s) 3)", curr_round,
					expression);
		} else if (token.kind == SIDENTIFIER && (v = search_variable(token.id))
				!= NULL && v->isMessage) {
			str_append(buff, "(select (rcv _proc r%d %s) %d)", curr_round,
					expression, v->other_field+1);
		}
		free(expression);
		break;
	case SLPAREN:
		gen_expr(buff, mode); /* Encode expression */
		/* ")" */
		break;
	default:
		error("parse error.");
	};
}

static void wrt_definitions() {
	int i;
	VTYPE *p;

	// Constants
	str_append(&outbuf, "; constant\n");
	str_append(&outbuf, "(define N::int %d) ; Number of processes\n", n);
	str_append(&outbuf, "(define NULL::int -1) ; Failure\n");
	str_append(&outbuf,"(define T::int) ; Therehold\n");
	str_append(&outbuf,"(define E::int) ; Enough\n");
	str_append(&outbuf,"(define a::int) ; α\n");
	str_append(&outbuf, "\n");

	str_append(&outbuf, "; type dec\n");
	// Process
	str_append(&outbuf, "(define-type process (scalar p1");
	for (i = 2; i <= n; i++) {
		str_append(&outbuf, " p%d", i);
	}
	str_append(&outbuf, "))\n");
	// Round
	str_append(&outbuf, "(define-type round (scalar r1");
	for (i = 2; i <= round_count; i++) {
		str_append(&outbuf, " r%d", i);
	}
	str_append(&outbuf, " r%d))\n", round_count + 1);
	// Message table
	str_append(&outbuf, ";; TABLE is [ack ,safe");
	if (coord_flag) str_append(&outbuf, ", coordinator");
	for (p = v_list; p != NULL; p = p->next) {
		if (!p->isHidden && p->type != TBOOL) str_append(&outbuf, ", %s",
				p->name);
	}
	str_append(&outbuf, "]\n");
	str_append(&outbuf, "(define-type data (tuple bool bool");
	if (coord_flag) str_append(&outbuf, " int");
	for (p = v_list; p != NULL; p = p->next) {
		if (!p->isHidden && p->type != TBOOL) str_append(&outbuf, " int");
	}
	str_append(&outbuf, "))\n\n");
}
