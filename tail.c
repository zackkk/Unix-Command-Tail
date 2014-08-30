#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#define FALSE 0
#define TRUE 1
int NUM = 10;
int F = FALSE;
int BUFFERSIZE = 1000;
int PREVFILESIZE = 0;

/*
void printMchars(FILE *fp, int m){
	int count = 0;
	// move fp backward, count the number of chars
	while(count < m){
		count++;
		fp--;
	}
	count = 0;
	while(count < m){ 
		count++;
		fp++;
	}
} */

// move fp backward, count the number of '\n', 
// use letterCount to check the corner case: the num of existing lines < n;
void printNlines(FILE *fp, int n){
	//char *buffer = malloc[BUFFERSIZE];
	int relativeToEnd = -1;
	int lineCount = 0;
	int letterCount = 0;
	char c = '';
	
	while(letterCount < PREVFILESIZE && lineCount <= n){
		fseek(fp, relativeToEnd, SEEK_END);
		c = fgetc(fp);
		//printf("%c", c);
		if(c == '\n') lineCount++;
		letterCount++;
		relativeToEnd--;
	}
	// print N lines of file using count of '\n'
	while((c = fgetc(fp)) != EOF){ 
		printf("%c",c);
	}
}

/*
void followPrint(FILE *fp){
	fseek (fp, 0L, SEEK_END);
	int sz = ftell(fp);
	int diff = 0;
	if(sz > PREVFILESIZE) {
		diff = sz - PREVFILESIZE;
		PREVFILESIZE = sz;
	}
	printMchars(fp, diff);
}

*/

int main(int argc, char *argv[]){
	assert(strcmp(argv[0], "./tail") == 0);

	char* input_file_name = (char*)malloc(sizeof(100)); // handle multiple file names
	input_file_name = "FILENAME";

	int i = 1;
	for(; i < argc - 1; ){ // file name is the last argument
		if(strcmp(argv[i], "-n") == 0){
			if(i + 1 < argc){
				NUM = atoi(argv[i+1]);
				if(NUM <= 0 || NUM > 2147483647) 
					fprintf(stderr, "Invalid parameter n\n");
				i+=2;
			}
			else fprintf(stderr, "Invalid parameter n\n");
		}
		else if(strcmp(argv[i], "-f") == 0){
			F = TRUE;	
			i++;
		}
		else
			break;
	}
	for( ; i < argc; i++){ // handle multiple file inputs
		input_file_name = argv[i];
		FILE *fp = fopen(input_file_name,"r");
		if(fp == NULL){	
			fprintf(stderr, "File doesn't exist\n");
			exit(-1);
		}
		fseek(fp, 0, SEEK_END);
		int sz = ftell(fp);
		PREVFILESIZE = sz;
		printNlines(fp, NUM);
		/* if(F) followPrint(fp); */
		// fseek(fp, 0L, SEEK_SET);	 seek back to the beginning
		fclose(fp);
	}
	return 0;
}
