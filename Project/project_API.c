#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define DEST (*names)[offset_dest]
#define ORIG (*names)[offset_orig]
#define NAME (*names)[offset_ent]
#define REL (*relations)[offset_rel]


/*///////////////////////////////////////////////////		DATA		//////////////////////////////////////////////////////////////*/	 

typedef struct rel{				//RELAZIONE DELL'ENTITA':							(stessa cosa di rel_type ma ristretta per una sola entità)
	
    char* id_rel;               //nome
    char** ent_tx;           	//entità che 'puntano' a quella in esame
	char** ent_px;				//entità alle quali 'punta' quella in esame
	int n_rel; 					//numero di entità che 'puntano' a quella in esame
	int n_pnt;					//numero di entità alle quali 'punta' quella in esame
} rel;
   
typedef struct ent_type{		//ENTITA':

    char* id_ent;               //nome
    rel* pointed_rel;         	//relazioni che l'entità riceve (array)
	int n_pointed;				//numero di relazioni che l'entità riceve
} ent_type;
    
typedef struct ent{				//ENTITA' DELLA RELAZIONE
	
    char* id_ent;		    	//nome
	int n_rx;					//numero di entità che 'puntano' a quella in esame
} ent;
	
typedef struct rel_type{		//RELAZIONE:

    char* id_rel;               //nome
    char** max_ent;             //entità con numero massimo di 'frecce ricevute'
    int max_people;				//numero di entità che condividono il posto di max_ent
	int n_max;                  //numero di 'frecce' che l'entità riceve
	int n_ent;					//numero di entità riceventi questa relazione
	ent* entities;				//entità che ricevono la relazione
} rel_type;

/*//////////////////////////////////////////////////		FUNCTIONS 		////////////////////////////////////////////////////////////*/
	
int check_ent(char* name, ent_type* names, int length);
int check_rel(char* name, rel_type* relations, int length);

int compare_rel(const void* arg1, const void* arg2)					//funzione che confronta due relazioni e le ordina (vedi qsort)
{
    const rel_type* a = (const rel_type*) arg1;
    const rel_type* b = (const rel_type*) arg2;
	
    return strcmp(a->id_rel, b->id_rel);
}

int compare_ent(const void* arg1, const void* arg2)					//funzione che confronta due entità e le ordina (vedi qsort)
{
    const ent_type* a = (const ent_type*) arg1;
    const ent_type* b = (const ent_type*) arg2;
	
    return strcmp(a->id_ent, b->id_ent);
}

int compare_max_ent(const void* a, const void* b) 					// "		" 		due entità in max_ent  "		"
{ 
    return strcmp(*(const char**)a, *(const char**)b); 
} 

char** delete(char** array, int term, int offset_word)
{
	if (term == 1 && offset_word == 0) return NULL;
	char** temp_arr;
	
	if(term-1 != offset_word) {
		array[offset_word] = realloc(array[offset_word], (strlen(array[term-1])+1)*sizeof(char));
		strcpy(array[offset_word],array[term-1]);
	}
	free(array[term-1]);
	array[term-1] = NULL;
	
	temp_arr = realloc(array, (term-1)*sizeof(char*));
	if (temp_arr == NULL) puts("ERRORE in delete()");
	return temp_arr;
}


/*////////////////////////////////////////////////         ENTITIES          ///////////////////////////////////////////////////////*/

int add_ent(char* line, ent_type** names, int length_names)
{
	//per tutta la funzione uso un puntatore all'area di memoria di names in modo da modificare 
	//direttamente la memoria e non una variabile locale della funzione add_ent
	//
	//(*names)[j] == (*names + (size_t)((long unsigned int)(j * 8))
	
    int  i = 8, l = length_names;
	char entity[30];
	ent_type* temp;
	
	// static char delim[4] = " \n\r\n";
	// entity = strtok(line, delim);
	// entity = strtok(NULL, delim);
	
	entity[0] = '"';
	while(line[i] != '"') {
		entity[i-7] = line[i];
		i++;
	}
	entity[i-7] = line[i];
	entity[i-6] = '\0';
	
	if (check_ent(entity, *names, length_names) != -1) {           			//se l'entità è presente non fare niente
		return length_names;
	}
	else {                                                					//se non è presente rialloco l'array names e la aggiungo
		temp = realloc(*names, sizeof(ent_type)*(l+1));
		if(temp == NULL) { puts("\nLa realloc(names) non funziona");}
		*names = temp;
		(*names)[l].id_ent = malloc(strlen(entity)+1);
		strcpy((*names)[l].id_ent, entity);
		
		(*names)[l].pointed_rel = NULL;
		(*names)[l].n_pointed = 0;
		
		length_names ++;
	}
	qsort(*names, length_names, sizeof(ent_type), compare_ent);
	
    return length_names;
}


