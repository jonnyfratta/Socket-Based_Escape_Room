#ifndef SCENARIO
#define SCENARIO
#include <arpa/inet.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

struct game{
    time_t tot_time;
    int player[5];
    int tot_players;
};

struct riddle{
    char question[1024];
    char answer[1024];
};

struct object{
    char name[50];
    char descr[1024];
    struct riddle *riddle;
    
    int usable;
    int taker;
    int locked;
};

struct location{
    char name[50];
    char descr[1024];

    struct object obj[3];
    int lookable;
};

struct location location[8];
struct riddle riddle[10];

void init_scen();

#endif