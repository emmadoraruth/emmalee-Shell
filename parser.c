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

/* *parse*

   Input: A pointer to a two-dimensional character array, "output"
          Pointers to two strings: "input" and "delimiter"
	  
   Description: Parses the characters in "input" by the delimeter, modifies output, filling it with
                the strings resulting from the parse.

   Ouput: An int, indicating whether any instances of the delimeter were found in "input"
*/
int parse(char * input, char ** output, char * delimeter){
  
  char input2[256];
  strncpy(input2, input, strlen(input));
  input2[strlen(input)] = (char)NULL;
  char * curr, * prev;
  int i, j;

  i = strlen(input2) - 1;
  while(!(input2[i] - 32)){
    input2[i] = (char)NULL;
    i--;
  }
  i = 0;
  while(!(input2[i] - 32))
    i++;
  j = i;
  while(input2[i]){
    input2[i - j] = input2[i];
    i++;
  }
  input2[i - j] = (char)NULL;
  
  curr = input2;
  prev = strsep(&curr, delimeter);

  i = 0;
  while(prev){
    output[i] = (char *)malloc(64 * sizeof(char));
    if(strncmp(prev, "", 1)){
      strncpy(output[i], prev, strlen(prev) + 1);
      i++;
    }
    prev = strsep(&curr, delimeter);
  }

  output[i] = NULL;
  
  return (output[1] == NULL);

}

  /*int main(){
  
    char ** test;
    char input[256];
    int i;
    test = (char **)malloc(16 * sizeof(char *));
    for (i = 0; i < 16; i++)
    test[i] = (char *)malloc(64 * sizeof(char));

    strncpy(input, "Hello my name is Lee.", 50);
    parse(input, test, " ");

    i = 0;
    while(test[i]){
    printf("%s\n",  test[i]);
    i++;
    }

    return 0;
    }
  */
