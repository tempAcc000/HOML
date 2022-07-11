#include "translator.h"

#define NOCHECK -1
#define NONE -9999

VTYPE *v_list = NULL; // Pointer to the list of variables
int n = 3; // Number of processes (default is 3)
int round_count = 0; // Number of rounds
int phase = 1; // Number of modeled phases
int coord_flag; // enable coordinators
int sendCoord = 0; // Coodinator's id becomes a message in current parsing round : 1
int deapth_of_nest = 0;

// Get new token
int next_tokn(FILE *fp, int kind);
int next_tokn2(FILE *fp, int kind);
// Functions relate to parsing
void prs_alg();
void prs_v_dec();
void prs_dom();
void prs_round_rep();
void prs_round();
void prs_s_part();
void prs_t_part();
void prs_inv();
void prs_uni();
void prs_stmt_rep(FILE *dest, int mode);
void prs_stmt(FILE *dest, int mode);
enum TYPE prs_expr(FILE *fp, int mode);
enum TYPE prs_and_expr(FILE *fp, int mode);
enum TYPE prs_equality_expr(FILE *fp, int mode);
enum TYPE prs_relational_expr(FILE *fp, int mode);
enum TYPE prs_simple_expr(FILE *fp, int mode);
enum TYPE prs_term(FILE *fp, int mode);
enum TYPE prs_factor(FILE *fp, int mode);

// Token judgment functions
int isEqualityOperator(enum SYMBOL kind);
int isRelationalOperator(enum SYMBOL kind);
int isAdditiveOperator(enum SYMBOL kind);
int isMultiplicativeOperator(enum SYMBOL kind);

// Check whether identifier is already reserved or not
int isReservedKeyword(const char *keyword);

// Functions to operate the variable table
void insert_variable(char *name, enum TYPE type, int value, int send,
		int hidden, int assign_count, int *ptr, int other_field);
VTYPE *search_variable(char *name);
/*
algorithm = "Algorithm" , algorithm-name, ";" , 
            variable-declaration , 
	        [domain] ,
            round-list , 
            [Invariant] , 
            [Univalence] ;
*/

void prs_alg() {
	VTYPE *v;

	next_tokn(outfile, SALGORITHM);						//Algorithm
	next_tokn(outfile, SIDENTIFIER);					//Algorithm name
	next_tokn(outfile, SSEMICOLON);						//;
	prs_v_dec();										//变量声明
	if (token.kind == SDOMAIN) {						//DOMAIN
		prs_dom();
		next_tokn(outfile, NONE);
	}
	prs_round_rep();									//round
	if (token.kind == SINVARIANT) {						//Invarant
		prs_inv();
		next_tokn(outfile, NONE);
	}
	if (token.kind == SUNIVALENCE) {					//Uninvarant
		prs_uni();
	}
	if (next_tokn(outfile, NONE)) syntax_error("syntax error.");

	if (coord_flag) {										//?
		for (v = v_list; v != NULL; v = v->next) {
			if (v->isHidden || v->type == TBOOL) continue;
			v->other_field++;
		}
	}
}

