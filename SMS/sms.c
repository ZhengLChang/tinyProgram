#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include <assert.h>
#include <time.h>
#include "sms.h"

#define CFG_FILEPATH_FOR_USER "./atcomConfig.db"

#define DB_TABLE_NAME "sms"
static sqlite3 *sms_db;
static short_message_data_t g_sms_data;

static int free_sms_data();
static void array_append( void **array,
	      unsigned elem_size,
	      unsigned *p_used,
	      unsigned *p_size,
	      const void *value)
{
	int i = 0;
	assert(array != NULL && p_used != NULL && p_size != NULL);
	if(*p_size == 0)
	{
		*p_size += 5;
		*array = malloc(5 * elem_size);
		assert(*array);
	}
	else if(*p_used == *p_size)
	{
		*p_size += 5;
		*array = realloc(*array, elem_size * (*p_size));
		assert(*array);
	}
	memcpy(*array + elem_size * (*p_used) ++, value, elem_size);
	return;
}

static int sqlite_get_single_select_callback(void *data, int argc, char **argv, char **ColName)
{
	int i = 0;
	message_format_t *message = data;
	if(message == NULL || message->index != 0)
	{
		return 0;
	}
	for(i = 0; i < argc; i++)
	{
		if(!strcasecmp(ColName[i], "smsindex"))
		{
			message->index = atoi(argv[i]);
		}
		else if(!strcasecmp(ColName[i], "smstype"))
		{
			message->type = atoi(argv[i]);
		}
		else if(!strcasecmp(ColName[i], "smssendto"))
		{
			strncpy(message->sendTo, argv[i], sizeof(message->sendTo));
		}
		else if(!strcasecmp(ColName[i], "smssendfrom"))
		{
			strncpy(message->sendFrom, argv[i], sizeof(message->sendFrom));
		}
		else if(!strcasecmp(ColName[i], "smsmessage"))
		{
			strncpy(message->message, argv[i], sizeof(message->message));
		}
		else if(!strcasecmp(ColName[i], "smsdonetime"))
		{
			message->doneTime = atol(argv[i]);
		}

	}
	return 0;
}

static int sqlite_exec_callback(void *data, int argc, char **argv, char **ColName)
{
	int i = 0;
	message_format_t message;
	for(i = 0; i < argc; i++)
	{
		if(!strcasecmp(ColName[i], "smsindex"))
		{
			message.index = atoi(argv[i]);
		}
		else if(!strcasecmp(ColName[i], "smstype"))
		{
			message.type = atoi(argv[i]);
		}
		else if(!strcasecmp(ColName[i], "smssendto"))
		{
			strncpy(message.sendTo, argv[i], sizeof(message.sendTo));
		}
		else if(!strcasecmp(ColName[i], "smssendfrom"))
		{
			strncpy(message.sendFrom, argv[i], sizeof(message.sendFrom));
		}
		else if(!strcasecmp(ColName[i], "smsmessage"))
		{
			strncpy(message.message, argv[i], sizeof(message.message));
		}
		else if(!strcasecmp(ColName[i], "smsdonetime"))
		{
			message.doneTime = atol(argv[i]);
		}
	}
	switch(message.type)
	{
		case SHORT_MESSAGE_IN:
			array_append((void **)&g_sms_data.inBox,
					sizeof(message_format_t),
			  		&g_sms_data.inBoxCnt,
			  		&g_sms_data.inBoxSize,
			  		(const void *)&message);
			break;
		case SHORT_MESSAGE_SENT:
			array_append((void **)&g_sms_data.sentBox,
					sizeof(message_format_t),
			  		&g_sms_data.sentBoxCnt,
			  		&g_sms_data.sentBoxSize,
			  		(const void *)&message);
			break;
		case SHORT_MESSAGE_SENDING:
			array_append((void **)&g_sms_data.sendingBox,
					sizeof(message_format_t),
			  		&g_sms_data.sendingBoxCnt,
			  		&g_sms_data.sendingBoxSize,
			  		(const void *)&message);
			break;
		case SHORT_MESSAGE_DRAFT:
			array_append((void **)&g_sms_data.draftBox,
					sizeof(message_format_t),
			  		&g_sms_data.draftBoxCnt,
			  		&g_sms_data.draftBoxSize,
			  		(const void *)&message);
			break;
	}
	return 0;
}

