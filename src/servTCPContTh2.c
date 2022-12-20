#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <sys/select.h>
#include "sqlite3.h"

extern int errno;
const char* db_path="db/database.db";
const char* query = "SELECT * FROM quiz";


//setul de fd pt select
fd_set file_descriptors_set;
//timer interactiune
struct timeval tv;
// mutex pt conditia de oprire
static pthread_mutex_t winner_mutex;
int stop_condition=0;
int winner_id=0;

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);
char* extrage_rand_formatat_baza_de_date(sqlite3_stmt* prepared_statement);
sqlite3* init_sql_db_connection(const char* db_path);
sqlite3_stmt* create_prepared_statement(sqlite3* database_handle, const char* query);
void cleanup(sqlite3* database_handle, sqlite3_stmt* prepared_statement);


int main (int argc, char *argv[])
{

  int PORT = atoi(argv[1]);
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;
  int nr;		//mesajul primit de trimis la client
  int sd;		//descriptorul de socket
  int pid;
  pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
  int i=0;

  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }
  /* utilizarea optiunii SO_REUSEADDR */
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));

  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 2) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }

  /* servim in mod concurent clientii...folosind thread-uri */
  while (1)
    {

      int client;
      thData * td; //parametru functia executata de thread
      int length = sizeof (from);

      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);


      // client= malloc(sizeof(int));
      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}

        /* s-a realizat conexiunea, se astepta mesajul */

	// int idThread; //id-ul threadului
	// int cl; //descriptorul intors de accept
	td=(struct thData*)malloc(sizeof(struct thData));
	td->idThread=i++;
	td->cl=client;

        /*adaugam socketul in set*/
	pthread_create(&th[i], NULL, &treat, td);

	}
};
static void *treat(void * arg)
{
		struct thData tdL;
		tdL= *((struct thData*)arg);
		printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
		fflush (stdout);
		pthread_detach(pthread_self());
		raspunde((struct thData*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
		return(NULL);

};

void transmite_mesaj_initial(struct thData tdL){

	const char* mesaj_initial = "Bun venit la QuizzGame! \n\n\nRegulament: \n \
			\n Aveti la dispozitie 10 secunde pentru a raspunde la o intrebare \n \
			\n Introduceti litera corespunzatoare variantei alese  \n \
			\n Dupa ce fiecare jucator a raspuns la toate intrebarile, pe ecran va aparea scorul final. \n \
			\n\nDistractie placuta!:)\
			\n\n\n";

	//trimitem mesajul de bun venit
	 if (write (tdL.cl, mesaj_initial, strlen(mesaj_initial)+1) <= 0)
		{
		 printf("[Thread %d] ",tdL.idThread);
		 perror ("[Thread]Eroare la write() catre client.\n");
		}


}

char* extrage_rand_formatat_baza_de_date(sqlite3_stmt* prepared_statement)
{
/*
Schema bazei de date
sqlite> pragma table_info('quiz');
0|id|INTEGER|0||1
1|question_text|varchar(255)|0||0
2|answer_1|varchar(127)|0||0
3|answer_2|varchar(127)|0||0
4|answer_3|varchar(127)|0||0
5|answer_4|varchar(127)|0||0
6|correct_answer|varchar(3)|0||0
*/
	int stop = -1;
	int rows_no=sqlite3_column_count(prepared_statement);
	char* data=(char*)malloc(255*sizeof(char));
	//iteram prin randurile bazei de date

	stop = sqlite3_step(prepared_statement);
	if (stop == SQLITE_ROW)
	{

		for(int i=0;i<rows_no;i++)
		{
			switch(i)
			{
				case 0:
					sprintf(data,"%s) ", sqlite3_column_text(prepared_statement,i));
					break;
				case 1:
					sprintf(data+strlen(data),"%s\n", sqlite3_column_text(prepared_statement,i));
					break;
				case 6:
					sprintf(data+strlen(data),"\n%s ", sqlite3_column_text(prepared_statement,i));
					break;
				default:
					//pentru a evita o eroare de compilare
					printf("");
					//reinitializam varianta de raspuns
					char varianta[255]="";
					// Generam litera din fata raspunului. Variantele de raspuns incep de la coloana 2,
					// deci trebuie sa adaptam valoarea variabilei i.
					varianta[0] = 'A' + i -2;
					varianta[1] = '.';
					strcat(varianta+2, sqlite3_column_text(prepared_statement,i));
					sprintf(data+strlen(data),"%s ", varianta);
					break;
			}
		}
		return data;
	}
	else
	{
		return "\0";
	}


}

sqlite3* init_sql_db_connection(const char* db_path)
{

	// Creem handle-ul catre baza de date
	sqlite3* database_handle= malloc(sizeof(sqlite3*));
	// Deschidem conexiunea cu baza de date
	errno = sqlite3_open(db_path,&database_handle);
	if (errno !=0)
	{
		printf("[database]Eroare la conectarea cu baza de date, Cod: %d \n",errno);
	}

	return database_handle;
}

sqlite3_stmt* create_prepared_statement(sqlite3* database_handle, const char* query)
{

	sqlite3_stmt* prepared_statement=malloc(sizeof(sqlite3_stmt*));


	errno = sqlite3_prepare(database_handle,query,strlen(query),&prepared_statement,NULL);
	if (errno !=0)
	{
		printf("[database]Eroare creere prepared statement, Cod: %d \n",errno);

	}
	return prepared_statement;

}
void cleanup(sqlite3* database_handle, sqlite3_stmt* prepared_statement)
{

	// Distrugem prepared-statement-ul
	errno = sqlite3_finalize(prepared_statement);
	if (errno !=0)
	{
		printf("[database]Eroare la inchiderea conexiunii cu baza de date, Cod: %d \n",errno);
	}
	// Inchidem conexiunea la baza de date
	errno = sqlite3_close(database_handle);
	if (errno !=0)
	{
		printf("[database]Eroare la inchiderea conexiunii cu baza de date, Cod: %d \n",errno);
	}

}


void raspunde(void *arg)
{
	int score=0;
	char mesaj_final[255];
	int ratio=0;
	int question_value=10;

	struct thData tdL;
	tdL= *((struct thData*)arg);
	transmite_mesaj_initial(tdL);

  	//initializam conexiunea la baza de date
 	sqlite3* database_handle = init_sql_db_connection(db_path);
  	sqlite3_stmt* prepared_statement = create_prepared_statement(database_handle,query);

	//reinitializam conexiunea pt permiterea de sesiuni in paralel
	pthread_mutex_lock(&winner_mutex);
	stop_condition=0;
	pthread_mutex_unlock(&winner_mutex);

  	char intrebare[255];
	char raspuns[255];
 	strcpy(intrebare,extrage_rand_formatat_baza_de_date(prepared_statement));

	while(1){

	while (strcmp(intrebare,"\0") != 0)
	{


	pthread_mutex_lock(&winner_mutex);


	if(stop_condition !=0)
	{

	sprintf(mesaj_final,"Jocul s-a incheiat! Castigator: Jucatorul %d .\n Ati obtinut %d puncte.\n Felicitari!",winner_id,score);
         /* transmitem scorul clientului */
	if(errno !=-1){
	if (write (tdL.cl,mesaj_final, strlen(mesaj_final)+1) <= 0)
		{
			printf("[Thread %d] deconectat.\n",tdL.idThread);
			errno=-1;
		}
	}

	}

	pthread_mutex_unlock(&winner_mutex);
	char raspuns_corect = intrebare[strlen(intrebare)-2];
	intrebare[strlen(intrebare)-2]='\0';


         /* transmitem intrebarea clientului */
	 if (write (tdL.cl,intrebare, strlen(intrebare)+1) <= 0)
		{
		 printf("[Thread %d] deconectat ",tdL.idThread);
		 errno=-1;
		 break;
		}
	else
	{
		printf ("[Thread %d]Mesaj transmis cu succes.\n",tdL.idThread);
	}

	//reinitalizam timpul maxim permis
	tv.tv_sec=5;
	tv.tv_usec=0;
  	//reinitializam lista de fd
  	FD_ZERO(&file_descriptors_set);
	FD_SET(tdL.cl,&file_descriptors_set);
	errno = select(tdL.cl+1,&file_descriptors_set,NULL,NULL,&tv);
	if (errno == -1)
	{
		  printf("Eroare la select");
		  break;
	}
	if(errno==0)
	{

		  ratio = 0;
	}
	else
	{
		  ratio = 1;
	}
	//reinitalizam timpul maxim permis
	if (read (tdL.cl, raspuns,255) <= 0)
	{
		  printf("[Thread %d] deconectat.\n",tdL.idThread);

		  errno=-1;
		  break;
	}
	printf ("[Thread %d]Mesaj receptionat: %s\n",tdL.idThread,raspuns);
	// verificam daca participantul a dat un raspuns corect
	// convertim raspunsul clientului la litera mica si comparam cu valoarea obtinuta de la baza de date
	if( raspuns_corect == tolower(raspuns[0]))
	{
		score+=question_value*ratio;
	}
	strcpy(intrebare,extrage_rand_formatat_baza_de_date(prepared_statement));

	}
	break;
	}

	pthread_mutex_lock(&winner_mutex);

 	stop_condition = stop_condition+1;

	winner_id = tdL.idThread;

	sprintf(mesaj_final,"Jocul s-a incheiat! Castigator: Jucatorul %d .\nAti obtinut %d puncte.\nFelicitari!",winner_id,score);
         /* transmitem scorul clientului */
	if(errno !=-1){
	if (write (tdL.cl,mesaj_final, strlen(mesaj_final)+1) <= 0)
		{
			printf("[Thread %d] deconectat.\n",tdL.idThread);
			errno=-1;
		}
	}
	pthread_mutex_unlock(&winner_mutex);


  cleanup(database_handle,prepared_statement);

}

