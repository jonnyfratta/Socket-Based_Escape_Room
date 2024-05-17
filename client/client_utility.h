#ifndef CUH
#define CUH
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

uint16_t outcome;
uint16_t port;
uint16_t game_time,passed_time;
time_t start_time,current_time;
static int logged;
int count;
char parameter[4][1024];
struct player{
    char player_id[1024];
    char obj[3][1024];
    int n_obj;
};
struct player client;

u_int16_t getComm();
int chooseComm(int sd);
void newPlayer(int sd, char* username);
void printTime();

#endif