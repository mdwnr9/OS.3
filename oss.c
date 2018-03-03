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

//Global Variables 
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
int detachandremove (int shmid, void* shmaddr);

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


//Options for getopt
if (help_flag) {
    print_help_message(argv[0], max_inital_slaves, log_file, max_run_time, max_sim_time);
    exit(EXIT_SUCCESS);
  }

 if (max_inital_slaves < 1) {
    fprintf(stderr, "Invalid argument for option -s\n");
    exit(EXIT_SUCCESS);
  }

  if (max_run_time < 1) {
    fprintf(stderr, "Invalid argument for option -t\n");
    exit(EXIT_SUCCESS);
  }

  if (max_sim_time < 1) {
    fprintf(stderr, "Invalid argument for option -m\n");
    exit(EXIT_SUCCESS);
  }

  if (setup_interrupt() == -1) {
    perror("Failed to set up handler for SIGPROF");
    return EXIT_FAILURE;
  }

  if (setup_interval_timer(max_run_time) == -1) {
    perror("Faled to set up the ITIMER_PROF interval timer");
    return EXIT_FAILURE;
  }

/*Open the log file*/
fp = fopen(log_file, "w+");

  if (fp == NULL) {
    perror("Failed to open log file");
    exit(EXIT_FAILURE);
  }

//Signal handlng
signal(SIGINT, free_shared_memory_and_abort);
signal(SIGCHLD, handle_child_termination);

//Shared memory
/*Create shared memory*/
 if ((shmid = shmget(SHM_KEY, sizeof(shared_memory_object_t), IPC_CREAT | 0600)) < 0) {
        perror("Error: shmget");
        exit(errno);
    }


return 0;

}


