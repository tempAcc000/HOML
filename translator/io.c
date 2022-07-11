#include "translator.h"

extern void yyerror(char *s);

void error(const char *message) {
	fprintf(stderr,"Error: %s\n", message);
	exit(1);
}

void syntax_error(const char *message) {
	yyerror((char *) message);
	exit(1);
}

void semantic_error(const char *message) {			//语义错误
	fprintf(stderr,"Error: %s [in line %d]\n", message, line_num);
	exit(1);
}

void usage() {
	fprintf(stderr, "Usage: translator [options] infile outfile\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -n <num> -- Set the number of processes to <num>.\n");
	fprintf(stderr, "  -a       -- Generate a formula to verify Agreement.\n");
	fprintf(stderr, "  -u       -- Generate a formula to verify Univalence.\n");
	fprintf(stderr, "  -i       -- Generate a formula to verify Invariant.\n");
	fprintf(stderr,
	"  -s       -- Generate smt2 code to verify.(default)\n");
	fprintf(stderr,
	"  -b       -- Generate smt2 bv code to verify.\n");
	fprintf(stderr,
	"  -y       -- Generate yices code to verify.\n");
}

FILE *open_tmpfile() {
	FILE *fp;

	fp = tmpfile();
	if (fp == NULL) {
		fprintf(stderr, "Error: could not open tmpfile.\n");
		exit(1);
	}
	return fp;
}

void str_append(char **src, const char *fmt, ...) {
	va_list list;
	char *buff1 = NULL, *buff2 = NULL;

	va_start(list, fmt);
	if (*src != NULL) {
		vasprintf(&buff1, fmt, list);
		asprintf(&buff2, "%s%s", *src, buff1);
		free(*src);
		*src = buff2;
	} else vasprintf(src, fmt , list);
	va_end(list);
}

void replaceStr(char *str, const char *target_str, const char *new_str) {
	int target_len = strlen(target_str); // Length of target string
	int new_len = strlen(new_str); // Length of new string
	int str_len = strlen(str); // Length of sourse string
	char *replace_pos; // Position of target string

	// Search position of target string
	while ((replace_pos = strstr(str, target_str)) != NULL) {
		// Create area to insert new string
		memmove(replace_pos + new_len, replace_pos + target_len, strlen(
				replace_pos) - target_len);

		// Insert new string
		memmove(replace_pos, new_str, new_len);

		// If length of replaced string becomes shorter than source, change NULL position.
		if (target_len > new_len) {
			*(str + str_len - target_len + new_len) = '\0';
			str_len = strlen(str);
		}
	}

}

void replaceCondition(char *src, char **dest, int pid) {
	char *tmp_buf = NULL;
	char new_str[128] = "";

	asprintf(&tmp_buf, src);
	sprintf(new_str, "p%d", pid);
	replaceStr(tmp_buf, "_proc", new_str);
	sprintf(new_str, "%d", pid);
	replaceStr(tmp_buf, "_pid", new_str);
	str_append(dest, tmp_buf);
	free(tmp_buf);
}
