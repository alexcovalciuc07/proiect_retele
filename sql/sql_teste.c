#include <stdio.h>
#include "sqlite3.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>

extern int errno;
const char* db_path="database.db";
const char* query = "SELECT * FROM quiz";

/*
Schema bazei de date:
sqlite> pragma table_info('quiz');
0|id|INTEGER|0||1
1|question_text|varchar(255)|0||0
2|answer_1|varchar(127)|0||0
3|answer_2|varchar(127)|0||0
4|answer_3|varchar(127)|0||0
5|answer_4|varchar(127)|0||0
6|correct_answer|varchar(3)|0||0
*/
char* extrage_rand_formatat_baza_de_date(sqlite3_stmt* prepared_statement)
{


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

sqlite3* init_sql_db_connection(const char* db_path){

	// Creem handle-ul catre baza de date
	sqlite3* database_handle;
	// Deschidem conexiunea cu baza de date
	errno = sqlite3_open(db_path,&database_handle);
	if (errno !=0)
	{
		printf("[database]Eroare la conectarea cu baza de date, Cod: %d \n",errno);
	}

	return database_handle;
}

sqlite3_stmt* create_prepared_statement(sqlite3* database_handle, const char* query){

	sqlite3_stmt* prepared_statement;

	errno = sqlite3_prepare(database_handle,query,strlen(query),&prepared_statement,NULL);
	if (errno !=0)
	{
		printf("[database]Eroare creere prepared statement, Cod: %d \n",errno);

	}
	return prepared_statement;

}
void cleanup(sqlite3* database_handle, sqlite3_stmt* prepared_statement){

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


int main()
{
	sqlite3* database_handle = init_sql_db_connection(db_path);

	sqlite3_stmt* prepared_statement = create_prepared_statement(database_handle,query);

	char row[255];

	strcpy(row,extrage_rand_formatat_baza_de_date(prepared_statement));

	while(strcmp(row,"\0") != 0)
	{

		printf("%s\n",row);
		strcpy(row,extrage_rand_formatat_baza_de_date(prepared_statement));
	}


	cleanup(database_handle,prepared_statement);
return 0;
}

