#pragma once

typedef enum {
	TEXT,
	AUDIO
} MSG_TYPE;

int get_rs232msg(void *buff, int buff_sz);
MSG_TYPE interpretor(void *buff, int buff_sz);