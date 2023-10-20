// Compiling: gcc GPIOHandler.c -o Builds/GPIOHandler
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "GPIOHandler.h"

int initPIN(int pinNr)
{
    #ifdef _WIN32
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__

    char initPin[33];
    FILE *export;
    export = fopen("/sys/class/gpio/export", "w");

    if (export == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(export, "%d", pinNr);
    fclose(export);

    sleep(1);

    #endif

    return 0;
}

int setupPIN(int pinNr, char *mode)
{
    #ifdef _WIN32
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__

    char setupPin[50];
    FILE *direction;
    sprintf(setupPin, "/sys/class/gpio/gpio%d/direction", pinNr);
    direction = fopen(setupPin, "w");

    if (direction == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(direction, "%s", mode);
    fclose(direction);
    
    #endif

    return 0;
}

int writePIN(int pinNr, int value)
{
    #ifdef _WIN32
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__

    char pinPath[50];
    FILE *writePin;
    sprintf(pinPath, "/sys/class/gpio/gpio%d/value", pinNr);
    writePin = fopen(pinPath, "w");

    if (writePin == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(writePin, "%d", value);
    fclose(writePin);
    
    #endif

    return 0;
}

int readPIN(int pinNr)
{

    
    #ifdef _WIN32
    int value;
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__

    int value;
    char pinPath[50];
    FILE *readPin;
    sprintf(pinPath, "/sys/class/gpio/gpio%d/value", pinNr);
    
    readPin = fopen(pinPath, "r");
    
    if (readPin == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fscanf(readPin, "%d", &value);
    
    fclose(readPin);
    
    #endif

    return value;
}

int deinitPIN(int pinNr)
{
    #ifdef _WIN32
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__

    char deinitPin[35];
    FILE *unexport;
    unexport = fopen("/sys/class/gpio/unexport", "w");

    if (unexport == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(unexport, "%d", pinNr);
    fclose(unexport);

    #endif

    return 0;
}