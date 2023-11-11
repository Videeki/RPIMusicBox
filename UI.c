// Compiling: gcc `pkg-config --cflags gtk+-3.0` -o Builds/UI UI.c `pkg-config --libs gtk+-3.0` -IGPIOHandler "./GPIOHandler/GPIOHandler.c" -IQueue "./Queue/Queue.c" -IpRegex "./pRegex/pRegex.c" -IConfigFile "./ConfigFile/ConfigFile.c" -IPlayMusic "./PlayMusic/PlayMusic.c" -lmpg123 -lao -lpthread -lm


// Standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include <gtk/gtk.h>        // UI
#include <pthread.h>        // Thread
#include <dirent.h>         // Folder structure

#include "Queue.h"          // Queue
#include "PlayMusic.h"      // Sound
#include "GPIOHandler.h"    // GPIO
#include "pRegex.h"         // Find Match function
#include "ConfigFile.h"     // Config


#define ON 1
#define OFF 0
#define BITS 8
#define QUEUE_SIZE 255
#define PATH_LENGTH 255

GtkBuilder *builder;
GObject *window;
GObject *btnFolderUP, *btnFolderDOWN, *btnSongUP, *btnSongDOWN, *btnAddMusic;
GObject *entryPath;
GObject *folderName;

GtkListStore *lsSongs;
GtkScrolledWindow *songSW;
GtkAdjustment *listAdjustment;

GtkTreeView *tvwSongs;
GtkTreeViewColumn *tvwcTitle;
GtkCellRenderer *rndrSong;
GtkTreeSelection *songSelection;
GtkTreeIter iter, nulliter;

GtkTreeIter actIter[255];

int currFolderIndex = 0;
int currSongIndex = 0;
int nrOfFolders = 0;
int nrOfSongs = 0;

char** folderNames;
char mainFolder[PATH_LENGTH];
char initFolder[PATH_LENGTH];
char title[PATH_LENGTH];
char musicPath[PATH_LENGTH];
enum {SONG_COLUMN, N_COLUMNS};

string_Queue_t playlist;
int run = 1;

int populateList(char* path);

static void scroll_to_selection(GtkTreeSelection *selection,gpointer user_data);
static void stepFolderNext();
static void stepFolderPrevious();
static void stepSoundNext();
static void stepSoundPrevious();
static void addMusic2Playlist();

static void* musicPlayer();
//static void* gpioHandling();
static void* gpioHandling(config *configINI);

int readFile(char* argv);
int getFolders(char* path);
int addArrayElements(int *intArray, int arraySize);


int main(int argc, char *argv[])
{
    //readFile("config.ini");
    pthread_t musicTID;
    pthread_t gpioHandlingTID;
    obtain(&playlist, "MusicQueue", 100);
    GError *error = NULL;

    config configINI;
    char* filename = "config.ini";
    char* configSection = "Default";

    char* configStr = openConfig(filename);
    parseConfig(configStr, &configINI);
    closeConfig(configStr);


    gtk_init(&argc, &argv);

    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new ();
    if (gtk_builder_add_from_file (builder, readKey(&configINI, "Default", "Theme"), &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
    }

    /* Connect signal handlers to the constructed widgets. */
    window = gtk_builder_get_object(builder, "window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    entryPath = gtk_builder_get_object(builder, "entryPath");
    g_object_set_data(G_OBJECT(window), "entryPath", entryPath);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entryPath),"Music Title");

    folderName = gtk_builder_get_object(builder, "folderName");
    g_object_set_data(G_OBJECT(window), "folderName", folderName);

    songSW = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "songSW"));
    tvwSongs = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tvwSongs"));
    rndrSong = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "rndrSong"));
    tvwcTitle = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "tvwcTitle"));
    gtk_tree_view_column_add_attribute(tvwcTitle, rndrSong, "text", 0);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tvwSongs), tvwcTitle);
    songSelection = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "songSelection"));
    g_signal_connect(songSelection,"changed",G_CALLBACK(scroll_to_selection),tvwSongs);


    strcpy(mainFolder, readKey(&configINI, configSection, "MusicFolder"));
    getFolders(mainFolder);

    strcpy(initFolder, mainFolder);
    strcat(initFolder, folderNames[currFolderIndex]);
    populateList(initFolder);
    

    btnFolderUP = gtk_builder_get_object (builder, "btnFolderUP");
    g_signal_connect(btnFolderUP, "clicked", G_CALLBACK(stepFolderNext), NULL);
  
    btnFolderDOWN = gtk_builder_get_object (builder, "btnFolderDOWN");
    g_signal_connect(btnFolderDOWN, "clicked", G_CALLBACK(stepFolderPrevious), NULL);

    btnSongUP = gtk_builder_get_object (builder, "btnSongUP");
    g_signal_connect(btnSongUP, "clicked", G_CALLBACK(stepSoundPrevious), NULL);  

    btnSongDOWN = gtk_builder_get_object (builder, "btnSongDOWN");
    g_signal_connect(btnSongDOWN, "clicked", G_CALLBACK(stepSoundNext), NULL);  

    btnAddMusic = gtk_builder_get_object (builder, "btnAddMusic");
    g_signal_connect(btnAddMusic, "clicked", G_CALLBACK(addMusic2Playlist), NULL);


    pthread_create(&musicTID, NULL, musicPlayer, NULL);
    pthread_create(&gpioHandlingTID, NULL, gpioHandling, (void*)&configINI);


    gtk_main ();

    flush(&playlist);

    return 0;
}