void prs_v_dec() {
	char name[NAME_LEN];
	enum TYPE type;
	int value = 0, hidden = 0, count = 1;
	int *ptr = NULL;

	next_tokn(outfile, SVARIABLE);					//Variable
	next_tokn(outfile, NOCHECK);					//暂不检查
	do {
		if (token.kind == SHIDDEN) {
			hidden = 1;
			next_tokn(outfile, NOCHECK);
		}
		switch (token.kind) {
		case SV:									//V
			type = TV;
			break;
		case SVD:									//V'
			type = TVD;
			break;
		case STS:									//TS
			type = TTS;				
			break;
		case SVARRAY:								//VArray
			if (!hidden) semantic_error("VArray must be declared as hidden.");
			type = TVARRAY;
			break;
		case STSARRAY:								//TSArray
			if (!hidden) semantic_error("TSArray must be declared as hidden.");
			type = TTSARRAY;
			break;
		case SBOOL:									//bool
			type = TBOOL;
			break;
		case SINT:									//int
			if (!hidden) semantic_error("int must be declared as hidden.");
			type = TINT;
			break;
		case SINTARRAY:								//intarray
			if (!hidden) semantic_error("intArray must be declared as hidden.");
			type = TINTARRAY;
			break;
		default:
			syntax_error("syntax error.");
		};
		do {
			next_tokn(outfile, SIDENTIFIER);								//变量名
			strcpy(name, token.id);
			if (isReservedKeyword(name)) semantic_error(					//是保留字
					"already reserved name.");
			next_tokn(outfile, NOCHECK);
			if (token.kind == SASSIGN) { // initialization of variable	=
				next_tokn(outfile, NOCHECK);
				switch (token.kind) {
				case SQUESTION:												//?
					if (type != TVD) semantic_error(
							"cannot be initialized with ?.");
					value = UNDEF;
					break;
				case SFALSE:
					if (type != TBOOL) semantic_error(
							"cannot be initialized with false.");
					value = FALSE;
					break;
				case STRUE:
					if (type != TBOOL) semantic_error(
							"cannot be initialized with true.");
					value = TRUE;
					break;
				case SNUM:
					if (type != TTS) semantic_error(
							"cannot be initialized with integer.");
					value = atoi(token.id);
					break;
				case SVP:													//Vp
					if (type != TV && type != TVD) semantic_error(
							"cannot be initialized with Vp.");
					value = ANY_V;
					break;
				default:
					syntax_error("syntax error.");
				}
				next_tokn(outfile, NOCHECK);
			} else value = UNINI; // no initialization
			if (type == TINTARRAY || type == TVARRAY || type == TTSARRAY) {
				ptr = (int *) calloc(n, sizeof(int));
				if (ptr == NULL) error("memory allocation error.");
			};
			if (search_variable(name) != NULL) semantic_error(
					"already declared name.");
			if (!hidden && type != TBOOL) // sendable variables
				insert_variable(name, type, value, 0, hidden, 0, ptr, ++count);
			else // unsendable variables
				insert_variable(name, type, value, 0, hidden, 0, ptr, 0);
		} while (token.kind == SCOMMA);
		if (token.kind != SSEMICOLON) syntax_error("syntax error.");
		next_tokn(outfile, NOCHECK);
	} while (token.kind != SROUND && token.kind != SDOMAIN);				//round domain
}

//Domain = "Domain" , "{" , statement-list , "}" , ";" ;

void prs_dom() {
	next_tokn(outfile, SLBRACE);								//{
	prs_stmt_rep(outfile, DOM_MODE);					
	if (token.kind != SRBRACE) syntax_error("syntax error.");	//}
	next_tokn(outfile, SSEMICOLON);								//;
}
/*
round-list = round , ";" , { round ";" } ; 
round = "Round" , 
        "{" , 
        "SendPart" , send-part , ";" , 
        "TransitionPart" , transition-part, 
        "}" ; 
*/
void prs_round_rep() {
	do {
		prs_round();
		next_tokn(outfile, SSEMICOLON);				//;
		round_count++;
	} while (next_tokn(outfile, NONE) && token.kind == SROUND);
	round_count *= phase;
}

void prs_round() {
	VTYPE *temp;

	if (token.kind != SROUND) syntax_error("syntax error.");		//round
	next_tokn(outfile, SLBRACE);									//{
	next_tokn(outfile, SSP);										//SendPart
	prs_s_part();
	if (token.kind != SSEMICOLON) syntax_error("syntax error.");
	next_tokn(outfile, STP);										//TransitionPart
	prs_t_part();
	if (token.kind != SRBRACE) syntax_error("syntax error.");		//}
	for (temp = v_list; temp != NULL; temp = temp->next)			
		temp->isMessage = 0; // reset message flag					//isMessage
}