short_message_data_t* cfg_get_sms_config()
{
	return &g_sms_data;
}

static int _get_sms_config()
{
	char sql[512] = "";
	int retSqlite = SQLITE_OK;
	char *dbErrMsg = 0;
	int n = 0;
	n = snprintf(sql, sizeof(sql), "select * from %s order by smsdonetime asc", 
				DB_TABLE_NAME);
	if(n > sizeof(sql))
	{
		fprintf(stderr, "SMS query too big!!!\n");
		return -1;
	}

	if((retSqlite = sqlite3_exec(sms_db, sql, sqlite_exec_callback, 0, &dbErrMsg)) != SQLITE_OK)	
	{
		fprintf(stderr, "Insert SQL error: %s\n", dbErrMsg);
		sqlite3_free(dbErrMsg);
		return -1;
	}
	return 0;
}	

int load_sms_config()
{
	int retSqlite = SQLITE_OK;
	char *dbErrMsg = 0;
	char *sql = "create table if not exists sms("\
		"smsindex integer primary key autoincrement,"
		"smstype int not null,"\
		"smssendto char(16),"\
		"smssendfrom char(16),"\
		"smsmessage char(256),"\
		"smsdonetime int"\
		");";
	
	if((retSqlite = sqlite3_open(CFG_FILEPATH_FOR_USER, &sms_db)) != SQLITE_OK)
	{
		fprintf(stderr, "open %s error: %s\n", CFG_FILEPATH_FOR_USER, sqlite3_errmsg(sms_db));
		return -1;
	}
	
	if((retSqlite = sqlite3_exec(sms_db, sql, NULL, 0, &dbErrMsg)) != SQLITE_OK)	
	{
		fprintf(stderr, "SQL error: %s\n", dbErrMsg);
		sqlite3_free(dbErrMsg);
	}
	_get_sms_config();
	return 0;	
}

static int free_sms_data()
{
	if(NULL != g_sms_data.inBox)
	{
		free(g_sms_data.inBox);
		g_sms_data.inBox = NULL;
		g_sms_data.inBoxCnt = 0;
		g_sms_data.inBoxSize = 0;
	}
	if(NULL != g_sms_data.sentBox)
	{
		free(g_sms_data.sentBox);
		g_sms_data.sentBox = NULL;
		g_sms_data.sentBoxCnt = 0;
		g_sms_data.sentBoxSize = 0;
	}
	if(NULL != g_sms_data.sendingBox)
	{
		free(g_sms_data.sendingBox);
		g_sms_data.sendingBox = NULL;
		g_sms_data.sendingBoxCnt = 0;
		g_sms_data.sendingBoxSize = 0;
	}
	if(NULL != g_sms_data.draftBox)
	{
		free(g_sms_data.draftBox);
		g_sms_data.draftBox = NULL;
		g_sms_data.draftBoxCnt = 0;
		g_sms_data.draftBoxSize = 0;
	}
	return 0;
}

int unload_sms_config()
{
	free_sms_data();
	sqlite3_close(sms_db);
	return 0;	
}

static int _add_sms_conf(message_format_t *message)
{
	char sql[512] = "";
	char *dbErrMsg = 0;
	int n = 0;
	int retSqlite = SQLITE_OK;
	n = snprintf(sql, sizeof(sql), "insert into %s(smstype, smssendto, smssendfrom, smsmessage, smsdonetime) values(%d, \"%s\", \"%s\", \"%s\", %ld);", 
			DB_TABLE_NAME,
			message->type, message->sendTo, message->sendFrom,
			message->message, message->doneTime);
	if(n > sizeof(sql))
	{
		fprintf(stderr, "%s %d, SQL too big: %s", __func__, __LINE__, sql);
		return -1;
	}
	if((retSqlite = sqlite3_exec(sms_db, sql, NULL, 0, &dbErrMsg)) != SQLITE_OK)	
	{
		fprintf(stderr, "Insert SQL error: %s\n", dbErrMsg);
		sqlite3_free(dbErrMsg);
		return -1;
	}
	return 0;
}