int del_ent (char* line, ent_type** names, rel_type** relations, int length_names, int* length_rel) 
{
	int i = 8, offset_ent, offset_dest, len_rel = *length_rel;
	char entity[30];
	
	// static char delim[4] = " \n\r\n";
	// entity = strtok(line, delim);
	// entity = strtok(NULL, delim);
	
	entity[0] = '"';
	while(line[i] != '"') {
		entity[i-7] = line[i];
		i++;
	}
	entity[i-7] = line[i];
	entity[i-6] = '\0';
	
	offset_ent = check_ent(entity, *names, length_names);
	
	if (offset_ent == -1)  return length_names;													//se l'entità non è presente non fare niente

	int j, k, n, offset_rel;											
	for(i = 0; i < NAME.n_pointed; i++) {														//controllo tutte le relazioni che l'entità riceve
		
		if(relations != NULL) offset_rel = check_rel(NAME.pointed_rel[i].id_rel, *relations, len_rel);						//qui mi salvo la relazione in esame (è uno strumento che ci servirà più tardi)
		
		for(j = 0; j < NAME.pointed_rel[i].n_rel ; j++){
			offset_dest = check_ent(NAME.pointed_rel[i].ent_tx[j], *names, length_names);		//vado a modificare l'entità che la punta in modo che non la punti più
			
			for(k = 0; k < DEST.n_pointed; k++){
				if(strcmp(DEST.pointed_rel[k].id_rel, NAME.pointed_rel[i].id_rel) == 0) {
					for(n = 0; n < DEST.pointed_rel[k].n_pnt; n++) {
						if( strcmp(DEST.pointed_rel[k].ent_px[n], entity) == 0){
							
							if(DEST.pointed_rel[k].n_pnt > 1){
								DEST.pointed_rel[k].ent_px = delete(DEST.pointed_rel[k].ent_px, DEST.pointed_rel[k].n_pnt, n);
								DEST.pointed_rel[k].n_pnt--;
							}
							else {
								free(DEST.pointed_rel[k].ent_px[0]);
								free(DEST.pointed_rel[k].ent_px);
								DEST.pointed_rel[k].ent_px = NULL;
								DEST.pointed_rel[k].n_pnt = 0;
							}
							if(DEST.pointed_rel[k].n_rel == 0 && DEST.pointed_rel[k].n_pnt == 0){
								
								if(DEST.n_pointed != k) DEST.pointed_rel[k] = DEST.pointed_rel[DEST.n_pointed -1];
								DEST.pointed_rel = realloc(DEST.pointed_rel, sizeof(rel)*(DEST.n_pointed -1));
								DEST.n_pointed--;
							}
							
							break;
						}
					}
					break;
				}
			}
		}
		
		for(j = 0; j < NAME.pointed_rel[i].n_pnt ; j++){
			offset_dest = check_ent(NAME.pointed_rel[i].ent_px[j], *names, length_names);
			//poi modifico quelle che l'entità da cancellare punta, in quanto queste potevano essere max_ent della relazione e in tal caso devo modificare quest'ultima
			
			for(k = 0; k < DEST.n_pointed; k++){
				if(strcmp(DEST.pointed_rel[k].id_rel, NAME.pointed_rel[i].id_rel) == 0) {
					for(n = 0; n < DEST.pointed_rel[k].n_rel; n++) {
						if( strcmp(DEST.pointed_rel[k].ent_tx[n], entity) == 0){
							
							if(DEST.pointed_rel[k].n_rel > 1){
								DEST.pointed_rel[k].ent_tx = delete(DEST.pointed_rel[k].ent_tx, DEST.pointed_rel[k].n_rel, n);
								DEST.pointed_rel[k].n_rel--;
							}
							else {
								free(DEST.pointed_rel[k].ent_tx[0]);
								free(DEST.pointed_rel[k].ent_tx);
								DEST.pointed_rel[k].ent_tx = NULL;
								DEST.pointed_rel[k].n_rel = 0;
							}
							if(DEST.pointed_rel[k].n_rel == 0 && DEST.pointed_rel[k].n_pnt == 0){
								
								if(DEST.n_pointed != k) DEST.pointed_rel[k] = DEST.pointed_rel[DEST.n_pointed -1];
								DEST.pointed_rel = realloc(DEST.pointed_rel, sizeof(rel)*(DEST.n_pointed -1));
								DEST.n_pointed--;
							}
							break;
						}
					}
					break;
				}
			}
			
			if(relations != NULL) {
				if (REL.n_ent == 1 && REL.n_max == 1) {										//se la relazione aveva solo questa entità puntata da quella da cancellare allora devo togliere tutto
					
					if (offset_rel != len_rel-1) REL = (*relations)[len_rel-1];
					rel_type* temp1 = realloc( (*relations), sizeof(rel_type)*(len_rel-1));
					len_rel--;
					(*length_rel)--;
					
					if (temp1 == NULL) {
						(*relations) = NULL;
						relations = NULL;
					}
					else {
						(*relations) = temp1;
						if(len_rel > 1) qsort(*relations, len_rel, sizeof(rel_type), compare_rel);
					}
				}
				else{																		//altrimenti nella relazione c'era un'altra entities o l'unica aveva altre entità che la puntavano
					for(k = 0; k < REL.n_ent; k++) {
						if(strcmp(DEST.id_ent, REL.entities[k].id_ent) == 0){
							if(REL.entities[k].n_rx == REL.n_max) {
								
								if(REL.entities[k].n_rx == 1){								//poi modifico o cancello l'entità puntata da quella da cancellare nella relazione
								
									if(REL.n_ent-1 != k) REL.entities[k] = REL.entities[REL.n_ent-1];
									ent* temp2 = realloc( REL.entities, sizeof(ent)*(REL.n_ent-1));
									if(temp2 == NULL) puts("errore nella realloc(entities");
									else{
										REL.entities = temp2;
										REL.n_ent--;
									}
								}
								else {
									REL.entities[k].n_rx--;
								}
								
								if(REL.max_people == 1){ 								//e se era l'unica max_ent ricalcolo chi metterci
								
									REL.max_people = 0;
									while(REL.max_people == 0){
										REL.n_max--;
										
										for(n = 0; n < REL.n_ent; n++){ 	//ciclo per riempire di nuovo max_ent
												if (REL.n_max == REL.entities[n].n_rx) {
													REL.max_ent = realloc(REL.max_ent, (REL.max_people+1) * sizeof(char*));
													REL.max_ent[REL.max_people] = malloc(sizeof(char)*strlen(REL.entities[n].id_ent)+1);
													strcpy(REL.max_ent[REL.max_people], REL.entities[n].id_ent);
													REL.max_people++;
												}
										}
										if(REL.max_people > 1) qsort(REL.max_ent, REL.max_people, sizeof(const char *), compare_max_ent);
										break;
									}
								}
								else{													//altrimenti la rimuovo semplicemente dalle max_ent
									for(n = 0; n < REL.max_people; n++){
										if( strcmp(REL.max_ent[n], NAME.pointed_rel[i].ent_px[j]) == 0){
											REL.max_ent = delete(REL.max_ent, REL.max_people, n);
											REL.max_people--;
											if(REL.max_people > 1) qsort(REL.max_ent, REL.max_people, sizeof(const char *), compare_max_ent);
											break;
										}
									}
								}
							}
							else{
								if(REL.entities[k].n_rx == 1){								//poi modifico o cancello l'entità puntata da quella da cancellare nella relazione
									if(REL.n_ent-1 != k) REL.entities[k] = REL.entities[REL.n_ent-1];
									ent* temp2 = realloc( REL.entities, sizeof(ent)*(REL.n_ent-1));
									if(temp2 == NULL) puts("errore nella realloc(entities");
									else{
										REL.entities = temp2;
										REL.n_ent--;
									}
								}
								else {
									REL.entities[k].n_rx--;
								}
							}
						}
					}
					
				}
			}
		}
		
		if(relations != NULL && offset_rel != len_rel) {							//la seconda condizione è vera quando un'entità punta se stessa ed elimino la relazione sopra
			for(k = 0; k < REL.n_ent; k++) {
				if(strcmp(REL.entities[k].id_ent, entity) == 0) {
					if(REL.n_ent != 1){
						if(REL.entities[k].n_rx == REL.n_max) {						//guardo se l'entità era nelle max_ent

							if(REL.max_people == 1){ 								//e se era l'unica max_ent ricalcolo chi metterci
							
								REL.max_people = 0;
								while(REL.max_people == 0){
									REL.n_max--;
									
									for(n = 0; n < REL.n_ent; n++){ 	//ciclo per riempire di nuovo max_ent
											if(n != k){
												if (REL.n_max == REL.entities[n].n_rx) {
													REL.max_ent = realloc(REL.max_ent, (REL.max_people+1) * sizeof(char*));
													REL.max_ent[REL.max_people] = malloc(sizeof(char)*strlen(REL.entities[n].id_ent)+1);
													strcpy(REL.max_ent[REL.max_people], REL.entities[n].id_ent);
													REL.max_people++;
												}
											}
									}
									if(REL.max_people > 1) qsort(REL.max_ent, REL.max_people, sizeof(const char *), compare_max_ent);
								}
							}
							else{													//altrimenti la rimuovo semplicemente dalle max_ent
								for(n = 0; n < REL.max_people; n++){
									if( strcmp(REL.max_ent[n], NAME.id_ent) == 0){
										REL.max_ent = delete(REL.max_ent, REL.max_people, n);
										REL.max_people--;
										if(REL.max_people > 1) qsort(REL.max_ent, REL.max_people, sizeof(const char *), compare_max_ent);
										break;
									}
								}
							}
						}
						
						//poi cancello l'entità
						
						if(REL.n_ent-1 != k) REL.entities[k] = REL.entities[REL.n_ent-1];
						ent* temp2 = realloc( REL.entities, sizeof(ent)*(REL.n_ent-1));
						if(temp2 == NULL) puts("errore nella realloc(entities");
						else{
							REL.entities = temp2;
							REL.n_ent--;
						}
					}
					else {  	//REL.ent == 1
						if (offset_rel != len_rel-1) REL = (*relations)[len_rel-1];
						rel_type* temp1 = realloc( (*relations), sizeof(rel_type)*(len_rel-1));
						len_rel--;
						(*length_rel)--;
						
						if (temp1 == NULL) {
							(*relations) = NULL;
							relations = NULL;
						}
						else {
							(*relations) = temp1;
							if(len_rel > 1) qsort(*relations, len_rel, sizeof(rel_type), compare_rel);
						}
					}
					break;
				}
			}
		}
	}
	
	
	if(length_names-1 != offset_ent) NAME = (*names)[length_names-1];
	ent_type* temp1 = realloc(*names, (length_names-1)*sizeof(ent_type));
	
	if (temp1 == NULL && length_names != 1) {
		puts("ERRORE nell'eliminare l'entità");
	}
	else{
		*names = temp1;
		qsort(*names, length_names-1, sizeof(ent_type), compare_ent);
	}
	length_names--;
	
	return length_names;
}



