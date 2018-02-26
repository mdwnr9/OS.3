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

// Global Variables


//Prototypes


//structs


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

