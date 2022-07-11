#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define NAME_LEN 16 // Maximum length of variable name
#define FNAME_LEN 256 // Maximum length of file name
#define MAX_N 50  // Maximum number of processes
#define MAX_PHASE 3 // Maximum number of modeled phase

/* Initial values of variables */
#define UNINI -1 // no initialization
#define ANY_V -2 // an arbitrary value in V
#define EMPTY -3 // the special value represents "empty"
#define TRUE -4   // true
#define FALSE -5  // false
#define UNDEF 0 // the special value represents "?"

/* Modes of parsing */
#define SEND_MODE 0 // parsing send part
#define NORMAL_MODE 1 // parsing transition part
#define INV_MODE 2 // parsing invariant block
#define UNI_MODE 3 // parsing univalence block
#define DOM_MODE 4 // parsing domain block
/* Symbols represents kinds of tokens */
enum SYMBOL {
	SACK,        SALGORITHM,  SALL,        SAND,        SASSERT,
	SASSIGN,     SASSUME,     SBOOL,       SBREAK,      SCOMMA,
	SCOORD,      SDECIDE,     SDIV,        SDOT,        SELSE,
	SEMPTY,      SEQUAL,      SFALSE,      SFOR,        SFOREACH,
	SGREAT,      SGREATEQUAL, SHIDDEN,     SIDENTIFIER, SIF,
	SINT,        SINTARRAY,   SINVARIANT,  SISCOORD,    SLBRACE,
	SLBRACKET,   SLESS,       SLESSEQUAL,  SLPAREN,     SMINUS,
	SMULT,       SNOTEQUAL,   SNULL,       SNUM,        SOR,
	SPHASE,      SPID,        SPLUS,       SPROC_COUNT, SQUESTION,
	SRBRACE,     SRBRACKET,   SRCV,        SROUND,      SRPAREN,
	SSEMICOLON,  SSEND,       SSIZE,       SSP,         STO,
	STP,         STRUE,       STS,         STSARRAY,    SUNIVALENCE,
	SV,          SVARIABLE,   SVARRAY,     SVD,         SVP,
	SVVAL,       SEOF,		  SDOMAIN,     STHRESHOLD,  SENOUGH,     
    SA,          SASIZE,      SSSIZE,
};

/* Informations about a token */
typedef struct TOKN TOKN;
struct TOKN {
	int kind; // Kind of token
	char id[15 + 1]; // Identifier of token
	int line; // line number
};

/* Type of variables */
enum TYPE {
	TV, TVD, TBOOL, TTS, TVARRAY, TTSARRAY, TINT, TINTARRAY, TNULL, TEMPTY
};

/* Informations about a variable */
typedef struct VTYPE VTYPE;
struct VTYPE {
	char name[NAME_LEN]; // variable name
	enum TYPE type; // variable type
	int value; // initial value
	int isMessage; // variable is sent -> 1 or 2, not -> 0
	int isHidden; // declared as hidden -> 1, not -> 0
	int assign_count; // count of assignment
	int *assign_count_elem; // if array -> count of assignment of element．not -> NULL
	int other_field; // declared as hidden -> order as message, not -> flag(is used as loop counter)
	struct VTYPE *next; // next variable
};

/* Variables defined in scanner.c */
extern FILE *yyin; // Input file to make tokens
extern char *yytext; // Token as string
extern TOKN token; // Current token
extern int line_num; // Current line number of input file

/* Variables defined in preprocess.c */
extern VTYPE *v_list; // Pointer to variable table
extern int n; // Number of processes
extern int round_count; // Number of rounds
extern int phase; // Number of modeled phases
extern int coord_flag; // Use coordinator -> 1, not -> 0

/* Variables defined in gen_code.c */
extern char *guard; // Pointer to the file stores guard conditions
extern char *domain_assertion; // Pointer to the file stores domains assert
extern char *domain; // Pointer to the file stores domains
extern char *domain_outbuf; //domains
extern char *assertion; // Pointer to the file stores assertions
extern char *assumption; // Pointer to the file stores assumptions

/* Variables defined in main.c */
extern FILE *infile; // Input file
extern char *outbuf; // Output file
extern char *varbuf; //variable buffer
extern char *var_outbuf;
extern char *roundbuf ; //round buffer
extern char *round_outbuf;
extern char *sizebuf; //HO AHO SHO buffer
extern char *invbuf;
extern char *inv_outbuf;
extern char *unibuf;
extern char *uni_outbuf;
extern FILE *outfile; // Output file
extern char outfilename[FNAME_LEN]; // Outputfile name
extern int agr_mode; // Generate a formula for agreement -> 1, not -> 0
extern int uni_mode; // Generate a formula for univalence -> 1, not -> 0
extern int inv_mode; // Generate a formula for invariant -> 1, not -> 0

/* Functions defined in scanner.c */
extern int gettokn(); // Get new token
extern void yyrestart(FILE *new_file); // Change input file

/* Functions defined in io.c */
extern void error(const char *message); // Output error message and Stop
extern void syntax_error(const char *message);
extern void semantic_error(const char *message);
extern void usage();
extern FILE *open_tmpfile(); // Make a temporary file
extern void str_append(char **src, const char *fmt, ...);
extern void file_copy(FILE *src, FILE *dest); // Copy contents of file to another file
extern void replaceStr(char *str, const char *target_str, const char *new_str);
extern void replaceCondition(char *src, char **dest, int pid); // Replace string in file

/* Functions defined in preprocess.c */
extern void prs_alg(); // Parse a whole algorithm
extern int isEqualityOperator(enum SYMBOL kind); // Check whether token is equality operator or not
extern int isRelationalOperator(enum SYMBOL kind); // Check whether token is relational operator or not
extern int isAdditiveOperator(enum SYMBOL kind); // Check whether token is additive operator or not
extern int isMultiplicativeOperator(enum SYMBOL kind); // Check whether token is multiplicative operator or not
extern void insert_variable(char *name, enum TYPE type, int value, int send,
		int hidden, int assign_count, int *ptr, int other_field); // Register new variable to variable table
extern VTYPE *search_variable(char *name); // Search variable from table

/* Functions defined in gen_smt2.c gen_yices.c gen_c.c*/
extern void gen_smt2(); // Generate smt2 code
extern void gen_smt2_bv(); // Generate smt2 code
extern void gen_yices(); // Generate yices code
extern void gen_c(); // Generate c code
extern int calc_expr(); // Evaluate an expression


