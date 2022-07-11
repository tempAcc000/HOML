#include "translator.h"

FILE *infile = NULL; // Input file
char *outbuf = NULL; // Output buffer
char *varbuf= NULL; //variable buffer
char *var_outbuf= NULL; //variable buffer
char *roundbuf = NULL; //round buffer
char *round_outbuf=NULL;
char *sizebuf=NULL; //HO AHO SHO
FILE *outfile = NULL; // Output file
char outfilename[FNAME_LEN] = ""; // Output file name
char *guard = NULL; // Guard condition
char *domain_assertion = NULL; //domains assert
char *domain = NULL; //domains
char *domain_outbuf = NULL; //domains
char *invbuf=NULL;
char *inv_outbuf=NULL;
char *unibuf=NULL;
char *uni_outbuf=NULL;
char *assertion = NULL; // Assertions
char *assumption = NULL; // Assumptions
int agr_mode = 0; // Agreement mode (-a)
int uni_mode = 0; // Univalence mode (-u)
int inv_mode = 0; // Invariant mode (-i)
int smt_mode=0,yices_mode=0,c_mode=0,smt_bv_mode=0;

int main(int argc, char *argv[]) {
	int result;
	extern char *optarg;
	extern int optind;
	char *check;
	char *command;

	command = argv[0];			//获取可执行程序的路径

	// Option
	while ((result = getopt(argc, argv, "n:auisyb")) != -1) {		//n和p必须加参数,a,u,i,t不需要
		switch (result) {
		case 'n':
			n = (int) strtol(optarg, &check, 10);			//将optarg转为数字,check指向转换结束的位置
			if (*check != '\0' || n < 3 || n > MAX_N) { // Check <num>
				error("Missing argument for option '-n <num>'.");
			}
			break;
		case 'a':
			agr_mode = 1;
			break;
		case 'u':
			uni_mode = 1;
			break;
		case 'i':
			inv_mode = 1;
			break;
		case 's':
			smt_mode=1;
			break;
		case 'b':
			smt_bv_mode=1;
			break;
		case 'y':
			yices_mode=1;
			break;
		case '?': // unknown option
			error("invalid option.");
			break;
		}
	}
	argc -= optind;
	argv += optind;

	// -t
	//Output the trace of the algorithm from an input file.


	if (argc != 2) { // Check number of arguments
		usage();
		exit(1);
	}

	if (agr_mode + uni_mode + inv_mode > 1) {
		error("-a, -u, and -i cannot be used at the same time.");
	} else if (phase > 1 && agr_mode + uni_mode + inv_mode == 1) {
		error("-p and -a (-u, -i) cannot be used at the same time.");
	} else if(smt_mode + smt_bv_mode + yices_mode + c_mode >1){
		error("-s, -b, -y, and -c cannot be used at the same time.");
	}
	else if(c_mode + inv_mode + uni_mode + agr_mode >1){
		error("-c cannot be used with -i -u -a at the same time.");
	}

	if(smt_mode + smt_bv_mode + yices_mode + c_mode == 0)
		smt_mode=1;
		

	// Open files
	infile = fopen(argv[argc - 2], "r");
	outfile = tmpfile();		

	if ((!infile) || (!outfile)) error("could not open file.");

	yyin = infile;
	rewind(yyin);
	yyrestart(yyin);				
	prs_alg(); 						
	fclose(yyin);

	infile = outfile;
	outfile = NULL;
	yyin = infile;
	rewind(yyin);				
	yyrestart(yyin);			
	if(smt_mode)
		gen_smt2();
	else if(smt_bv_mode)
		gen_smt2_bv();
	else if(yices_mode)
		gen_yices();
	else
		error("error");
	fprintf(stdout, "Completed\n");

	fclose(infile);
	free(guard);
	free(domain);
	free(assertion);
	free(assumption);

	// Write 'outbuf' into 'outfile'
	strcpy(outfilename, argv[argc - 1]);
	outfile = fopen(outfilename, "w");
	if (!outfile) error("could not open file.");
	fprintf(outfile, "%s", outbuf);
	fclose(outfile);
	free(outbuf);

	return 0;
}
