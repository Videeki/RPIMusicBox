#ifndef PLAYMUSIC_H
#define PLAYMUSIC_H

#define PLAYMUSIC_VERSION_MAJOR     0
#define PLAYMUSIC_VERSION_MINOR     2
#define PLAYMUSIC_VERSION_PATCH     1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ao/ao.h>
#include <mpg123.h>
#define BITS 8


int initMusic();
int playMusic(const char *path);
int closeMusic();

#endif  /* PLAYMUSIC_H */