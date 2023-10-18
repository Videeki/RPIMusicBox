#ifndef PLAYMUSIC_H
#define PLAYMUSIC_H

#define PLAYMUSIC_VERSION_MAJOR   0
#define PLAYMUSIC_VERSION_MINOR   1
#define PLAYMUSIC_VERSION_FIX     0

int initMusic();
int playMusic(char *path);
int closeMusic();

#endif