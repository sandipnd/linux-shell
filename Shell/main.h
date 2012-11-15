#ifndef _SHELL_H
#define _SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#define MAXCMDSIZE 100
#define MAXARGSIZE  50
#define MAXSEMICOLON 20
//typedef struct StoreCommand StoreCommand; 
 typedef struct StoreCommand {
  char *commandsArg[MAXCMDSIZE]; 
  int FirstElement;
  int CheckPipeBegining;  //begining of the pipe , like a > b > c | d | e so d is the begining of pipe
  int CheckPipeEnd;
  int commandArgCount; //check the number of arguments
  int outRedirAppend;  // flag to check whther > or >> , both will act same regarding output redirection except >> will append
  int ISPIPE;  // to check whether the node is part of pipe
  int ISINREDIR; // to check whether the node is part of inredirection
  int ISOUTREDIR; // to check whether the node is part of outredirection
  struct StoreCommand *pipe;
  struct  StoreCommand *inredir;
  struct StoreCommand *outredir;
 }Store;

//extern StoreCommand *Store;
extern int parseInput( char *,Store **,int *); // A function to parse. parser is the heart of the Shell program
//extern void *delNode( queue *);
extern char inputArray[MAXSEMICOLON][MAXCMDSIZE * MAXARGSIZE]; // the Array where we store the input data
extern pid_t pid;
extern int argCount;

//extern Store *head;

#endif
