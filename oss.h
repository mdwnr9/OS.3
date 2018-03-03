#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>

#define MSGSZ     128
#define SHMi_KEY 9823
#define MESSAGE_QUEUE_KEY 3318 



typedef struct system_clock {
    int seconds; 
    int nano_seconds;
} clock;

typedef struct msgbuf {
    long mtype; 
    char mtext[MSGSZ];
}message_t; 
