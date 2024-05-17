#include "server_utility.h"

void start();
void sign_up();
void login();
void printTime();
void look(int curr_sock);
void take(int curr_sock);
void drop(int curr_sock);
void use(int curr_sock);
void askForHelp(int curr_sock);
void offerHelp(int curr_sock);
void end(int curr_sock);


int main(int arg, char* argv[]){
    //set
    fd_set read_fds;              
    int fdmax; 

    //socket
    int listener, newsd, ret, i, optval = 1;         
    struct sockaddr_in sv_addr, cl_addr;
    socklen_t addrlen;

    //inizializzazione partita
    init_scen();
    init_game();
    start();
    port = (uint16_t)strtoul(argv[1], NULL, 10);
    port=htons(port);

    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    
    //creazione socket
    
    if((listener = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Errore nella creazione di listener\n");
        exit(-1);
    }

    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Errore nell'impostazione dell'command SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = INADDR_ANY; 
    sv_addr.sin_port = port;
    
    if((ret=bind(listener, (struct sockaddr*)& sv_addr, sizeof(sv_addr)))<0){
        perror("Errore nella bind di listener\n");
        exit(-1);
    }
    listen(listener, 10);

    // Aggiungo il listener al set e std_input
    FD_SET(listener, &master); 
    FD_SET(0,&master);
    
    fdmax = listener;          
    for(;;){                       
        read_fds = master;   
        select(fdmax + 1, &read_fds, NULL, NULL, NULL);

        for(i=0; i<=fdmax; i++) {                          
            if(FD_ISSET(i, &read_fds)) {
                if(i == listener) {                         
                   addrlen = sizeof(cl_addr);
                   newsd = accept(listener,(struct sockaddr *)&cl_addr, &addrlen);
                   FD_SET(newsd, &master);
                   fdmax = (newsd > fdmax) ? newsd : fdmax;
               }
               else{ 
                    uint16_t aux, start, command = 0;

                    //ricevo segnale inzio comunicazione
                    if((ret=recv(i, (void*) &start, sizeof(start), 0)) < 0){
                       perror("Errore nella ricezione dello start\n");
                       exit(-1);
                    }

                    //invio outcome
                    aux = htons(outcome);
                    if((ret = send(i, (void*) &aux, sizeof(aux), 0)) < 0){
                       perror("Errore nell'invio dell'outcome\n");
                       exit(-1);
                    }

                    if(outcome!=0){
                        end(i);
                        return 0;
                    }
                    
                    //ricevo comando
                    if((ret = recv(i, (void*)&command, sizeof(command), 0)) < 0){
                       perror("Errore nella ricezione del comando\n");
                       exit(-1);
                    }
                    
                    //gestisco caso disconnessione improvvisa del giocatore
                    if(ret == 0){
                        end(i);    
                        continue;
                    }
                
                    command=ntohs(command);

                    printf(">comando ricevuto: %d\n", command);

                    switch(command){
                    case 1:
                        sign_up(i);
                        break;

                    case 2:
                        login(i);
                        break;

                    case 3:
                        look(i);
                        break;

                    case 4:
                        take(i);
                        break;

                    case 5:
                        drop(i);
                        break;

                    case 6:
                        use(i);
                        break;

                    case 7:
                        askForHelp(i);
                        break;

                    case 8:
                        offerHelp(i);
                        break;

                    case 9:
                        end(i);
                        break;

                    default:
                        break;

                    }
                    printTime();
                }    
            } 
        }
    } 
    return 0;
}

//_______________________________________________________________________________________________ START SERVER _________________________________________________________________________________________________
void start(){
    //rimane in attesa di un comando
    while(1){
        char buffer[1024];
        char *delimiter=" ";
        char *sottostringa;
        uint16_t lunghezza;

        printf("********************************* SERVER STARTED *******************************\n"
               "digita un comando: \n-start\n-stop\n"
               "********************************************************************************\n");
        
        //preleva comando da tastiera
        fgets(buffer, sizeof(buffer), stdin);
        
        //gestione casistiche
        if(strcmp(buffer,"\n")  ==  0)
            continue;

        lunghezza = strlen(buffer);
        buffer[lunghezza-1] = '\0';

        sottostringa = strtok(buffer,delimiter);

        if(strcmp(sottostringa, "start") ==  0)
            break;

        if(strcmp(sottostringa, "stop")  ==  0)
            exit(0);
    }
}

//_______________________________________________________________________________________________ REGISTRAZIONE _________________________________________________________________________________________________
void sign_up(int curr_sock) {
    char username[1024]; 
    char password[1024];
    char db_record[2048];
    char *delimiter = "-";
    int fd, ret, len = 6;

    if ((fd = open("accountDB.txt", O_WRONLY | O_APPEND)) == -1) {
        perror("Errore nell'apertura del file");
        exit(-1);
    }

    if ((ret = recv(curr_sock, (void*)username, len, 0)) < 0) {
        perror("Errore nella ricezione dell'username");
        close(fd);
        exit(-1);
    }

    if ((ret = recv(curr_sock, (void*)password, len, 0)) < 0) {
        perror("Errore nella ricezione della password");
        close(fd);
        exit(-1);
    }

    // Concatena username e password con il delimitatore
    sprintf(db_record, "%s%s%s\n", username, delimiter, password);


    if ((ret = write(fd, db_record, strlen(db_record))) < 0) {
        perror("Errore nella scrittura sul file");
        close(fd);
        exit(-1);
    }

    printf(">Account registrato\n");

    newPlayer(1, curr_sock, fd);

    close(fd);
}

//_____________________________________________________________________________________________________ LOGIN _____________________________________________________________________________________________________
void login(int curr_sock) {
    char username[1024]; 
    char password[1024];
    char db_record[2048];
    char *delimiter = "-";
    char buffer[1024]; 
    int ret, fd, len = 8;
    uint16_t aux, found = 0;

    if ((fd = open("accountDB.txt", O_RDONLY)) == -1) {
        perror("Errore nell'apertura del file");
        exit(-1);
    }

    if ((ret = recv(curr_sock, (void*)username, len, 0)) < 0) {
        perror("Errore nella ricezione dell'username");
        close(fd);
        exit(-1);
    }

    if ((ret = recv(curr_sock, (void*)password, len, 0)) < 0) {
        perror("Errore nella ricezione della password");
        close(fd);
        exit(-1);
    }

    // Concatena username e password con il delimiter
    sprintf(db_record, "%s%s%s", username, delimiter, password);

    // Leggi dal file riga per riga e confronta con le credenziali ricevute
    while (read(fd, buffer, sizeof(buffer)) > 0) {
        if (strstr(buffer, db_record) != NULL) {
            found = 1;
            break;
        }
    }

    aux = htons(found);
    if ((ret = send(curr_sock, (void*)&aux, sizeof(aux), 0)) < 0) {
        perror("Errore nell'invio della conferma");
        close(fd);
        exit(-1);
    }

    newPlayer(found, curr_sock, fd);

    close(fd);
}

//___________________________________________________________________________________________________ LOOK _________________________________________________________________________________________________________
void look(int curr_sock){
    int ret, i, j, loc = 0;
    uint16_t len, aux;
    char subject[1024];
    char buf[1024];
    int results[3] = {0, 0, 0};

    memset(subject, 0, sizeof(subject));
    memset(buf, 0, sizeof(buf));

    //ricezione dimensione oggetto
    if((ret=recv(curr_sock, (void*) &len, sizeof(len), 0)) < 0){   
        perror("Errore ricezione dimensione1 look");
        exit(-1);
    }
    len=ntohs(len);

    //ricezione oggetto
    if((ret=recv(curr_sock, (void*) subject, len, 0)) < 0){   
        perror("Errore ricezione oggetto look");
        exit(-1);
    }

    //ricerca oggetto
    ret = findObj(subject, 0, results);

    if(ret){
        i = results[0];
        j = results[1];
        loc = results[2];
    }
    else if(!ret){
        printf(">errore lato client\n");
        len = 0;
        len = htons(len);
        ret = send(curr_sock, (void*) &len, sizeof(len), 0);
        return;
    }

    //invio dimensione descrizione
    if(loc == 1){
        //è stata richiesta una location
        if(location[i].lookable){
            len = strlen(location[i].descr);
            strcpy(buf, location[i].descr);
        }
        else{
            strcpy(buf, "location al momento non osservabile");
            len = strlen(buf);
        }
    }
    else{
        //era un oggetto
        len = strlen(location[i].obj[j].descr);
        strcpy(buf, location[i].obj[j].descr);
    }
    
    //invio dimensione
    aux=htons(len);
    if((ret = send(curr_sock, (void*) &aux , sizeof(aux), 0)) < 0){
        perror("Errore nell'invio della dimensione2 look");
        exit(-1);
    }

    //invio descrizione
    if((ret = send(curr_sock, (void*) buf, len, 0)) < 0){
        perror("Errore nell'invio della descrizione");
        exit(-1);
    }
}

//___________________________________________________________________________________________________ TAKE _________________________________________________________________________________________________________
void take(int curr_sock){
    int ret, i, j;
    int16_t len, aux;
    int results[3] = {0,0,0};
    char* question;
    char first_letter[2];
    char object[1024];
    memset(object, 0, sizeof(object));

    //ricezione dimensione oggetto
    len=ntohs(len);
    if((ret=recv(curr_sock, (void*) &len, sizeof(len), 0)) < 0){   
        perror("Errore nella ricezione della dimensione 1 take");
        exit(-1);
    }
    if(len==-1)
        return;

    //ricezione oggetto
    if((ret=recv(curr_sock, (void*) object, len, 0)) < 0){   
        perror("Errore nella ricezione dell'oggetto take");
        exit(-1);
    }

    if(strcmp(object, "already") == 0 || strcmp(object, "X") == 0){
        printf("oggetto nullo o già in possesso");
        return;
    }

    ret = findObj(object, 1, results);
    
    i = results[0];
    j = results[1];

    if(ret < 1 || location[i].obj[j].taker != 0){
        //oggetto non disponibile
        if(location[i].obj[j].taker==curr_sock)
            len = -1;
        else if(ret == -1){
            len = -2;
        }
        else{
            len = 0;
        }

        aux=htons(len);

        if((ret = send(curr_sock, (void*) &aux, sizeof(aux), 0)) < 0){   
            perror("Errore nell'invio dell'ext take");
            exit(-1);
        }
        return;
    }    
    
    //invio dimensione indovinello (se presente)
    len = (location[i].obj[j].locked) ? strlen(location[i].obj[j].riddle->question) : 2;

    if(location[i].obj[j].taker==0)
        location[i].obj[j].taker=curr_sock;
    else
        len=-1; 

    aux=htons(len);
    if((ret = send(curr_sock, (void*) &len, sizeof(len), 0)) < 0){
        perror("Errore nell'invio della dimensione indovinello take");
        exit(-1);
    }

    //oggetto già in mano a qualcun altro
    if(len == -1)
        return;

    if(!location[i].obj[j].locked ){
        strcpy(object, "l'oggetto selezionato non è bloccato da indovinello\n");
        len = strlen(object);
        aux = htons(len);

        location[i].obj[j].taker = curr_sock;

        //invio dimensione messaggio
        if((ret = send(curr_sock, (void*)&aux, sizeof(aux), 0)) < 0){
            perror("Errore nell'invio dimensione1 messaggio take");
            exit(-1);
        }

        //invio risposta
        if((ret = send(curr_sock, (void*)object, len, 0)) < 0){
            perror("Errore nell'invio messaggio1 take");
            exit(1);
        }
        return;
    }

    strcpy(object, "> stai cercando di prendere un oggetto che è bloccato da un indovinello, risolvilo\n");
    
    len = strlen(object);
    aux = htons(len);

    //invio dimensione
    if((ret = send(curr_sock, (void*)&aux, sizeof(aux), 0)) < 0){
        perror("Errore nell'invio dimensione1.1 messaggio take");
        exit(-1);
    }

    //invio messaggio
    if((ret = send(curr_sock, (void*)object, len, 0)) < 0){
        perror("Errore nell'invio messaggio1.1 take");
        exit(-1);
    }
    
    question = location[i].obj[j].riddle->question;

    if((bonus == 1) && 
    //il giocatore ha diritto ad un aiuto bonus
       (strcmp(location[i].obj[j].riddle->answer, "canberra") != 0) && 
       (strcmp(location[i].obj[j].riddle->answer, "nilo") != 0) && 
       (strcmp(location[i].obj[j].riddle->answer, "gulliver") != 0)){

        strcat(question, "\n la prima lettera della risposta è: ");
        first_letter[0] = *location[i].obj[j].riddle->answer;
        first_letter[1] = '\0';
        strcat(question, first_letter);
        bonus = 0;
    }

    len = strlen(question);
    aux = htons(len);

    //invio indovinello
    if((ret = send(curr_sock, (void*)question, len, 0)) < 0){
        perror("Errore nell'invio indovinello take");
        exit(-1);
    }

    //ricezione dimensione risposta
    if((ret=recv(curr_sock, (void*) &len, sizeof(len), 0)) < 0){   
        perror("Errore nella ricezione dimensione risposta take");
        exit(-1);
    }

    len=ntohs(len);
    memset(object, 0, sizeof(object));

    //ricezione risposta
    if((ret=recv(curr_sock, (void*) object, len, 0)) < 0){   
        perror("Errore nella ricezione risposta take");
        exit(-1);
    }
    //caso interruzione inattesa
    if(!ret){
        end(curr_sock);    
        location[i].obj[j].taker=0;
        return;
    }

    //gestione conseguenze di prendere alcuni oggetti specifici
    if(strcmp(object,location[i].obj[j].riddle->answer)==0){
        len=0;
        //se prendo un token 
        if((strcmp(location[i].obj[j].name, "acqua") == 0) || (strcmp(location[i].obj[j].name, "lama_drago") == 0) || (strcmp(location[i].obj[j].name, "chiave") == 0)){
            aq_token++;
            if(aq_token == 3){
                len = 3;
                location[5].lookable = 1;
                location[6].lookable = 1;
                location[7].lookable = 1;
            }
        }
    }
    else{
        len=1;
        location[i].obj[j].taker=0;
    }

    //comunico esito del confronto
    aux=htons(len);
    if((ret = send(curr_sock, (void*) &aux, sizeof(aux), 0)) < 0){
        perror("Errore nell'invio esito del confronto take");
        exit(-1);
    }
}

//___________________________________________________________________________________________________ DROP _________________________________________________________________________________________________________
void drop(int curr_sock){
    int ret, i, j;
    uint16_t len;
    char object[1024];
    int results[3];

    memset(object, 0, sizeof(object));

    //ricezione dimensione oggetto
    if((ret=recv(curr_sock, (void*) &len, sizeof(len), 0)) < 0){   
        perror("Errore ricezione dimensione oggetto drop");
        exit(-1);
    }

    len=ntohs(len);

    if(len==-1)
        return;
    
    //ricezione oggetto
    if((ret=recv(curr_sock, (void*) object, len, 0)) < 0){   
        perror("Errore ricezione oggetto drop");
        exit(-1);
    }

    //ricerca oggettp
    ret = findObj(object, 0, results);

    i = results[0];
    j = results[1];

    if (ret == -1){
        printf("l'oggetto è una location");
        return;
    }
    else if(!ret){
        printf("Oggetto non trovato\n");
        return;
    } 
    
    if(location[i].obj[j].taker != 0){
        location[i].obj[j].taker=0;
        printf("> oggetto %s lasciato\n", location[i].obj[j].name);
        
        if(strcmp(object, "acqua") == 0 || strcmp(object, "lama del drago") == 0 || strcmp(object, "chiave") == 0)
            aq_token--;
    }
    else
        printf("> l'oggetto %s non è in tuo possesso\n", location[i].obj[j].name);
}

//___________________________________________________________________________________________________ USE _________________________________________________________________________________________________________
void use(int curr_sock){
    int ret, i, j;
    int16_t len, aux;
    char object[1024];
    char def[]="questo oggetto non ha utilizzi utili\n";
    int results[2];

    memset(object, 0, sizeof(object));

    //ricezione dimensione oggetto
    if((ret=recv(curr_sock, (void*) &len, sizeof(len), 0)) < 0){   
        perror("Errore nella ricezione della dimensione oggetto use");
        exit(-1);
    }

    len=ntohs(len);
    if(len==-1){
        printf("> chiamata errata lato client\n");
        return;
    }

    //RICEVO NOME OGGETTO SE NON CI SONO ERRORI
    if((ret=recv(curr_sock, (void*) object, len, 0)) < 0){   
        perror("Errore nella ricezione dell' oggetto use");
        exit(-1);
    }

    ret = findObj(object, 1, results);

    i = results[0];
    j = results[1];

    if(!ret){
        printf("errore\n");
        return;
    }

    //controllo se usabile        
    if(location[i].obj[j].usable==0){
        len = strlen(def);
        aux=htons(len);

        //invio dimensione def
        if((ret = send(curr_sock, (void*) &aux, sizeof(aux), 0)) < 0){
            perror("Errore invio dimensione mess default");
            exit(-1);
        }

        //invio def
        if((ret = send(curr_sock, (void*) def, len, 0)) < 0){
            perror("Errore invio messaggio default");
            exit(-1);
        }
    }
    else{
        //gestione effetti
        char messaggio[1024];

        handleEffects(object, messaggio);

        //invio dimensione messaggio
        len = strlen(messaggio);
        aux=htons(len);
        if((ret = send(curr_sock, (void*) &aux, sizeof(aux), 0)) < 0){
            perror("Errore nell'invio dimensione messaggio use");
            exit(-1);
        }
        //INVIO MESSAGGIO
        if((ret = send(curr_sock, (void*) messaggio, len, 0)) < 0){
            perror("Errore nell'invio dimensione messaggio use");
            exit(-1);
        }
    }

}

//___________________________________________________________________________________________________ ASK FOR HELP _________________________________________________________________________________________________________
void askForHelp(int curr_sock){
    int ret;
    int16_t len, aux;
    char buf[1024];

    //ricevo dimensione indovinello
    if((ret = recv(curr_sock, (void*)&len, sizeof(len), 0)) < 0){
        printf("Errore nella ricezione della dimensione1 afh");
        exit(-1);
    }

    aux = ntohs(len);

    //ricevo enigma
    if((ret = recv(curr_sock, (void*)buf, aux, 0)) < 0){
        printf("Errore nella ricezione dell'enigma afh");
        exit(-1);
    }

    //invio dimensione risposta
    if(strcmp(enigma, "null") != 0 && strcmp(enigma, buf) != 0){
        //qualcuno ha già chiesto aiuto per un altro enigma
        len = -1;
        len = htons(len);
        if((ret = send(curr_sock, (void*)&len, sizeof(len), 0)) < 0){
            printf("Errore nell'invio della dimensione2.1 afh");
            exit(-1);
        }
        return;
    }
    else if(strcmp(enigma, buf) == 0){
        //qualcuno ha già chiesto aiuto per lo stesso enigma
        if(strcmp(ans, "null") == 0){
            len = -2;
            len = htons(len);
            if((ret = send(curr_sock, (void*)&len, sizeof(len), 0)) < 0){
                printf("Errore nell'invio della dimensione2.2 afh");
                exit(-1);
            }
            return;
        }
        //qualcuno ha risposto alla tua richiesta di aiuto

        strcpy(buf, ans);

        aux = strlen(buf);
        len = htons(aux);

        //invio dimensione risposta
        if((ret = send(curr_sock, (void*)&len, sizeof(len), 0)) < 0){
            printf("Errore nell'invio della dimensione2.3 afh");
            exit(-1);
        }
    
        //invio risposta
        if((ret = send(curr_sock, (void*)buf, aux, 0)) < 0){
            printf("Errore nell'invio della risposta afh");
            exit(-1);
        }

        //libero la board degli aiuti
        strcpy(enigma, "null");
        strcpy(ans, "null");
    }
    else{
        //nessuno ha ancora chiesto aiuto
        strcpy(enigma, buf);
        len = -3;
        len = htons(len);
        if((ret = send(curr_sock, (void*)&len, sizeof(len), 0)) < 0){
            printf("Errore nell'invio della dimensione2.4 afh");
            exit(-1);
        }
        return;
    }    
}

//___________________________________________________________________________________________________ OFFER HELP _________________________________________________________________________________________________________
void offerHelp(int curr_sock){
    int ret;
    int16_t len, aux;
    char buf[1024];

    //ricevo dimensione risposta
    if((ret = recv(curr_sock, (void*)&len, sizeof(len), 0)) < 0){
        printf("Errore nella ricezione della dimensione1 offer");
        exit(-1);
    }

    if(strcmp(enigma, "null") == 0){
        //nessun giocatore ha richiesto aiuto
        len = 0;
        len = htons(len);
        if((ret = send(curr_sock, (void*)&len, sizeof(len), 0)) < 0){
            printf("Errore nell'invio della dimensione2 offer");
            exit(-1);
        }
        return;
    }
    else if(strcmp(enigma, "null") != 0  && strcmp(ans, "null") != 0){
        //un altro utente ha già prestato aiuto
        len = -1;
        len = htons(len);
        if((ret = send(curr_sock, (void*)&len, sizeof(len), 0)) < 0){
            printf("Errore nell'invio della dimensione");
            exit(-1);
        }
        return;
    }
    //è stata fatta una richiesta a cui puoi rispondere

    strcpy(buf, enigma);

    
    aux = strlen(buf);
    len = htons(len);

    //invio dimensione
    if((ret = send(curr_sock, (void*)&len, sizeof(len), 0)) < 0){
        printf("Errore nell'invio della dimensione enigma offer");
        exit(-1);
    }

    //invio enigma
    if((ret = send(curr_sock, (void*)buf, aux, 0)) < 0){
        printf("Errore nell'invio dell'enigma offer");
        exit(-1);
    }

    //ricevo dimensione risposta
    if((ret = recv(curr_sock, (void*)&len, sizeof(len), 0)) < 0){
        printf("Errore nella ricezione della dimensione risposta offer");
        exit(-1);
    }

    aux = ntohs(len);

    //ricevo risposta
    if((ret = recv(curr_sock, (void*)buf, aux, 0)) < 0){
        printf("Errore nella ricezione della risposta offer");
        exit(-1);
    }

    if(strcmp(buf, "annulla\n") != 0){
        strcpy(ans, buf);
        end(curr_sock);
    }
}

//___________________________________________________________________________________________________ END _________________________________________________________________________________________________________
void end(int curr_sock){
    int ret, i, j;
    uint16_t aux;

    //invio l'outcome
    aux = htons(outcome);
    if((ret = send(curr_sock, (void*) &aux, sizeof(aux), 0)) < 0){   
        perror("Errore nell'invio dell'outcome end");
        exit(-1);
    }

    //libero risorse
    for(i = 0; i < 5; i++){
        for(j = 0; j < 3; j++){
            if(location[i].obj[i].taker == curr_sock)
                location[i].obj[i].taker = 0;
        }
    }

    //elimino giocatore dalla partita
    for(i=0; i<5 ;i++){ 
        if(game.player[i]==curr_sock){
            game.player[i] = 0;
            break;
        } 
    }
    game.tot_players--;
    if(game.tot_players<0)
        game.tot_players=0;

    printf("> Il numero di players rimasti è: %d\n",game.tot_players);
    close(curr_sock);
    FD_CLR(curr_sock, &master);

    if(game.tot_players == 0)
        exit(0);
}