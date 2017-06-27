#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>

static int callback(void *data, int argc, char **argv, char **azColName)
{
	int i;
	fprintf(stderr, "%s: ", (const char *)data);
	for(i = 0; i < argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i]?argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int main(int argc, char **argv)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;
	const char *data = "Callback function called";

	rc = sqlite3_open("testDB.db", &db);
	if(rc)
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}	
	else
	{
		fprintf(stderr, "Opened database successfully\n");
	}
	sql = "UPDATE COMPANY set SALARY = 25000.00 where ID = 1;"\
		"SELECT * FROM COMPANY";
	rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "Operation done successfully\n");
	}
	sqlite3_close(db);
	return 0;
}











