#include "server_utility.h"

//aggiunge un giocatore alla partita
void addPlayer(int curr_sock){
    int ret, i;
    uint16_t free_place;

    if(game.tot_players == 5){
        free_place = 0;
        printf(">Raggiunto numero massimo di giocatori\n");

        //comunico che non ci sono posti liberi
        free_place=htons(free_place);
        if((ret = send(curr_sock, (void*)&free_place, sizeof(free_place), 0)) < 0){
            perror("Errore nel comunicare posti esauriti\n");
            exit(-1);
        }
        return;
    }

    free_place=1;
    free_place=htons(free_place);

    //comunico la presenza di un posto libero
    if((ret = send(curr_sock, (void*) &free_place, sizeof(free_place), 0)) < 0){
            perror("Errore nel comunicare presenza di posti liberi\n");
            exit(-1);
    }

    for(i=0; i < 5; i++){ 
        if(game.player[i] == 0){
            game.player[i] = curr_sock;
            break;
        } 
    }
    game.tot_players++;
}


void newPlayer(int found, int curr_sock, int fd){
    int ret;
    u_int16_t aux;

    if (found) {
        //aggiunta giocatore alla partita
        addPlayer(curr_sock);
        if (game.tot_players == 1)
            time(&game.tot_time);

        time(&current_time);
        passed_time = difftime(current_time, game.tot_time);
        aux = htons(passed_time);

        if ((ret = send(curr_sock, (void*)&aux, sizeof(aux), 0)) < 0) {
            perror("Errore nell'invio del tempo trascorso");
            close(fd);
            exit(-1);
        }
    }
}


void init_game(){
    int i;

    aq_token = 0;
    bonus = 0;
    game.tot_players=0;
    modify = 0;
    outcome=0;
    total_used = 0;

    location[5].lookable = 0;
    location[6].lookable = 0;
    location[7].lookable = 0;

    for(i = 0; i < 5; i++)
        game.player[i]=0;

    for(i = 0; i < 3; i++)
        used_token[i]=0;

    strcpy(enigma, "null");
    strcpy(ans, "null");
}


void printTime(){
    time(&current_time);
    passed_time = difftime(current_time, game.tot_time);

    if(modify != 0){
        passed_time = (modify == -600) ? 600 : (passed_time-modify);
    }
    rem_time = 600-passed_time;

    printf("tempo rimanente: %d \n", rem_time);

    //in caso il tempo scada comunico che la partita è terminata
    if(rem_time <= 0 && game.tot_players!=0)
        outcome=1;

    modify = 0;
}


int findObj(char* o_name, int no_loc, int* results){
    int i, j;
    u_int8_t loc = 0;

    //ricerca nome tra le location
    for(i = 0; i < 8; i++){
        if(strcmp(location[i].name, o_name) == 0){
            loc = 1;
            break;
        }
    }
    if(loc == 1){
        results[0] = i;
        results[2] = loc;

        if(no_loc == 1)
            return -1;
        else 
            return 1;
    }

    //ricerca nome tra gli oggetti
    if(loc == 0){
        for(i = 0; i < 8; i++){
            for(j = 0; j < 3; j++){
                if(strcmp(location[i].obj[j].name, o_name) == 0){
                    results[0] = i;
                    results[1] = j;
                    results[2] = loc;

                    return 1;
                }
            }
        }
    }

    return 0;    
}


void handleEffects(char* object, char* messaggio){
    int i;

    if((strcmp(object,"lama_drago")==0) || (strcmp(object,"acqua")==0) || (strcmp(object,"chiave")==0)){
        printf("token usati: {%d,%d,%d}\n", used_token[0], used_token[1], used_token[2]);
        if(location[7].lookable == 0){
            strcpy(messaggio, "C'è un luogo e un momento per ogni cosa, ma non ora!\n");
        }
        else{
            total_used++;
            for (i = 0; i < 3; i++){
                if(used_token[i] == 0){
                    if(strcmp(object,"lama_drago")==0)
                        used_token[i] = 1;
                    else if(strcmp(object,"acqua")==0)
                        used_token[i] = 2;
                    else if(strcmp(object,"chiave")==0)
                        used_token[i] = 3;
                    break;
                }
            }
            printf("token usati: {%d,%d,%d}\n", used_token[0], used_token[1], used_token[2]);
            if (used_token[0] == 1 && used_token[1] == 2 && used_token[2] == 3){
                outcome = 2;
                strcpy(messaggio, "Hai liberato il drago e te lo sei fatto amico, gli sali in groppa e volate via\n hai vinto\n");
            }
            else if(used_token[1] == 0 || used_token[2] == 0)
                sprintf(messaggio, "Hai usato: %d/3 token\n", total_used);
            else{
                strcpy(messaggio,"Gli oggetti sembrano corretti, ma c'è qualcosa che non va, prova a cambiare qualcosa\n");
                for(i = 0; i < 3; i++)
                    used_token[i] = 0;
                total_used = 0;
            }
        }
    }
    else if(strcmp(object,"saltatempo")==0){
        strcpy(messaggio, "Purtroppo si trattava di una trappola, tempo scaduto\n");
        modify = -600;
    }
    else if(strcmp(object, "incantesimo_veldora") == 0){
        if(location[7].lookable == 1){
            outcome = 2;
            strcpy(messaggio, "Hai liberato il drago e te lo sei fatto amico, gli sali in groppa e volate via\n hai vinto\n");
        }   
        else
            strcpy(messaggio, "C'è un luogo e un momento per ogni cosa, ma non ora!\n");
    }
    else if(strcmp(object, "bastone_magico") == 0)
            strcpy(messaggio, "Hai sparato l'ultimo incantesimo contenuto nel bastone...non è successo niente\n");
    
    else if(strcmp(object, "clessidra") == 0){
        modify = 60;
        strcpy(messaggio, "Hai guadagnato un minuto extra\n");
    }
    else if(strcmp(object, "rotolo") == 0){
        if(bonus == 0){
            bonus = 1;
            strcpy(messaggio, "Hai guadagnato un aiuto nel tuo prossimo indovinello che non sia a scelta multipla\n");
        }
        else{
            strcpy(messaggio, "Oggetto già utilizzato\n");
        }
    }
}