static int _update_sms_conf(message_format_t *message)
{
	char sql[512] = "";
	char *dbErrMsg = 0;
	int n = 0;
	int retSqlite = SQLITE_OK;
	n = snprintf(sql, sizeof(sql), "update %s set smstype=%d,smssendto=\"%s\",smssendfrom=\"%s\",smsmessage=\"%s\",smsdonetime=%ld where smsindex=%d;",
			DB_TABLE_NAME,
			message->type, message->sendTo, message->sendFrom,
			message->message, message->doneTime, message->index);
	if(n > sizeof(sql))
	{
		fprintf(stderr, "%s %d, SQL too big: %s", __func__, __LINE__, sql);
		return -1;
	}
	if((retSqlite = sqlite3_exec(sms_db, sql, NULL, 0, &dbErrMsg)) != SQLITE_OK)	
	{
		fprintf(stderr, "Insert SQL error: %s\n", dbErrMsg);
		sqlite3_free(dbErrMsg);
		return -1;
	}
	return 0;
}


int add_sms_config(message_format_t *message)
{
	char sql[512] = "";
	char *dbErrMsg = 0;
	int n = 0;
	int retSqlite = SQLITE_OK;
	if(message == NULL || message->type >= SHORT_MESSAGE_CNT)
	{
		fprintf(stderr, "SMS syntax Error\n");
		return -1;
	}	

	if(message->index > 0)
	{
		message_format_t mes_tmp;
		memset(&mes_tmp, 0, sizeof(mes_tmp));
		n = snprintf(sql, sizeof(sql), "select * from %s where smsindex = %d;",
				DB_TABLE_NAME,
				message->index);
		if(n > sizeof(sql))
		{
			fprintf(stderr, "SMS message too big, Can't store\n");
			return -1;
		}
		if((retSqlite = sqlite3_exec(sms_db, sql, sqlite_get_single_select_callback, &mes_tmp, &dbErrMsg)) != SQLITE_OK)	
		{
			fprintf(stderr, "%s %d, SQL too big: %s", __func__, __LINE__, sql);
			sqlite3_free(dbErrMsg);
			return -1;
		}
		if(mes_tmp.index > 0)
		{
			_update_sms_conf(message);
		}
		else
		{
			_add_sms_conf(message);
		}
	}
	else
	{
		_add_sms_conf(message);
	}
	free_sms_data();
	_get_sms_config();
	return 0;
}

int delete_sms_config_by_index(int index)
{
	char sql[512] = "";
	char *dbErrMsg = 0;
	int n = 0;
	int retSqlite = SQLITE_OK;
	if(index < 0)
	{
		fprintf(stderr, "%s %d, index is negative\n", __func__, __LINE__);
		return -1;
	}
	n = snprintf(sql, sizeof(sql), "delete from %s where smsindex = %d;", 
				DB_TABLE_NAME,
				index);
	if(n > sizeof(sql))
	{
		fprintf(stderr, "SMS message too big, Can't store\n");
		return -1;
	}
	if((retSqlite = sqlite3_exec(sms_db, sql, NULL, 0, &dbErrMsg)) != SQLITE_OK)	
	{
		fprintf(stderr, "Delete SQL error: %s\n", dbErrMsg);
		sqlite3_free(dbErrMsg);
		return -1;
	}
	free_sms_data();
	_get_sms_config();
	return 0;
}

int empty_sms_config_by_type(int type)
{
	char sql[512] = "";
	char *dbErrMsg = 0;
	int n = 0;
	int retSqlite = SQLITE_OK;
	if(index < 0)
	{
		fprintf(stderr, "%s %d, index is negative\n", __func__, __LINE__);
		return -1;
	}
	n = snprintf(sql, sizeof(sql), "delete from %s where smstype = %d;", 
				DB_TABLE_NAME,
				type);
	if(n > sizeof(sql))
	{
		fprintf(stderr, "SMS message too big, Can't store\n");
		return -1;
	}
	if((retSqlite = sqlite3_exec(sms_db, sql, NULL, 0, &dbErrMsg)) != SQLITE_OK)	
	{
		fprintf(stderr, "Delete SQL error: %s\n", dbErrMsg);
		sqlite3_free(dbErrMsg);
		return -1;
	}
	free_sms_data();
	_get_sms_config();
	return 0;
}

int reset_sms_config()
{
	char sql[512] = "";
	char *dbErrMsg = 0;
	int n = 0;
	int retSqlite = SQLITE_OK;
	n = snprintf(sql, sizeof(sql), "drop database %s;", 
				DB_TABLE_NAME);
	if(n > sizeof(sql))
	{
		fprintf(stderr, "SMS message too big, Can't store\n");
		return -1;
	}
	if((retSqlite = sqlite3_exec(sms_db, sql, NULL, 0, &dbErrMsg)) != SQLITE_OK)	
	{
		fprintf(stderr, "Drop SQL error: %s\n", dbErrMsg);
		sqlite3_free(dbErrMsg);
		return -1;
	}
	return 0;
}

