#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/sem.h>

/* This flag controls termination of the main loop. */
volatile sig_atomic_t keep_going = 1;

/* The signal handler just clears the flag and re-enables itself. */
void catch_alarm (int sig)
{
  keep_going = 0;
  signal (sig, catch_alarm);
}

//Prototypes
static int setup_interval_timer(int time);


//Clock struct
typedef struct system_clock {
    int seconds;
    int nano_seconds;
}clock;


#define SHMKEY  859047
int clock_id;
clock* shm_clock_ptr; //pointer to address of clock struct--> should it be an int ptr?
pid_t pid;
int status;

int main() {
  int help_flag = 0;
  int max_inital_slaves = 5;
  char* log_file = "oss.out";
  int max_run_time = 20;
  int max_sim_time = 2;
  //opterr = 0;
  int c;
  
  clock myClock;
  myClock.seconds = 0;
  myClock.nano_seconds = 0;

/* Establish a handler for SIGALRM signals. */
  signal (SIGALRM, catch_alarm);

/* Set an alarm to go off in a little while. */
  alarm (10);

  
//Shared memory
/*Create shared memory for clock*/
 clock_id = shmget ( SHMKEY, 2*sizeof(int), 0777 | IPC_CREAT );
  if (clock_id == -1) {
    perror("Failed to get shared memory");
    exit(1);
  }
   printf("Received a shared memory of clock struct...\n");

  //Attach to clock shared memory segment
   shm_clock_ptr =  shmat(clock_id, 0, 0);
   
    if(shm_clock_ptr == -1) {
        perror("could not attach to memory....shmat error");
        exit(1);
   }


   printf("Attached the clock shared memory...\n");

   printf("About to fork a child process...\n");
//Spawn Children -- 5 for now CHANGE IT--
//Make sure children can read from clock's shared memory
//Fork and exec
 for (int i = 0; i < 5; i++) {
    //Add do while loop{
    pid_t pid = fork();
    if (pid < 0) {
      perror("Failed to fork");
      exit(1);
    }
    
    else if(pid == 0){
    int clock_id_arr[5];
	    clock_id_arr[i] = pid; 
    printf("Printing the clock id, %d", clock_id);
    printf("child can read shared memory...\n");
	printf(clock_id_arr, "%d", clock_id);
    }
	
	sleep(2);
	

//Detach & remove from shared memory 
wait(&status);
     printf("Child has completed...\n");
     shmdt((void *) shm_clock_ptr);
     printf("Clock has detached  its shared memory...\n");
     shmctl(clock_id, IPC_RMID, NULL);
     printf("Clock has removed its shared memory...\n");
     printf("Clock  exits...\n");
     exit(0);
	 
}	 


return 0;

}



