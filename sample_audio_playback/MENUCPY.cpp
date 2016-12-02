char input[MAX];
char *str1;
char check[MAX];
int checker;
OPTIONS options;
//gets_s(input);


while (1) {
	gets_s(input);
	for (int i = 0; input[i]; i++) {
		input[i] = tolower(input[i]);
	}
	str1 = strtok(input, " -");
	if (strcmp("help", str1) == 0)
	{
		str1 = strtok(NULL, " -");
		if (str1 == NULL)
			options = HELP;
		else if (strcmp("compose", str1) == 0)
			options = HELPCOMPOSE;
		else if (strcmp("display", str1) == 0)
			options = HELPDISPLAY;
		else if (strcmp("playback", str1) == 0)
			options = HELPPLAYBACK;
		else if (strcmp("receive", str1) == 0)
			options = HELPRECEIVE;
		else if (strcmp("record", str1) == 0)
			options = HELPRECORD;
		else if (strcmp("select", str1) == 0)
			options = HELPSELECT;
		else if (strcmp("settings", str1) == 0)
			options = HELPSETTINGS;
		else if (strcmp("remove", str1) == 0)
			options = HELPREMOVE;
	}
	else if (strcmp("compose", str1) == 0)
		options = COMPOSE;
	else if (strcmp("display", str1) == 0)
		options = DISPLAY;
	else if (strcmp("playback", str1) == 0)
		options = PLAYBACK;
	else if (strcmp("receive", str1) == 0)
		options = RECEIVE;
	else if (strcmp("record", str1) == 0)
		options = RECORD;
	else if (strcmp("select", str1) == 0)
		options = SELECT;
	else if (strcmp("settings", str1) == 0)
		options = SETTINGS;
	else if (strcmp("remove", str1) == 0)
		options = REMOVE;
	else
		printf("invlaid input. Defaulting to help\n");
	options = HELP;
}

switch (options)
{
case HELP:
	helpmenu();
	break;
case HELPCOMPOSE:
	helpcompose();
	break;
case HELPDISPLAY:
	helpdisplay();
	break;
case HELPPLAYBACK:
	helpplayback();
	break;
case HELPRECEIVE:
	helpreceive();
	break;
case HELPRECORD:
	helprecord();
	break;
case HELPSELECT:
	helpselect();
	break;
case HELPSETTINGS:
	helpsettings();
	break;
case HELPREMOVE:
	helpremove();
	break;
case COMPOSE:
	composeMsg();
	break;
case DISPLAY:
	displayMsg();
	break;
case PLAYBACK:
	playback();
	break;
case RECEIVE:
	receive();
	break;
case RECORD:
	record();
	break;
case SELECT:
	select();
	break;
case SETTINGS:
	settings();
	break;
case REMOVE:
	remove();
	break;
}