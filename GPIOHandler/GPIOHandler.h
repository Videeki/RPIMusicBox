#ifndef GPIOHANDLER_H
#define GPIOHANDLER_H

#define GPIOHANDLER_VERSION_MAJOR   0
#define GPIOHANDLER_VERSION_MINOR   2
#define GPIOHANDLER_VERSION_PATCH	1

#ifdef _WIN32
	typedef struct _hw
	{
		char comport[5];
		int direction;
		int lines;
		int* pins;
	}hw;

	#define GPIO hw
	#define DEV_NAME "ExternalDevice"

#elif __linux__
	#include <linux/gpio.h>
	#include <sys/ioctl.h>
	#include <sys/poll.h>

	#define GPIO struct gpiohandle_request
	#define DEV_NAME "/dev/gpiochip0"

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>

#define ON 1
#define OFF 0

#define INPUT 0
#define OUTPUT 1

int initGPIO(GPIO* rq, int pins[], int nrOfPins, int direction);
int writeGPIO(GPIO* rq, int* values);
int readGPIO(GPIO* rq, int* values);
int pollGPIO(int offset);
int detectButtonAction(GPIO* rq, int msdelay);
int closeGPIO(GPIO* rq);

int initPIN(int pinNr);
int setupPIN(int pinNr, char *mode);
int writePIN(int pinNr, int value);
int readPIN(int pinNr);
int deinitPIN(int pinNr);

#endif	/* GPIOHANDLER_H */