#include "odbc.h"
#include "table.h"

int score, i;

int main(int argc, char const *argv[]) {



  char scrname[100], comment[1024];
  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret;
  char aids[512];
  long int num;
  table_t *table;
  type_t types[4] = {LLNG, STR, INT, STR};
  void * values[4];

  if(argc < 4){
    printf("Wrong parameters, correct use is:\n");
    printf("\t%s <scrname> <score> \"<comment>\"\n", argv[0]);
    return -1;
  }
  else{
    strcpy(scrname, argv[1]);
    score = atoi(argv[2]);
    if(score < 1 || score > 100){
      printf("Invalid score");
      return -1;
    }
    strcpy(comment, "");
    for(i=3; i<argc-1; i++){
          strcat(comment, argv[i]);
          strcat(comment, " ");
        }
        strcat(comment, argv[i]);
  }

  /* CONNECT */
  ret = odbc_connect(&env, &dbc);
  if(!SQL_SUCCEEDED(ret)) {
    return EXIT_FAILURE;
  }

  /* Allocate Handle */
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  /* Preparation */
  sprintf(aids, "SELECT user_id FROM users WHERE screenname='%s'", scrname);

  SQLExecDirect(stmt, (SQLCHAR*) aids, SQL_NTS);

  SQLBindCol(stmt, 1, SQL_C_SBIGINT, &num, sizeof(num), NULL);

  if(!SQL_SUCCEEDED(ret = SQLFetch(stmt))){
    printf("User %s doesnt exist", scrname);
    return -1;
  }

  table = table_open("score.dat");

  if(!table){
    table_create("score.dat", 4, types);
    table = table_open("score.dat");
  }

  values[0] = (void*)&num;
  values[1] = calloc(1, strlen(scrname));
  memcpy(values[1], scrname, strlen(scrname));
  values[2] = (void*)&score;
  values[3] = calloc(1, strlen(comment)+1);
  memcpy(values[3], comment, strlen(comment));

  table_insert_record(table, values);
  table_close(table);
  free(values[1]);
  free(values[3]);

  return 0;
}
