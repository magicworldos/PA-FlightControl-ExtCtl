/*
 * main.c
 *
 *  Created on: Jul 5, 2018
 *      Author: lidq
 */

#include "main.h"

int main(int argc, char *argv[])
{
	printf("PA3-Extctl\n");

	extctl_start(NULL);

	while (1)
	{
		sleep(1);
	}

	return 0;
}