int populateList(char* path)
{
    struct dirent *pDirent;
    DIR *pDir;

    //rewinddir(pDir);

    g_object_unref(lsSongs);

    lsSongs = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);

    // Ensure we can open directory.
    pDir = opendir (path);
    if(pDir == NULL)
    {
        printf("Cannot open directory '%s'\n", path);
    }
    else
    {   
        int i = 0;
        while ((pDirent = readdir(pDir)) != NULL)
        {   
            if(strstr(pDirent->d_name, ".mp3"))
            {
                gtk_list_store_append(lsSongs, &iter);
                gtk_list_store_set(lsSongs, &iter, SONG_COLUMN, pDirent->d_name, -1);
                actIter[i] = iter;
                i++;
            }
        }
        nrOfSongs = i - 1;

        rewinddir(pDir);
        
        closedir (pDir);

        gtk_tree_view_set_model(GTK_TREE_VIEW(tvwSongs), GTK_TREE_MODEL(lsSongs));


        //gtk_tree_model_get_iter_first(GTK_TREE_MODEL(lsSongs), &nulliter);
        currSongIndex = 0;
        gtk_tree_selection_select_iter(GTK_TREE_SELECTION(songSelection), &actIter[currSongIndex]);

        gchar *value;
        gtk_tree_model_get(GTK_TREE_MODEL(lsSongs), &actIter[currSongIndex], 0, &value, -1);

        strcpy(musicPath, value);

        gtk_scrolled_window_get_vadjustment(songSW);

    }
    
    return 0;
}

static void scroll_to_selection(GtkTreeSelection *selection,gpointer user_data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreePath *path;

    GtkWidget *treeview = GTK_WIDGET(user_data);

    if(gtk_tree_selection_get_selected(selection,&model,&iter))
    {
        path = gtk_tree_model_get_path(model,&iter);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(treeview),path,NULL,FALSE,0.0,0.0);
        gtk_tree_path_free(path);
    }
}

static void addMusic2Playlist()
{
    strcpy(title, initFolder);
    strcat(title, "/");
    strcat(title, musicPath);
    enqueue(&playlist, title);
}

static void stepFolderNext()
{   
    currFolderIndex++;
    if(currFolderIndex <= nrOfFolders)
    {
        strcpy(initFolder, mainFolder);
        gtk_entry_set_text(GTK_ENTRY(folderName), folderNames[currFolderIndex]);
        strcat(initFolder, folderNames[currFolderIndex]);

        populateList(initFolder);
    }
    else
    {
        currFolderIndex--;
    }
}

static void stepFolderPrevious()
{
    currFolderIndex--;
    if(currFolderIndex >= 0)
    {
        strcpy(initFolder, mainFolder);
        gtk_entry_set_text(GTK_ENTRY(folderName), folderNames[currFolderIndex]);
        strcat(initFolder, folderNames[currFolderIndex]);

        populateList(initFolder);
    }
    else
    {
        currFolderIndex = 0;
    }
}

static void stepSoundNext()
{   
    currSongIndex++;
    if(currSongIndex <= nrOfSongs)
    {
        gtk_tree_selection_select_iter(GTK_TREE_SELECTION(songSelection), &actIter[currSongIndex]);

        gchar *value;
        gtk_tree_model_get(GTK_TREE_MODEL(lsSongs), &actIter[currSongIndex], 0, &value, -1);

        strcpy(musicPath, value);
    }
    else
    {
        currSongIndex--;
    }
}

