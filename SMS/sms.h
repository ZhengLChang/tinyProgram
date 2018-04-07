#ifndef SMS_H_
#define SMS_H_

#define MAX_CNT_SHORT_MESSAGE_IN 5
#define MAX_CNT_SHORT_MESSAGE_UNREAD 5
#define MAX_CNT_SHORT_MESSAGE_SENT 5
#define MAX_CNT_SHORT_MESSAGE_SENDING 5
#define MAX_CNT_SHORT_MESSAGE_DRAFT 3
#include <string.h>
#include <stdlib.h>
enum{
	SHORT_MESSAGE_IN = 0,
	SHORT_MESSAGE_UNREAD,
	SHORT_MESSAGE_SENT,
	SHORT_MESSAGE_SENDING,
	SHORT_MESSAGE_DRAFT,
	SHORT_MESSAGE_CNT,
};

typedef struct message_format_t_
{
	int index;
	int type;
	char sendTo[16];
	char sendFrom[16];
	char message[256];
	long doneTime;
}message_format_t;

typedef struct short_message_data_t_
{
	message_format_t *inBox;
	int inBoxCnt;
	int inBoxSize;
	message_format_t *sentBox;
	int sentBoxCnt;
	int sentBoxSize;
	message_format_t *sendingBox;
	int sendingBoxCnt;
	int sendingBoxSize;
	message_format_t *draftBox;
	int draftBoxCnt;
	int draftBoxSize;
}short_message_data_t;


#endif
