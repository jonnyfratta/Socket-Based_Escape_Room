#include "scenario.h"

void init_scen(){
    int i, j;

//inizializzazione stanza iniziale
    strcpy(location[0].name, "dungeon");
    strcpy(location[0].descr, "Ti trovi all'interno di una grande grotta \nintorno a te vedi:\nun ++laghetto++ alla tua destra,\nuna specie di ++nido++ di grandi dimensioni davanti a te,\nun enorme ++scaffale++ sulla sinistra.\nInoltre noti che davanti a te, oltre il nido si erge un'imponente ++porta++.\n");

    strcpy(location[0].obj[0].name , "?");
    strcpy(location[0].obj[0].descr, "???");

    strcpy(location[0].obj[1].name , "?");
    strcpy(location[0].obj[1].descr, "???");

    strcpy(location[0].obj[2].name , "?");
    strcpy(location[0].obj[2].descr, "???");
    
//Parte destra della stanza
    strcpy(location[1].name , "laghetto");
    strcpy(location[1].descr, "L' **acqua** cristallina del lago lascia intravedere delle **alghe** luminose sul fondale.\nSulla riva si estende per molti metri una distesa di **minerali**\n");

    strcpy(location[1].obj[0].name , "acqua");
    strcpy(location[1].obj[0].descr, "l'acqua di questo lago sembra essere potabile\n");

    strcpy(location[1].obj[1].name , "alghe");
    strcpy(location[1].obj[1].descr, "le alghe sul fondo emettono luce, che abbiano qualche proprietà magica? \n");

    strcpy(location[1].obj[2].name , "minerali");
    strcpy(location[1].obj[2].descr , "cristalli di un minerale che sembra quasi indistruttibile, potrebbe essere utile per costruire strumenti, armi e armature \n");

//Parte Centrale della stanza
    strcpy(location[2].name , "nido");
    strcpy(location[2].descr, "si tratta di un nido di mostri del sottosuolo tra cui goblin, uomini pipistrello e orchi.\nAl suo interno si possono ancora intravedere i cadaveri delle vittime passate e alcuni oggetti di loro proprietà:\n **saltatempo** \n **incantesimo_veldora**\n");

    strcpy(location[2].obj[0].name , "saltatempo");
    strcpy(location[2].obj[0].descr , "sembra una collana proveniente da una famosa Saga, a quanto pare la parte interna può girare\n");

    strcpy(location[2].obj[1].name , "incantesimo_veldora");
    strcpy(location[2].obj[1].descr, "un rotolo scritto in un'antica lingua\n");

    strcpy(location[2].obj[2].name , "?");
    strcpy(location[2].obj[2].descr, "???");

//Parte sinistra della stanza
    strcpy(location[3].name , "scaffale");
    strcpy(location[3].descr , "trovi davanti a te uno scaffale cotenente decine di oggetti appartenenti a chissà quanti avventurieri prima di te. Ti saltano all'occhio tre oggetti in particolare:\n **lama_drago**\n **bastone_magico**\n **teschio**\n");

    strcpy(location[3].obj[0].name , "lama_drago");
    strcpy(location[3].obj[0].descr , "spada lucente la cui lama emette elettricità al tatto\n");

    strcpy(location[3].obj[1].name , "bastone_magico");
    strcpy(location[3].obj[1].descr, "bastone che sembra in grado di utilizzare un'ultima volta un incantesimo d'attacco prima di spezzarsi, potrebbe essere utile contro una minaccia\n");

    strcpy(location[3].obj[2].name , "teschio");
    strcpy(location[3].obj[2].descr , "sembra nascondere una leva, prova a prenderlo\n");

    strcpy(location[4].name , "catacomba");
    strcpy(location[4].descr , "Noti per terra alcuni oggetti, ad esempio una **clessidra**, un **rotolo** e una **chiave**\n");

    strcpy(location[4].obj[0].name , "clessidra");
    strcpy(location[4].obj[0].descr , "pare si possa utilizzare per ottenere del tempo extra\n");

    strcpy(location[4].obj[1].name , "rotolo");
    strcpy(location[4].obj[1].descr , "si tratta del Rotolo d'invocazione,\nti permette di chiedere l'aiuto della dea Lili e ottenere la prima lettera della soluzione al prossimo indovinello a patto che non si tratti di una scelta multipla \n");

    strcpy(location[4].obj[2].name , "chiave");
    strcpy(location[4].obj[2].descr, "una chiave dall'aria misteriosa\n");

//Porta
    strcpy(location[5].name , "porta");
    strcpy(location[5].descr, "A quanto pare il dungeon si compone di più stanze, adesso hai accesso alla ++sala_finale++");

//Stanza Finale
    strcpy(location[6].name, "sala_finale");
    strcpy(location[6].descr, "Trovi davanti a te un immenso ++drago++ che è tenuto in questa stanza da catene.\n ");

    strcpy(location[7].name , "drago");
    strcpy(location[7].descr , "Il drago ti racconta di chiamarsi Veldora e di essere imprigionato da 100 anni. Osservando l'immensa creatura noti che:\nla coda del drago sembra essere stata mozzata, a giudicare dalle scaglie e dalla forma del moncone doveva essere affilata come una lama,\nla bocca del drago è molto asciutta, deve essere molto assetato, e\nla serratura delle catene mostra una fessura di una forma già vista... ma dove?\n Usa degli oggetti per cercare la soluzione\n");

//Indovinelli
    location[1].obj[0].riddle = &riddle[0];
    location[2].obj[0].riddle = &riddle[1];
    location[2].obj[1].riddle = &riddle[2];
    location[3].obj[0].riddle = &riddle[3];
    location[3].obj[1].riddle = &riddle[4];
    location[3].obj[2].riddle = &riddle[5];
    location[4].obj[0].riddle = &riddle[6];
    location[4].obj[2].riddle = &riddle[7];

    strcpy(riddle[0].question, "Cosa ha un occhio ma non può vedere?\n");
    strcpy(riddle[0].answer,"ago");
    
    strcpy(riddle[1].question, "Cosa ha un collo ma non ha una testa?\n");
    strcpy(riddle[1].answer,"bottiglia");
   
    strcpy(riddle[2].question, "Cosa ha la testa ma non ha i capelli?\n");
    strcpy(riddle[2].answer,"chiodo");
    
    strcpy(riddle[3].question, "Qual’è il nome della santa a cui ogni studente che si rispetti porge le proprie preghiere in periodo di sessione?\n");
    strcpy(riddle[3].answer,"rita");
   
    strcpy(riddle[4].question, "Chi incontrò i lillipuziani?\nA) pinocchio\nB) gulliver\nC) alice\n");
    strcpy(riddle[4].answer,"gulliver");
    
    strcpy(riddle[5].question, "Qual è il fiume più lungo del mondo?\nA) amazzone\nB) nilo\nC) mississippi\nD) danubio\n");
    strcpy(riddle[5].answer,"nilo");
    
    strcpy(riddle[6].question, "Qual è la capitale dell'Australia?\nA) canberra\nB) sidney \nC) melbourne\n");
    strcpy(riddle[6].answer,"canberra");

    strcpy(riddle[7].question, "Quando sono giovane, sono alta ma quando sono vecchia sono bassa. Chi sono?\n");
    strcpy(riddle[7].answer, "candela");
    

//inzializzo campo lookable:
    for(i = 0; i < 8; i++)
        location[i].lookable = 1;

//inizializzo campo usable e takeable:
    for(i = 0; i < 5; i++){
        for(j = 0; j < 3; j++){
            location[i].obj[j].usable = 1;
        }
    }

    location[1].obj[1].usable = 0;
    location[1].obj[2].usable = 0;

//inizializzo campo locked
    for(i = 0; i < 5; i++){
        for(j = 0; j < 3; j++){
            location[i].obj[j].locked = 1;
        }
    }
    location[1].obj[1].locked = 0;
    location[1].obj[2].locked = 0;
    location[2].obj[2].locked = 0;
    location[4].obj[1].locked = 0;
}
