/****************************************************************************/
/*                                                                          */
/*  gcc main.c -o .\Builds\RPIMusicBox -ICLIHandler .\CLIHandler\Builds\CLIHandler.o -IFolderHandler .\FolderHandler\Builds\FolderHandler.o -IGPIOHandler .\GPIOHandler\Builds\GPIOHandler.o -IPlayMusic .\PlayMusic\Builds\PlayMusic.o "-IC:/msys64/mingw64/include/glib-2.0" "-IC:/msys64/mingw64/lib/glib-2.0/include" "-lglib-2.0" -lwinmm
/*                                                                          */
/****************************************************************************/


#include <stdio.h>
//#include <gtk/gtk.h>        // UI
#include <glib.h>

#include "GPIOHandler/GPIOHandler.h"    // GPIO
#include "PlayMusic/PlayMusic.h"      // Sound
#include "FolderHandler/FolderHandler.h"

#include "CLIHandler/CLIHandler.h"

#include "Debug/debugmalloc.h"

typedef struct MusicBoxData
{
    GString* projectFolderPath;
    GString* musicFolderPath;
    GList* musicFolders;
    GString* actFolderPath;
    GList* actFolderContent;
    GQueue* playQueue;
    GString* actMusic;
    GPIO* gpio;
    gboolean stop;

}MBData;

void stepFolderNext(MBData* data);
void stepFolderPrevious(MBData* data);
void stepSoundNext(MBData* data);
void stepSoundPrevious(MBData* data);
void addMusic2Playlist(MBData* data);


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        g_printerr("There are less parameter added what required\n");
        return -1;
    }

    GKeyFile *key_file;
    GError *error = NULL;

    printf("Create a new GKeyFile and Load the configuration file\n");
    key_file = g_key_file_new();
    if(!g_key_file_load_from_file(key_file, argv[1], G_KEY_FILE_NONE, &error))
    {
        if(error)
        {
            g_printerr("Error (code:%d) loading config file: %s\n", error->code, error->message);
            g_error_free(error);
        }
        g_key_file_free(key_file);
        return -2;
    }

    MBData data;

    data.stop = FALSE;
    printf("Get musicFolderPath\n");

    error = NULL;
    data.musicFolderPath = NULL;
    data.musicFolderPath = g_string_new(g_key_file_get_string(key_file, "Default", "MusicFolder", &error));
    
    printf("Get music folders\n");
    data.musicFolders = NULL;
    data.musicFolders = listFolderElements(data.musicFolders, data.musicFolderPath->str, NULL);
    
    printf("Init queue\n");
    data.playQueue = g_queue_new();

    int ctrlPINS[5];
    error = NULL;
    ctrlPINS[0] = g_key_file_get_integer(key_file, "Default", "AddButton", &error);
    error = NULL;
    ctrlPINS[1] = g_key_file_get_integer(key_file, "Default", "NextFolder", &error);
    error = NULL;
    ctrlPINS[2] = g_key_file_get_integer(key_file, "Default", "PreviousFolder", &error);
    error = NULL;
    ctrlPINS[3] = g_key_file_get_integer(key_file, "Default", "NextSong", &error);
    error = NULL;
    ctrlPINS[4] = g_key_file_get_integer(key_file, "Default", "PreviousSong", &error);

    printf("Init GPIO\n");
    GPIO gpio;
    initGPIO(&gpio, ctrlPINS, 5, GPIO_INPUT);
    data.gpio = &gpio;

    //pthread_t musicTID;
    //pthread_t gpioHandlingTID;
    //pthread_create(&musicTID, NULL, musicPlayer, (MBData*)&data);
    //pthread_create(&gpioHandlingTID, NULL, gpioHandling, (MBData*)&data);

    if (!isatty(fileno(stdin)))
    {
        printf("The application was not started from a terminal.\n");
        //GUI* guidata = initGUI();

        //guidata->stepFolderNext = &stepFolderNext;
        //guidata->stepFolderPrevious = &stepFolderPrevious;
        //guidata->stepSoundNext = &stepSoundNext;
        //guidata->stepSoundPrevious = &stepSoundPrevious;
        //guidata->addMusic2Playlist = &addMusic2Playlist;

        //GString* dboardPath = g_string_new(NULL);
        //dboardPath = g_string_append(dboardPath, g_key_file_get_string(key_file, "GUI", "Dashboard", &error));
        //guidata = initGUIDashboard(guidata, dboardPath, argc, argv);
        //g_string_free(dboardPath, TRUE);

    }
    else
    {
        printf("CLIInit\n");
        error = NULL;
        CLIInit(g_key_file_get_string(key_file, "CLI", "Dashboard", &error));

        CLIUpdateTrack("Hello, world!");

        g_key_file_free(key_file);
        data.actFolderPath = g_string_new(g_strdup(data.musicFolderPath->str));

        data.actFolderPath = path_Join(data.actFolderPath, g_strdup((gchar*)data.musicFolders->data), NULL);
        CLIUpdateAlbum((char*)data.musicFolders->data);

        data.actFolderContent = NULL;
        data.actFolderContent = listFolderElements(data.actFolderContent, data.actFolderPath->str, "mp3");

        CLIUpdateTrackList(data.actFolderContent);

        //CLIMainloop();

        CLIClose();
    }
    
    if(data.projectFolderPath)  g_string_free(data.projectFolderPath, TRUE);
    if(data.musicFolderPath)    g_string_free(data.musicFolderPath, TRUE);
    if(data.actFolderPath)  g_string_free(data.actFolderPath, TRUE);
    if(data.actMusic)   g_string_free(data.actMusic, TRUE);
    if(data.musicFolders)   g_list_free(data.musicFolders);
    if(data.actFolderContent)    g_list_free(data.actFolderContent);
    if(data.playQueue)  g_queue_free(data.playQueue);
    closeGPIO(data.gpio);
    return 0;
}


