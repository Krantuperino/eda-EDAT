#include "table.h"
#include "odbc.h"
#include "index.h"

int main(int argc, char const *argv[])
{
	SQLHENV env;
	SQLHDBC dbc;
	SQLHSTMT stmt;
	SQLRETURN ret;
	index_t *nEw_iNdEx = NULL;
	int score, i, pos, escore;
	table_t * table;
	long long int uid;
	long ** j;
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

	i = index_create(0);
	if (i == -1) {
		table_close(table);
		return -1;
	}

	nEw_iNdEx = index_open("index.dat");
	if (!nEw_iNdEx) {
		table_close(table);
		return -1;
	}

	for(i = table_first_pos(table); i < table_last_pos(table); ){

		pos = i;
		i = table_read_record(table, i);

		escore = *(int*)table_column_get(table, 2);

		if(!escore)
			return -1;

		index_put(nEw_iNdEx, escore, pos);
	}

	/*Connect to ODBC*/
	ret = odbc_connect(&env, &dbc);
	if(!SQL_SUCCEEDED(ret)) {
		return EXIT_FAILURE;
	}

	/* Allocate Handle */
	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

	for(i = score; i < 101; i++)
	{
		j = index_get(nEw_iNdEx, i,  &pos);

		for(;pos>0; pos--)
		{
			table_read_record(table, *j[pos-1]);
			uid = *(long long int *) table_column_get(table, 0);
			printf("\n\n%s \t %d \n %s \n\n",
				(char *) table_column_get(table, 1),
				*(int *) table_column_get(table, 2),
				(char *) table_column_get(table, 3)
			);


			SQLCloseCursor(stmt);

			sprintf(query, "SELECT tweettext FROM tweets WHERE userwriter=%lld ORDER BY tweettimestamp LIMIT 7", uid);

			SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

			while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
				ret = SQLGetData(stmt, 1, SQL_C_CHAR, aidd, sizeof(aidd), NULL);
				printf("%s\n", aidd);
			}
		}
	}

	return 0;
}
