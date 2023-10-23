#ifndef PLAYMUSIC_H
#define PLAYMUSIC_H

#define PLAYMUSIC_VERSION_MAJOR   0
#define PLAYMUSIC_VERSION_MINOR   2
#define PLAYMUSIC_VERSION_FIX     1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
	#include <windows.h>
    #include <mmsystem.h>

#elif __linux__
	#include <ao/ao.h>
    #include <mpg123.h>
    #define BITS 8

#else
    printf("Sorry, the system are not implemented yet... :'(\n")

#endif

int initMusic();
int playMusic(char *path);
int closeMusic();

#endif