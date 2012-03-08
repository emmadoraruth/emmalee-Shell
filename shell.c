#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <sys/wait.h>

#include "parser.h"

void redirect(char ** command, char * infile, char * outfile);
void piped(char ** com1, char ** com2);
void execute(char ** command);

/* *redirect*

    Input: "command" in the form of a pointer to a two-dimensional character array
           Two file names "infile" and "outfile", in the form of pointers to character arrays

    Description: Appropriately redirects STDIN and/or STDOUT, calls execute on "command", and fixes STDIN and STDOUT

    Output: void
*/

void redirect(char ** command, char * infile, char * outfile){

  int id, od, fdo, fdi;
		
  if(infile){
    fdi = open(infile, O_RDWR, 0664);
    if(fdi > 0){
      id = dup(STDIN_FILENO);
      dup2(fdi, STDIN_FILENO);
    }
  }
  if(outfile){
    fdo = open(outfile, O_RDWR | O_CREAT, 0664);
    od = dup(STDOUT_FILENO);
    dup2(fdo, STDOUT_FILENO);
  }
  execute(command);
  if(infile && fdi > 0){
    dup2(id, STDIN_FILENO);
    close(fdi);
  }
  if(outfile){
    dup2(od, STDOUT_FILENO);
    close(fdo);
  }

}

/* *piped*
   
   Input: Two commands, "com1" and "com2" in the form of pointers to  two-dimensional character arrays

   Description: Calls redirect on "com1" with NULL as the infile and ".temp" as the outfile and calls redirect on "com2" with ".temp" as the infile and NULL as the outfile (thus directing STDOUT of "com1" to STDIN of "com2"); then removes .temp

   Output: void
*/

void piped(char ** com1, char ** com2){

  redirect(com1, NULL, ".temp");	
  redirect(com2, ".temp", NULL);
  remove(".temp");	
}
  
/* *execute*

   Input: A command, in the form of a two-dimensional character array
   
   Desccription: Forks a child process and executes command.

   Output: void
*/

void execute(char ** command){

  int p, status, err;
  
  p = fork();
    
  if(p == 0){
    err = execvp(command[0], &command[0]);
    if(err == -1)
      printf("Error: Command %s not found.\n", command[0]);
    exit(0);
  }

  else
    waitpid(p, &status, 0);    
    
}      

/* *main*

   Input: none

   Description: Prompts user for input, interprets the input, and calls proper functions to execute the input
   							Continues until user exits the shell

   Output: 0
*/

int main(){
    
  while(1){
    char input[256], ** parsedc, ** parseds,  ** parseds2, ** parseds3, ** parsedp, ** parsedr, **parsedr2, buffer[256];
    int p, q, i, status;
    i = 0;
    parseds = (char **)malloc(16 * sizeof(char *));
    for (p = 0; p < 16; p++)
      parseds[p] = (char *)malloc(64 * sizeof(char));
    parsedc = (char **)malloc(16 * sizeof(char *));
    for (p = 0; p < 16; p++)
      parsedc[p] = (char *)malloc(64 * sizeof(char));
    parsedp = (char **)malloc(16 * sizeof(char *));
    for (p = 0; p < 16; p++)
      parsedp[p] = (char *)malloc(64 * sizeof(char));
    parseds2 = (char **)malloc(16 * sizeof(char *));
    for (p = 0; p < 16; p++)
      parseds2[p] = (char *)malloc(64 * sizeof(char));
    parsedr = (char **)malloc(16 * sizeof(char *));
    for (p = 0; p < 16; p++)
      parsedr[p] = (char *)malloc(64 * sizeof(char));
    parsedr2 = (char **)malloc(16 * sizeof(char *));
    for (p = 0; p < 16; p++)
      parsedr2[p] = (char *)malloc(64 * sizeof(char));
    parseds3 = (char **)malloc(16 * sizeof(char *));
    for (p = 0; p < 16; p++)
      parseds3[p] = (char *)malloc(64 * sizeof(char));

    getcwd(buffer, (size_t)256);
    printf("emmalee: %s$ ",  buffer);
    fgets(input, sizeof(input), stdin);
    *(strchr(input, '\n')) = 0;
  
    if(!(strncmp(input,"exit", 4)))
      return 0;
  
    parse(input, parsedc, ";");

    while(parsedc[i]){
  
      if(!parse(parsedc[i], parsedr, "|")){
				parse(parsedr[0], parseds, " ");
				parse(parsedr[1], parseds2, " ");
				piped(parseds, parseds2);
      }
		
      else if(!parse(parsedc[i], parsedr, "<") && !parse(parsedc[i], parsedr, ">")){
				if(!parse(parsedr[0], parsedr2, "<")){
	  			parse(parsedr2[0], parseds, " ");
	  			parse(parsedr2[1], parseds2, " ");
	  			parse(parsedr[1], parseds3, " ");
				}
				else if(!parse(parsedr[1], parsedr2, "<")){
					parse(parsedr[0], parseds, " ");
	  			parse(parsedr2[0], parseds3, " ");
	  			parse(parsedr2[1], parseds2, " ");
				}
				redirect(parseds, parseds2[0], parseds3[0]);
			}
		
      else if(!parse(parsedc[i], parsedr, ">")){
				parse(parsedr[0], parseds, " ");
				parse(parsedr[1], parseds2, " ");
				redirect(parseds, NULL, parseds2[0]);
      }
		
      else if(!parse(parsedc[i], parsedr, "<")){
				parse(parsedr[0], parseds, " ");
				parse(parsedr[1], parseds2, " ");
				redirect(parseds, parseds2[0], NULL);
      }
		
      else{
				parse(parsedc[i], parseds, " ");
				if(!strncmp(parseds[0], "exit", 4))
	  			exit(0);
				else if(!strncmp(parseds[0], "cd", 2)){
	  			if(parseds[1])
	    			chdir(parseds[1]);
	  			else
	    			chdir(getenv("HOME"));
				}
				else
	  			execute(parseds);
      }
		
      i++;
    } 
  }
  return 0;
}
