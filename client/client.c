/*
//utility
int chooseComm(int sd);
u_int16_t getComm();
//gestisci tempo()
void newPlayer(int conferma, int sd, char* username);
*/
void options();

#include "client_utility.h"

//game
void start();
void signUp(int sd);
int login(int sd);
void printTime();
void look(int sd);
void take(int sd);
void drop(int sd);
void use(int sd);
void objs();
void askForHelp(int sd);
void offerHelp(int sd);
void end(int sd);

int main (int arg, char* argv[]){
    int ret, sd;
    struct sockaddr_in server_addr; 
    logged=0;
    outcome=0;

    //schermata iniziale
    start();

    port = (u_int16_t)strtoul(argv[1], NULL, 10);
    port=htons(port);

    //creazione socket
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Errore nella connessione al server");
        exit(-1);
    }

    //address    
    memset(&server_addr, 0, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET ; 
    server_addr.sin_port = htons(4242); 
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); 

    //connessione
    if((ret = connect(sd, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0){
        perror("Errore nella connessione al server");
        exit(-1);
    }
    
    printf("> Connessione stabilita\n");
    
    while(1){
        int choice=chooseComm(sd);
        
        switch(choice){
        case 1:
            signUp(sd);
            break;

        case 2:
            login(sd);
            break;

        case 3:
            look(sd);
            break;

        case 4:
            take(sd);
            break;

        case 5:
            drop(sd);
            break;

        case 6:
            use(sd);
            break;

        case 7:
            askForHelp(sd);
            break;

        case 8:
            offerHelp(sd);
            break;

        case 9:
            end(sd);
            break;

        case 10:
            objs();
            break;

        case 11:
            options(sd);
            break;

        default:
            break;
        }
        printTime();
    }
    printf("> connessione terminata\n");

    return 0;
}

//_______________________________________________________________________________________________ START CLIENT ___________________________________________________________________________________________________
void start(){
    while(1){
    printf("*********************************CLIENT STARTED*********************************\n"
           "in quale stanza vuoi giocare: \n-dungeon \n-coming_soon...\n"
           "********************************************************************************\n");
           
        char buf[1024];
        int len;

        fgets(buf, sizeof(buf), stdin);
        len = strlen(buf);
        buf[len-1] = '\0';

        if(strcmp(buf,"dungeon")==0)
            break;

        printf("> altri scenari saranno presto disponibili\n");
    }
}

//_______________________________________________________________________________________________ REGISTRAZIONE _________________________________________________________________________________________________
void signUp(int sd){
    char username[1024]; 
    char password[1024];
    int ret;
    u_int16_t len;

    printf(">Puoi registrarti inserendo il nome utente (massimo 6 caratteri)\n");

    fgets(username, sizeof(username), stdin);
    len = strlen(username);
    username[len-1] = '\0';
        
    if((ret = send(sd, (void*)username, len, 0)) < 0){
        perror("Errore nell'invio dell'username signup");
        exit(-1);
    }
        
    printf("> Inserisci una password (massimo 6 caratteri)\n");
    
    fgets(password, sizeof(password), stdin);

    len = strlen(password);
    password[len-1] = '\0';
    
    if((ret = send(sd, (void*)password, len, 0)) < 0){   
        perror("Errore nell'invio della password signup");
        exit(-1);
    }

    newPlayer(sd, username);
}

//_____________________________________________________________________________________________________ LOGIN ___________________________________________________________________________________________________
int login(int sd){
//controlla esistenza account e in caso fa login inizializzando anche timer
    int ret;
    char username[1024]; 
    char password[1024];
    u_int16_t found, len;

    printf("> utente: \n");
    
    //prelevo username e lo invio
    fgets(username, sizeof(username), stdin);
    len = strlen(username);
    username[len-1] = '\0';

    if((ret = send(sd, (void*)username, len, 0)) < 0){
        perror("Errore nell'invio username login");
        exit(-1);
    }
    
    printf("> password: \n");

    //prelevo password e la invio
    fgets(password, sizeof(password), stdin);
    len = strlen(password);
    password[len-1] = '\0';

    if((ret = send(sd, (void*)password, len, 0)) < 0){   
        perror("Errore nell'invio password login");
        exit(-1);
    }

    //ricevo conferma che l'account esista
    if((ret=recv(sd, (void*)&found, sizeof(found), 0)) < 0){   
        perror("Errore nella ricezione della conferma");
        exit(-1);
    }

    found = ntohs(found);
    if(found){
        newPlayer(sd, username);
    }
    else{
        printf("> Non è stato posibile effetuare alcun tipo di login\n");
        logged = 0;
    }

    return 0;
}

//_____________________________________________________________________________________________________ LOOK ____________________________________________________________________________________________________
void look(int sd){
    int ret;
    u_int16_t len, aux;

    char mess[1024];
    memset(mess, 0, sizeof(mess));

    //se non è esplicitata una location o un oggetto si fa la look della stanza principal
    if(strcmp(parameter[1],"")==0)
        strcpy(parameter[1],"dungeon");

    //invio dimensione oggetto
    len = strlen(parameter[1]);
    aux=htons(len);

    if((ret = send(sd, (void*) &aux, sizeof(aux), 0)) < 0){
        perror("Errore nell'invio della dimensione oggetto look");
        exit(-1);
    }

    //invio nome oggetto
    if((ret = send(sd, (void*) parameter[1], len, 0)) < 0){
        perror("Errore nell'invio dell'oggetto look");
        exit(-1);
    }

    //ricezione dimensione descrizione
    if((ret=recv(sd, (void*) &len, sizeof(len), 0)) < 0){   
        perror("Errore nella ricezione dimensione della descrizione look");
        exit(-1);
    }

    len=ntohs(len);
    if(!len){
        printf("> Impossibile trovare %s, ritenta\n",parameter[1]);
        return;
    }

    //ricezione descrizione
    if((ret=recv(sd, (void*) mess, len, 0)) < 0){   
        perror("Errore nella ricezione della descrizione look");
        exit(-1);
    }

    printf("> %s\n",mess);
}

//_____________________________________________________________________________________________________ TAKE ____________________________________________________________________________________________________
void take(int sd){
    int ret, i;
    int16_t len;
    u_int16_t aux;

    char mess[1024];
    char riddle[1024];
    char obj[1024];
    memset(mess, 0, sizeof(mess));
    memset(riddle, 0, sizeof(riddle));
    memset(obj, 0, sizeof(obj));

    strcpy(obj, parameter[1]);

    //Comunico al server che è stata fatta una take senza oggetto inviando una X
    if(strcmp(parameter[1],"")==0){
        strcpy(obj,"X");
    }
    
    //controllo se è già in possesso di questo giocatore
    for(i = 0; i < 3; i++){
        if(strcmp(client.obj[i], parameter[1]) == 0){
            strcpy(parameter[1], "already");
        }
    }

    if(client.n_obj==3)
        len = -1;
    else
        len = strlen(obj);

    aux=htons(len);

    //invio dimensione oggetto
    if((ret = send(sd, (void*) &aux, sizeof(aux), 0)) < 0){
        perror("Errore nell'invio dimensione oggetto take");
        exit(-1);
    }

    if(len==-1){
        printf("> hai troppi oggetti lasciane prima uno\n");
        return;
    }

    //invio nome oggetto
    if((ret = send(sd, (void*) obj, len, 0)) < 0){
        perror("Errore nell'invio oggetto take");
        exit(-1);
    }

    printf("%s\n", obj);
    if(strcmp(obj, "already") == 0){
        printf("> Oggetto già in possesso\n");
        return;
    }

    if(strcmp(obj, "X") == 0){
        printf("nessun oggetto indicato, riprovare\n");
        return;
    }

    //ricezione dimensione indovinello (oppure con certi valori particolari gestisco errori)
    if((ret=recv(sd, (void*) &len, sizeof(len), 0)) < 0){   
        perror("Errore nella ricezione dimensione oggetto take");
        exit(-1);
    }

    len=ntohs(len);
    //gestione casi particolari
    if(!len){
        printf("> l'oggetto %s non esiste o non può essere preso, ritenta\n",parameter[1]);
        return;
    }
    else if(len==-1){
        printf("> oggetto già in possesso\n");
        return;
    }
    else if(len == -2){
        printf("> Non è possibile prendere una location, riprova con un oggetto\n");
        return;
    }
        
    //ricezione dimensione messaggio
    if((ret=recv(sd, (void*) &aux, sizeof(aux), 0)) < 0){   
        perror("Errore nella ricezione dimensione messaggio take");
        exit(-1);
    }
    aux=ntohs(aux);

    //ricezione messaggio
    if((ret=recv(sd, (void*) mess, aux, 0)) < 0){   
        perror("Errore nella ricezione messaggio take");
        exit(-1);
    }
    printf("%s\n",mess);

    if(strcmp(mess, "l'oggetto selezionato non è bloccato da indovinello\n") == 0){
        printf("> complimenti sei riuscito a prendere %s\n",parameter[1]);
        strcpy(client.obj[client.n_obj],parameter[1]);
        client.n_obj++;
        return;
    }

    //ricezione indovinello
    if((ret=recv(sd, (void*) riddle, len, 0)) < 0){   
        perror("Errore ricezione indovinello take");
        exit(-1);
    }
    printf("> %s\n",riddle);

    //prelievo risposta da input
    fgets(mess, sizeof(mess), stdin);

    len = strlen(mess);
    mess[len-1] = '\0';

    //invio dimensione risposta
    aux=htons(len);
    if((ret = send(sd, (void*) &aux, sizeof(aux), 0)) < 0){
        perror("Errore nell'invio dimensione risposta take");
        exit(-1);
    }

    //invio risposta
    if((ret = send(sd, (void*) mess, len, 0)) < 0){
        perror("Errore nell'invio risposta take");
        exit(-1);
    }

    //ricezione esito risposta
    if((ret=recv(sd, (void*) &len, sizeof(u_int16_t), 0)) < 0){   
        perror("Errore nella ricezione esito risposta take");
        exit(-1);
    }

    len=ntohs(len);
    if(!len){
        if(strcmp(parameter[1], "teschio") == 0){
            printf("Toccando l'oggetto, hai aperto una camera segreta. Sembra essere una ++catacomba++\n");
            return;
        }
        else 
            printf("> complimenti sei riuscito a prendere %s\n",parameter[1]);

        strcpy(client.obj[client.n_obj],parameter[1]);
        client.n_obj++;
    }
    else if(len == 3){
        //è stato raccolto l'ultimo dei 3 token
        printf("> complimenti sei riuscito a prendere %s\n",parameter[1]);
        strcpy(client.obj[client.n_obj],parameter[1]);
        client.n_obj++;
        printf("si sente un enorme boato rimbombare nella stanza, tutti i token sono stati trovati, la ++porta++ si è aperta\n");
    }
    else{
        printf("> riprova la risposta non è giusta \n");
    }
}

//______________________________________________________________________________________________________ DROP ____________________________________________________________________________________________________
void drop(int sd){
    int ret, i, j;
    u_int16_t len, aux;

    //controllo se l'oggetto è in possesso del giocatore e in caso lo rimuovo
    for(i = 0; i < 3; i++){
        if(strcmp(client.obj[i], parameter[1]) == 0){
            client.n_obj--;
            printf("> oggetto %s, lasciato con successo \n",client.obj[i]);

            if(strcmp(client.obj[i+1], "") != 0){
                for(j = i; j < 2; j++)
                    strcpy(client.obj[i], client.obj[i+1]);

            }
            strcpy(client.obj[i], "");
            break;
        }
    }
    if(i==3)
        printf("> Non hai raccolto nessun oggetto con questo nome\n");
    
    //invio dimensione oggetto lasciato
    len = strlen(parameter[1]);
    aux = htons(len);

    if((ret = send(sd, (void*) &aux, sizeof(aux), 0)) < 0){
        perror("Errore nell'invio della dimensione oggetto drop");
        exit(-1);
    }

    //invio oggetto lasciato
    if((ret = send(sd, (void*) parameter[1], len, 0)) < 0){
        perror("Errore nell'invio dell'oggetto drop");
        exit(-1);
    }
}

//________________________________________________________________________________________________________ USE ____________________________________________________________________________________________________
void use(int sd){
    int ret, i;
    int16_t len;
    u_int16_t aux;
    char mess[1024];
    memset(mess, 0, sizeof(mess));

    //controllo possesso oggetto
    len = strlen(parameter[1]);
    for(i=0; i<3; i++){
        if(strcmp(client.obj[i], parameter[1])==0){
            break;
        }
    }
    if(i==3){
        printf("> non puoi usare un oggetto che non hai!\n");
        len=-1;
    }

    if(strcmp(parameter[1], "clessidra") == 0)
        game_time+=60;
    else if(strcmp(parameter[1], "saltatempo") == 0)
        game_time = 0;
    
    //invio dimensione oggetto
    aux=htons(len);
    if((ret = send(sd, (void*) &aux, sizeof(aux), 0)) < 0){
        perror("Errore nell'invio dimensione oggetto use");
        exit(-1);
    }
    if(len== -1) 
        return;

    //invio oggetto
    if((ret = send(sd, (void*) parameter[1], len, 0)) < 0){
        perror("Errore nell'invio oggetto use");
        exit(-1);
    }
    
    //ricezione dimensione messaggio
    if((ret=recv(sd, (void*) &len, sizeof(u_int16_t), 0)) < 0){   
        perror("Errore nell'invio dimensione messaggio use");
        exit(-1);
    }
    len=htons(len);

    //ricezione messaggio
    if((ret=recv(sd, (void*) mess, len, 0)) < 0){   
        perror("Errore nella ricezione oggetto use");
        exit(-1);
    }
    printf("> %s\n",mess);
}

//________________________________________________________________________________________________________ OBJS ____________________________________________________________________________________________________
void objs(){
    int i, cont=0, token = 0;

    printf("> Gli ogetti di cui sei in possesso sono:\n");

    for(i = 0; i < 3; i++){
        if(strcmp(client.obj[i], "")==0)
            continue;
        if((strcmp(client.obj[i], "acqua") == 0) || (strcmp(client.obj[i], "lama_drago") == 0) || (strcmp(client.obj[i], "chiave") == 0))
            token++;
        cont ++;
        printf("> %d. %s \n",cont,client.obj[i]);
    }
    if(cont==0){
        printf("> Non hai raccolto ancora nessun oggetto\n");
    }

    printf("> Hai raccolto %d/3 token\n", token);

}

//______________________________________________________________________________________________________ ASK FOR HELP ____________________________________________________________________________________________________
void askForHelp(int sd){
    int ret;
    int16_t len, aux;
    char buf[1024];

    printf("Inserire testo dell'indovinello: \n");
    fgets(buf, sizeof(buf), stdin);

    len = strlen(buf);
    aux = htons(len);

    buf[len-1] = '\0';

    //invio dimensione enigma
    if((ret = send(sd, (void*)&len, sizeof(len), 0)) < 0){
        printf("Errore nell'invio dell'enigma");
        exit(-1);
    }
    
    //invio enigma
    if((ret = send(sd, (void*)buf, len, 0)) < 0){
        printf("Errore nell'invio dell'enigma");
        exit(-1);
    }

    //ricezione dimensione risposta
    if((ret = recv(sd, (void*)&len, sizeof(len), 0)) < 0){
        printf("Errore nella ricezione della dimensione");
        exit(-1);
    }

    aux = ntohs(len);

    if(aux == -1){
        printf("Qualcuno ha già fatto richiesta di aiuto\n");
        return;
    }
    else if(aux == -2){
        printf("Ancora nessuna risposta, riprova tra poco\n");
        return;
    }
    else if(aux == -3){
        printf("Richiesta d'aiuto presentata, riprova tra poco per scoprire se qualcuno si è sacrificato per te\n");
        return;
    }

    memset(buf, 0, sizeof(buf));

    //ricezione risposta
    if((ret = recv(sd, (void*)buf, aux, 0)) < 0){
        printf("Errore nella ricezione della risposta");
        exit(-1);
    }

    printf("Risposta: %s\n", buf);
}

//____________________________________________________________________________________________________ OFFER HELP ____________________________________________________________________________________________________
void offerHelp(int sd){
    int ret;
    int16_t len, aux;
    char buf[1024];

    len = htons(1);

    //invio richiesta per offrirmi
    if((ret = send(sd, (void*)&len, sizeof(len), 0)) < 0){
        printf("Errore nell'invio");
        exit(-1);
    }

    //ricezione dimensione enigma (utilizzata anche per gestire casi di errore)
    if((ret = recv(sd, (void*)&len, sizeof(len), 0)) < 0){
        printf("Errore nella ricezione della dimensione 2");
        exit(-1);
    }

    aux = ntohs(len);

    if(aux == 0){
        printf("Nessun giocatore ha bisogno di aiuto\n");
        return;
    }
    else if(aux == -1){
        printf("Un altro utente ha già prestato aiuto, riprova tra poco per verificare se ci siano nuove richieste\n");
        return;
    }

    memset(buf, 0, sizeof(buf));

    //ricezione enigma
    if((ret = recv(sd, (void*)buf, aux, 0)) < 0){
        printf("Errore nella ricezione dell'enigma");
        exit(-1);
    }
    printf("L'enigma è: %s\n", buf);

    //prelievo risposta da input
    printf("Inserire risposta all'indovinello, digita 'annulla' se non sai la risposta: \n");
    fgets(buf, sizeof(buf), stdin);

    aux = strlen(buf);
    len = htons(aux);

    //invio dimensione
    if((ret = send(sd, (void*)&len, sizeof(len), 0)) < 0){
        printf("Errore nell'invio dell'enigma");
        exit(-1);
    }

    //invio risposta
    if((ret = send(sd, (void*)buf, aux, 0)) < 0){
        printf("Errore nell'invio dell'enigma");
        exit(-1);
    }

    if(strcmp(buf, "annulla\n") != 0)
        end(sd);
}

void end(int sd){
    int ret;

    //richiedo outcome per determinare la causa termine partita
    if((ret=recv(sd, (void*) &outcome, sizeof(u_int16_t), 0)) < 0){   
        perror("Errore nella ricezione dell'outcome end");
        exit(-1);
    }
    outcome=ntohs(outcome);

    if(outcome==0){
         printf("> hai abbandonato\n");
    }
    else if(outcome==1){
        if((game_time)<=0)
            printf("> spiacente non hai più tempo a disposizione\n");

        printf("> La tua partita si è conclusa senza una vittoria\n");
    }
    else{
        printf("> La tua partita si è conclusa con una vittoria\n");
    }

    //chiusura socket
    printf("> socket chiuso\n");
    close(sd);
    exit(0);
}

void options(){
    printf("********************************************************************************\n"
        "Comandi:\n"
        "- look:   per guardare in modo ravvicinato un oggetto\n"
        "- take:   per prendere un oggetto (massimo 3)\n"
        "- drop:   per lasciare un oggetto di cui si è in possesso\n"
        "- use :   per utilizzare alcune proprietà nascoste di alcuni oggetti\n"
        "- objs:   per vedere di quali oggetti si è in possesso al momento\n"
        "- help:   per chiedere aiuto a qualcuno su un determinato indovinello\n"
        "- offer:  permette di offrirsi come sacrificio per sbloccare un altro giocatore dall'indovinello che non lo fa avanzare\n"
        "- end :   per terminare la partita\n"
        "digitare i comando seguito dall' oggetto sul quale si vuole usare l'azione(escluse le operazioni 5,6,7,8 che non richiedono oggetti)\n"
        "***********************************************************************************\n"
        );
}