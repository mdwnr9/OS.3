/*Add globals here*/

typedef struct system_clock {
    int seconds; 
    int nano_seconds;
} system_clock_t;

typedef struct message {
    long message_address; 
    int dead_or_done; 
    system_clock_t clock_info;
    int return_address; 
} message_t;