void print_box_data()
{
	int i = 0;
	fprintf(stderr, "--------------------------------\n");
	for(i = 0; i < g_sms_data.inBoxCnt; i++)
	{
		fprintf(stderr, "%5d %5d %10s %10s %20s %ld\n", g_sms_data.inBox[i].index,
				g_sms_data.inBox[i].type,
				g_sms_data.inBox[i].sendTo,
				g_sms_data.inBox[i].sendFrom,
				g_sms_data.inBox[i].message,
				g_sms_data.inBox[i].doneTime);
	}
	for(i = 0; i < g_sms_data.sentBoxCnt; i++)
	{
		fprintf(stderr, "%5d %5d %10s %10s %20s %ld\n", g_sms_data.sentBox[i].index,
				g_sms_data.sentBox[i].type,
				g_sms_data.sentBox[i].sendTo,
				g_sms_data.sentBox[i].sendFrom,
				g_sms_data.sentBox[i].message,
				g_sms_data.sentBox[i].doneTime);
	}
	for(i = 0; i < g_sms_data.sendingBoxCnt; i++)
	{
		fprintf(stderr, "%5d %5d %10s %10s %20s %ld\n", g_sms_data.sendingBox[i].index,
				g_sms_data.sendingBox[i].type,
				g_sms_data.sendingBox[i].sendTo,
				g_sms_data.sendingBox[i].sendFrom,
				g_sms_data.sendingBox[i].message,
				g_sms_data.sendingBox[i].doneTime);
	}
	for(i = 0; i < g_sms_data.draftBoxCnt; i++)
	{
		fprintf(stderr, "%5d %5d %10s %10s %20s %ld\n", g_sms_data.draftBox[i].index,
				g_sms_data.draftBox[i].type,
				g_sms_data.draftBox[i].sendTo,
				g_sms_data.draftBox[i].sendFrom,
				g_sms_data.draftBox[i].message,
				g_sms_data.draftBox[i].doneTime);
	}
	fprintf(stderr, "--------------------------------\n");
	return;
}
int main()
{
	message_format_t message;
	load_sms_config();

	print_box_data();
	empty_sms_config_by_type(SHORT_MESSAGE_IN);
	empty_sms_config_by_type(SHORT_MESSAGE_SENT);
	empty_sms_config_by_type(SHORT_MESSAGE_SENDING);
	empty_sms_config_by_type(SHORT_MESSAGE_DRAFT);
	print_box_data();

	message.type = SHORT_MESSAGE_IN;
	strncpy(message.sendTo, "111", sizeof(message.sendTo));
	strncpy(message.sendFrom, "120", sizeof(message.sendFrom));
	strncpy(message.message, "sms test", sizeof(message.sendFrom));
	message.doneTime = time(NULL);
	add_sms_config(&message);
	print_box_data();

	sleep(2);
	message.type = SHORT_MESSAGE_SENT;
	strncpy(message.sendTo, "111", sizeof(message.sendTo));
	strncpy(message.sendFrom, "120", sizeof(message.sendFrom));
	strncpy(message.message, "sms test", sizeof(message.sendFrom));
	message.doneTime = time(NULL);
	add_sms_config(&message);
	add_sms_config(&message);
	print_box_data();

	sleep(2);
	message.type = SHORT_MESSAGE_SENDING;
	strncpy(message.sendTo, "111", sizeof(message.sendTo));
	strncpy(message.sendFrom, "120", sizeof(message.sendFrom));
	strncpy(message.message, "sms test", sizeof(message.sendFrom));
	message.doneTime = time(NULL);
	add_sms_config(&message);
	print_box_data();

	sleep(2);
	message.type = SHORT_MESSAGE_DRAFT;
	strncpy(message.sendTo, "111", sizeof(message.sendTo));
	strncpy(message.sendFrom, "120", sizeof(message.sendFrom));
	strncpy(message.message, "sms test", sizeof(message.sendFrom));
	message.doneTime = time(NULL);
	add_sms_config(&message);
	print_box_data();

	unload_sms_config();
	return 0;
}












