#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void* azioniStudente( void* identificatoreStudente );
void* azioniAssistente();

#define NUMERO_SEDIE_ATTESA 3
#define NUMERO_DEGLI_STUDENTI 5

sem_t semaforoS;
sem_t semaforoA;
pthread_mutex_t mutexThread;

int sediaAttesa[3];
int numeroStudentiInAttesa = 0;
int newPosizione = 0;
int posizioneSuccessiva = 0;
int variabileBandiera = 0;

int main( int argc, char **argv ){
	int i;
	int numeroStudenti;
	if (argc > 1 ) {
		if ( verificaNumero( argv[1] ) == 1) {
			numeroStudenti = atoi( argv[1] );
		}
		else {
			printf("ERRORE!! Hai inserito un input non valido");
			return 0;
		}
	}
	else { numeroStudenti = NUMERO_DEGLI_STUDENTI;
	}
	int identificatoreStudentes[numeroStudenti];
	pthread_t students[numeroStudenti];
	pthread_t ta;
	sem_init( &semaforoS, 0, 0 );
	sem_init( &semaforoA, 0, 1 );
	pthread_mutex_init( &mutexThread, NULL );
	pthread_create( &ta, NULL, azioniAssistente, NULL );
	for( i = 0; i < numeroStudenti; i++ ) {
		identificatoreStudentes[i] = i + 1;
		pthread_create( &students[i], NULL, azioniStudente, (void*) &identificatoreStudentes[i] );
	}
	pthread_join(ta, NULL);
	for( i =0; i < numeroStudenti; i++ ) {
		pthread_join( students[i],NULL );
	}
	return 0;
}


void* azioniStudente( void* identificatoreStudente ) {
	int id_studente = *(int*)identificatoreStudente;
	while( 1 ) {
		if ( verificaAttesa( id_studente ) == 1 ) { continue; }
		int time = rand() % 5;
		printf( "\tLo studente %d sta programmando per %d secondi.\n", id_studente, time );
		sleep( time );
		pthread_mutex_lock( &mutexThread );
		if( numeroStudentiInAttesa < NUMERO_SEDIE_ATTESA ) {
			sediaAttesa[newPosizione] = id_studente;
			numeroStudentiInAttesa++;
			printf( "\t Lo studente %d prende posto, lo studente ha aspettato = %d.\n", id_studente, numeroStudentiInAttesa );
			newPosizione = ( newPosizione + 1 ) % NUMERO_SEDIE_ATTESA;
			pthread_mutex_unlock( &mutexThread );
			sem_post( &semaforoS );
			sem_wait( &semaforoA );
}
		else { 
			pthread_mutex_unlock( &mutexThread );
			printf( "Lo studente %d aspettera' \n",id_studente );
		}
	}
}


void* azioniAssistente() {
	printf( "Controllo gli studenti \n" );
	while( 1 ) {
		if ( numeroStudentiInAttesa > 0 ) {
			variabileBandiera = 0;
			sem_wait( &semaforoS );
			pthread_mutex_lock( &mutexThread );
			int tempoAiuto = rand() % 5;
			printf( "Studente aiutato per %d secondi, dopo ever aspettato per %d.\n", tempoAiuto, (numeroStudentiInAttesa - 1) );
			printf( "Lo studente %d sta ricevendo aiuto.\n",sediaAttesa[posizioneSuccessiva] );
			sediaAttesa[posizioneSuccessiva]=0;
			numeroStudentiInAttesa--;
			posizioneSuccessiva = ( posizioneSuccessiva + 1 ) % NUMERO_SEDIE_ATTESA;
			sleep( tempoAiuto );
			pthread_mutex_unlock( &mutexThread );
			sem_post( &semaforoA );
		}
		else {
			if ( variabileBandiera == 0 ) {
				printf( "Nessun studente sta aspettando. \n Dormendo...\n" );
				variabileBandiera = 1;
			}
		}
	}
}


int verificaAttesa( int identificatoreStudente ) {
	int i;
	for ( i = 0; i < 3; i++ ) {
		if ( sediaAttesa[i] == identificatoreStudente ) { return 1; }
	}
	return 0;
}


int verificaNumero(char numero[]){
	int i;
	for ( i = 0 ; numero[i] != 0; i++ ) {
        if (!isdigit(numero[i])){
            return 0;
	}
    }
    return 1;
}