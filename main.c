#include <stdio.h>
//#include <gtk/gtk.h>        // UI
#include <glib.h>

#include "GPIOHandler/GPIOHandler.h"    // GPIO
#include "PlayMusic/PlayMusic.h"      // Sound
#include "FolderHandler/FolderHandler.h"

#include "CLIHandler/CLIHandler.h"

typedef struct UIData
{
    GString* projectFolder;
    GString* musicFolder;

}UIData;

void stepFolderNext(MusicBoxUI* UIData);
void stepFolderPrevious(MusicBoxUI* UIData);
void stepSoundNext(MusicBoxUI* UIData);
void stepSoundPrevious(MusicBoxUI* UIData);
void addMusic2Playlist(MusicBoxUI* UIData);


int main(int argc, char* argv[])
{
    #ifdef _WIN32
        system("chcp 65001");
    #endif

    if(argc < 2)
    {
        g_printerr("There are less parameter added what required\n");
        return -1;
    }

    //const gchar *file_path = "C:\\Users\\Videeki\\Documents\\GitRepos\\RPIMusicBox\\Builds\\config.ini";
    
    GKeyFile *key_file;
    GError *error = NULL;
    GQueue *playlist = g_queue_new();

    // Create a new GKeyFile
    key_file = g_key_file_new();

    // Load the configuration file
    //if(!g_key_file_load_from_file(key_file, file_path, G_KEY_FILE_NONE, &error))
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

    GString* mfoldPath = g_string_new(NULL);
    mfoldPath = g_string_append(mfoldPath, g_key_file_get_string(key_file, "Default", "MusicFolder", &error));

    GList* trackList = listFolderElements(trackList, mfoldPath->str, NULL);

    g_string_free(mfoldPath, TRUE);

    pthread_t musicTID;
    pthread_t gpioHandlingTID;


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
        GString* dboardPath = g_string_new(NULL);
        dboardPath = g_string_append(dboardPath, g_key_file_get_string(key_file, "CLI", "Dashboard", &error));
        initCLIDashboard(dboardPath->str);
        g_string_free(dboardPath, TRUE);

        GString* charString = g_string_new(NULL);
        charString = g_string_append(charString, g_key_file_get_string(key_file, "CLI", "ASCIIart", &error));
        char* separator = g_key_file_get_string(key_file, "CLI", "Separator", &error);
        
        GList* charList = parseChararcterType(charList, charString->str, separator);
        g_string_free(charString, TRUE);
    
        //updateCLITrack(charList, "ABCDEFGHIJKLMNOPQRST");
        //updateCLIAlbum(charList, "abcdefghijklmnopqrst");
        //updateCLITrackList(trackList);
        getchar();
        
        g_list_free(charList);
    }
    pthread_create(&musicTID, NULL, musicPlayer, (void*)entryPath);
    pthread_create(&gpioHandlingTID, NULL, gpioHandling, (MusicBoxUI*)&UIData);

    

    g_key_file_free(key_file);
    g_list_free(trackList);
    g_queue_free(playlist);
    return 0;
}


void addMusic2Playlist(MusicBoxUI* UIData)
{
    strcpy(UIData->title, UIData->initFolder);
    strcat(UIData->title, "/");
    strcat(UIData->title, UIData->musicPath);
    enqueue(&playlist, UIData->title);
}

void stepFolderNext(MusicBoxUI* UIData)
{   
    UIData->currFolderIndex++;
    if(UIData->currFolderIndex <= UIData->nrOfFolders - 1)
    {   
        printf("Folder number: %d Number of folders: %d\n", UIData->currFolderIndex, UIData->nrOfFolders);
        strcpy(UIData->initFolder, UIData->mainFolder);
        gtk_entry_set_text(GTK_ENTRY(UIData->folderName), UIData->folderNames[UIData->currFolderIndex]);
        strcat(UIData->initFolder, UIData->folderNames[UIData->currFolderIndex]);

        populateList(UIData);
    }
    else
    {
        UIData->currFolderIndex--;
    }
}