/*
send-part =  "if" , "(" expression , ")" , "send" , "(" , message , "," , destination , ")" ,
             [ "else" , "send" , "(" , message , "," , destination , ")" ] |
             "send" , "(" , message , "," , destination , ")" ;
message = "<" , variable-name | "Coord" , { "," , variable-name | "Coord" } , ">" |
        	"<" "ack" ">" ;
*/
void prs_s_part() {													//SendPart
	int if_flag;
	enum SYMBOL dest;
	VTYPE *v;

	if_flag = 0;
	sendCoord = 0;
	next_tokn(outfile, NOCHECK);
	if (token.kind == SIF) {											//if
		if_flag = 1;
		next_tokn(outfile, SLPAREN);									//(
		if (prs_expr(outfile, SEND_MODE) != TBOOL) semantic_error(		//expression
				"type mismatch.");
		if (token.kind != SRPAREN) syntax_error("syntax error.");		//)
		next_tokn(outfile, NOCHECK);
	}
	if (token.kind != SSEND) syntax_error("syntax error.");				//send
	next_tokn(outfile, SLPAREN);										//(
	/*
	发送variable或coord coord表示协调器的ID
	message = "<" , variable-name | "Coord" , { "," , variable-name | "Coord" } , ">" |
		"<" "ack" ">" ;
	*/
	next_tokn(outfile, SLESS);											//<
	next_tokn(outfile, NOCHECK);						
	if (token.kind != SACK) { // send message							不是ack
		do {
			if (token.kind != SIDENTIFIER && token.kind != SCOORD) syntax_error(		//identifier
					"syntax error.");
			if (token.kind == SCOORD)													//Coord
				sendCoord = 1;															//发送Coord ID
			else {
				if ((v = search_variable(token.id)) == NULL || v->isHidden				//is hidden?		
						|| v->type == TBOOL) {
					semantic_error("cannot be used as message.");
				} else v->isMessage = 1; // the variable is used as message				被用作message
			}
			next_tokn(outfile, NOCHECK);
			if (token.kind != SCOMMA) break;											//,
		} while (next_tokn(outfile, NOCHECK));
	} else { // send only Ack
		next_tokn(outfile, NOCHECK);
	};
	if (token.kind != SGREAT) syntax_error("syntax error.");						//>
	next_tokn(outfile, SCOMMA);														//,

	next_tokn(outfile, NOCHECK);
	switch (token.kind) {														//destination All | Coord
	case SALL:
	case SCOORD:
		dest = token.kind;
		break;
	default:
		syntax_error("syntax error.");
	}
	next_tokn(outfile, SRPAREN);												//)

	next_tokn(outfile, NOCHECK);
	if (if_flag && token.kind == SELSE) {										//else
		next_tokn(outfile, SSEND);												//send
		next_tokn(outfile, SLPAREN);											//(
		next_tokn(outfile, SLESS);												//<
		next_tokn(outfile, NOCHECK);					
		if (token.kind != SACK) { // send message
			do {
				if (token.kind != SIDENTIFIER && token.kind != SCOORD) syntax_error(	//identifier
						"syntax error.");
				if (token.kind == SCOORD)												//Coord
					sendCoord = 2;														//=2表示在else中
				else {
					if ((v = search_variable(token.id)) == NULL || v->isHidden
							|| v->type == TBOOL) {
						semantic_error("cannot be used as message.");
					} else {
						v->isMessage = 2; // the variable is used as message in else block
					}
				}
				next_tokn(outfile, NOCHECK);
				if (token.kind != SCOMMA) break;					//,
			} while (next_tokn(outfile, NOCHECK));
		} else { // send only Ack
			next_tokn(outfile, NOCHECK);
		};
		if (token.kind != SGREAT) syntax_error("syntax error.");				//>
		next_tokn(outfile, SCOMMA);							//,
		next_tokn(outfile, NOCHECK);
		switch (token.kind) {								//destination All | Coord
		case SALL:
		case SCOORD:
			if (token.kind != dest) semantic_error("inconsistent destination.");
			break;
		default:
			syntax_error("syntax error.");
		}
		next_tokn(outfile, SRPAREN);										//)
		next_tokn(outfile, NOCHECK);
	} else if (!if_flag && token.kind == SELSE) syntax_error("syntax error.");
}

