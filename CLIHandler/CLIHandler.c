#include "CLIHandler.h"

static int loadDashboard(const char* path);
static void printCharacterType(GList* charList, const char* title, const int x, const int y, const int weight);
static GList*  parseChararcterType(GList* charList, const char* charString, const char* separator);
int strlenUTF8(const char *str);

static gboolean mainloopStop = TRUE;
static GList* charList = NULL;
static int fontHeight = 0;
static int trackOffsetStartY = 0;
static int trackOffsetStartX = 0;
static int trackOffsetEndX = 0;
static int trackMaxWeight = 0;
static int albumOffsetStartY = 0;
static int albumOffsetStartX = 0;
static int albumOffsetEndX = 0;
static int albumMaxWeight = 0;
static int listOffsetStartY = 0;
static int listOffsetStartX = 0;
static int listOffsetEndX = 0;
static int listMaxLength = 0;
static int listMaxWeight = 0;


static int loadDashboard(const char* path)
{
    SETCONSOLEUTF8;
    printf(CLRSC JUMP2ZERO);

    FILE *fp;
    char *str;
    long size;

    fp = fopen(path, "rb"); // read mode

    if(fp == NULL)
    {
        perror("Error while opening the file.\n");
        return EXIT_FAILURE;
    }

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    str = malloc((size + 1) * sizeof(char));

    if(str == NULL)
    {
        fclose(fp);
        perror("Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    fread(str, size, 1, fp);
    fclose(fp);

    printf("%s", str);
    free(str);
    
    return 0;
}


static int loadDashboardAndChars(const char* const path)
{
    GKeyFile *key_file;
    GError *error = NULL;
    key_file = g_key_file_new();

    // Load the configuration file
    if(!g_key_file_load_from_file(key_file, path, G_KEY_FILE_NONE, &error))
    {
        if(error)
        {
            g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
            g_error_free(error);
        }
        g_key_file_free(key_file);
        return -1;
    }

    int height = g_key_file_get_integer(key_file, "Theme", "Height", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    fontHeight = g_key_file_get_integer(key_file, "Font", "Height", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    trackOffsetStartY = g_key_file_get_integer(key_file, "Theme", "TrackOffsetStartY", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    trackOffsetStartX = g_key_file_get_integer(key_file, "Theme", "TrackOffsetStartX", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    trackOffsetEndX = g_key_file_get_integer(key_file, "Theme", "TrackOffsetEndX", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    trackMaxWeight = g_key_file_get_integer(key_file, "Theme", "TrackMaxWeight", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    albumOffsetStartY = g_key_file_get_integer(key_file, "Theme", "AlbumOffsetStartY", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    albumOffsetStartX = g_key_file_get_integer(key_file, "Theme", "AlbumOffsetStartX", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    albumOffsetEndX = g_key_file_get_integer(key_file, "Theme", "AlbumOffsetEndX", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    albumMaxWeight = g_key_file_get_integer(key_file, "Theme", "AlbumMaxWeight", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    listOffsetStartY = g_key_file_get_integer(key_file, "Theme", "ListOffsetStartY", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    listOffsetStartX = g_key_file_get_integer(key_file, "Theme", "ListOffsetStartX", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    listOffsetEndX = g_key_file_get_integer(key_file, "Theme", "ListOffsetEndX", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    listMaxLength = g_key_file_get_integer(key_file, "Theme", "ListMaxLength", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    listMaxWeight = g_key_file_get_integer(key_file, "Theme", "ListMaxWeight", &error);
    if(error)
    {
        g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return -2;
    }

    SETCONSOLEUTF8;
    printf(CLRSC JUMP2ZERO);

    char line[9];
    for(int i=0; i < height; i++)
    {
        sprintf(line, "Line%04d", i);
        //printf("%s\n", g_key_file_get_string(key_file, "Theme", line, &error));
        gchar* lineContent = g_key_file_get_string(key_file, "Theme", line, &error);
        if(error)
        {
            g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
            g_error_free(error);
            g_key_file_free(key_file);
            return -3;
        }
        printf("%s\n", lineContent);
    }
    
    charList = parseChararcterType(charList, g_key_file_get_string(key_file, "Font", "ASCIIart", &error), g_key_file_get_string(key_file, "Font", "Separator", &error));

    return 0;
}


int CLIInit(const char* const path)
{
    mainloopStop = FALSE;
    return loadDashboardAndChars(path);
}


int CLIClose()
{
    g_list_free(charList);
}


void CLIUpdateAlbum(const char* title)
{
    printCharacterType(charList, title, albumOffsetStartX, albumOffsetStartY, albumMaxWeight);
}


void CLIUpdateTrack(const char* title)
{
    printCharacterType(charList, title, trackOffsetStartX, trackOffsetStartY, trackMaxWeight);
}


void CLIUpdateTrackList(GList* trackList)
{
    char substring[listMaxWeight];
    memset(substring, ' ', listMaxWeight);
    substring[listMaxWeight] = '\0';

    for(int i = 0; i < listMaxLength; i++)
    {
        //printf(JUMP2POS((LIST_OFFSET_START_Y + i), LIST_OFFSET_START_X));
        printf("\e[%d;%dH", (listOffsetStartY + i), listOffsetStartY);
        
        GList* element = g_list_nth(trackList, i);
        strncpy(substring, element->data, listMaxWeight);

        //if(i == 0) printf(DESIGN(BF_WHITE, BG_YELLOW, BOLD));
        if(i == 0) printf("\e[%d;%d;%d;m", BF_WHITE, BG_MAGENTA, BOLD);

        printf("%s"RESET, substring);
    }
}


void CLIUpdateActiveTrack(GList* trackList, const int actTrack, const int prevTrack)
{
    char substring[listMaxWeight];
    for(int i = 0; i < listMaxWeight; i++)
    {
        substring[i] = ' ';
    }

    if(actTrack > prevTrack)
    {
        printf("\033[%d;%dH", listOffsetStartY + prevTrack, listOffsetStartX);
        GList* prevElement = g_list_nth(trackList, prevTrack);
        int prevElemLen = strlen(prevElement->data);
        if(prevElemLen > listMaxWeight)
        {
            strncpy(substring, prevElement->data, listMaxWeight);
        }
        else
        {
            memcpy(substring, prevElement->data, prevElemLen);
        }
        printf("%s", substring);
        

        printf("\033[%d;%dH", listOffsetStartY + actTrack, listOffsetStartX);
        GList* actElement = g_list_nth(trackList, actTrack);
        int actElemLen = strlen(actElement->data);
        if(actElemLen > listMaxWeight)
        {
            strncpy(substring, actElement->data, listMaxWeight);
        }
        else
        {
            memcpy(substring, actElement->data, actElemLen);
        }
        printf("%s%s%s", DESIGN(BF_WHITE, BG_YELLOW, BOLD), substring, RESET);
        
    }
    else if(actTrack > prevTrack)
    {
        printf("\033[%d;%dH", listOffsetStartY + actTrack, listOffsetStartX);
        GList* actElement = g_list_nth(trackList, actTrack);
        int actElemLen = strlen(actElement->data);
        if(actElemLen > listMaxWeight)
        {
            strncpy(substring, actElement->data, listMaxWeight);    
        }
        else
        {
            memcpy(substring, actElement->data, actElemLen);
        }
        printf("%s", substring);
        
        printf("\033[%d;%dH", listOffsetStartY + prevTrack, listOffsetStartX);
        GList* prevElement = g_list_nth(trackList, prevTrack);
        int prevElemLen = strlen(prevElement->data);
        if(prevElemLen > listMaxWeight)
        {
            strncpy(substring, prevElement->data, listMaxWeight);
        }
        else
        {
            memcpy(substring, prevElement->data, prevElemLen);
        }
        printf("%s%s%s", DESIGN(BF_WHITE, BG_YELLOW, BOLD), substring, RESET);
    }
    else
    {

    }
}


void CLIMainloop()
{
    while(!mainloopStop)
    {
        g_usleep(100000);
    }
}


void CLIMainloopStop()
{
    mainloopStop = TRUE;
}


static GList* parseChararcterType(GList* charList, const char* charString, const char* separator)
{
    //This section remove the first and the last character
    int length = strlen(charString) - 2;
    char trimmedStr[length];
    strncpy(trimmedStr, charString + 1, length);
    trimmedStr[length] = '\0';

    char *token = strtok(trimmedStr, separator);
    
    // Loop through the tokens and print them
    int i = 0;
    while (token != NULL)
    {
        charList = g_list_append(charList, g_strdup(token));
        token = strtok(NULL, separator);
    }
    return charList;
}


static void printCharacterType(GList* charList, const char* title, const int x, const int y, const int weight)
{
    for(int i = 0; i < fontHeight; i++)
    {
        char printedline[weight];
        printedline[0] = '\0';

        //printf(JUMP2POS(y+i, x));   //Position the cursor
        printf("\033[%d;%dH", (y + i),  x);
        for(int j = 0; title[j] != 0; j++)
        {
            char character = title[j];
            GList* element = g_list_nth(charList, ((character  - 32) * 3) + i);
            if(element != NULL)
            {          	
                int codelen = strlen((char*)element->data);
                if((strlen(printedline) + codelen) < weight)
                {
                    strcat(printedline, (char*)element->data);
                }
            }
            else
            {
                if((strlen(printedline) + 3) < weight)
                {
                    strcat(printedline, "   ");
                }
            }
        }
        printf("%s\n", printedline);
        printedline[0] = '\0';
    }
}


void colorMixer(const char* colorCode, const int type)
{
    int start = 0;
    int code = 16;

    if(colorCode[0] == '#') start = 1;
    if(colorCode[0] == '0' && colorCode[1] == 'x') code = 0;

    int color = (int)strtol(colorCode + start, NULL, code);
    int r = (color & 0xFF0000) >> 16;
    int g = (color & 0x00FF00) >> 8;
    int b = (color & 0x0000FF);

    printf("\x1B[%d;2;%03d;%03d;%03d;m", type, ((color & 0xFF0000) >> 16), ((color & 0x00FF00) >> 8), (color & 0x0000FF));
}


void fontColor(const char* colorCode)
{
    colorMixer(colorCode, FONT);
}


void backgroundColor(const char* colorCode)
{
    colorMixer(colorCode, BACKGROUND);
}


int strlenUTF8(const char *str)
{
    int count = 0;
    while(*str)
    {
        if((*str & 0x80) == 0)
        {
            // 1-byte character (ASCII)
            str += 1;
        }
        else if((*str & 0xE0) == 0xC0)
        {
            // 2-byte character
            str += 2;
        }
        else if((*str & 0xF0) == 0xE0)
        {
            // 3-byte character
            str += 3;
        }
        else if ((*str & 0xF8) == 0xF0)
        {
            // 4-byte character
            str += 4;
        }
        else
        {
            // Invalid UTF-8 byte sequence
            return -1;
        }
        count++;
    }
    return count;
}


/**********************************************     Test     **********************************************

gpointer thread_function(gpointer data)
{
    printf("Hello from the new thread! Argument: %s\n", (char *)data);
    CLIUpdateTrack("Hello, world!");
    CLIUpdateAlbum("FOOBAR");
    int pushedChar;
    gboolean stop = FALSE;
    do
    {
        pushedChar = getchar();
        switch (pushedChar)
        {
        case 'q':
            CLIMainloopStop();
            stop = TRUE;
            break;
        
        default:
            printf("%c\n", pushedChar);
            break;
        }
    } while(!stop);
    
    return NULL;
}


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        g_printerr("There are not enough parameters\n");
        return -1;
    }

    // Create a new thread
    CLIInit(argv[1]);
    
    GThread *thread = g_thread_new("example-thread", thread_function, "Thread Argument");
    CLIMainloop();
    
    // Wait for the thread to finish
    g_thread_join(thread);
    printf("Thread has finished execution.\n");

    CLIClose();
    printf("The Mainloop has finished execution.\n");

    return 0;
}

**********************************************************************************************************/