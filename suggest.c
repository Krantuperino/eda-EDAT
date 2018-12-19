#include "table.h"
#include "odbc.h"

int main(int argc, char const *argv[])
{
	SQLHENV env;
	SQLHDBC dbc;
	SQLHSTMT stmt;
	SQLRETURN ret;
	int score, i;
	table_t * table;
	long long int uid;
	char query[1024];
	char aidd[512];

	if(argc != 2){
		printf("Wrong parameters, correct use is:\n");
		printf("\t %s <score>\n", argv[0]);
		return -1;
	}
	
	score = atoi(argv[1]);
	if(score < 1 || score > 100){
		printf("Incorrect score, the values are between 1 and 100\n");
		return -1;
	}

	table = table_open("score.dat");

	if(!table){
		printf("You must give a score to a user first\n");
		return -1;
	}

	/*Connect to ODBC*/
	ret = odbc_connect(&env, &dbc);
	if(!SQL_SUCCEEDED(ret)) {
		return EXIT_FAILURE;
	}

	/* Allocate Handle */
	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

	for(i = table_first_pos(table); i < table_last_pos(table); ){
		
		i = table_read_record(table, i);

		if(score <= *(int *) table_column_get(table, 2)){

			uid = *(long long int *) table_column_get(table, 0);
			printf("%s \t %d \n %s \n", 
				(char *) table_column_get(table, 1),
				*(int *) table_column_get(table, 2),
				(char *) table_column_get(table, 3)
			);

			SQLCloseCursor(stmt);

			sprintf(query, "SELECT tweettext FROM tweets WHERE userwriter=%lld ORDER BY tweettimestamp", uid);

			SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

			while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
				ret = SQLGetData(stmt, 1, SQL_C_CHAR, aidd, sizeof(aidd), NULL);
				printf("%s\n", aidd);
			}
		}
	}
	
	return 0;
}
