/*
<CTRL><C> = history
<CTRL><D> = exit
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#define MAX_LINE 80
#define BUFFER_SIZE 50
#define buffer1 "\nCommand History:\n"
#define buffer2 "\nCOMMAND->"

int count = 0;
int signflag = 0;
//int errflag = 0;
char history[11][BUFFER_SIZE] = {'\0'};


/* the signal handler function */
void handle_SIGINT()
{
    fflush(stdin);
    write(STDOUT_FILENO, buffer1, sizeof(buffer1));
    int history_counter = count;

    for(int i = 0; i < 10; i++)
    {
        int j = 0;
        if (history_counter == 0 || history[i][j] == '\0')
            break;
        printf("%d. ", history_counter);
         for(int j = 0; (j < BUFFER_SIZE) && history[i][j] != '\n'; j++)
        {
	    if(history[i][j] == '\0') printf(" ");
            else printf("%c", history[i][j]);
        }
        printf("\n");
        history_counter--;
    }
    write(STDOUT_FILENO, buffer2, sizeof(buffer2));
    signflag = 1;
}

/** setup() reads in the next command line, separating it into
distinct tokens using whitespace as delimiters.
setup() modifies the args parameter so that it holds pointers
to the null-terminated strings that are the tokens in the most
recent user command line as well as a NULL pointer, indicating
the end of the argument list, which comes after the string
pointers that have been assigned to args. */


void setup(char inputBuffer[], char *args[],int *background)
{
    int length, start, p;

    /* read what the user enters on the command line */
    length = read(STDIN_FILENO,inputBuffer,MAX_LINE);
     //printf("read:");
     //printf("%c\n",inputBuffer[0]);
     if (signflag == 1)
     {
          length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
          signflag = 0;  //if ^c already filled
     }

     if(length == 0) exit(0);            /* ^d was entered, end of user command stream */
     if(length < 0)
     {
        perror("Error reading the command\n");
        exit(-1);           /* terminate with error code of -1 */
     }

    for(int i = 10; i > 0; i--)
        for(int j = 0; j < BUFFER_SIZE; j++)
           history[i][j] = history[i - 1][j];

    for(int j = 0; j < length; j++)
        {history[0][j] = inputBuffer[j];}

    for(int j = length - 1; j < BUFFER_SIZE; j++)
        history[0][j] = ' ';  //new clean history

    start = -1;
    p = 0;
    for(int i = 0;i < length; i++)
    { /* examine every character in the inputBuffer */
        switch (inputBuffer[i])
        {
            case ' ':
            case '\t' :               /* argument separators */
                if(start != -1)
                {
                    args[p] = &inputBuffer[start];    /* set up pointer */
                    p++;
                }
                inputBuffer[i] = '\0'; /* add a null char; make a C string */
                start = -1;
                break;

            case '\n':                 /* should be the final char examined */
                if (start != -1)
                {
                    args[p] = &inputBuffer[start];
                    p++;
                }
                inputBuffer[i] = '\0';
                args[p] = NULL; /* no more arguments to this command */
                break;

            default :             /* some other character */
                if (start == -1) start = i;
                //if (inputBuffer[i] == '&')
                //{
		    //inputBuffer[i] == '\0';
                    //inputBuffer[i-1] = '\n';
		    //*background  = 1;
                //}
        } /* end of switch */
     }    /* end of for */
     if(args[p - 1][0] == '&') {args[p - 1] = NULL; *background = 1;}
     args[p] = NULL; /* just in case the input line was > 80 */

    //for (i = 0; i < ct; i++)//Show parser result
		//printf("args %d = %s\n",i,args[i]);

     //examine if it calls command in history
    if(strcmp(args[0],"r")==0)
    {
        if (args[1] == NULL) //repeat the last command
            //strcpy(inputBuffer,history[0]);
        {
            for(int j = 0; j < BUFFER_SIZE; j++)
            {
                history[0][j] = history[1][j];} //printf("%c", history[0][j]);}

            start = -1;
            p = 0;
            for(int i = 0;i < BUFFER_SIZE; i++)
            { /* examine every character in the history[1] */
                switch (history[1][i])
                {
                    case ' ':
                    case '\t' :               /* argument separators */
                        if(start != -1)
                        {
                            args[p] = &history[1][start];    /* set up pointer */
                            p++;
                        }
                        history[1][i] = '\0'; /* add a null char; make a C string */
                        start = -1;
                        break;

                    case '\n':                 /* should be the final char examined */
                        if (start != -1)
                        {
                            args[p] = &history[1][start];
                            p++;
                        }
                        history[1][i] = '\0';
                        args[p] = NULL; /* no more arguments to this command */
                        break;

                    default :             /* some other character */
                        if (start == -1) start = i;
                        if (history[1][i] == '&')
                        {
                            *background  = 1;
                            history[1][i-1] = '\0';
                        }
                } /* end of switch */
             }    /* end of for */
            args[p] = NULL; /* just in case the input line was > 80 */

		//for (i = 0; i < ct; i++)//Show parser result
            //printf("args %d = %s\n",i,args[i]);
        }

        else
        {
            char x = args[1][0]; // x


            int found = 0;
            for(int j = 1; j <= 10; j++)
            {
                if(history[j][0] == x)
                {
                    for(int k = 0; k < BUFFER_SIZE; k++)
                    history[0][k] = history[j][k]; //printf("%c", history[0][k]);}
                    //strcpy(inputBuffer, history[j]);

                    start = -1;
                    p = 0;
                    for(int i = 0;i < BUFFER_SIZE; i++)
                    { /* examine every character in the inputBuffer */
                        switch (history[j][i])
                        {
                            case ' ':
                            case '\t' :               /* argument separators */
                                if(start != -1)
                                {
                                    args[p] = &history[j][start];    /* set up pointer */
                                    p++;
                                }
                                history[j][i] = '\0'; /* add a null char; make a C string */
                                start = -1;
                                break;

                            case '\n':                 /* should be the final char examined */
                                if (start != -1)
                                {
                                    args[p] = &history[j][start];
                                    p++;
                                }
                                history[j][i] = '\0';
                                args[p] = NULL; /* no more arguments to this command */
                                break;

                            default :             /* some other character */
                                if (start == -1) start = i;
                                if (history[j][i] == '&')
                                {
                                    *background  = 1;
                                    history[j][i-1] = '\0';
                                }
                        } /* end of switch */
                     }    /* end of for */
                    args[p] = NULL;
                    found = 1;
                    break;
                }
            }
            if (found == 0)
            {
                printf("\nNo such command in the last 10 commands.");
                args[0] = NULL;
            }

        }
	}
	//for (i = 0; i < ct; i++)//Show parser result
            //printf("args %d = %s\n",i,args[i]);

    count++;

} /* end of setup routine */