int check_ent(char* name, ent_type* names, int length)
{
	//binary search
	int a = 0, b = length, j;
	while((a - b) > 2) {										
		if (names[(a + b)/2].id_ent[1] < name[1]) {
			a = (a + b)/2;
		}
		else {
			if (names[(a + b)/2].id_ent[1] > name[1]) {
				b = (a + b)/2;
			}
			else{
				if(strcmp(names[(a + b)/2].id_ent, name) == 0) {
					return (a + b)/2;
				}
				else{
					for(j = a; j < b; j++){
						if(strcmp(names[j].id_ent, name) == 0) {
							return j;
						}
					}
					if (j == b) {
						return -1;
					}
				}
			}
		}
	}
	
	// ricerca lineare
	for(j = a; j < b; j++) {
		if (strcmp((names[j]).id_ent, name) == 0) {
			return j;
		}
	}
	return (-1);
}



/*//////////////////////////////////////////////         RELATIONS         /////////////////////////////////////////////////////////*/


int add_rel(char* line_input, rel_type** relations, ent_type** names, int length_names, int length_rel)
{
    int j, i, k, b = 7;
	//static char s[4] = " \n\r\n";
	//strtok(line_input, s);
    char tid_orig[30];// = strtok(NULL, s);
    char tid_dest[30];// = strtok(NULL, s);
    char tid_rel[30];// = strtok(NULL, s);
	rel_type* temp1;
	rel* temp2;
	
    //ALTERNATIVA A STRTOK PER RIEMPIRE I TRE CAMPI
    for(j = 0; j < 3; j++){
        i = 0;
        while(line_input[b] != ' ' && line_input[b] != '\n' && line_input[b] != '\r' && b < 100) {
            if(j == 0){
                tid_orig[i] = line_input[b];
                }
			else {
				if(j == 1){
					tid_dest[i] = line_input[b];
					}
				else {
					tid_rel[i] = line_input[b];
					}
				}
            b++;
            i++;
            }
		if(j == 0){
                tid_orig[i] = '\0';
                }
		else { 
			if (j == 1){
				tid_dest[i] = '\0';
				}
			else {
				tid_rel[i] = '\0';
				}
			}
        b++;
		}
		
    
    
    int offset_orig = check_ent(tid_orig, *names, length_names);
    int offset_dest = check_ent(tid_dest, *names, length_names);
    int offset_rel = check_rel(tid_rel, *relations, length_rel);
    if( offset_dest != -1 && offset_orig != -1) {                                         	//controllo se le entità esistono (altrimenti non faccio nulla)
	
		if( offset_rel == -1) {                                                 			//La relazione non esiste ancora quindi la creo
		
			int l = length_rel;
			temp1 = realloc(*relations, sizeof(rel_type)*(l+1));
			if(temp1 == NULL) { puts("\nLa realloc(relations) non funziona");}
			*relations = temp1;
			
			(*relations)[l].id_rel = malloc((strlen(tid_rel)+1) * sizeof(char));			//inizializzo tutti i campi della nuova relazione
			(*relations)[l].max_ent = malloc(sizeof(char*));
			(*relations)[l].max_ent[0] = malloc((strlen(tid_dest)+1) * sizeof(char));
			
			(*relations)[l].entities = malloc(sizeof(ent));
			(*relations)[l].entities[0].id_ent = malloc((strlen(tid_dest)+1) * sizeof(char));
			(*relations)[l].entities[0].n_rx = 1;
			(*relations)[l].n_max = 1;
			(*relations)[l].n_ent = 1;
			(*relations)[l].max_people = 1;
			
			strcpy((*relations)[l].id_rel, tid_rel);          								//nome
			strcpy((*relations)[l].max_ent[0], tid_dest);       							//entità con più 'frecce' è unica
			strcpy((*relations)[l].entities[0].id_ent, tid_dest);
			
			length_rel++;
			
			//e inserisco questo nuovo tipo di relazione nell'entità
			
			temp2 = realloc(DEST.pointed_rel, sizeof(rel)* (DEST.n_pointed+1));
			if (temp2 == NULL) { puts("\nLa realloc (pointed_rel) non funziona"); }
			DEST.pointed_rel = temp2;
			
			DEST.pointed_rel[DEST.n_pointed].id_rel = malloc((strlen(tid_rel)+1)*sizeof(char));
			DEST.pointed_rel[DEST.n_pointed].ent_tx = malloc(sizeof(char*));
			DEST.pointed_rel[DEST.n_pointed].ent_tx[0] = malloc((strlen(tid_orig)+1)*sizeof(char));
			DEST.pointed_rel[DEST.n_pointed].ent_px = NULL;
			
			strcpy(DEST.pointed_rel[DEST.n_pointed].id_rel, tid_rel);
			strcpy(DEST.pointed_rel[DEST.n_pointed].ent_tx[0], tid_orig);
			DEST.pointed_rel[DEST.n_pointed].n_rel = 1;
			DEST.pointed_rel[DEST.n_pointed].n_pnt = 0;

			
			if (offset_dest != offset_orig) {
				DEST.n_pointed++;
				temp2 = realloc(ORIG.pointed_rel, sizeof(rel)* (ORIG.n_pointed+1));
				if (temp2 == NULL) { puts("\nLa realloc (pointed_rel) non funziona"); }
				ORIG.pointed_rel = temp2;
			
				ORIG.pointed_rel[ORIG.n_pointed].id_rel = malloc((strlen(tid_rel)+1)*sizeof(char));
				strcpy(ORIG.pointed_rel[ORIG.n_pointed].id_rel, tid_rel);
				ORIG.pointed_rel[ORIG.n_pointed].ent_tx = NULL;
				ORIG.pointed_rel[ORIG.n_pointed].n_rel = 0;
			}
			
			ORIG.pointed_rel[ORIG.n_pointed].ent_px = malloc(sizeof(char*));
			ORIG.pointed_rel[ORIG.n_pointed].ent_px[0] = malloc((strlen(tid_dest)+1)*sizeof(char));
			strcpy(ORIG.pointed_rel[ORIG.n_pointed].ent_px[0], tid_dest);
			ORIG.pointed_rel[ORIG.n_pointed].n_pnt = 1;
			ORIG.n_pointed++;
			
			
			qsort(*relations, length_rel, sizeof(rel_type), compare_rel); 						//ho aggiunto una relazione intera quindi le riordino
		}
	
		else {  //(temp_rel != -1) --> la relazione esiste già
		
			for(i = 0; i < DEST.n_pointed; i++){											//la relazione esiste già, quindi controllo se
				if( strcmp(DEST.pointed_rel[i].id_rel, tid_rel) == 0) {						//l'entità destinataria ha già questa relazione registrata
					for(j = 0; j < DEST.pointed_rel[i].n_rel; j++) {                   		//quindi controllo se "orig" è già presente
						if( strcmp(DEST.pointed_rel[i].ent_tx[j], tid_orig) == 0){			//e in tal caso non faccio niente
							return length_rel;
						}
					}
					if (j == DEST.pointed_rel[i].n_rel) {									//caso in cui orig non sia presente
						
						//aggiungo orig alle entità che puntano a dest
						DEST.pointed_rel[i].ent_tx = realloc(DEST.pointed_rel[i].ent_tx, sizeof(char*)*(j+1));
						DEST.pointed_rel[i].ent_tx[j] = malloc(sizeof(char*)*(strlen(tid_orig)+1)); 
						strcpy(DEST.pointed_rel[i].ent_tx[j], tid_orig);
						DEST.pointed_rel[i].n_rel ++;
						
						//aggiungo dest alle entità puntate da orig
						for(k = 0; k < ORIG.n_pointed; k++){
							if( strcmp(ORIG.pointed_rel[k].id_rel, tid_rel) == 0) {			//se ORIG ha già quel tipo di relazione
							
								ORIG.pointed_rel[k].ent_px = realloc(ORIG.pointed_rel[k].ent_px, sizeof(char*)*(ORIG.pointed_rel[k].n_pnt+1));
								ORIG.pointed_rel[k].ent_px[ORIG.pointed_rel[k].n_pnt] = malloc((strlen(tid_dest)+1)*sizeof(char));
								strcpy(ORIG.pointed_rel[k].ent_px[ORIG.pointed_rel[k].n_pnt], tid_dest);
								ORIG.pointed_rel[k].n_pnt++;
								break;
							}
						}
						if (k == ORIG.n_pointed) {											//ORIG non puntava ancora nessuno con questa relazione
							temp2 = realloc(ORIG.pointed_rel, sizeof(rel)* (ORIG.n_pointed+1));
							if (temp2 == NULL) { puts("\nLa realloc (pointed_rel) non funziona"); }
							ORIG.pointed_rel = temp2;
							
							ORIG.pointed_rel[ORIG.n_pointed].id_rel = malloc((strlen(tid_rel)+1)*sizeof(char));
							ORIG.pointed_rel[ORIG.n_pointed].ent_px = malloc(sizeof(char*));
							ORIG.pointed_rel[ORIG.n_pointed].ent_px[0] = malloc((strlen(tid_dest)+1)*sizeof(char));
							ORIG.pointed_rel[ORIG.n_pointed].ent_tx = NULL;
							
							strcpy(ORIG.pointed_rel[ORIG.n_pointed].id_rel, tid_rel);
							strcpy(ORIG.pointed_rel[ORIG.n_pointed].ent_px[0], tid_dest);
							ORIG.pointed_rel[ORIG.n_pointed].n_rel = 0;
							ORIG.pointed_rel[ORIG.n_pointed].n_pnt = 1;
							ORIG.n_pointed++;
						}
						
						//e aumento il numero di entità puntanti a dest nella relazione
						for(k = 0; k < REL.n_ent; k++){
							if(strcmp(REL.entities[k].id_ent, tid_dest) == 0){
								REL.entities[k].n_rx++;
								break;
							}
						}
						if(k == REL.n_ent) {												//DEST non aveva altre entità che puntassero a lui (non era ancora registrato nella REL
							REL.entities = realloc(REL.entities, (REL.n_ent+1) * sizeof(ent));
							REL.entities[REL.n_ent].id_ent = malloc(sizeof(char)*strlen(tid_dest)+1);
							REL.entities[REL.n_ent].n_rx = 1;
							strcpy( REL.entities[REL.n_ent].id_ent, tid_dest);
							REL.n_ent++;
							
							
						}
						break;
					}
				}
			}
			
			if (i == DEST.n_pointed) {                                    						//l'entità non ha ancora questo tipo di relazione
				temp2 = realloc(DEST.pointed_rel, sizeof(rel)* (DEST.n_pointed+1));				//quindi creo la relazione nelle pointed_rel dell'entità
				if (temp2 == NULL) { puts("La realloc (pointed_rel) non funziona"); }
				DEST.pointed_rel = temp2;
				DEST.pointed_rel[i].id_rel = malloc((strlen(tid_rel)+1)*sizeof(char));
				DEST.pointed_rel[i].ent_px = NULL;
				DEST.pointed_rel[i].ent_tx = malloc(sizeof(char*));
				DEST.pointed_rel[i].ent_tx[0] = malloc((strlen(tid_orig)+1)*sizeof(char));
				strcpy(DEST.pointed_rel[i].id_rel, tid_rel);
				strcpy(DEST.pointed_rel[i].ent_tx[0], tid_orig);
				DEST.pointed_rel[i].n_rel = 1;
				DEST.pointed_rel[i].n_pnt = 0;
				DEST.n_pointed++;
				
				for(k = 0; k < ORIG.n_pointed; k++){
					if( strcmp(ORIG.pointed_rel[k].id_rel, tid_rel) == 0) {						//guardo se ORIG ha già quel tipo di relazione
					
						ORIG.pointed_rel[k].ent_px = realloc(ORIG.pointed_rel[k].ent_px, sizeof(char*)*(ORIG.pointed_rel[k].n_pnt+1));
						ORIG.pointed_rel[k].ent_px[ORIG.pointed_rel[k].n_pnt] = malloc((strlen(tid_dest)+1)*sizeof(char));
						strcpy(ORIG.pointed_rel[k].ent_px[ORIG.pointed_rel[k].n_pnt], tid_dest);
						ORIG.pointed_rel[k].n_pnt++;
						break;
					}
				}
				if (k == ORIG.n_pointed) {											//ORIG non puntava ancora nessuno con questa relazione
					temp2 = realloc(ORIG.pointed_rel, sizeof(rel)* (ORIG.n_pointed+1));
					if (temp2 == NULL) { puts("\nLa realloc (pointed_rel) non funziona"); }
					ORIG.pointed_rel = temp2;
					
					ORIG.pointed_rel[ORIG.n_pointed].id_rel = malloc((strlen(tid_rel)+1)*sizeof(char));
					ORIG.pointed_rel[ORIG.n_pointed].ent_px = malloc(sizeof(char*));
					ORIG.pointed_rel[ORIG.n_pointed].ent_px[0] = malloc((strlen(tid_dest)+1)*sizeof(char));
					ORIG.pointed_rel[ORIG.n_pointed].ent_tx = NULL;
					
					strcpy(ORIG.pointed_rel[ORIG.n_pointed].id_rel, tid_rel);
					strcpy(ORIG.pointed_rel[ORIG.n_pointed].ent_px[0], tid_dest);
					ORIG.pointed_rel[ORIG.n_pointed].n_rel = 0;
					ORIG.pointed_rel[ORIG.n_pointed].n_pnt = 1;
					ORIG.n_pointed++;
				}
				
				
				/*//aggiungo dest alle entità puntate da orig
				for(k = 0; k < ORIG.n_pointed; k++){
					if( strcmp(ORIG.pointed_rel[k].id_rel, tid_rel) == 0) {			//se ORIG ha già quel tipo di relazione
						temp2 = realloc(ORIG.pointed_rel, sizeof(rel)* (ORIG.n_pointed+1));
						if (temp2 == NULL) { puts("\nLa realloc (pointed_rel) non funziona"); }
						ORIG.pointed_rel = temp2;
						
						ORIG.pointed_rel[ORIG.n_pointed].id_rel = malloc((strlen(tid_rel)+1)*sizeof(char));
						ORIG.pointed_rel[ORIG.n_pointed].ent_px = malloc(sizeof(char*));
						ORIG.pointed_rel[ORIG.n_pointed].ent_px[0] = malloc((strlen(tid_orig)+1)*sizeof(char));
						ORIG.pointed_rel[ORIG.n_pointed].ent_tx = NULL;
						
						strcpy(ORIG.pointed_rel[ORIG.n_pointed].id_rel, tid_rel);
						strcpy(ORIG.pointed_rel[ORIG.n_pointed].ent_px[0], tid_dest);
						ORIG.pointed_rel[ORIG.n_pointed].n_rel = 0;
						ORIG.n_pointed++;
					}
				}*/
				
				REL.entities = realloc(REL.entities, (REL.n_ent+1) * sizeof(ent));				//e aggiungo l'entità alla relazione
				REL.entities[REL.n_ent].id_ent = malloc(sizeof(char)*strlen(tid_dest)+1);
				REL.entities[REL.n_ent].n_rx = 1;
				strcpy( REL.entities[REL.n_ent].id_ent, tid_dest);
				REL.n_ent++;
			}
			
			if (REL.n_max < DEST.pointed_rel[i].n_rel) {                             			//Controllo se aggiungendo la relazione cambia n_max 
				REL.max_people = 1;
				REL.n_max = DEST.pointed_rel[i].n_rel;
				REL.max_ent = realloc(REL.max_ent, sizeof(char*));
				REL.max_ent[0] = realloc(REL.max_ent[0], (strlen(tid_dest)+1)*sizeof(char));
				strcpy(REL.max_ent[0], tid_dest);
			}
			else { 
				if (REL.n_max == DEST.pointed_rel[i].n_rel) {									//se le entità ricevono lo stesso numero di relazioni le accodo
					REL.max_people++;
					REL.max_ent = realloc(REL.max_ent, (REL.max_people) * sizeof(char*));
					REL.max_ent[REL.max_people-1] = malloc(sizeof(char)*strlen(tid_dest)+1);
					strcpy(REL.max_ent[REL.max_people-1], tid_dest);
					
					qsort(REL.max_ent, REL.max_people, sizeof(const char *), compare_max_ent);			//ho aggiunto una relazione tra due entità quindi riordino i max_ent
				}
			}
		}
	}
	return length_rel;
}



