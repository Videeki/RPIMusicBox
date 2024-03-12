#include "ConfigFile.h"

char* openConfig(char* iniPath)
{
    FILE *fp;
    long size;

    fp = fopen(iniPath, "rb"); // read mode

    if (fp == NULL) {
        return "Error while opening the file.";
    }

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char* configStr = calloc(size + 1, sizeof(char));

    if (configStr == NULL) {
        fclose(fp);
        return "Memory allocation failed.";
    }

    fread(configStr, size, 1, fp);
    fclose(fp);

    return configStr;
}


int parseConfig(char* configStr, config* parsedConfig)
{
    int i = 0;
    int j = 0;
    int found = 0;
    char shiftRegister[strlen(configStr)];

    strcpy(shiftRegister, configStr);
    do
    {
        char before[255] = "";
        char match[255] = "";
        char after[255] = "";
        found = matchPattern(shiftRegister, "\n", before, match, after);

        if(before[0] == '[')
        {
            strncpy(parsedConfig->sections[i].sectionName, before + 1, strlen(before) - 2);
            parsedConfig->sections[i].sectionName[strlen(before) - 2] = '\0';

            i++;
            parsedConfig->nrOfsections = i;
            j = 0;
        }
        else
        {
            char key[255] = "";
            char value[255] = "";
            
            if(0 == matchPattern(before, "=", key, match, value))
            {
                trim(key);
                trim(value);
                strcpy(parsedConfig->sections[i - 1].keyvalue[j].key, key);
                strcpy(parsedConfig->sections[i - 1].keyvalue[j].value, value);

                j++;
                parsedConfig->sections[i - 1].nrOfkeyvalue = j;
            }
        }
        strcpy(shiftRegister, after);
    
    }while(strlen(shiftRegister));

    return 0;
}


int closeConfig(char* configStr)
{
    free(configStr);
  
    return 0;
}


char* readKey(config* parsedConfig, char* section, char* key)
{
    int i = 0;
    int j = 0;
    int runSection;
    int runKey;
  
    do
    {
        runSection = strcmp(parsedConfig->sections[i].sectionName, section);
        if(runSection == 0)
        {
            do
            {
                runKey = strcmp(parsedConfig->sections[i].keyvalue[j].key, key);
                j++;
            }while(runKey && j < parsedConfig->sections[i].nrOfkeyvalue);
        }
        i++;
    }while(runSection && i < parsedConfig->nrOfsections);

    return parsedConfig->sections[i - 1].keyvalue[j - 1].value;
}


int writeKey(config* parsedConfig, char* section, char* key, char* value)
{
    return 0;
}


void trim(char *str)
{
    // Find the first non-whitespace character from the left
    int start = 0;
    while (isspace(str[start]))
    {
        start++;
    }

    // Find the last non-whitespace character from the right
    int end = strlen(str) - 1;
    while (isspace(str[end]))
    {
        end--;
    }

    // Copy the trimmed substring to the original string
    int i = 0;
    for (int j = start; j <= end; j++)
    {
        str[i] = str[j];
        i++;
    }

    // Add a null terminator at the end
    str[i] = '\0';
}
