#include "rs232.h"
#include "rcv_station.h"

int get_rs232msg(void *buff, int buff_sz)
{
	initPort();
	inputFromPort(buff, buff_sz);
}

MSG_TYPE interpretor(void *buff, int buff_sz)
{
	
}