int del_rel(char* line_input, rel_type** relations, ent_type** names, int length_names, int length_rel)
{
    int i, k, b = 7, j = 0;
	//static char s[4] = " \n\r\n";
	//strtok(line_input, s);
    char tid_orig[30];// = strtok(NULL, s);
    char tid_dest[30];// = strtok(NULL, s);
    char tid_rel[30];// = strtok(NULL, s);
	
    //ALTERNATIVA A STRTOK PER RIEMPIRE I TRE CAMPI
    for(j = 0; j < 3; j++){
        i = 0;
        while(line_input[b] != ' ' && line_input[b] != '\n' && line_input[b] != '\r' && b < 100) {
            if(j == 0){
                tid_orig[i] = line_input[b];
                }
			else {
				if(j == 1){
					tid_dest[i] = line_input[b];
					}
				else {
					tid_rel[i] = line_input[b];
					}
				}
            b++;
            i++;
            }
		if(j == 0){
                tid_orig[i] = '\0';
                }
		else { 
			if (j == 1){
				tid_dest[i] = '\0';
				}
			else {
				tid_rel[i] = '\0';
				}
			}
        b++;
		}
		
    

    int offset_orig = check_ent(tid_orig, *names, length_names);
    int offset_dest = check_ent(tid_dest, *names, length_names);
    int offset_rel = check_rel(tid_rel, *relations, length_rel);
	
	//controllo se le entità esistono (altrimenti non faccio nulla)
    if( offset_dest != -1 && offset_orig != -1) {
		
		//controllo se la relazione esiste
		if( offset_rel != -1) {
		
			for(i = 0; i < DEST.n_pointed; i++){											//la relazione esiste, quindi controllo se
				if( strcmp(DEST.pointed_rel[i].id_rel, tid_rel) == 0) {						//l'entità destinataria ha una relazione di quel tipo con qualcuno (altrimenti non faccio nulla)
					
					for(j = 0; j < DEST.pointed_rel[i].n_rel; j++) {                   		//quindi controllo se "orig" punta "dest",
						if( strcmp(DEST.pointed_rel[i].ent_tx[j], tid_orig) == 0){			//se la relazione esiste procedo a rimuoverla
						
						
							//////////// qui lavoro sulle relazioni //////////////////////////////////////////////
							
							if(REL.n_ent == 1 && REL.n_max == 1){												//se questa era l'unica entità ricevente, rimuovo completamente la relazione
								if(length_rel == 1){
									length_rel--;
								}
								else{
									if(length_rel-1 != offset_rel) REL = (*relations)[length_rel-1];
									rel_type* temp1 = realloc(*relations, (size_t)(length_rel-1)*sizeof(rel_type));
									length_rel--;
									
									if (temp1 == NULL) {
										(*relations) = NULL;
										relations = NULL;
									}
									else{
										*relations = temp1;
										qsort(*relations, length_rel, sizeof(rel_type), compare_rel);
									}
								}
							}
							
							else {
								for(k = 0; k < REL.n_ent; k++){
									if(strcmp(REL.entities[k].id_ent, tid_dest) == 0){			//trovo l'offset dell'entità nella relazione
										
										if(REL.entities[k].n_rx == 1){							//se solo orig puntava a dest rimuovo dest dal gruppo di entità riceventi
											if(REL.n_ent-1 != k) REL.entities[k] = REL.entities[REL.n_ent-1];
											
											ent* temp3 = realloc(REL.entities, (REL.n_ent-1)*sizeof(ent));
								
											if (temp3 == NULL) {
												puts("ERRORE nell'eliminare una entities");
											}
											else{
												REL.entities = temp3;
												REL.n_ent--;
											}
										}
										else{														//altrimenti devo solo diminuire n_rx (l'entità vera e propria la tolgo sotto)
											REL.entities[k].n_rx--;
										}
										
										break;
									}
								}
								
								if(REL.n_max == DEST.pointed_rel[i].n_rel){					//prima guardo se era nelle max_ent
									
									if(REL.max_people == 1){ 								//e se era l'unica max_ent ricalcolo chi metterci
									
										REL.max_people = 0;
										while(REL.max_people == 0){
											REL.n_max--;
											REL.max_people = 0;
											for(k = 0; k < REL.n_ent; k++){
												//ciclo per riempire di nuovo max_ent
												if (REL.n_max == REL.entities[k].n_rx) {
													REL.max_ent = realloc(REL.max_ent, (REL.max_people+1) * sizeof(char*));
													REL.max_ent[REL.max_people] = malloc(sizeof(char)*strlen(REL.entities[k].id_ent)+1);
													strcpy(REL.max_ent[REL.max_people], REL.entities[k].id_ent);
													REL.max_people++;
												}
											}
											qsort(REL.max_ent, REL.max_people, sizeof(const char *), compare_max_ent);
										}
									}
									else{													//altrimenti la rimuovo semplicemente dalle max_ent
										for(k = 0; k < REL.max_people; k++){
											if( strcmp(REL.max_ent[k], tid_dest) == 0){
												REL.max_ent = delete(REL.max_ent, REL.max_people, k);
												REL.max_people--;
												qsort(REL.max_ent, REL.max_people, sizeof(const char *), compare_max_ent);
												break;
											}
										}
									}
								}
							}
							
							
							/////////// qui lavoro sulle entità //////////////////////////////////////////////////
							
							if(DEST.pointed_rel[i].n_rel == 1 && DEST.pointed_rel[i].n_pnt == 0) {			//se solo orig puntava a dest rimuovo tutta la relazione dall'entità
								
								if(DEST.n_pointed == 1){													//dest aveva solo una relazione quindi pointed_rel = NULL
									free(DEST.pointed_rel[i].id_rel);
									free(DEST.pointed_rel[i].ent_tx);
									free(DEST.pointed_rel);
									DEST.pointed_rel = NULL;
									DEST.n_pointed = 0;
								}
								else{
									if(DEST.n_pointed-1 != i) DEST.pointed_rel[i] = DEST.pointed_rel[DEST.n_pointed-1];
									
									rel* temp2 = realloc(DEST.pointed_rel, (DEST.n_pointed-1)*sizeof(rel));
									
									if (temp2 == NULL) {
										puts("ERRORE nell'eliminare pointed_rel");
									}
									else{
										DEST.pointed_rel = temp2;
										DEST.n_pointed--;
									}
								}
							}
							else{															//altrimenti rimuovo solo orig dalle entità che la puntano												
								DEST.pointed_rel[i].ent_tx = delete(DEST.pointed_rel[i].ent_tx , DEST.pointed_rel[i].n_rel, j);
								DEST.pointed_rel[i].n_rel--;
							}
							
							//ciclo per trovare la pointed_rel corrispondente in ORIG
							for(k = 0; k < ORIG.n_pointed; k++){
								if( strcmp(ORIG.pointed_rel[k].id_rel, tid_rel) == 0) break;
							}
							if(k == ORIG.n_pointed) printf("%s non ha %s tra le sue pointed_rel", ORIG.id_ent, tid_rel);
							
							
							if(ORIG.pointed_rel[k].n_rel == 0 && ORIG.pointed_rel[k].n_pnt == 1) {			//se solo orig puntava a dest rimuovo tutta la relazione dall'entità
								
								if(ORIG.n_pointed == 1){													//ORIG aveva solo una relazione quindi pointed_rel = NULL
									free(ORIG.pointed_rel[k].id_rel);
									free(ORIG.pointed_rel[k].ent_px);
									free(ORIG.pointed_rel);
									ORIG.pointed_rel = NULL;
									ORIG.n_pointed = 0;
								}
								else{
									if(ORIG.n_pointed-1 != k) ORIG.pointed_rel[k] = ORIG.pointed_rel[ORIG.n_pointed-1];
									
									rel* temp2 = realloc(ORIG.pointed_rel, (ORIG.n_pointed-1)*sizeof(rel));
									
									if (temp2 == NULL) {
										puts("ERRORE nell'eliminare pointed_rel");
									}
									else{
										ORIG.pointed_rel = temp2;
										ORIG.n_pointed--;
									}
								}
							}
							else{																			//altrimenti rimuovo solo orig dalle entità che la puntano
								for(j = 0; j < ORIG.pointed_rel[k].n_pnt; j++) {
									if( strcmp(ORIG.pointed_rel[k].ent_px[j], tid_dest) == 0){
										ORIG.pointed_rel[k].ent_px = delete(ORIG.pointed_rel[k].ent_px , ORIG.pointed_rel[k].n_pnt, j);
										ORIG.pointed_rel[k].n_pnt--;
										break;
									}
								}
							}
							
							break;
						}
					}
					break;
				}
			}
		}
	}
	return length_rel;
}

