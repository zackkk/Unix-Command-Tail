/*
 *	 FILE: tail.c
 * AUTHOR: Zack
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

#define FALSE 0
#define TRUE 1
#define FILE_NUM_LIMIT 10
#define FILE_NAME_SIZE 20

int g_print_line_num = 0;
int g_follow = FALSE;
int g_prev_file_size = 0;
int g_file_num = 0;

// move fp backward, count the number of '\n', 
// use letterCount to check the corner case: the num of existing lines < n;
void tail_n_lines_print(FILE *fp, int n){
	//char *buffer = malloc[BUFFERSIZE];
	int relativeToEnd = -1;
	int lineCount = 1;
	char c;	
	printf("g_prev_file_size: %d\n", g_prev_file_size);	
	while( -relativeToEnd <= g_prev_file_size && lineCount <= n){
		fseek(fp, relativeToEnd, SEEK_END);
		c = fgetc(fp);
		//printf("%c", c);
		if(c == '\n') lineCount++;
		relativeToEnd--;
	}
	// adjust pointer in the file (not *fp) if it exceeds the beginning frontier.
	if( -relativeToEnd  > g_prev_file_size){
		relativeToEnd++;
		fseek(fp, relativeToEnd, SEEK_END);
	}
	
	while((c = fgetc(fp)) != EOF){ 
		printf("%c",c);
		fflush(stdout);
	}
}

void follow_print(char *inputFileName){
	while(TRUE){
		usleep(2000000); // sleep one second, avoid busy wait;
		FILE *fp = fopen(inputFileName,"r");
		if(fp == NULL){	
			fprintf(stderr, "File doesn't exist\n");
			exit(-1);
		}
		fseek(fp, 0, SEEK_END);
		int sz = ftell(fp);
		// file size keeps growing
		
		if(sz > g_prev_file_size) {
			fseek(fp, g_prev_file_size, SEEK_SET);
			char c;
			while((c = fgetc(fp)) != EOF){ 
				printf("%c",c);
			}
			// update previous file size
			g_prev_file_size = sz;
		}
		fclose(fp);
	}
}

int main(int argc, char *argv[]){
	int i = 0;
	/* assert the first argument is "./tail" */
	assert(strcmp(argv[0], "./tail") == 0);

	/* initialization */
	char files[FILE_NUM_LIMIT][FILE_NAME_SIZE];
	i = 0;
	for(; i < FILE_NUM_LIMIT; i++){
		strcpy(files[i], "");
	}
	/* default value of the number of lines */
	g_print_line_num = 10;

	/* check all arguments */
	i = 1;
	for(; i < argc; i++){ 
		/* case 1: the argument is "-n", then it must followed by a number */
		if(strcmp(argv[i], "-n") == 0){
			if(i + 1 < argc){
				/* if -n is specified multiple times, then it would cover the previous one */
				g_print_line_num = atoi(argv[i+1]);
				if(g_print_line_num == 0){ 
					fprintf(stderr, "Invalid parameter of -n\n");
					exit(-1);
				}
				/* treat it as its absolute value if it is negative */
				else if(g_print_line_num < 0){ 
					g_print_line_num = -g_print_line_num;
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
			g_follow = TRUE;	
		}
		/* case 3: the argument is a file name */
		else{
			strcpy(files[g_file_num], argv[i]);
			g_file_num++;
		}
	}

	/* handle multiple file inputs */
	i = 0;
	fflush(stdout);
	for( ; i < g_file_num; i++){ 
		FILE *fp = fopen(files[i],"r");
		if(fp == NULL){	
			fprintf(stderr, "File doesn't exist\n");
			exit(-1);
		}
		/* get file size */
		fseek(fp, 0, SEEK_END);
		int sz = ftell(fp);
		g_prev_file_size = sz;
		printf("==> %s <==\n",files[i]);
		fflush(stdout);
		tail_n_lines_print(fp, g_print_line_num);
		fclose(fp);
	}
	
	i = 0;
	if(g_follow){
		for( ; i < g_file_num; i++){ 
			/* if -f is specified */
			fflush(stdout);
			follow_print(files[i]); 
		}
	}
	return 0;
}
