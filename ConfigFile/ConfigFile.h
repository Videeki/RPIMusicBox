#ifndef INIPARSER_H
#define INIPARSER_H

#define INIPARSER_VERSION_MAJOR   0
#define INIPARSER_VERSION_MINOR   2
#define INIPARSER_VERSION_FIX     0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include "pRegex.h"

typedef struct _kvc
{
    char key[255];
    char value[255];
}kvc;

typedef struct _section
{
    char sectionName[255];
    int nrOfkeyvalue;
    kvc keyvalue[255];
}section;

typedef struct _config
{
    int nrOfsections;
    section sections[10];
}config;

char* openConfig(char* iniPath);
int parseConfig(char* configStr, config* parsedConfig);
int closeConfig(char* configStr);

char* readKey(config* parsedConfig, char* section, char* key);
int writeKey(config* parsedConfig, char* section, char* key, char* value);
char** getSectionNames(config* parseConfig);
char** getKeyNames(config* parseConfig, char* section);

void trim(char *str);

#endif /* INIPARSER_H */