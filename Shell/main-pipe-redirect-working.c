#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include<error.h>
#include<unistd.h>
#include<fcntl.h>
#include"parser.h"
#include"main.h"
#include<sys/types.h>
#include<pwd.h>
#include<signal.h>

#define MAXCMDSIZE 100
#define MAXARGSIZE  50
#define MAXSEMICOLON 20
char inputArray[MAXSEMICOLON][MAXCMDSIZE * MAXARGSIZE];
char commandArray[MAXCMDSIZE * MAXARGSIZE];
//  WHERE is declared in parser.h a macro to track where i am :)
int argCount;
Store *head = NULL;;
void CommandFree ( Store **);
void copyInToCommand ( int);
int
main ( int argc , char *argv[])
{
int AMPERS = 0, DifferentCommands , j ,tmp, status1;
Store *tempHead, *currentHead , *TEMPHEADTOFREE;
char c,buffer[100],cdbuffer[1024],*shellPrompt ,cc;
char s[100];
char pipeBuffer[1000]; //1000 is any value dont be specific , this array is for redirection
int fd,stLen,fd1,count=0,i=0,input=-1,status,LINKFLAG=1;
int nullFlag,countChar,fd_pipe[2],old_fd_pipe[2],myPipe[2], retval;
int SHELLFLAG = 1 , retvalP; // to keep it running in LOOP
int flagPipe = 0 ,flagOutRedir = 0, flagInRedir = 0;
pid_t pid,pid1;
	//input Array is defined 
	// shell prompt will be myshell$ 
	//take inputArray and parse it 
	
if ( argc > 1 )
 shellPrompt = argv[1];
else
  shellPrompt = "MYSHELL";
 printf(" \n Welcome to MYSHELL . This Shell works for | ; & > < >> \n");
 printf (" Please see the cases_not_working.txt for the cases which i find not working during unit test \n");
 printf(" Code runs on Ubuntu 11.\n\n \n ");
 while ( SHELLFLAG ) {
 //printing the prompt of shell
    printf("%s :",shellPrompt);
    DifferentCommands=0;
    strcpy(pipeBuffer, " ");
       //unset count variable
        count= 0;
        AMPERS = 0;
// Inserting character one by one to the Array
	while (1) {
	  c=getchar();
           if ( c == EOF ) {
             printf("\n");
             exit(0);
            }         
	  inputArray[DifferentCommands][count++] = c;
          if ( c == ';') {
            DifferentCommands ++;
            count=0;
           }
	  if ( count >=  MAXCMDSIZE * MAXARGSIZE ) {
	//	       stderr("the input statement length is greater than buffersize : exiting");
		exit(1);
	       }
	  if ( c == '\n') 
		   break;
	}
  
	i=0;
      // calling parse function
        head = NULL; 
 // execute the commands for each ';' encountered
        for ( j = 0 ; j <= DifferentCommands ; j++ ) {
        copyInToCommand(j);
        retvalP = parseInput(commandArray,&head,&AMPERS);
             
        // EXIT from SHELL IF exit encountered.
        if ( retvalP == 0 )
              continue;
        if ( strcmp(head->commandsArg[0],"exit") == 0 ) 
		exit(0);
       
	if ( retvalP ) {
                TEMPHEADTOFREE = head;
//******************** TRIAL CODE FOR DISPLAY AND LINK
                // to check whether the links are formed properly  printing out linklist
		/*LINKFLAG = 1;
                currentHead = head;
		while ( LINKFLAG) {   
		i=0;
		while(head->commandsArg[i] != NULL )  
		printf(" number %s \t %d  pb %d",head->commandsArg[i++], head->ISPIPE,head-> CheckPipeBegining);
		if ( head-> pipe )       {
		printf("\n pipe \n");LINKFLAG=1; head=head->pipe; }
		else if( head->inredir)  { printf("\n inredir : %d\n", head-> ISINREDIR);LINKFLAG=1; head=head->inredir;}
		else if ( head->outredir){ printf("\n outredir %d\n" , head->ISOUTREDIR);LINKFLAG=1; head=head->outredir;}
		else 
		 LINKFLAG=0;
		}
		 head=currentHead;
               */
//*******************************************************
		// //code help is taken from http://stackoverflow.com/questions/948221/does-this-multiple-pipes-code-in-c-makes-sense
		// maurice bach chapter 7
		
                //to take care of first element
		if ( head-> pipe ) head-> ISPIPE=1;
		if (head -> outredir ) head->ISOUTREDIR=1;
		if (head -> inredir ) head->ISINREDIR=1;
		head->FirstElement = 1;
                pid = fork();
                if ( pid < 0 ) {
                  perror(" Error calling Fork : ");
                  break;
                 } 
     // this is to track the first head so that we can free the list from head
                
		if ( pid == 0 ) {  
			LINKFLAG=1;
			input=-1;
			while ( LINKFLAG )
			{
			//#####################################
			// START OF PIPE 
			// head-pipe condition to check the initial element , ispipe is to check whther its a part of pipe , its associated it all elements after the pipe.
                       flagPipe = 0;
			if ( head-> ISPIPE || head->pipe) {
			// WHERE
                        //head->ISPIPE=1;
			//printf("\n");
			//nullflag variable is to check how long we need to create the pipe , if end of command is reached we will not need pipe as output will be on STDOUT
			nullFlag = head->pipe || head->outredir || head->inredir;
			if (nullFlag) {
			     if ( pipe(fd_pipe) == -1 ) { //call pipe here so that fd_pipe can be shared 
		               perror("Error Calling Pipe : ");
		               goto INREDIREXIT;
		              }
		          }
		        pid1 = fork();
		        if ( pid1 < 0 ) {
		          perror(" Error calling Fork : ");
		          goto INREDIREXIT;
		         }
			if ( pid1 == 0) {
			   // we consider that 
			  // if ( !head-> CheckPipeBegining  && !head -> FirstElement)  
			     
			    if ( !head-> CheckPipeBegining  && !head -> FirstElement) {
			//WHERE
			//check the link specified above for the concept.
                            if (!head -> outredir || !head -> inredir) // to handle redirection from PIPE
			       {
                            
                                dup2(old_fd_pipe[0],0);
			        close(old_fd_pipe[0]);
			        close(old_fd_pipe[1]);
                               } 
			     }
			    if (nullFlag) {
			// WHERE
			      close(fd_pipe[0]);
			      dup2(fd_pipe[1],1);
			      close(fd_pipe[1]);
			     }
			   if ( (tmp =execvp(head -> commandsArg[0],head->commandsArg)) < 0 ) {
                               
                                 
                                // kill(pid, SIGINT);
                                 perror(" Error calling execvp in pipe : ");
                                 kill(pid1, SIGQUIT);
                                 close(fd_pipe[0]);
                                 close(fd_pipe[1]);
                                 close(old_fd_pipe[0]);
			         close(old_fd_pipe[1]);
                                 j++;
                                 goto INREDIREXIT;
                                  // exit from the loop
			      }
			} 
			  else {

			    if ( !head -> CheckPipeBegining  && !head -> FirstElement ) {
			//WHERE
			        if (!head -> outredir || !head  -> inredir) // to handle redirection from PIPE for cases ls | wc > A
			        {
                                 dup2(old_fd_pipe[0],0);
			         close(old_fd_pipe[0]);
			         close(old_fd_pipe[1]);
                                } 
			      }
			    if (nullFlag) {
			 // transfer old pipe values to new one
			      old_fd_pipe[0]=fd_pipe[0];
			      old_fd_pipe[1]=fd_pipe[1];
			      }
			       wait(&status1);
			   }
			  currentHead=head;
                          flagPipe=1;
			 }
			//end of pipe
			//##################################################
			//begining of output redirection
			 else if( head -> ISOUTREDIR) {
			// the tempHead variable is for the first variable commands like ls > A , so to take care ls
			  
			  //temphead needed for the first element
			  tempHead = head;
			  while( head -> outredir != NULL)
			   {
			// We are doing this to take care of multiple redirection like ls > a > b > c 
			   fd=open(head -> outredir -> commandsArg[0],O_RDWR | O_CREAT);
                            if ( fd == -1 ) // error in file opening
			      {
			       fprintf(stderr," file open error : filename %s \n", head -> outredir -> commandsArg[0]);
			       goto INREDIREXIT; // exit from the loop
			      }
			   head=head->outredir;
			    }
			   if ( head-> outRedirAppend ) {
			     fd=open(head ->commandsArg[0], O_RDWR | O_CREAT | O_APPEND,0777);    
                             if ( fd == -1 ) // error in file opening
			      {
			       fprintf(stderr," file open error : filename %s \n", head -> commandsArg[0]);
			       goto INREDIREXIT; // exit from the loop
			      }
                            } 
			      else  {
			      fd=open(head->commandsArg[0], O_RDWR | O_CREAT, 0777);
                                if ( fd == -1 ) // error in file opening
			       {
			       fprintf(stderr," file open error : filename %s", head -> commandsArg[0]);
			       goto INREDIREXIT; // exit from the loop
			       }
                             }
			// for output redirection we close the STDOUT and redirect it to the file 
                          if ( fork () == 0) { 
			     dup2(fd,1);
			     //close(1); //close(1) will not work here for the first element , as before redirecting to STDOUT , it closed:
			// As we said tempHead is for the 1st variable like cat f1 > 1
			     if ( tempHead -> FirstElement ) {
				if ( (tmp=execvp(tempHead -> commandsArg[0],tempHead->commandsArg)) < 0) {
		                 close(fd);
                                 close(1);
                                 perror(" Error calling execvp : ");
                                 goto INREDIREXIT;
                                 }
		               }
			    // it  is for the scenario ls | grep p | wc > 1 , the output of pipe will be stored here
// modifying 
                         //  ||
			     if (  currentHead -> ISPIPE ) {
                                
                     //   WHERE           
                                 close(1); //close(1); // fix , as so many things are going to output , not working properly .
				 countChar=read(fd_pipe[0],&pipeBuffer,900);  // read from pipe
                                 write(fd,&pipeBuffer,countChar);   //write or append to the file
                                  
                               
				 close(fd);                  //close the file descriptor
				 close(fd_pipe[0]);
				 close(fd_pipe[1]);
                                 // flagPipe = 0;
			       }
			    // to take care cases like wc < f1 > f2
                            /*
			      if ( currentHead -> ISINREDIR ) {
                                  //close(1);
				  countChar=read(myPipe[0],&pipeBuffer,900);  // read from pipe
				  write(fd,&pipeBuffer,countChar);   //write or append to the file
				  close(fd);                  //close the file descriptor
				  close(myPipe[0]);
				  close(myPipe[1]);
			       } */
                            }
                            else
                               wait(&status);
                             currentHead=head;			   
			      
			 } 
			//end of output redirection
			//###########################################################################
			// begining of input redir
			 else if ( head-> ISINREDIR) {
                         //WHERE
			  tempHead=head;
			// for the cases wc < A < B < C
			  while( head -> inredir != NULL)
			   {
			     if(pipe(myPipe) == -1 ) {
		               perror(" Error calling pipe : ");
		               goto INREDIREXIT;
		              }
			     fd=open(head -> inredir -> commandsArg[0],O_RDONLY); // we are reading the file only , like tac < f1 , we dont write anything
			     if ( fd == -1 ) // error in file opening
			      {
			       fprintf(stderr," file open error : filename : %s \n",head -> inredir -> commandsArg[0]);
			       goto INREDIREXIT; // exit from the loop
			      }
			     head=head->inredir;
			    }
			    fd=open(head->commandsArg[0], O_RDWR , 0777);  
			    if ( fd == -1 ) // error in file opening
			       {
				  fprintf(stderr," file open error :filename :  %s \n ",head -> commandsArg[0]);
				  goto INREDIREXIT;
				}
                               char t[30];
                               sprintf(t,"%s", head->commandsArg[0]);              
                        if ( fork() == 0 ) {
                             
			     //close(0); // consider the fd as STDIN
			     
			     
			    // this is to take care all cases like ls | grep w > 11 > 12 > 13 < 14 
			   // testcases : ls | grep w > 11 > 12 | wc < 14 , wc will take 14 as input
			      nullFlag = head->pipe || head->outredir || head->inredir;
                              
			    if (!nullFlag) {
                              dup2(fd,0);
			// this is for following :
			   // testcases : ls | grep w > 11 > 12 | wc < 14 , wc will take 14 as input and print output
			      if ( tempHead -> FirstElement ) {
				 currentHead=tempHead;  // i know it makes no sense for the step.
                              //WHERE
			       if ( (tmp =execvp(currentHead -> commandsArg[0],currentHead->commandsArg)) < 0 ) {
		                  close(fd);
                                  close(0);
                                  perror(" Error calling execvp : ");
                                  goto INREDIREXIT;
                                }
			      }
			      if ( currentHead -> ISPIPE ) {  //testcase ls | grep w | wc < A < B , working properly.
                             //  WHERE
				  if ( (tmp = execvp(currentHead -> commandsArg[0],currentHead->commandsArg)) < 0 ) {
		                  perror(" Error calling execvp : ");
                                  
                                  goto INREDIREXIT;
                                  }
                                }
                               if ( currentHead -> ISOUTREDIR )
                                      execvp(currentHead -> commandsArg[0],currentHead->commandsArg);
			     }
			    else {
                                                          
		           // if there is any command after > like tac < f1 > f2 , so there will nothing on stdout
                              // if ( head -> ISOUTREDIR ) {
                                   if ( head-> outredir ) {
                                   //head=head-> outredir;
                                   //fd1=open(head -> commandsArg[0], O_RDWR | O_CREAT | O_APPEND,0777);          
                                   
                                   
                                    while( head -> outredir != NULL)
				   {
				// We are doing this to take care of multiple redirection like ls > a > b > c 
				   fd1 = open(head -> outredir -> commandsArg[0],O_RDWR | O_CREAT);
				    if ( fd1 == -1 ) // error in file opening
				      {
				       fprintf(stderr," file open error : filename %s \n", head -> outredir -> commandsArg[0]);
				       goto INREDIREXIT; // exit from the loop
				      }
				   head=head->outredir;
				    } //end of while
				   if ( head-> outRedirAppend ) {
				     fd1=open(head ->commandsArg[0], O_RDWR | O_CREAT | O_APPEND,0777);    
				     if ( fd1 == -1 ) // error in file opening
				      {
				       fprintf(stderr," file open error : filename %s \n", head -> commandsArg[0]);
				       goto INREDIREXIT; // exit from the loop
				      }
				    } 
				      else  {
				      fd=open(head->commandsArg[0], O_RDWR | O_CREAT, 0777);
					if ( fd1 == -1 ) // error in file opening
				       {
				       fprintf(stderr," file open error : filename %s", head -> commandsArg[0]);
				       goto INREDIREXIT; // exit from the loop
				       }
                                      } //end of else */
                                     
                                     // dup2(fd1,1);
                                      //close(1);
                                      stLen = strlen(currentHead -> commandsArg[0]);
                                      currentHead -> commandsArg[0][stLen+1]='\0';
				      sprintf(s,"%s < %s > %s" ,currentHead -> commandsArg[0],t,head->commandsArg[0]);
                                      //printf(" %s == s ",s);
                                      system(s);
                                   } 
                                 
                                  //close(myPipe[0])    
                              }  // end of else
                           } //end of fork
                         else { 
                           close(fd);
                           close(fd1);                            
                           wait(&status);
                           }
                  	     currentHead=head;
			     
	} 
			else  { 
				// if the first element is cd , then change directory
				 if( strcmp(head->commandsArg[0],"cd") == 0 ) 
				       {   // for the commands cd 
                                          if ( head->commandsArg[1] == NULL || strcmp(head->commandsArg[1],"~") == 0 ) {
                                            struct passwd *pw=getpwuid(getuid());
                                            char *homedir = pw->pw_dir;
                                             chdir(homedir);
                                             getcwd(cdbuffer,sizeof(cdbuffer)); //to show the user which location he is in
					     printf("  current working directory %s \n",cdbuffer); //printing for the user
                                           }/*
                                          if ( strcmp(head->commandsArg[1],"..") == 0 ) {
                                             getcwd(cdbuffer,sizeof(cdbuffer));
                                             strgetprevdir(cdbuffer);
                                             chdir(homedir);
                                             getcwd(cdbuffer,sizeof(cdbuffer)); //to show the user which location he is in
					     printf("  current working directory %s \n",cdbuffer); //printing for the user
                                           }*/
                                         else  {
					  chdir(head->commandsArg[1]);  // command to change the directory
					  getcwd(cdbuffer,sizeof(cdbuffer)); //to show the user which location he is in
					  printf("  current working directory %s \n",cdbuffer); //printing for the user
                                          }
					} 
			
				// if only one command is there like ls only execute this.
				  else {
				      if ((tmp=execvp(head->commandsArg[0], head->commandsArg)) == -1 ) { 
				       perror(" Error calling execvp : ");
                                       goto INREDIREXIT;
                                      }
                                   }
				 }
				//traverse the list according to where any link exists or not.
				if     ( head-> pipe )       { head=head->pipe; continue;     } 
				else if( head->inredir)      { head=head->inredir; continue;  }  
				else if ( head->outredir)    { head=head->outredir; continue; }
				else 
				 LINKFLAG=0;   
				// if no file is there wwe will throw  error  
				INREDIREXIT :
				   LINKFLAG= 0;
				}
		// printf("\n ************** end of  command# %d: for better understanding ****************** \n",j); 
		}
             if ( AMPERS == 0 ) 
		// wait for the parent fork
		waitpid(pid,&status,0);
             else // if any back ground process running
                waitpid(pid,&status, WNOHANG);
	 }
       //to avoid memory leak;
      CommandFree(&TEMPHEADTOFREE); 
       } // end of multiple commands
        
   }
 return 1;
}
// free the list required to store the commands to avoid memory leak;
  void 
  CommandFree ( Store **tempHead) 
  {
     Store *cur,*prev;
     prev = *tempHead;
     if ( prev == NULL ) 
         return;
     for ( cur = *tempHead ;  ; ) 
       {
         if ( (cur -> pipe || cur -> inredir || cur -> outredir) == 0)
             {
               prev = cur;
               free(prev);
               return;
              }
         else {
           if ( cur -> pipe ) {
               prev= cur ;
               cur = cur->pipe;
               free(prev);
              }
            if ( cur -> inredir ) {
               prev= cur ;
               cur = cur-> inredir;
               free(prev);
              }
             if ( cur -> outredir ) {
               prev= cur ;
               cur = cur-> outredir;
               free(prev);
              }
           }
     }
 }
void
 copyInToCommand(int index)
 {
	 int i = 0,j = 0;
	 char c;
// continue the loop until we get ; or \n
         while ( (c = inputArray[index][j]) != ';' && (c = inputArray[index][j]) != '\n') 
          {
	  commandArray[j]= c;
	  j++;
	 }
	// this is in order to be compatible with the parse engine
	 commandArray[j] = '\n';
 }

