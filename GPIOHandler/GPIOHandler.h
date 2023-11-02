#ifndef GPIOHANDLER_H
#define GPIOHANDLER_H

#define GPIOHANDLER_VERSION_MAJOR   0
#define GPIOHANDLER_VERSION_MINOR   2
#define GPIOHANDLER_VERSION_FIX     0

#ifdef _WIN32
	typedef struct _hw
	{
		char comport[5];
		int* pins;
		int direction;
	}hw;

	#define refStruct hw
	//puts("The GPIO Handling has not implemented yet on Windows.");

#elif __linux__
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

#define refStruct struct gpiohandle_request

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define ON 1
#define OFF 0

#define DEV_NAME "/dev/gpiochip0"
#define INPUT 0
#define OUTPUT 1

int initPIN(int pinNr);
int setupPIN(int pinNr, char *mode);
int writePIN(int pinNr, int value);
int readPIN(int pinNr);
int deinitPIN(int pinNr);

int initGPIO(refStruct* rq, int pins[], int nrOfPins, int direction);
int writeGPIO(refStruct* rq, int* values);
int readGPIO(refStruct* rq, int* values);
int pollGPIO(int offset);
int detectButtonAction(refStruct* rq, int* values, int msdelay);
int closeGPIO(refStruct* rq);

#endif