int check_rel(char* name, rel_type* relations, int length)
{
    int j;
    for(j = 0; j < length; j++) {
            if (strcmp((relations[j]).id_rel, name) == 0) {
                return j;
                }
            }
    return (-1);
}


/*/////////////////////////////////////   REPORT /////////////////////////////////////////////////*/

/*void report_names(ent_type* names, int len_ent)
{
	//Controllo entità
    int i = 0, j = 0, n = 0;
	
    for (i = 0; i < len_ent; i++){
		for (j = 0; j < names[i].n_pointed; j++){
			for(n = 0; n < names[i].pointed_rel[j].n_rel; n++){
				printf("l'entita %s riceve una relazione di tipo %s da %s. \n", names[i].id_ent, names[i].pointed_rel[j].id_rel, names[i].pointed_rel[j].ent_tx[n]);
			}
			if(n == 0) printf("%s non riceve nessuna relazione del tipo %s\n", names[i].id_ent, names[i].pointed_rel[j].id_rel);
			for(n = 0; n < names[i].pointed_rel[j].n_pnt; n++){
				printf("l'entita %s ha una relazione di tipo %s con %s. \n", names[i].id_ent, names[i].pointed_rel[j].id_rel, names[i].pointed_rel[j].ent_px[n]);
			}
			if(n == 0) printf("%s non ha nessuna relazione del tipo %s\n", names[i].id_ent, names[i].pointed_rel[j].id_rel);
		}
		if(names[i].n_pointed == 0) printf("%s non ha relazioni con nessuno\n", names[i].id_ent);
		puts("");
	}
    puts("");
}*/