//transition-part = statement-list ;
void prs_t_part() {
	prs_stmt_rep(outfile, NORMAL_MODE);
}
//Invariant = "Invariant" , "{" , statement-list , "}" , ";" ;
void prs_inv() {
	next_tokn(outfile, SLBRACE);								//{
	prs_stmt_rep(outfile, INV_MODE);					
	if (token.kind != SRBRACE) syntax_error("syntax error.");	//}
	next_tokn(outfile, SSEMICOLON);								//;
}
//Univalence = "Univalence" , "{" , statement-list , "}" , ";" ;
void prs_uni() {
	next_tokn(outfile, SLBRACE);								//{
	prs_stmt_rep(outfile, UNI_MODE);
	if (token.kind != SRBRACE) syntax_error("syntax error.");
	next_tokn(outfile, SSEMICOLON);								//;
}
//statement-list = statement , ";" , { statement , ";" } 
void prs_stmt_rep(FILE *dest, int mode) {
	next_tokn(dest, NOCHECK);
	do {
		prs_stmt(dest, mode);
		if (token.kind != SSEMICOLON) syntax_error("syntax error.");		//;
		next_tokn(dest, NOCHECK);
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
*/
void prs_stmt(FILE *dest, int mode) {
	VTYPE *v;
	enum TYPE type1, type2;
	int i;
	int init_value, final_value;
	int start_line;
	FILE *before_for_fp = NULL, *in_for_fp = NULL, *after_for_fp = NULL;

	switch (token.kind) {
	case SIDENTIFIER: 				//assignment 		assignment = variable , "=" , expression ;
		if ((v = search_variable(token.id)) == NULL) semantic_error(
				"undeclared variable.");
		if (v->type != TINTARRAY && v->type != TVARRAY && v->type != TTSARRAY) { // normal variable
			if ((mode == INV_MODE || mode == UNI_MODE || mode == DOM_MODE) && !v->isHidden) semantic_error(
					"process variable cannot be assigned here.");
			if (v->other_field < 0) semantic_error(
					"loop counter cannot be updated here.");
			type1 = v->type;
		} else {										 // array variable
			next_tokn(dest, SLBRACKET);												//{
			if (prs_expr(dest, mode) != TINT) semantic_error("type mismatch.");
			if (token.kind != SRBRACKET) syntax_error("syntax error.");
			switch (v->type) {
			case TINTARRAY:
				type1 = TINT;
				break;
			case TVARRAY:
				type1 = TV;
				break;
			case TTSARRAY:
				type1 = TTS;
				break;
			default:
				semantic_error("type mismatch.");
			}
		}
		next_tokn(dest, SASSIGN);												//=
		type2 = prs_expr(dest, mode);
		if (type1 == TV && type2 == TVD && !strcmp(token.id, "?")) semantic_error(
				"type mismatch.");
		if (!(((type1 == TV || type1 == TVD) && (type2 == TV || type2 == TVD
				|| type2 == TNULL /*|| type2 == TEMPTY*/)) || (type1 == TTS
				&& (type2 == TTS || type2 == TNULL /*|| type2 == TEMPTY*/))
				|| (type1 == type2))) semantic_error("type mismatch.");
		break;
	case SDECIDE: // decision statement decision = "DECIDE" , "(" , expression , ")" ;
		if (mode == INV_MODE || mode == UNI_MODE || mode == DOM_MODE) semantic_error(
				"DECIDE cannot be used here.");
		next_tokn(dest, SLPAREN);
		type1 = prs_expr(dest, mode);
		if (token.kind != SRPAREN) syntax_error("syntax error.");
		next_tokn(dest, NOCHECK);
		if (type1 != TV && type1 != TVD) semantic_error("type mismatch.");
		break;
	case SFOR: // for loop  "for" , "[" , variable , "=" , expression , "to" , expression , "]" , statement
		/* preprocessing */
		if (mode == DOM_MODE) semantic_error(
				"for loop cannot be used here.");
		deapth_of_nest++;
		if (!gettokn() || token.kind != SLBRACKET) syntax_error("syntax error.");	//[
				if (!gettokn() || token.kind != SIDENTIFIER) syntax_error(			//variable
				"syntax error.");
		if ((v = search_variable(token.id)) == NULL) semantic_error(				
				"undeclared variable.");
		if (v->type != TINT) semantic_error("roop counter must be integer.");
		if (v->other_field < 0) semantic_error(
				"already used as a roop counter.");
		v->other_field = -1; 													/* use 'v' as a loop counter */
		if (!gettokn() || token.kind != SASSIGN) syntax_error("syntax error.");	//=
		if ((init_value = calc_expr()) < 1) semantic_error(						//expr
				"index is out of range.");
		if (token.kind != STO) syntax_error("syntax error.");					//to
		if ((final_value = calc_expr()) > n) semantic_error(					//expr
				"index is out of range.");
		if (token.kind != SRBRACKET) syntax_error("syntax error.");				//]
		before_for_fp = yyin;

		/* copy block statement of for loop to tmp_fp */
		in_for_fp = open_tmpfile();
		next_tokn(in_for_fp, NOCHECK);
		start_line = line_num;
		i = 0;
		do {
			i = (token.kind == SLBRACE) ? i + 1 : (token.kind == SRBRACE) ? i				//{  }
					- 1 : i;
			if (i == 0) {
				while (token.kind != SSEMICOLON)												//;
					next_tokn(in_for_fp, NOCHECK);
				break;
			}
		} while (next_tokn(in_for_fp, NOCHECK));

		/* copy after block statement of for loop to tmp_fp2 */
		after_for_fp = open_tmpfile();
		while (next_tokn(after_for_fp, NONE))
			;

		/* extract block statement (final_value - init_value + 1) times */
		fprintf(dest, "(%s) {\n", v->name);
		for (i = init_value; i <= final_value; i++) {
			v->value = i;
			fprintf(dest, "%s = %d;\n", v->name, i);
			yyin = in_for_fp;
			rewind(yyin);
			yyrestart(yyin);
			line_num = start_line;
			next_tokn(dest, NOCHECK);
			prs_stmt(dest, mode);
			fprintf(dest, "\n");
			if (yyin != in_for_fp) fclose(yyin);
		}
		fprintf(dest, "};");

		/* post processing */
		deapth_of_nest--;
		v->other_field = 0;
		fclose(in_for_fp);
		if (!deapth_of_nest) fclose(before_for_fp);
		yyin = after_for_fp;
		rewind(yyin);
		yyrestart(yyin);
		break;
		/*
		"if" , "(" , expression , ")" , statement , [ "else" , statement ] |
            "assume" , "(" , expression , ")" |
            "assert" , "(" , expression , ")" |
		*/
	case SIF: /* if statement */
		if (mode == DOM_MODE) semantic_error(
				"if statement cannot be used here.");
		next_tokn(dest, SLPAREN);												//(
		if (prs_expr(dest, mode) != TBOOL) semantic_error("type mismatch.");	//expr
		if (token.kind != SRPAREN) syntax_error("syntax error.");				//)
		next_tokn(dest, NOCHECK);	
		prs_stmt(dest, mode);													//statement
		if (token.kind == SELSE) {												//else
			next_tokn(dest, NOCHECK);									
			prs_stmt(dest, mode);												//statement
		}
		break;
	case SASSUME: /* assume statement */
		if (mode != INV_MODE) semantic_error("assume cannot be used here.");	//inv内才可以用assume
		next_tokn(dest, SLPAREN);												//(
		type1 = prs_expr(dest, mode);											//expr
		if (type1 != TBOOL) semantic_error("type mismatch.");					
		if (token.kind != SRPAREN) syntax_error("syntax error.");				//)
		next_tokn(dest, NOCHECK);
		break;
	case SASSERT: /* assert statement */
		if (mode != UNI_MODE && mode != DOM_MODE) semantic_error("assert cannot be used here.");	//uni和dom内才可以用assert
		next_tokn(dest, SLPAREN);												//(
		type1 = prs_expr(dest, mode);											//expr
		if (type1 != TBOOL) semantic_error("type mismatch.");
		if (token.kind != SRPAREN) syntax_error("syntax error.");				//)
		next_tokn(dest, NOCHECK);
		break;
	case SLBRACE: /* block statement */
		prs_stmt_rep(dest, mode);
		if (token.kind != SRBRACE) syntax_error("syntax error.");
		next_tokn(dest, NOCHECK);
		break;
	default:
		syntax_error("syntax error.");
	}
}
//expression  =  conjunctive-expression , { "||" , conjunctive-expression } ;
enum TYPE prs_expr(FILE *fp, int mode) {
	enum TYPE type1, type2;

	type1 = prs_and_expr(fp, mode);
	while (token.kind == SOR) {						// ||
		type2 = prs_and_expr(fp, mode);
		if (type1 != type2 || type1 != TBOOL) semantic_error("type mismatch.");
	};
	return type1;
}

//conjunctive-expression  =  equality-expression { "&&" equality-expression }
enum TYPE prs_and_expr(FILE *fp, int mode) {
	enum TYPE type1, type2;

	type1 = prs_equality_expr(fp, mode);
	while (token.kind == SAND) {						//&&
		type2 = prs_equality_expr(fp, mode);
		if (type1 != type2 || type1 != TBOOL) semantic_error("type mismatch.");
	};
	return type1;
}
//equality-expression = relational-expression , [ equality-operator , relational-expression ] ;
enum TYPE prs_equality_expr(FILE *fp, int mode) {
	enum TYPE type1, type2;

	type1 = prs_relational_expr(fp, mode);
	if (isEqualityOperator(token.kind)) {						//== !=
		type2 = prs_relational_expr(fp, mode);
		if (!(type1 == type2 || ((type1 == TV || type1 == TVD || type1 == TNULL
				|| type1 == TEMPTY) && (type2 == TV || type2 == TVD || type2
				== TNULL || type2 == TEMPTY)) || ((type1 == TTS || type1
				== TNULL || type1 == TEMPTY || type1 == TINT) && (type2 == TTS
				|| type2 == TNULL || type2 == TEMPTY || type2 == TINT)))) semantic_error(
				"type mismatch.");
		type1 = TBOOL;
	};
	return type1;
}
//relational-expression = simple-expression , [ relational-operator , simple-expression ] ;
enum TYPE prs_relational_expr(FILE *fp, int mode) {
	enum TYPE type1, type2;

	type1 = prs_simple_expr(fp, mode);
	if (isRelationalOperator(token.kind)) {						//> < >= <=
		type2 = prs_simple_expr(fp, mode);
		if (!(type1 == type2 || ((type1 == TV || type1 == TVD || type1 == TNULL
				|| type1 == TEMPTY) && (type2 == TV || type2 == TVD || type2
				== TNULL || type2 == TEMPTY)) || ((type1 == TTS || type1
				== TNULL || type1 == TEMPTY || type1 == TINT) && (type2 == TTS
				|| type2 == TNULL || type2 == TEMPTY || type2 == TINT)))) semantic_error(
				"type mismatch.");
		type1 = TBOOL;
	};
	return type1;
}
//simple-expression = term , { additive-operator , term } ;
enum TYPE prs_simple_expr(FILE *fp, int mode) {
	enum TYPE type1, type2;

	type1 = prs_term(fp, mode);
	while (isAdditiveOperator(token.kind)) {			//* /
		type2 = prs_term(fp, mode);
		if (type1 != type2 || type1 != TINT) semantic_error("type mismatch.");
	};
	return type1;
}
//term = factor , { multiplicative-operator , factor } ;
enum TYPE prs_term(FILE *fp, int mode) {
	enum TYPE type1, type2;

	type1 = prs_factor(fp, mode);
	next_tokn(fp, NOCHECK);
	while (isMultiplicativeOperator(token.kind)) {
		type2 = prs_factor(fp, mode);
		if (type1 != type2 || type1 != TINT) semantic_error("type mismatch.");
		next_tokn(fp, NOCHECK);
	};
	return type1;
}
/*
factor = variable | constant | "isCoord" | "Coord" |
         "rcv" , "[" , expression , "]" , "." , variable | "(" , expression , ")" |
constant = "N" | "T" | "E" | "a"| "SIZE" | "SSIZE" | "ASIZE" | "NULL" | "phase" | "DECISION" | simple-constant ; 
variable = normal-variable | array-variable ;
normal-variable = variable-name ;
array-variable = variable-name , "[" , expression , "]" ;
*/
enum TYPE prs_factor(FILE *fp, int mode) {
	VTYPE *v;
	enum TYPE type;

	next_tokn(fp, NOCHECK);
	switch (token.kind) {
	case SIDENTIFIER:									//var
		if ((v = search_variable(token.id)) == NULL) semantic_error(
				"undeclared variable.");
		if (v->type != TINTARRAY && v->type != TVARRAY && v->type != TTSARRAY) { // normal variable
			if (mode == SEND_MODE || mode == NORMAL_MODE ) {
				if (mode == SEND_MODE && v->isHidden) semantic_error(
						"hidden variable cannot be used here.");
			} else { // Invariant，Univalence
				if (!v->isHidden) {
					next_tokn(fp, SLBRACKET);
					if (prs_expr(fp, mode) != TINT) semantic_error(
							"type mismatch.");
					if (token.kind != SRBRACKET) syntax_error("syntax error.");
				}
			}
			type = v->type;
		} else { // array variable
			if (mode == SEND_MODE && v->isHidden) semantic_error(
					"hidden variable cannot be used here.");
			next_tokn(fp, SLBRACKET);
			if (prs_expr(fp, mode) != TINT) semantic_error("type mismatch.");
			if (token.kind != SRBRACKET) syntax_error("syntax error.");
			switch (v->type) {
			case TINTARRAY:
				type = TINT;
				break;
			case TVARRAY:
				type = TV;
				break;
			case TTSARRAY:
				type = TTS;
				break;
			default:
				semantic_error("type mismatch.");
			}
		}
		break;
	case SQUESTION:				//?
		type = TVD;
		break;
	case SFALSE:				//false
		type = TBOOL;
		break;
	case STRUE:					//ture
		type = TBOOL;
		break;
	case SNUM:					//num
		type = TINT;
		break;
	case SPROC_COUNT:			//N
		type = TINT;
		break;
	case SSIZE:					//SIZE
		type = TINT;
		if (mode == SEND_MODE) semantic_error("SIZE cannot be used here.");
		break;
	case SNULL:					//NULL
		type = TNULL;
		if (mode == SEND_MODE) semantic_error("NULL cannot be used here.");
		break;
	case SPHASE:				//phase
		type = TTS;
		break;
	case SISCOORD:				//isCoord 在round中使用
		if (mode == INV_MODE || mode == UNI_MODE) semantic_error(
				"isCoord cannot be used here.");
		type = TBOOL;
		if (!coord_flag) coord_flag = 1;
		break;
	case SCOORD:				//Coord
		type = TINT;
		if (mode == SEND_MODE) semantic_error("Coord cannot be used here.");
		if (!coord_flag) coord_flag = 1;
		break;
	case STHRESHOLD:			//T	用于transpart和INV
		type = TINT;
		if( mode == SEND_MODE) semantic_error(
				"T cannot be used here.");
		break;
	case SENOUGH:				//E	用于transpart和INV
		type = TINT;
		if( mode == SEND_MODE) semantic_error(
				"E cannot be used here.");
		break;
	case SA:					//a	用于dom和transpart
		type = TINT;
		if( mode != DOM_MODE && mode != NORMAL_MODE && mode != UNI_MODE) semantic_error(
				"a cannot be used here.");
		break;
	case SASIZE:				//ASIZE 用于dom
		type = TINT;
		if( mode != DOM_MODE ) semantic_error(
				"ASIZE cannot be used here.");
		break;
	case SSSIZE:				//SSIZE 用于dom
		type = TINT;
		if( mode != DOM_MODE ) semantic_error(
				"SSIZE cannot be used here.");
		break;
	case SPID:					//pid
		if (mode == INV_MODE || mode == UNI_MODE) semantic_error(
				"pid cannot be used here.");
		type = TINT;
		break;
	case SVVAL:					//v
		if (mode != UNI_MODE) semantic_error("v cannot be used here.");
		type = TV;
		break;
	case SRCV:					//rcv
		if (mode == SEND_MODE || mode == INV_MODE || mode == UNI_MODE) semantic_error(
				"rcv cannot be used here.");
		next_tokn(fp, SLBRACKET);							//[
		if (prs_expr(fp, mode) != TINT) semantic_error("type mismatch.");
		if (token.kind != SRBRACKET) syntax_error("syntax error.");
		next_tokn(fp, SDOT);								//.
		next_tokn(fp, NOCHECK);	
		if (token.kind != SIDENTIFIER && token.kind != SCOORD) syntax_error(	//! var,Coord
				"syntax error.");
		if (token.kind == SCOORD && sendCoord)									//Coord
			type = TINT;
		else if (token.kind == SIDENTIFIER && (v = search_variable(token.id))
				!= NULL && v->isMessage) {
			if (v->type != TVARRAY && v->type != TTSARRAY) { // normal variable
				type = v->type;
			} else { // array variable
				semantic_error("cannot be used as message.");
			}
		} else semantic_error("cannot be used as message.");
		break;
	case SLPAREN:															//(
		type = prs_expr(fp, mode);											//expr
		if (token.kind != SRPAREN) syntax_error("syntax error.");
		break;
	default:
		syntax_error("syntax error.");
	};
	return type;
}

int isEqualityOperator(enum SYMBOL kind) { // if '==' or '!='  then 1 else 0
	switch (kind) {
	case SEQUAL:						//==
	case SNOTEQUAL:						//!=
		return 1;
	default:
		return 0;
	}
}

int isRelationalOperator(enum SYMBOL kind) { // if '<' or '<=' or '>' or '>='  then 1 else 0
	switch (kind) {
	case SLESS:
	case SLESSEQUAL:
	case SGREAT:
	case SGREATEQUAL:
		return 1;
	default:
		return 0;
	}
}

int isAdditiveOperator(enum SYMBOL kind) { // if '+' or '-' then 1 else 0
	switch (kind) {
	case SPLUS:
	case SMINUS:
		return 1;
	default:
		return 0;
	}
}

int isMultiplicativeOperator(enum SYMBOL kind) { // if '*' or '/' then 1 else 0
	switch (kind) {
	case SMULT:
	case SDIV:
		return 1;
	default:
		return 0;
	}
}

int isReservedKeyword(const char * name) {			
	return (!strcmp(name, "dv") || !strcmp(name, "NULL") || !strcmp(name,
			"coord") || !strcmp(name, "phase") || !strcmp(name, "rcv")
			|| !strcmp(name, "N") || !strcmp(name, "size"));
}

void insert_variable(char *name, enum TYPE type, int value, int send,
		int hidden, int assign_count, int *ptr, int other_field) {
	VTYPE *newnode, *p;

	newnode = (VTYPE *) malloc(sizeof(VTYPE));
	strcpy(newnode->name, name);
	newnode->type = type;
	newnode->value = value;
	newnode->isMessage = send;
	newnode->isHidden = hidden;
	newnode->assign_count = assign_count;
	newnode->assign_count_elem = ptr;
	newnode->other_field = other_field;
	newnode->next = NULL;
	if (v_list != NULL) {
		for (p = v_list; p->next != NULL; p = p->next)
			;
		p->next = newnode;
	} else {
		v_list = newnode;
	}
}

VTYPE *search_variable(char *name) {
	VTYPE *p;

	for (p = v_list; p != NULL; p = p->next) {
		if (!strcmp(name, p->name)) break;
	}
	return p;
}

int next_tokn(FILE *fp, int kind) {
	int i;
	int result;
	int lineNumber;

	lineNumber = token.line;
	if ((result = gettokn()) != 0) {
		if (kind != NOCHECK && kind != NONE && kind != token.kind) 
			syntax_error("syntax error.");
		switch (token.kind) {
		case SEMPTY:
		case SFOREACH:
			syntax_error("syntax error.");
			break;
		}
		if (token.line != 1) {
			for (i = lineNumber; i < token.line; i++)
				fprintf(fp, "\n");
		}
		fprintf(fp, "%s ", yytext);
	} 
	else if (kind != NONE) syntax_error("syntax error.");
	return result;
}



int next_tokn2(FILE *fp, int kind) {
	int i;
	int result;
	int lineNumber;

	lineNumber = token.line;
	if ((result = gettokn()) != 0) {
		if (kind != NOCHECK && kind != NONE && kind != token.kind) 
			syntax_error("syntax error.");
		switch (token.kind) {
		case SEMPTY:
		case SFOREACH:
			syntax_error("syntax error.");
			break;
		}
		if (token.line != 1) {
			for (i = lineNumber; i < token.line; i++)
				fprintf(fp, "\n");
		}
		fprintf(fp, "%s ", yytext);
	} 
	else if (kind != NONE) syntax_error("syntax error.");
	printf("%s\n",yytext);
	return result;
}