static void stepSoundPrevious()
{   
    currSongIndex--;
    if(currSongIndex >= 0)
    {
        gtk_tree_selection_select_iter(GTK_TREE_SELECTION(songSelection), &actIter[currSongIndex]);

        gchar *value;
        gtk_tree_model_get(GTK_TREE_MODEL(lsSongs), &actIter[currSongIndex], 0, &value, -1);

        strcpy(musicPath, value);
    }
    else
    {
        currSongIndex = 0;
    }
}

static void* musicPlayer()
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
            
            #ifdef _WIN32
            gtk_entry_set_text(GTK_ENTRY(entryPath), strrchr(musicTitle, '\\') + 1);
            #elif __linux__
            gtk_entry_set_text(GTK_ENTRY(entryPath), strrchr(musicTitle, '/') + 1);
            #endif
            
            playMusic(musicTitle);
        }
        
    }
    
    //puts("Music closed");
    closeMusic();
}

static void* gpioHandling(config *configINI)
{
    int addButton = atoi(readKey(configINI, "Default", "AddButton"));
    int nextFolder = atoi(readKey(configINI, "Default", "NextFolder"));
    int prevFolder = atoi(readKey(configINI, "Default", "PreviousFolder"));
    int nextSong = atoi(readKey(configINI, "Default", "NextSong"));
    int prevSong = atoi(readKey(configINI, "Default", "PreviousSong"));

    puts("Start, polling");
    struct gpiohandle_request req;
    int ctrlPINS[5] = {addButton, nextFolder, prevFolder, nextSong, prevSong};
    int regPINSValeus[5] = {0,0,0,0,0};
    int i;

    initGPIO(&req, ctrlPINS, 5, INPUT);

    while (run)
    {   
        detectButtonAction(&req, regPINSValeus, 100);

        int sum = 0;
        for(i = 0; i < req.lines; i++)
        {
            regPINSValeus[i] *= (int)pow((double)2, (double)i);
            sum += regPINSValeus[i];
        }
        
        switch(sum)
        {
            case 1:     //addMusic2Playlist
            {
                addMusic2Playlist();
                break;
            }

            case 2:     //stepFolderNext
            {
                stepFolderNext();
                break;
            }

            case 4:     //stepFolderPrevious
            {
                stepFolderPrevious();
                break;
            }

            case 8:     //stepSoundNext
            {
                stepSoundNext();
                break;
            }

            case 16:    //stepSoundPrevious
            {
                stepSoundPrevious();
                break;
            }

            case 31:    //Ultimate combo
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

int readFile(char* argv)
{
    FILE *fp;
    char *str;
    long size;

    fp = fopen(argv, "rb"); // read mode

    if (fp == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    str = calloc(size + 1, sizeof(char));

    if (str == NULL) {
        fclose(fp);
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    fread(str, size, 1, fp);
    fclose(fp);

    printf("%s\n", str);
    free(str);

    return 0;
}

int getFolders(char* path)
{
    struct dirent *pDirent;
    DIR *pDir;

    //rewinddir(pDir);

    pDir = opendir (path);
    if (pDir == NULL)
    {
        printf ("Cannot open directory '%s'\n", path);
        return 1;
    }

    int i = 0;
    // Process each entry.
    while ((pDirent = readdir(pDir)) != NULL)
    {
        i++;
    }
    nrOfFolders = i;

    rewinddir(pDir);
    
    folderNames = (char**)malloc(i * sizeof(char*));  
    int j = 0;
    char dirName[255];
    while ((pDirent = readdir(pDir)) != NULL)
    {   
        strcpy(dirName, pDirent->d_name);
        int foundDot = strcspn(dirName, ".");
        if(foundDot == strlen(dirName) && foundDot != 1)
        {
            folderNames[j] = (char*)malloc(strlen(dirName) * sizeof(char*));
            strcpy(folderNames[j], dirName);
            j++;
        }
    }
    
    closedir (pDir);
    return i;
}

int addArrayElements(int *intArray, int arraySize)
{
    int i;
    int sum = 0;

    for(i = 0; i < arraySize; i++)
    {
        sum += intArray[i];
    }

    return sum;
}