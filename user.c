#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/sem.h>
#include <time.h>
#include <ctype.h>

//Clock struct
typedef struct system_clock {
    int seconds;
    int nano_seconds;
}clock;

static struct system_clock get_end_time(struct system_clock start, int duration);
static int is_passed_end_time(struct system_clock end);
static int is_message_empty();

const int NANO_SECONDS_PER_SECOND = 1000000000;

int* clock_shared_memory;
int* message_shared_memory;

void handler(int signal_number) {
  shmdt(clock_shared_memory);
  shmdt(message_shared_memory);
  exit(EXIT_SUCCESS);
}
