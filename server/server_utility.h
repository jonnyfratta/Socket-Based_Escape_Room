#ifndef SUH
#define SUH
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>

#include "../scenario/scenario.h"

struct game game;
fd_set master;
int used_token[3];
int aq_token, total_used;
int bonus;
int modify;
uint16_t outcome;
uint16_t  port;
uint16_t passed_time, rem_time;
time_t current_time;
char enigma[1024];
char ans[1024];

void addPlayer(int curr_sock);
void newPlayer(int found, int curr_sock, int fd);
void init_game();
void printTime();
int findObj(char* o_name, int no_loc, int* results);
void handleEffects(char* object, char* messaggio);

#endif