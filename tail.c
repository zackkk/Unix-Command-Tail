/*
 *	 FILE: tail.c
 * AUTHOR: Zack (Zhe Xie)
 *   TIME: 08/30/2014, doing my Master's at USC.
 *   DESC: Unix command tail (-n, -f)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "tail.h"


void tail_n_lines_print(FILE *fp, int n, int file_prev_size){
	int relativeToEnd = -1;
	int lineCount = 1;
	char c;	

	/* check if it has reached the beginning frontier. */
	while( -relativeToEnd <= file_prev_size && lineCount <= n){
		fseek(fp, relativeToEnd, SEEK_END);
		c = fgetc(fp);
		if(c == '\n') lineCount++;
		relativeToEnd--;
	}
	/* adjust pointer in the file (not *fp) if it exceeds the beginning frontier. */
	if( -relativeToEnd  > file_prev_size){
		relativeToEnd++;
		fseek(fp, relativeToEnd, SEEK_END);
	}
	
	/* print char until it reached the end of file */
	while((c = fgetc(fp)) != EOF){ 
		printf("%c",c);
	}
}

void follow_print(char *inputFileName, int *file_prev_size){
	
	FILE *fp = fopen(inputFileName,"r");
	if(fp == NULL){	
		fprintf(stderr, "File doesn't exist\n");
		return;
	}
	fseek(fp, 0, SEEK_END);
	int sz = ftell(fp);

	/* if the file size has increased */
	if(sz > *file_prev_size) {
		fseek(fp, *file_prev_size, SEEK_SET);
		char c;
		/* print the newly added chars */
		while((c = fgetc(fp)) != EOF){ 
			printf("%c",c);
		}
		
	}
	/* update previous file size */
	*file_prev_size = sz;
	fclose(fp);
	
}

void argument_process(int argc, char *argv[]){
	int i = 0;
	/* check all arguments */
	for(i = 1; i < argc; i++){ 
		
		/* case 1: the argument is "-n", then it must followed by a number */
		if(strcmp(argv[i], "-n") == 0){
			if(i + 1 < argc){
				/* if -n is specified multiple times, then it would cover the previous one */
				g_argument_n = atoi(argv[i+1]);
				if(g_argument_n == 0){ 
					fprintf(stderr, "Invalid parameter of -n\n");
					exit(-1);
				}
				/* treat it as its absolute value if it is negative */
				else if(g_argument_n < 0){ 
					g_argument_n = -g_argument_n;
				}
				/* it takes two arguments one time */
				i++;
			}
			/* "-n" must be followed by a number */
			else{ 
				fprintf(stderr, "Invalid parameter of -n\n");
				exit(-1);
			}
		}
		/* case 2: the argument is "-f" */
		else if(strcmp(argv[i], "-f") == 0){
			g_argument_f = TRUE;	
		}
		/* case 3: the argument is a file name */
		else{
			strcpy(files[g_file_num], argv[i]);
			g_file_num++;
		}
	}
}

int main(int argc, char *argv[]){
	int i = 0;
	/* assert the first argument is "./tail" */
	assert(strcmp(argv[0], "./tail") == 0);

	/* initialization */
	int files_prev_size[FILE_NUM_LIMIT];
	for(i = 0; i < FILE_NUM_LIMIT; i++){
		strcpy(files[i], "");
		files_prev_size[i] = 0;
	}

	/* default value of the number of lines */
	g_argument_n = 10;

	/* process arguments */
	argument_process(argc, argv);

	/* handle multiple file inputs */
	for(i = 0; i < g_file_num; i++){ 
		FILE *fp = fopen(files[i],"r");
		if(fp == NULL){	
			fprintf(stderr, "File doesn't exist\n");
			exit(-1);
		}
		/* get file size */
		fseek(fp, 0, SEEK_END);
		int sz = ftell(fp);
		files_prev_size[i] = sz;
		/* print head to separate files */
		if(g_file_num > 1){
			printf("\n==> %s <==\n",files[i]);
		}
		tail_n_lines_print(fp, g_argument_n, files_prev_size[i]);
		fflush(stdout);
		fclose(fp);
	}
	
	/* if the argument f is specified */
	if(g_argument_f){
		while(TRUE){
			for(i = 0; i < g_file_num; i++){ 
				follow_print(files[i], &files_prev_size[i]); 
				fflush(stdout);
			}
			/* sleep for one second */
			usleep(1000000);			
		}
	}
	return 0;
}
