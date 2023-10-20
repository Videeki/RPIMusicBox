#ifndef GPIOHANDLER_H
#define GPIOHANDLER_H

#define GPIOHANDLER_VERSION_MAJOR   0
#define GPIOHANDLER_VERSION_MINOR   0
#define GPIOHANDLER_VERSION_FIX     0

#define ON 1
#define OFF 0

int initPIN(int pinNr);
int setupPIN(int pinNr, char *mode);
int writePIN(int pinNr, int value);
int readPIN(int pinNr);
int deinitPIN(int pinNr);

#endif