void addMusic2Playlist(MBData* data)
{
    g_queue_push_head(data->playQueue, g_strdup(data->actMusic->str));
}


void stepFolderNext(MBData* data)
{   
    //UIData->currFolderIndex++;
    //if(UIData->currFolderIndex <= UIData->nrOfFolders - 1)
    //{   
    //    printf("Folder number: %d Number of folders: %d\n", UIData->currFolderIndex, UIData->nrOfFolders);
    //    strcpy(UIData->initFolder, UIData->mainFolder);
    //    gtk_entry_set_text(GTK_ENTRY(UIData->folderName), UIData->folderNames[UIData->currFolderIndex]);
    //    strcat(UIData->initFolder, UIData->folderNames[UIData->currFolderIndex]);
    //
    //    populateList(UIData);
    //}
    //else
    //{
    //    UIData->currFolderIndex--;
    //}
}


void stepFolderPrevious(MBData* data)
{
    //UIData->currFolderIndex--;
    //if(UIData->currFolderIndex >= 0)
    //{
    //    strcpy(UIData->initFolder, UIData->mainFolder);
    //    gtk_entry_set_text(GTK_ENTRY(UIData->folderName), UIData->folderNames[UIData->currFolderIndex]);
    //    strcat(UIData->initFolder, UIData->folderNames[UIData->currFolderIndex]);
    //
    //    populateList(UIData);
    //}
    //else
    //{
    //    UIData->currFolderIndex = 0;
    //}
}


void stepSoundNext(MBData* data)
{   
    //UIData->currSongIndex++;
    //if(UIData->currSongIndex <= UIData->nrOfSongs)
    //{
    //    gtk_tree_selection_select_iter(GTK_TREE_SELECTION(UIData->songSelection), &UIData->actIter[UIData->currSongIndex]);
    //
    //    gchar *value;
    //    gtk_tree_model_get(GTK_TREE_MODEL(UIData->lsSongs), &UIData->actIter[UIData->currSongIndex], 0, &value, -1);
    //
    //    strcpy(UIData->musicPath, value);
    //}
    //else
    //{
    //    UIData->currSongIndex--;
    //}
}


void stepSoundPrevious(MBData* data)
{   
    //UIData->currSongIndex--;
    //if(UIData->currSongIndex >= 0)
    //{
    //    gtk_tree_selection_select_iter(GTK_TREE_SELECTION(UIData->songSelection), &UIData->actIter[UIData->currSongIndex]);
    //
    //    gchar *value;
    //    gtk_tree_model_get(GTK_TREE_MODEL(UIData->lsSongs), &UIData->actIter[UIData->currSongIndex], 0, &value, -1);
    //
    //    strcpy(UIData->musicPath, value);
    //}
    //else
    //{
    //    UIData->currSongIndex = 0;
    //}
}


void* musicPlayer(MBData* data)
{
    //puts("Music initialization");
    char musicTitle[255];
    initMusic();
    
    while(data->stop)
    {
        if(g_queue_is_empty(data->playQueue))
        {
            g_usleep(200*1000);
        }
        else
        {
            playMusic((char*)g_queue_pop_head(data->playQueue));
        }
        
    }
    
    //puts("Music closed");
    closeMusic();
}


void* gpioHandling(MBData* data)
{
    while(data->stop)
    {
        switch(detectButtonAction(data->gpio, 200))
        {
            case 0b00001:     //addMusic2Playlist
            {
                addMusic2Playlist(data);
                //G_CALLBACK(addMusic2Playlist);
                break;
            }

            case 0b00010:     //stepFolderNext
            {
                stepFolderNext(data);
                //G_CALLBACK(stepFolderNext);
                break;
            }

            case 0b00100:     //stepFolderPrevious
            {
                stepFolderPrevious(data);
                //G_CALLBACK(stepFolderPrevious);
                break;
            }

            case 0b01000:     //stepSoundNext
            {
                stepSoundNext(data);
                //G_CALLBACK(stepSoundNext);
                break;
            }

            case 0b10000:    //stepSoundPrevious
            {
                stepSoundPrevious(data);
                //G_CALLBACK(stepSoundPrevious);
                break;
            }

            case 0b10001:    //Ultimate combo
            {
                puts("Ultimate combo!");
            }

            default:
            {
                puts("NO... NO... Nem lenni!");
            }
        }
    }
}

