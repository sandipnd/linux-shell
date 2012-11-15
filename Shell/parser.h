#ifndef _PARSER_H
#define _PARSER_H


#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<ctype.h>
#include"main.h"

 enum { FALSE = 0 , TRUE = 1 };
//different states of the state machine
#define WHERE fprintf(stdout,"%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
#define START     0 
#define PIPE      1
#define COMMAND   2
#define INREDIR   3
#define ERROR     4
#define EXIT      5
#define NEWLINE   6
#define OUTREDIR  7
#define BLANK     8
#define AMPER     9
extern int INREDIRFLAG ;
extern int OUTREDIRFLAG ;


extern int countArg;

typedef struct parserEngine {

int state ; // check the states 
char bufrx[MAXCMDSIZE]; //receive buffer
}parserEngine;
//extern char *fileName; 
extern Store *createNode( );
extern void unset();
#endif