void report(int len_rel, rel_type* relations)
{
    int i = 0, j = 0, n;
	
    while (i < len_rel) {
		j = 0;
		fputs(relations[i].id_rel, stdout);
		while(j < relations[i].max_people) {
			putchar(' ');
			fputs(relations[i].max_ent[j], stdout); 
			j++; }
		putchar(' ');
		n = relations[i].n_max;
		if(n < 10) putchar('0' + n);
		else{
			if(n < 100) {
				putchar('0' + n/10);
				putchar('0' + n%10);
			}
			else printf("%d", n);
		}
		putchar(';');
		putchar(' ');
        i++;
        }
	puts("");
}


/*/////////////////////////////////////////////////////////////////////////////////////////////////
//                                        MAIN                                                   //
/////////////////////////////////////////////////////////////////////////////////////////////////*/

int main ()//int argc, char* argv[]*/)
{
	//FILE* file_input = fopen(argv[1], "r");
	int num_ent = 0;
	int num_rel = 0;
	char* line_input = calloc(100, sizeof(char));
	
    rel_type* relations = NULL;
    ent_type* names = NULL;
	
    while(1) {
    //Ciclo per leggere ogni riga di comando e riconoscerla

		if (fgets(line_input, 100, stdin) == NULL) printf("\n errore \nt");
		//puts(line_input);
		
		if (line_input[0] == 'a'){
			if (line_input[3] == 'e'){
				num_ent = add_ent(line_input, &names, num_ent);
				continue;           
			}
			if (line_input[3] == 'r'){
				if(num_ent > 0){ 
					num_rel = add_rel(line_input, &relations, &names, num_ent, num_rel);
				}
				continue;
			}
		}
		if (line_input[0] == 'd'){
			if (line_input[3] == 'e'){
				if(num_ent > 0){
					num_ent = del_ent(line_input, &names, &relations, num_ent, &num_rel);
					
				}
			}
			if (line_input[3] == 'r'){
				if(num_ent > 0 && num_rel > 0) {
					num_rel = del_rel(line_input, &relations, &names, num_ent, num_rel);
				}
			}
			continue;
		}
		if (line_input[0] == 'r'){
			if(relations == NULL){
				puts("none");
			}
			else{
				report(num_rel, relations);
				//report_names(names, num_ent);
				}
			continue;
		}
		if (line_input[0] == 'e'){
			//report_names(names, num_ent);
			break;
		}
    }
	return 0;
}