int main(void)
{
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2 + 1];/* command line arguments */
    int errflag;
    int fd[2];
    int *write_fd = &fd[1];
    int *read_fd = &fd[0];
    pipe(fd);
    char readbuffer[2];
    pid_t pid;

    /* set up the signal handler */
    struct sigaction handler;
    handler.sa_handler = handle_SIGINT;
    sigaction(SIGINT, &handler, NULL);


    /* loop until we receive <Control><C> */
    while (1)
    {
        readbuffer[0] == '0';
        background = 0;
        errflag = 0;
        printf("COMMAND->");
        fflush(stdout);
        /*setup() calls exit() when Control-D is entered */
        setup(inputBuffer, args, &background);
        /** the steps are:
        (1) fork a child process using fork()
        (2) the child process will invoke execvp()
        (3) if background == 0, the parent will wait,
        otherwise it will invoke the setup() function again. */
        // (1) fork a child process using fork()
        pid = fork();
        // (2) the child process will invoke execvp()
        if (pid == 0)
        {
            //write(*write_fd,"0",strlen("0"));
            if(execvp(args[0], args) == -1)
	       {
		        printf("Error command. Execution fail.\n");
                errflag = 1;
                write(*write_fd,"1",strlen("1"));
                //read(*read_fd,readbuffer,sizeof(readbuffer)-1);
                 //printf("now%c %c \n",readbuffer[0], readbuffer[1]);
		        exit(0);
            }
        }

        // (3) if background == 0, the parent will wait,
        // otherwise it will invoke the setup() function again.
        else
        {
            if (background == 0)
            wait(NULL);
            readbuffer[0] = '0';
            fcntl(*read_fd, F_SETFL, O_NONBLOCK);
            read(*read_fd,readbuffer,sizeof(readbuffer)-1);
            //printf("afterreadpipe%c %c\n",readbuffer[0], readbuffer[1]);
            if(readbuffer[0] == '1')
            {for(int i = 0; i <= 10; i++)
			for(int j = 0; j < BUFFER_SIZE; j++)
			  {history[i][j] = history[i + 1][j];}
             count--;
             //write(*write_fd,"0",strlen("0"));
             //printf("afterclean%c %c \n",readbuffer[0], readbuffer[1]);
            }//printf("%c", history[i][j]);}
        }
    }
}