void stepFolderPrevious(MusicBoxUI* UIData)
{
    UIData->currFolderIndex--;
    if(UIData->currFolderIndex >= 0)
    {
        strcpy(UIData->initFolder, UIData->mainFolder);
        gtk_entry_set_text(GTK_ENTRY(UIData->folderName), UIData->folderNames[UIData->currFolderIndex]);
        strcat(UIData->initFolder, UIData->folderNames[UIData->currFolderIndex]);

        populateList(UIData);
    }
    else
    {
        UIData->currFolderIndex = 0;
    }
}

void stepSoundNext(MusicBoxUI* UIData)
{   
    UIData->currSongIndex++;
    if(UIData->currSongIndex <= UIData->nrOfSongs)
    {
        gtk_tree_selection_select_iter(GTK_TREE_SELECTION(UIData->songSelection), &UIData->actIter[UIData->currSongIndex]);

        gchar *value;
        gtk_tree_model_get(GTK_TREE_MODEL(UIData->lsSongs), &UIData->actIter[UIData->currSongIndex], 0, &value, -1);

        strcpy(UIData->musicPath, value);
    }
    else
    {
        UIData->currSongIndex--;
    }
}

void stepSoundPrevious(MusicBoxUI* UIData)
{   
    UIData->currSongIndex--;
    if(UIData->currSongIndex >= 0)
    {
        gtk_tree_selection_select_iter(GTK_TREE_SELECTION(UIData->songSelection), &UIData->actIter[UIData->currSongIndex]);

        gchar *value;
        gtk_tree_model_get(GTK_TREE_MODEL(UIData->lsSongs), &UIData->actIter[UIData->currSongIndex], 0, &value, -1);

        strcpy(UIData->musicPath, value);
    }
    else
    {
        UIData->currSongIndex = 0;
    }
}

void* musicPlayer(void *entryPath)
{
    //puts("Music initialization");
    char musicTitle[255];
    initMusic();
    
    while(run)
    {
        if(nrOfElements(&playlist) == 0)
        {
            sleep(1);
        }
        else
        {   
            strcpy(musicTitle, dequeue(&playlist));
            
            //#ifdef _WIN32
            //gtk_entry_set_text(GTK_ENTRY(entryPath), strrchr(musicTitle, '\\') + 1);
            //#elif __linux__
            //gtk_entry_set_text(GTK_ENTRY(entryPath), strrchr(musicTitle, '/') + 1);
            //#endif
            
            playMusic(musicTitle);
        }
        
    }
    
    //puts("Music closed");
    closeMusic();
}

void* gpioHandling(MusicBoxUI* UIData)
{
    int addButton = atoi(readKey(UIData->configINI, "Default", "AddButton"));
    int nextFolder = atoi(readKey(UIData->configINI, "Default", "NextFolder"));
    int prevFolder = atoi(readKey(UIData->configINI, "Default", "PreviousFolder"));
    int nextSong = atoi(readKey(UIData->configINI, "Default", "NextSong"));
    int prevSong = atoi(readKey(UIData->configINI, "Default", "PreviousSong"));

    puts("Start, polling");
    //struct gpiohandle_request req;
    refStruct req;
    int ctrlPINS[5] = {addButton, nextFolder, prevFolder, nextSong, prevSong};
    int i;

    initGPIO(&req, ctrlPINS, 5, INPUT);

    while(run)
    {
        switch(detectButtonAction(&req, 200))
        {
            case 0b00001:     //addMusic2Playlist
            {
                addMusic2Playlist(UIData);
                //G_CALLBACK(addMusic2Playlist);
                break;
            }

            case 0b00010:     //stepFolderNext
            {
                stepFolderNext(UIData);
                //G_CALLBACK(stepFolderNext);
                break;
            }

            case 0b00100:     //stepFolderPrevious
            {
                stepFolderPrevious(UIData);
                //G_CALLBACK(stepFolderPrevious);
                break;
            }

            case 0b01000:     //stepSoundNext
            {
                stepSoundNext(UIData);
                //G_CALLBACK(stepSoundNext);
                break;
            }

            case 0b10000:    //stepSoundPrevious
            {
                stepSoundPrevious(UIData);
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

    closeGPIO(&req);
}

