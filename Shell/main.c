//#include"main.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include<error.h>
#include<unistd.h>
#include<fcntl.h>
#include"parser.h"
#include"main.h"


#define MAXCMDSIZE 100
#define MAXARGSIZE  50
char inputArray[MAXCMDSIZE * MAXARGSIZE];
char commandArray[MAXARGSIZE][MAXCMDSIZE];
//MAXCMDSIZE * MAXARGSIZE
int argCount;
Store *head;
int pipe_exec ( char *,char **,int,int);
void run_pipeline ( Store *,int );

int
main ( )
{
char c,buffer[100],cdbuffer[1024];
int fd,count=0,i=0,input=-1,status,LINKFLAG=1,fd_in = -1,fd_out ,fd1;
int newpipe[2],oldpipe[2],pid,countChar,fd_pipe[2];
	//input Array is defined 
	// shell prompt will be myshell$ 
	//take inputArray and parse it 
	//

	while (1) {
          c=getchar();
	  inputArray[count++] = c;
	  if ( count >=  MAXCMDSIZE * MAXARGSIZE ) {
//	       stderr("the input statement length is greater than buffersize : exiting");
		exit(1);
	       }
          if ( c == '\n') 
                   break;
	}
  i=0;
  LINKFLAG=1;
  if (parseInput(inputArray,&head)) {
  // to check whether the links are formed properly  printing out linklist , it was for trial to 
/*
while ( LINKFLAG) {   
  i=0;
      while(head->commandsArg[i] != NULL )  
        printf(" number %s ",head->commandsArg[i++]);
      if ( head-> pipe )       {
 printf("\n pipe \n");LINKFLAG=1; head=head->pipe; }
      else if( head->inredir)  { printf("\n inredir \n");LINKFLAG=1; head=head->inredir;}
      else if ( head->outredir){ printf("\n outredir \n");LINKFLAG=1; head=head->outredir;}
      else 
         LINKFLAG=0;
       }
  */ 
  if (fork () == 0 ) {  
  LINKFLAG=1;
  input=-1;
   while ( LINKFLAG )
    {
  //WHERE
      if ( head->pipe) {
        printf("\n");
          run_pipeline(head,input);
          //write(input,buffer,90);
          //pipe(newpipe);
         while(head->pipe != NULL) head=head->pipe;   
         //WHERE 
         //printf("in pipe %s ",head-> commandsArg[0]);
         }
         else if(head->outredir) {
         
         
          pipe(fd_pipe);
          Store *currentHead=head;
          while( head -> outredir -> outredir != NULL)
           {
           fd=open(head -> outredir -> commandsArg[0],O_RDWR | O_CREAT,0777);
           head=head->outredir;
            }
            if ( fork() == 0) 
            {
              dup2(fd_pipe[1],1);
              close(fd_pipe[0]);
             if( currentHead -> FirstElement)
               {
               execvp(currentHead -> commandsArg[0],currentHead->commandsArg);
               }
              }
             else {
               if ( head-> outRedirAppend )
                fd=open(head->outredir->commandsArg[0], O_RDWR | O_CREAT | O_APPEND, 0777);    
               else
                   fd=open(head->outredir->commandsArg[0], O_RDWR | O_CREAT, 0777); 
             dup2(fd_pipe[0],0);
             close(fd_pipe[1]);
             while((countChar = read(0,&c,1)) > 0)
              write(fd,&c,1);
             close(fd_pipe[1]);
             }    
          head=head-> outredir;
             
         } // begining of input redir
         else if ( head-> inredir) {
         
         
          //pipe(fd_pipe);
          Store *currentHead=head;
          while( head -> inredir -> inredir != NULL)
           {
           fd=open(head -> inredir -> commandsArg[0],O_RDWR,0777);
           if ( fd == -1 ) // error in file opening
             {
              perror(" file open error");
              goto INREDIREXIT;
             }
           head=head->inredir;
            }
            fd=open(head->inredir->commandsArg[0], O_RDWR , 0777);  
            if ( fd == -1 ) // error in file opening
                 {
                  perror(" file open error");
                  goto INREDIREXIT;
                  }
             close(0);
             dup(fd);
             close(fd);
             execvp(currentHead -> commandsArg[0],currentHead->commandsArg);
             head=head-> inredir;
             
          } 
         //close(fd);
     //   dup2(fd,1);
       // close(fd);
       else  {
         if( strcmp(head->commandsArg[0],"cd") == 0 ) 
               {
                 chdir(head->commandsArg[1])
                 getcwd(cdbuffer,sizeof(cdbuffer));
                 printf("  current working directory %s ",cdbuffer);
                }
          else
           execvp(head->commandsArg[0], head->commandsArg); 
         }
      if ( head ->pipe == NULL && head-> inredir == NULL && head->outredir == NULL )  
           LINKFLAG=0;      
      INREDIREXIT :
           LINKFLAG= 0;
        }
     }
   wait(&status);
 }
 return 1;
}

      //code help is taken from http://stackoverflow.com/questions/948221/does-this-multiple-pipes-code-in-c-makes-sense
void run_pipeline(Store *cmd, int input) {
  int pfds[2] = { -1, -1 };
 
 // if( cmd-> pipe == NULL )
   // return ;  
  if (cmd->pipe != NULL) {
    pipe(pfds);
  //WHERE
  }
 
  if (fork() == 0) { /* child */
    if (input != -1) {
      dup2(input, STDIN_FILENO);
      close(input);
    }
    if (pfds[1] != -1) {
      dup2(pfds[1], STDOUT_FILENO);
      close(pfds[1]);
    }
    if (pfds[0] != -1) {
      close(pfds[0]);
    }
 //  WHERE
    execvp(cmd->commandsArg[0], cmd->commandsArg);
    exit(1);
  }
  else { /* parent */
    if (input != -1) {
      close(input);
    }
    if (pfds[1] != -1) {
      close(pfds[1]);
    }
    if (cmd-> pipe != NULL) { 
      run_pipeline(cmd->pipe, pfds[0]);
    }
  }
}
