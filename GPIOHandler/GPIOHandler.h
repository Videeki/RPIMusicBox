#ifndef GPIOHANDLER_H
#define GPIOHANDLER_H

#define GPIOHANDLER_VERSION_MAJOR   0
#define GPIOHANDLER_VERSION_MINOR   2
#define GPIOHANDLER_VERSION_FIX     0

#ifdef _WIN32
	printf("The GPIO Handling has not implemented yet on Windows.\n");

#elif __linux__
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

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

int initGPIO(struct gpiohandle_request* rq, int pins[], int nrOfPins, int direction);
int writeGPIO(struct gpiohandle_request* rq, int* values);
int readGPIO(struct gpiohandle_request* rq, int* values);
int pollGPIO(int offset);
int closeGPIO(struct gpiohandle_request* rq);

#endif