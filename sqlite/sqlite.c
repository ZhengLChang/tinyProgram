#include <stdio.h>
#include "sqlite3.h"

int main(void)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;

	int retcode;

	if((retcode = sqlite3_open("MyDB", &db)) != SQLITE_OK)
	{
		sqlite3_close(db);
		fprintf(stderr, "Could not open MyDB\n");
		return retcode;
	}
	if((retcode = sqlite3_prepare(db, "select SID from Students order by SID", -1, &stmt, 0))
		!= SQLITE_OK)
	{
		sqlite3_close(db);
		fprintf(stderr, "Could not execute SELECT\n");
		return retcode;
	}
	while(sqlite3_step(stmt) == SQLITE_ROW)
	{
		int i = sqlite3_column_int(stmt, 0);
		printf("SID = %d\n", i);
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return SQLITE_OK;
}
