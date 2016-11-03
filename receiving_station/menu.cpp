#include "rs232.h"
#include "menu.h"

void menu(int sample_sec, int record_time)
{
	printf("Welcome to Receiving Station XQ-197!\n");
	printf("Please choose one the follow option:\n");
	printf("1 - Receiving Mode\n");
	printf("2 - Security Settings\n");
	printf("3 - Exit\n");
	printf("\n\nParameters: sample second = %d, record second = %d\n", sample_sec, record_time);
}