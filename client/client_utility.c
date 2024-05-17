#include "client_utility.h"

uint16_t getComm(){
    char buf[1024];
    char *delimiter=" ";
    char *substring;
    int i = 0;
    
    //prendo comando in input
    fgets(buf, sizeof(buf), stdin);

    uint16_t lunghezza = strlen(buf);
    buf[lunghezza-1] = '\0';

    //divido comando e destinatario
    substring = strtok(buf,delimiter);

    for ( i = 0; i < 4; i++)
        memset(parameter[i], 0, sizeof(parameter[i]));

    i=0;
    while (substring != NULL) {
        strcpy(parameter[i], substring);
        i++;
        substring = strtok(NULL, delimiter); 
    }
    if(strcmp(parameter[0],"registrazione")==0)
            return 1;

    else if(strcmp(parameter[0],"login")==0)
            return 2;

    else if(strcmp(parameter[0],"look")==0)
            return 3;

    else if(strcmp(parameter[0],"take")==0)
            return 4;
        
    else if(strcmp(parameter[0],"drop")==0)
            return 5;
        
    else if(strcmp(parameter[0],"use")==0)
            return 6;
        
    else if(strcmp(parameter[0],"help")==0)
            return 7;
        
    else if(strcmp(parameter[0],"offer")==0)
            return 8;
        
    else if(strcmp(parameter[0],"end")==0)
            return 9;
        
    else if(strcmp(parameter[0],"objs")==0)
            return 10;
        
    else if (strcmp(parameter[0], "options") == 0)
        return 11;
    
    return 11;     
} 

//invio il comando richiesto al server e ricevo l'esito
int chooseComm(int sd){
    uint16_t command, aux;
    int ret;

    //avvio comunicazione con server
    aux = htons(10);
    ret = send(sd, (void*)&aux, sizeof(aux), 0);

    if((ret=recv(sd, (void*) &outcome, sizeof(outcome), 0)) < 0){
        perror("Errore nella ricezione dell'outcome chooseComm");
        exit(-1);
    }
    outcome=ntohs(outcome);

    if(outcome!=0){
        //la partita è terminata per qualche motivo quindi chiamo la end
        command=9;
    }
    else{
        //se non si è loggati si fa scegliere solo tra registrazione e login
        if (!logged)
            printf("> Cosa desideri fare? \n-  registrazione\n-  login\n");
        else
            printf("\n> Quale operazione vuoi eseguire adesso?\nDigita 'options' per visualizzare la lista dei comandi\n");

        command = getComm();

        if((logged && (command==1 || command==2)) || (!logged && command > 2))
            command=12;
    }

    aux=htons(command);

    if((ret = send(sd, (void*) &aux, sizeof(aux), 0)) < 0){
        perror("Errore nell'invio opzione scelta");
        exit(-1);
    }

    return command;
} 


void newPlayer(int sd, char* username){
    int ret;
    u_int16_t len;
   
    logged = 1;
    printf("> login effettuato con successo %s \n",username);  

    //qua gestiamo se ci sono troppi giocatori
    if((ret=recv(sd, (void*)&len, sizeof(uint16_t), 0)) < 0){   
        perror("Errore nella ricezione numero giocatori login");
        exit(-1);
    }

    len= htons(len);
    if(len==0){
        printf("> Siamo spiacenti di comunicare che la partita ha il numero massimo di player\n");
        logged = 0;
        return;
    }

    strcpy(client.player_id, username);
    strcpy(client.obj[0], "");
    strcpy(client.obj[1], "");
    strcpy(client.obj[2], "");
    client.n_obj=0;

    time(&start_time);
    if((ret=recv(sd, (void*)&game_time, sizeof(uint16_t), 0)) < 0){   
        perror("Errore nella ricezione tempo di gioco login");
        exit(-1);
    } 
    game_time=ntohs(game_time);
    game_time=600-game_time;
}

//calcolo tempo rimanente e lo stampo
void printTime(){
    int rem_time;

    time(&current_time);
    passed_time = difftime(current_time, start_time);
    rem_time = ((game_time-passed_time) >= 0) ? (game_time-passed_time) : 0;

    printf("> il tempo che ti rimane a disposizione è: %d.%d\n", rem_time/60, rem_time%60);
}