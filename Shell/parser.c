#include "parser.h"
#include"main.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>


int AMPERS;
int 
 parseInput( char *inputArr,Store **head,int *background)

  {
     Store *commandHead,*h22;
     int LOOPFLAG = 1; 
     int INREDIRFLAG = 0;
     int OUTREDIRFLAG = 0;
     int checkFlag = 1;
     int argCount = 0;
     int c=0,cmdCount=0,parseState,commandTrace=0;
     parserEngine *parse = (parserEngine *)malloc(sizeof(parserEngine));
     parse->state= START;
     
    // strcpy(parse->bufrx,"HELLO");;

     while ( LOOPFLAG )

      {
        if (argCount > MAXARGSIZE ) {
          printf(" The max argsize is attained : So exiting the program \n");
          return FALSE;
          }
        //WHERE
        if ( cmdCount >= MAXCMDSIZE )
           printf("COMMAND size is more than MAX ");
        switch (parse->state) { 

         case START :
               //WHERE
              //we are now in the begining of the start state
               if ( '\t' == inputArr[c] || ' ' ==  inputArr[c] )
                  parse->state=START;
               else if ( '\n' == inputArr[c] )
                  parse->state = BLANK;
               else if ( '<' == inputArr[c] || '|' ==  inputArr[c]  || '>' == inputArr[c] || '&' == inputArr[c] || ';' == inputArr[c])
                   parse->state = ERROR;
               else {
//link list head is generated  As we are in START state
                   commandHead=createNode();
                  // commandHead->FirstElement = 1;
                   *head=commandHead; // head is passed as reference and we assign the address of header to it.
                   h22=commandHead;
// When we get any character we are moving to COMMAND state
                   parse->state = COMMAND; // we have got a character so moving to COMMAND state
                   parse->bufrx[cmdCount++]=inputArr[c];
                 }
               break;
         
         case COMMAND :
// we are in the COMMAND state , now if we reach 
                 if ( '\n' == inputArr[c]  ) {
                  //WHERE
                   if (cmdCount) {
                   //if newline  character is obtained after the
		           parse->bufrx[cmdCount]='\0';
                          // command is copied to the link list
		          commandHead->commandsArg[commandTrace]=(char *)malloc(strlen(parse->bufrx)+1);
		           strcpy(commandHead->commandsArg[commandTrace],parse->bufrx);
		           commandTrace++; 
		       //   commandHead->commandsArg[commandTrace]=NULL;
		           commandHead->commandArgCount=commandTrace;
                           //commandHead->CheckPipeEnd =1;
		          //WHERE
		           // copy to link list
                           // unset the variables , although i feel its of no use 
		           cmdCount=0;
		           commandTrace=0;
		           OUTREDIRFLAG = 0;
		           INREDIRFLAG =  0;
                           
                  }
                 // printf("\n");
           //      printf(" \n buffer %s ",parse->bufrx);
                    if ( strcmp (parse->bufrx,"exit") == 0) {
                       parse-> state = NEWLINE;
                       //LOOPFLAG=0;
                      }
                    parse-> state = NEWLINE;
                  }
                   else if ( '|' == inputArr[c]) {
                     // add to link list
                    if ( cmdCount ) {
                      parse->bufrx[cmdCount]='\0';
                     // printf("\n buffer %s \n",parse->bufrx);

            //Allocate memory before copying else same address will be stored every where.
                      commandHead->commandsArg[commandTrace]=(char *)malloc(strlen(parse->bufrx)+1);
                      strcpy(commandHead->commandsArg[commandTrace],parse->bufrx);
                      commandTrace++; 
                      commandHead->CheckPipeEnd =1;
                      //commandHead->commandsArg[commandTrace]=NULL;
                      commandHead->commandArgCount=commandTrace;
                      
                     } 
       // whewe get a pipe we create a pipe node .
                      commandHead->pipe=createNode();
                      commandHead=commandHead->pipe;
                      commandHead->ISPIPE=1;
      // if pipe has occured after > or < , we are setting the pipe that time
                      if ( !checkFlag )
                       commandHead->CheckPipeBegining= 1;
                       checkFlag = 1;
                       parse->state= PIPE;
                       cmdCount=0;
                       commandTrace=0;
                      }
                   else if ( '<' == inputArr[c])  {
                         //add to link list
                    if (cmdCount ) {
		              parse->bufrx[c]='\0';
                            //  printf("\n buffer %s \n",parse->bufrx);
		              commandHead->commandsArg[commandTrace]=(char *)malloc(strlen(parse->bufrx)+1);
		              strcpy(commandHead->commandsArg[commandTrace],parse->bufrx);
		              commandTrace++; 
                              commandHead->CheckPipeEnd =1;
                                    
		        //      commandHead->commandsArg[commandTrace]=NULL;
		              commandHead->commandArgCount=commandTrace;
		              }
            // creating node against inredirection                   
		              commandHead->inredir=createNode();
		              commandHead=commandHead->inredir;
                              commandHead->ISINREDIR = 1;
		              parse->state = INREDIR;
		              cmdCount=0;
              // checkflag variable is to determine the position from which pipe started , like ls > a > b | cat a | wc pipe started from cat 
                              checkFlag=0;
		              commandTrace=0;
                       }
                   else if ( '>' == inputArr[c] )  {
                       parse->state=OUTREDIR;
                       if ( cmdCount) {
                      parse->bufrx[cmdCount]='\0';
                     // printf("\n buffer %s \n",parse->bufrx);
                      commandHead->commandsArg[commandTrace]=(char *)malloc(strlen(parse->bufrx)+1);
                      strcpy(commandHead->commandsArg[commandTrace],parse->bufrx);
                      //commandHead-> ISPIPE = 1;
                      commandTrace++;
                
                     // commandHead->commandsArg[commandTrace]=NULL;
                       }
                      checkFlag=0;
                      commandHead->outredir=createNode();
                       
                      commandHead=commandHead->outredir;
                      cmdCount= 0;
                      commandHead->ISOUTREDIR = 1;
                      commandTrace=0;
                      if ( '>' == inputArr[c+1]) {
                           c++;
                           commandHead->outRedirAppend=1;
                         }  
                            
                      // add to Link list
                        
                      }
                   else if ( '&' ==  inputArr[c] ) {
                            //parse->state = COMMAND;
                     
                        *background = 1;
                        if ( cmdCount ) {
                            parse->bufrx[cmdCount]='\0';
                        // copy to link list
                        commandHead->commandsArg[commandTrace]=(char *)malloc(strlen(parse->bufrx)+1);
                        strcpy(commandHead->commandsArg[commandTrace],parse->bufrx);
                        commandTrace++;
                        cmdCount= 0;
                          }
                       parse->state = NEWLINE;
                         }  
                   else if ( '\t' == inputArr[c] || ' ' ==  inputArr[c] ) {
                            parse->state = COMMAND;
                        if ( cmdCount ) {
                            parse->bufrx[cmdCount]='\0';
                        // copy to link list
                        commandHead->commandsArg[commandTrace]=(char *)malloc(strlen(parse->bufrx)+1);
                        strcpy(commandHead->commandsArg[commandTrace],parse->bufrx);
                        commandTrace++;
                        cmdCount= 0;
                        
                          }
                         }  
                       
                   else {
                         //WHERE
                        parse->bufrx[cmdCount]=inputArr[c];
                        cmdCount++;
                        parse->state = COMMAND;
                          }
                         break;
                  case PIPE:
                       if ( '\t' == inputArr[c] || ' ' ==  inputArr[c] )
                        parse->state = PIPE;
                       else if ( '|' == inputArr[c] || '>' == inputArr[c] || '<' == inputArr[c] || '\n' == inputArr[c] || '&' == inputArr[c] || ';' == inputArr[c]) {
                        //WHERE
                         parse->state = ERROR;
                      } 
                       else  {
                     // WHERE
                         parse->state = COMMAND;
                         cmdCount= 0;
                         continue;
                         }
                         break;
                    case INREDIR :
                       if ( '\t' == inputArr[c] || ' ' ==  inputArr[c] )
                         parse->state = INREDIR;
                       else if ( '|' == inputArr[c] || '>' == inputArr[c] || '<' == inputArr[c] || '&' == inputArr[c] || ';' == inputArr[c])
                         parse->state = ERROR; 
                       
                      else if ( '\n' == inputArr[c])
                          parse->state = ERROR; 
                      else {
                         parse->state = COMMAND;
                         INREDIRFLAG = 1;
                         cmdCount= 0;
                         continue;
			   }
                         break;
                      case OUTREDIR :
                       if ( '\t' == inputArr[c] || ' ' ==  inputArr[c] )
                         parse->state = OUTREDIR;
                       else if ( '|' == inputArr[c] || '<' == inputArr[c] || '&' == inputArr[c] || ';' == inputArr[c])
                         parse->state = ERROR; 
                       
                      else if ( '\n' == inputArr[c])
                          parse->state = ERROR; 
                      else {
                         parse->state = COMMAND;
                         OUTREDIRFLAG = 1;
                         cmdCount= 0;
                         continue;
			   }
                         break; 
                       case ERROR :
                          //printf("\n Error has occurred \n");
                          LOOPFLAG=0;
                        break;
                       case NEWLINE :
                          //err(" Error has occurred \n");
                          LOOPFLAG=0;
                        break;
                       case BLANK :
                           // WHERE
                            LOOPFLAG=0;
                        break;
                       case EXIT :
                          printf(" exit has encountered ");
                           LOOPFLAG = 0;
                       default:
                           printf("BOOM\n");
               }
            c++;
          } //while ( LOOPFLAG && inputArr[c++] != '\n');
 

//if we free the parse memory , the state variable will not be present so take a back up;
  parseState = parse->state;
 
  free(parse); // free the allocated memory, to avoid memory leak
  if ( parseState == NEWLINE ) return TRUE;
  if( parseState == ERROR || parse->state == BLANK) {
            printf("\n Either string is blank or error in input string , so try again\n");
            return FALSE;
       }   
  //return TRUE;  
  }
//creating a empty node , and initialising the value while setting.
Store 
  *createNode ( )
{
   Store *headd ;
   int i;
   headd=( Store *)malloc(sizeof(Store));
       if ( headd == NULL ) return NULL;
       headd->outRedirAppend=0;
       headd->commandArgCount=0;
       headd->pipe = NULL; 
       headd->inredir= NULL;
       headd->outredir=NULL;
    for ( i =0 ; i < 30 ; i++)
       headd->commandsArg[i]=NULL;
       headd->CheckPipeBegining=0;
       headd->CheckPipeEnd=0;
       headd->FirstElement=0;
       headd ->ISPIPE=0;
       headd ->ISINREDIR=0;
       headd ->ISOUTREDIR=0;
   return headd;
  }
  
