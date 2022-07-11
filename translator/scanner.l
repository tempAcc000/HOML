%option noyywrap
%{
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
  SVVAL,       SEOF,        SDOMAIN,     STHRESHOLD,  SENOUGH,     
  SA,          SASIZE,      SSSIZE,
};

typedef struct TOKN TOKN;

struct TOKN {
  int kind;      /* kind of token */
  char id[15+1];  /* identifier */
  int line;      /* line number */
};

int line_num = 1;
int in_annotation = 0;
TOKN token;

void yyerror(const char *message);

#undef   YY_DECL
#define  YY_DECL  int gettokn()

%}
%%
assume { if (in_annotation == 0) {token.kind = SASSUME; token.line = line_num; return 1;}; }
assert { if (in_annotation == 0) {token.kind = SASSERT; token.line = line_num; return 1;}; }
Algorithm { if (in_annotation == 0) {token.kind = SALGORITHM; token.line = line_num; return 1;}; }
Variable { if (in_annotation == 0) {token.kind = SVARIABLE; token.line = line_num; return 1;}; }
hidden {if (in_annotation == 0) {token.kind = SHIDDEN; token.line = line_num; return 1;};} 
V { if (in_annotation == 0) {token.kind = SV; token.line = line_num; return 1;}; }
V' { if (in_annotation == 0) {token.kind = SVD; token.line = line_num; return 1;}; }
TS { if (in_annotation == 0) {token.kind = STS; token.line = line_num; return 1;}; }
VArray { if (in_annotation == 0) {token.kind = SVARRAY; token.line = line_num; return 1;}; }
TSArray { if (in_annotation == 0) {token.kind = STSARRAY; token.line = line_num; return 1;}; }
bool { if (in_annotation == 0) {token.kind = SBOOL; token.line = line_num; return 1;}; }
int { if (in_annotation == 0) {token.kind = SINT; token.line = line_num; return 1;}; }
intArray { if (in_annotation == 0) {token.kind = SINTARRAY; token.line = line_num; return 1;}; }
pid { if (in_annotation == 0) {token.kind = SPID; token.line = line_num; return 1;}; }
Round { if (in_annotation == 0) {token.kind = SROUND; token.line = line_num; return 1;}; }
SendPart { if (in_annotation == 0) {token.kind = SSP; token.line = line_num; return 1;}; }
TransitionPart { if (in_annotation == 0) {token.kind = STP; token.line = line_num; return 1;}; }
send { if (in_annotation == 0) {token.kind = SSEND; token.line = line_num; return 1;}; }
All { if (in_annotation == 0) {token.kind = SALL; token.line = line_num; return 1;}; }
Coord { if (in_annotation == 0) {token.kind = SCOORD; token.line = line_num; return 1;}; }
foreach { if (in_annotation == 0) {token.kind = SFOREACH; token.line = line_num; return 1;}; }
for { if (in_annotation == 0) {token.kind = SFOR; token.line = line_num; return 1;}; }
to { if (in_annotation == 0) {token.kind = STO; token.line = line_num; return 1;}; }
break { if (in_annotation == 0) {token.kind = SBREAK; token.line = line_num; return 1;}; }
if { if (in_annotation == 0) {token.kind = SIF; token.line = line_num; return 1;}; }
else { if (in_annotation == 0) {token.kind = SELSE; token.line = line_num; return 1;}; }
rcv { if (in_annotation == 0) {token.kind = SRCV; token.line = line_num; return 1;}; }
ack { if (in_annotation == 0) {token.kind = SACK; token.line = line_num; return 1;}; }
"(" { if (in_annotation == 0) {token.kind = SLPAREN; token.line = line_num; return 1;}; }
")" { if (in_annotation == 0) {token.kind = SRPAREN; token.line = line_num; return 1;}; }
"{" { if (in_annotation == 0) {token.kind = SLBRACE; token.line = line_num; return 1;}; }
"}" { if (in_annotation == 0) {token.kind = SRBRACE; token.line = line_num; return 1;}; }
"[" { if (in_annotation == 0) {token.kind = SLBRACKET; token.line = line_num; return 1;}; } 
"]" { if (in_annotation == 0) {token.kind = SRBRACKET; token.line = line_num; return 1;}; }
"=" { if (in_annotation == 0) {token.kind = SASSIGN; token.line = line_num; return 1;}; }
";" { if (in_annotation == 0) {token.kind = SSEMICOLON; token.line = line_num; return 1;}; }
"," { if (in_annotation == 0) {token.kind = SCOMMA; token.line = line_num; return 1;}; }
"." { if (in_annotation == 0) {token.kind = SDOT; token.line = line_num; return 1;}; }
"==" { if (in_annotation == 0) {token.kind = SEQUAL; strcpy(token.id, "="); token.line = line_num; return 1;}; }
"!=" { if (in_annotation == 0) {token.kind = SNOTEQUAL; strcpy(token.id, "/="); token.line = line_num; return 1;}; }
"<" { if (in_annotation == 0) {token.kind = SLESS; strcpy(token.id, "<"); token.line = line_num; return 1;}; }
"<=" { if (in_annotation == 0) {token.kind = SLESSEQUAL; strcpy(token.id, "<="); token.line = line_num; return 1;}; }
">" { if (in_annotation == 0) {token.kind = SGREAT; strcpy(token.id, ">"); token.line = line_num; return 1;}; }
">=" { if (in_annotation == 0) {token.kind = SGREATEQUAL; strcpy(token.id, ">="); token.line = line_num; return 1;}; }
"+" { if (in_annotation == 0) {token.kind = SPLUS; strcpy(token.id, "+"); token.line = line_num; return 1;}; }
"-" { if (in_annotation == 0) {token.kind = SMINUS; strcpy(token.id, "-"); token.line = line_num; return 1;}; }
"||" { if (in_annotation == 0) {token.kind = SOR; strcpy(token.id, "or"); token.line = line_num; return 1;}; }
"*" { if (in_annotation == 0) {token.kind = SMULT; strcpy(token.id, "*"); token.line = line_num; return 1;}; }
"/" { if (in_annotation == 0) {token.kind = SDIV; strcpy(token.id, "div"); token.line = line_num; return 1;}; }
"&&" { if (in_annotation == 0) {token.kind = SAND; strcpy(token.id, "and"); token.line = line_num; return 1;}; }
"?" { if (in_annotation == 0) {token.kind = SQUESTION; strcpy(token.id, "?"); token.line = line_num; return 1;}; }
false { if (in_annotation == 0) {token.kind = SFALSE; token.line = line_num; return 1;}; }
true { if (in_annotation == 0) {token.kind = STRUE; token.line = line_num; return 1;}; }
Vp { if (in_annotation == 0) {token.kind = SVP; token.line = line_num; return 1;}; }
phase { if (in_annotation == 0) {token.kind = SPHASE; token.line = line_num; return 1;}; }
"N" { if (in_annotation == 0) {token.kind = SPROC_COUNT; token.line = line_num; return 1;}; } 
"T" { if(in_annotation == 0) {token.kind = STHRESHOLD; token.line = line_num; return 1;}; } 
"E" { if(in_annotation == 0) {token.kind = SENOUGH; token.line = line_num; return 1;}; } 
"a" { if(in_annotation == 0) {token.kind = SA; token.line = line_num; return 1;}; }
SSIZE { if(in_annotation == 0) {token.kind = SSSIZE; token.line = line_num; return 1;}; }
ASIZE { if(in_annotation == 0) {token.kind = SASIZE; token.line = line_num; return 1;}; }
SIZE { if (in_annotation == 0) {token.kind = SSIZE; token.line = line_num; return 1;}; } 
NULL { if (in_annotation == 0) {token.kind = SNULL; token.line = line_num; return 1;}; }
EMPTY { if (in_annotation == 0) {token.kind = SEMPTY; token.line = line_num; return 1;}; }
isCoord { if (in_annotation == 0) {token.kind = SISCOORD; token.line = line_num; return 1;}; }
Univalence { if (in_annotation == 0) {token.kind = SUNIVALENCE; token.line = line_num; return 1;}; }
Invariant { if (in_annotation == 0) {token.kind = SINVARIANT; token.line = line_num; return 1;}; }
Domain { if (in_annotation == 0) {token.kind = SDOMAIN; token.line = line_num; return 1;}; }
DECIDE { if (in_annotation == 0) {token.kind = SDECIDE; token.line = line_num; return 1;}; }
v { if (in_annotation == 0) {token.kind = SVVAL; token.line = line_num; return 1;}; }
<<EOF>> { token.kind = SEOF; token.line = line_num; return 0; }
[1-9][0-9]* |
"0" { if (in_annotation == 0) {token.kind = SNUM; strncpy(token.id, yytext, 15); token.line = line_num; return 1;}; }
[a-zA-Z][a-zA-Z0-9]* { if (in_annotation == 0) {token.kind = SIDENTIFIER; strncpy(token.id, yytext, 15); token.line = line_num; return 1;}; }
"/*" { in_annotation = 1; }
"*/" { if (in_annotation == 1) in_annotation = 0;  else yyerror("Illegal input");}
\n { line_num++; }
[\t ]+ 

%%

void yyerror(const char *message)
{
   fprintf(stderr,"Error: %s [in line %d, token = '%s']\n", message, line_num, yytext);
}
