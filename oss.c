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

//Global Variables - do define
 static int clock_segment_id;
 static int* clock_shared_memory;
 static int message_segment_id;
 static int* message_shared_memory;
 int num_slaves_completed = 0;
 const int MAX_SLAVES = 100;
 FILE* fp;
 const int NANO_SECONDS_PER_SECOND = 1000000000;

//Prototypes
static int create_timer(int time);

//getopt flags


int main(int argc, char* argv[]) {
  int help_flag = 0;
  int max_inital_slaves = 5;
  char* log_file = "oss.out";
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
        max_inital_slaves = atoi(optarg);
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
        if (is_required_argument(optopt)) {
          print_required_argument_message(optopt);
        } else if (isprint(optopt)) {
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        } else {
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        return EXIT_FAILURE;
      default:
        abort();
    }
  }


//If statemets go here



fp = fopen(log_file, "w+");

  if (fp == NULL) {
    perror("Failed to open log file");
    exit(EXIT_FAILURE);
  }

//Signal handlng
signal(SIGINT, free_shared_memory_and_abort);
signal(SIGCHLD, handle_child_termination);

//Shared memory
get_shared_memory();
attach_to_shared_memory();


return 0;

}

/*Function to create interval timer*/
/*static int create_timer(int time){
   struct itimer t;
   t.it_interval.tv_sec = time;
   t.it_interval.tv_usec = 0;
   t.it_value = value.it_interval;
   return (setitimer(ITIMER_PROF, &t, NULL));
   
}*/

/*Another way to implement the system clock*/
/*void clock_incrementation_function(system_clock_t *destinationClock, system_clock_t sourceClock, int additional_nano_seconds) {
    destinationClock->nano_seconds = sourceClock.nano_seconds + additional_nano_seconds;
    if(destinationClock->nano_seconds > 1000000000) {
        destinationClock->seconds++;
        destinationClock->nano_seconds -= 1000000000;
    }
}*/
  


/*Detach shared memory and remove shared memory segment*/
int detachandremove (int shmid, void* shmaddr){
    int error = 0;

    if (shmdt(shmaddr) == - 1)
        error = errno;
    if ((shmctl(shmid, IPC_RMID, NULL) == -1) && !error)
        error = errno;
    if (!error)
        return 0;
    errno = error;
    return -1;
}

void terminate_child(int signum) {
  int status;
  pid_t pid = wait(&status);

  fprintf(
    fp,
    "[Master] Child %d is terminating at my time %d:%d because it reached %d:%d in slave\n",
    pid,
    *clock_shared_memory,
    *(clock_shared_memory + 1),
    *(message_shared_memory),
    *(message_shared_memory + 1)
  );

  empty_message();
  fork_and_exec_child();
  num_slaves_completed++;
}


