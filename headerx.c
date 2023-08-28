#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int opt_verbose = 0;

void usage(char **argv)
{
	fprintf(stdout, "Usage: %s [<options>] <source_files>\n", argv[0]);
	fprintf(stdout, "Extract headers from mixed files (header + c code).\n");
	fprintf(stdout, "Header portion must start with a line '//HEADERX(<header filename>,<header tag>)'.\n");
	fprintf(stdout, "Header portion must end with '//ENDX'.\n");
	fprintf(stdout, "Both starting at the begining of the line.\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "     <source_files> : list of mixed (header + c code) filesx \n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Options\n");
	fprintf(stdout, "       -v        Verbose output\n");
	fprintf(stdout, "       -h        Displays this help\n");
	fprintf(stdout, "\n");
}

int isAlphaNumeric(char ch)
{
	if(ch>='0' && ch<='9') return 1;
	if(ch>='a' && ch<='z') return 1;
	if(ch>='A' && ch<='Z') return 1;

	return 0;
}

int isWS(char ch)
{
	if(ch==' ') return 1;
	if(ch=='\t') return 1;
	return 0;
}

//Parser for:
//	//HEADERX WS ( WS <header filename> WS , WS <header protective tag> WS )
//	WS :== ' ' | <tab> 
//

int parse_header_line(char *src, char **h_filename, char **h_tag)
{
char *p, *ptmp;
int  c;

	if(src==NULL) return -1;

	if(strncmp(src,"//HEADERX",9)) return -1;
	src = src + 9;

	while(isWS(*src)) *src++;

	if(*src++!='(') {
		fprintf(stderr,"Error HEADERLINE: expected '(', found %c\n", *(src-1));
		return -1;
	}

	while(isWS(*src)) *src++;

	for(ptmp=src, c=0; (isAlphaNumeric(*src) || *src=='_' || *src=='.' || *src=='/'); src++) c++;
	if(c>0) *h_filename = strndup(ptmp, c);

	while(isWS(*src)) *src++;

	if(*src++!=','){
		fprintf(stderr,"Error HEADERLINE: expect ',', found %c\n", *(src-1));
		return -1;
	}

	while(isWS(*src)) *src++;

	for(ptmp=src, c=0; (isAlphaNumeric(*src) || *src=='_'); src++) c++;
	if(c>0) *h_tag = strndup(ptmp, c);

	while(isWS(*src)) *src++;

	if(*src++!=')'){
		fprintf(stderr,"Error HEADERLINE: expect ')', found %c\n", *(src-1));
		return -1;
	}
}

FILE *start_new_header(char *header_line, char **h_filename, char **h_tag)
{
FILE *hout;

	*h_filename = NULL;
	*h_tag = NULL;

	if(header_line==NULL) return NULL;

	parse_header_line(header_line, h_filename, h_tag);

	if(*h_filename==NULL || *h_tag==NULL) return NULL;

	if((hout=fopen(*h_filename,"w"))==NULL) return NULL;

	return hout;
}

void print_line_directive(FILE *fp, int line, char *filename)
{
	fprintf(fp,"#line %d \"%s\"\n", line, filename);
}

void trim(char *line)
{
int l;

	if(line==NULL) return;
	
	l = strlen(line);
	while(l-->0){
		if(line[l]=='\n' || line[l]=='\r') {
			line[l] = 0;
		}else{
			return;
		}
	}
}

int process(char *filename)
{
FILE *fp, *fout_h;
char lbuffer[1024], *c_filename, *h_filename, *h_tag;
int  state = 0, lines = 0;

	if((fp=fopen(filename,"r"))==NULL){
		fprintf(stderr,"Could not open %s\n", filename);
		return -1;
	}	

	while(fgets(lbuffer, 1024, fp)){
		lines++;

		switch(state){
		case 0:
			if(!strncmp(lbuffer,"//HEADERX",9)){
				if(opt_verbose)
					fprintf(stdout,"File %s, line %d: Found //HEADERX\n", filename, lines);
				if((fout_h=start_new_header(lbuffer, &h_filename, &h_tag))==NULL){
					fprintf(stderr,"Erro in file %s, line %d: Could not open output file %s\n", filename, lines, h_filename);
					return -1;
				}
				if(opt_verbose)
					fprintf(stdout,"File %s, line %d: Parsed header file name: %s, and tag: %s\n", filename, lines, h_filename, h_tag);
				fprintf(fout_h,"#ifndef %s\n", h_tag);
				fprintf(fout_h,"#define %s\n", h_tag);
				print_line_directive(fout_h, lines, filename);
				state=1;
			}
			break;
		case 1: 
			if(!strncmp(lbuffer,"//ENDX",6)){
				if(opt_verbose)
					fprintf(stdout,"File %s, line %d: Found //ENDX\n", filename, lines);
				fprintf(fout_h,"#endif\n");
				fclose(fout_h);
				state=0;
			}else{
				fprintf(fout_h,"%s",lbuffer);
			}
			break;
		}
	}

	return 0;
}	

int main(int argc, char **argv)
{
char *filename = NULL;
int  argcount = 1;
int  fcount = 0;

	if(argc<2){
		usage(argv);
		return -1;
	}

	for(argcount=1; argcount<argc; argcount++){

		if(!strcmp(argv[argcount],"-h")){
			usage(argv);
			return 0;
		}

		if(!strcmp(argv[argcount],"-v")){
			opt_verbose = 1;
			continue;
		}

		fcount++;

		filename = argv[argcount];
		if (process(filename)) {
			fprintf(stderr,"Failed processing file %s\n", filename);
			return -1;
		}
	}

	if(fcount<1){
		fprintf(stderr,"Not files to processs\n");
		return -1;
	}

	return 0;
}

