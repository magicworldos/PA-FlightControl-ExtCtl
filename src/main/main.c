/*
 * main.c
 *
 *  Created on: Jul 5, 2018
 *      Author: lidq
 */

#include <main.h>

int main(int argc, char *argv[])
{
	printf("[PA3] Start...\n");

	orb_main(argc, argv);

	extctl_main(argc, argv);

	commander_main(argc, argv);

	while (1)
	{
		sleep(1);
	}

	return 0;
}
