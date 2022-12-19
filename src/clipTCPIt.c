/* cliTCPIt.c - Exemplu de client TCP
   Trimite un numar la server; primeste de la server numarul incrementat.

   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

/* nume utilizator*/
char username[32];

/*raspunsul care va fi trimis la server*/
char raspuns[16];

void citeste_afiseaza_mesaj_bun_venit(int sd)
{

  // mesajul trimis
  char mesaj[512];
  /* citirea mesajului de bun venit */
  if (read (sd, mesaj,512) < 0)
    {
      perror("[client]Eroare la read() de la server.\n");
    }
  printf("%s",mesaj);


}

void introducere_nume_utilizator(int sd)
{

   printf("%s","\nIntroduceti un nume de utilizator: ");
   scanf("%s",username);

}

int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare

  char intrebare[255];

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);

  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }

  citeste_afiseaza_mesaj_bun_venit(sd);

  introducere_nume_utilizator(sd);

  while(1)
  {
  /* citirea variantei de raspuns introdusa de client */
  read (sd, intrebare, 255);

  printf("%s\n",intrebare);

  printf("Introduceti raspunsul dumneavoastra:\n[%s]: ",username);
  fflush (stdout);
  scanf("%s",raspuns);
  /* trimiterea mesajului la server */
  if (write (sd,raspuns,strlen(raspuns)+1) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }

//  /* inchidem conexiunea, am terminat */
//  close (sd);
  }
}

