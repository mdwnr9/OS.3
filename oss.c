#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <ctype.h>
#include "oss.h"

/* This flag controls termination of the main loop. */
volatile sig_atomic_t keep_going = 1;

/* The signal handler just clears the flag and re-enables itself. */
void catch_alarm (int sig)
{
  keep_going = 0;
  signal (sig, catch_alarm);
}

//Prototypes
void fork_exec();

//Global Variables 
int clock_id; //This will hold the id for the shared memory portion of the Clock struct - shmget
int message_queue_id;
int msgflg;
Clock *shm_time_ptr; //pointer to shared memory address of clock struct--> should it be an int ptr? - shmat
pid_t pid;
int status;

int main(int argc, char* argv[]) {
	
  //Flags for getopt	
  int help_flag = 0;
  int max_children = 5;
  char* log_file = "log";
  int max_run_time = 20;
  int max_sim_time = 2;
  opterr = 0;
  int c;
  
  while ((c = getopt(argc, argv, "hs:l:t:m:")) != -1) {
    switch (c) {
      case 'h':
        help_flag = 1;
        break;
      case 's':
        max_children = atoi(optarg);
        break;
      case 'l':
        log_file = optarg;
        break;
      case 't':
        max_run_time = atoi(optarg);
        break;
      case 'm':
        max_sim_time = atoi(optarg);
        break;
      case '?':
		 if (optopt == 's' || optopt == 'l' || optopt == 't')
                    fprintf(stderr, "Option %c requires an argument.\n", optopt);
                else
                    fprintf(stderr, "Unknown option -%c\n", optopt);
                return -1;
        }
}

	if (help_flag == 1)
    {
        printf("'-h' Help menu\n");
        printf("'-s x' Number of child processes spawned (default value = %d)\n", max_children);
        printf("'-l' This sets the name of the logfile");
        printf("'-t z' Total amounnt of seconds that will pass until the master process kills itself and all the children (default value = %d)\n", max_run_time);
        exit(0);
	}
  
  
  //This is for message
  msgflg = IPC_CREAT|0777;
  size_t message_length;
  
  

/* Establish a handler for SIGALRM signals. */
  signal (SIGALRM, catch_alarm);

/* Set an alarm to go off in a little while. */
  alarm (10);

  
/*****Shared memory*****/
/*Allocating shared memory for a clock struct.
The clock struct is in oss.h and it has two integers:
seconds and nanoseconds. the SHMKEY is also in oss.h
to ensure I use the same key in oss and user
 I am also setting permissions*/
 clock_id = shmget ( SHMKEY, sizeof(Clock), 0777 | IPC_CREAT );
  if (clock_id == -1) {
    perror("Failed to get shared memory");
    exit(1);
  }
   printf("Created  shared memory for clock struct...\n");

  //Attach to clock shared memory segment
   shm_time_ptr =  shmat(clock_id, 0, 0);
   
    if(shm_time_ptr == -1) {
        perror("could not attach to memory....shmat error");
        exit(1);
   }

     
   printf("Attached clock shared memory...\n");
   
   
   //Initialize Clock
   shm_time_ptr->seconds = 0;
   shm_time_ptr->nano_seconds = 0;
   
   
  /*Master creates a message queue with msget. 
  It gets a key that is defined in oss.h so oss
  and user have the same key. The message_queue_id it 
  returns can be used to access the message queue*/
   (void) fprintf(stderr, "\nmsgget: Calling msgget from OSS(%#lx,\
     %#o)\n",
    MSGKEY, msgflg);

    if ((message_queue_id = msgget(MSGKEY, IPC_CREAT | 0600)) < 0) {
        perror("Error: msgget");
        exit(1);
    }
    else 
     (void) fprintf(stderr,"msgget: msgget in OSS succeeded: message_queue_id = %d\n", message_queue_id);

 /*
     * Sending message type 1 from OSS
     */
     
    myMessage.mtype = 1;
    
    (void) fprintf(stderr,"msgget: msgget succeeded: msqid = %d\n", message_queue_id);
    
    (void) strcpy(myMessage.mtext, "Did you get this?");
    
    (void) fprintf(stderr,"msgget: msgget succeeded: msqid = %d\n", message_queue_id);
    
    message_length = strlen(myMessage.mtext) + 1 ;
    
    

    /*
     * Send a message.
     */
    if (msgsnd(message_queue_id, &myMessage, message_length, 0) < 0) {
       printf ("%d, %d, %s, %d\n", message_queue_id, myMessage.mtype, myMessage.mtext, message_length);
        perror("msgsnd");
        exit(1);
    }

   else 
      printf("Message: \"%s\" Sent\n", myMessage.mtext);
      
    
  printf("About to fork a child ...\n");
//Fork and exec a child ---> I will change this to several children instead of one child
  for(int i = 0; i < 2; i++){
  fork_exec();    
  sleep(3);
  }
  
   
//Detach & remove from shared memory 
wait(&status);
     printf("Child has completed...\n");
     shmdt((void *) shm_time_ptr);
     printf("time has detached  its shared memory...\n");
     shmctl(clock_id, IPC_RMID, NULL);
     printf("time has removed its shared memory...\n");
     printf("time  exits...\n");
     exit(0);
	 

   
   msgctl(message_queue_id, IPC_RMID, NULL);

return 0;

}

/*This function forks one child and execs it to user.*/

void fork_exec(){
pid_t pid = fork();
    if (pid < 0) {
      perror("Failed to fork");
      exit(1);
    }
    
    else if(pid == 0){
    printf("I am child in OSS and I am about to exec...\n");
	execlp("./user",(char*) NULL); 	
		perror("Failed to exec");
        exit(-1);
	 }
}
