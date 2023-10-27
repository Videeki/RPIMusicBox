// Compiling: gcc `pkg-config --cflags gtk+-3.0` -o Builds/UI UI.c `pkg-config --libs gtk+-3.0` -IGPIOHandler "./GPIOHandler/GPIOHandler.c" -IQueue "./Queue/Queue.c" -IPlayMusic "./PlayMusic/PlayMusic.c" -lmpg123 -lao -lpthread -lm


// Standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

// UI
#include <gtk/gtk.h>

// Thread
#include <pthread.h>

// Folder structure
#include <dirent.h>

// Queue
#include "Queue.h"

// Sound
#include "PlayMusic.h"

// GPIO
#include "GPIOHandler.h"

#define ON 1
#define OFF 0
#define BITS 8
#define QUEUE_SIZE 255
#define PATH_LENGTH 255

GtkBuilder *builder;
GObject *window;
GObject *btnFolderUP, *btnFolderDOWN, *btnSongUP, *btnSongDOWN, *btnAddMusic, *btnTurnON, *btnTurnOFF;
GObject *entryPath;

GtkListStore *lsSongs;
    
GtkTreeView *tvwSongs;
GtkTreeViewColumn *tvwcTitle;
GtkCellRenderer *rndrSong;
GtkTreeSelection *songSelection;
GtkTreeIter iter, nulliter;

GtkTreeIter actIter[255];

int currFolderIndex = 0;
int currSongIndex = 0;

char** folderNames;
char mainFolder[PATH_LENGTH];
char initFolder[PATH_LENGTH];
char title[PATH_LENGTH];
enum {SONG_COLUMN, N_COLUMNS};

string_Queue_t playlist;
int run = 1;

int populateList(char* path);

static void stepFolderNext(GtkWidget *widget, gpointer data);
static void stepFolderPrevious(GtkWidget *widget, gpointer data);
static void stepSoundNext(GtkWidget *widget, gpointer data);
static void stepSoundPrevious(GtkWidget *widget, gpointer data);
static void addMusic2Playlist(GtkWidget *widget, gpointer data);

static void turnON(GtkWidget *widget, gpointer data);
static void turnOFF(GtkWidget *widget, gpointer data);

static void musicPlayer();
static void gpioHandling();

static void addMusicGPIO();
static void nextFolderGPIO();
static void previousFolderGPIO();
static void nextSongGPIO();
static void previousSongGPIO();

int readFile(char* argv);
int getFolders(char* path);
int addArrayElements(int *intArray, int arraySize);

static void addMusic2PlaylistGPIO();

int main(int argc, char *argv[])
{
    //readFile("config.ini");
    pthread_t musicTID;
    pthread_t gpioHandlingTID;
    //pthread_t addMusicTID, nextFolderTID, previousFolderTID, nextSongTID, previousSongTID;
    obtain(&playlist, "MusicQueue", 100);
    GError *error = NULL;

    gtk_init(&argc, &argv);

    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new ();
    if (gtk_builder_add_from_file (builder, "MusicBox.ui", &error) == 0)
    {
        g_printerr ("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
    }

    /* Connect signal handlers to the constructed widgets. */
    window = gtk_builder_get_object(builder, "window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    entryPath = gtk_builder_get_object(builder, "entryPath");
    g_object_set_data(G_OBJECT(window), "entryPath", entryPath);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entryPath),"4.1.0 Write the main Music folder");

    tvwSongs = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tvwSongs"));
    rndrSong = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "rndrSong"));
    tvwcTitle = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "tvwcTitle"));
    gtk_tree_view_column_add_attribute(tvwcTitle, rndrSong, "text", 0);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tvwSongs), tvwcTitle);
    songSelection = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "songSelection"));


    strcpy(mainFolder, "/media/videeki/Adatok/Zene/");
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

    btnTurnON = gtk_builder_get_object (builder, "btnTurnON");
    g_signal_connect(btnTurnON, "clicked", G_CALLBACK(turnON), NULL);

    btnTurnOFF = gtk_builder_get_object (builder, "btnTurnOFF");
    g_signal_connect(btnTurnOFF, "clicked", G_CALLBACK(turnOFF), NULL);


    pthread_create(&musicTID, NULL, musicPlayer, NULL);
    pthread_create(&gpioHandlingTID, NULL, gpioHandling, NULL);
    //pthread_create(&addMusicTID, NULL, addMusicGPIO, NULL);
    //pthread_create(&nextFolderTID, NULL, nextFolderGPIO, NULL);
    //pthread_create(&previousFolderTID, NULL, previousFolderGPIO, NULL);
    //pthread_create(&nextSongTID, NULL, nextSongGPIO, NULL);
    //pthread_create(&previousSongTID, NULL, previousSongGPIO, NULL);


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

        rewinddir(pDir);
        
        closedir (pDir);

        gtk_tree_view_set_model(GTK_TREE_VIEW(tvwSongs), GTK_TREE_MODEL(lsSongs));


        //gtk_tree_model_get_iter_first(GTK_TREE_MODEL(lsSongs), &nulliter);
        currSongIndex = 0;
        gtk_tree_selection_select_iter(GTK_TREE_SELECTION(songSelection), &actIter[currSongIndex]);

        gchar *value;
        gtk_tree_model_get(GTK_TREE_MODEL(lsSongs), &actIter[currSongIndex], 0, &value, -1);

        gtk_entry_set_text(entryPath, value);

    }
    
    return 0;
}

static void stepFolderNext(GtkWidget *widget, gpointer data)
{   
    currFolderIndex++;
    strcpy(initFolder, mainFolder);
    strcat(initFolder, folderNames[currFolderIndex]);

    populateList(initFolder);
}

static void stepFolderPrevious(GtkWidget *widget, gpointer data)
{
    currFolderIndex--;
    strcpy(initFolder, mainFolder);
    strcat(initFolder, folderNames[currFolderIndex]);

    populateList(initFolder);
}

static void stepSoundNext(GtkWidget *widget, gpointer data)
{   
    currSongIndex++;
    gtk_tree_selection_select_iter(GTK_TREE_SELECTION(songSelection), &actIter[currSongIndex]);

    gchar *value;
    gtk_tree_model_get(GTK_TREE_MODEL(lsSongs), &actIter[currSongIndex], 0, &value, -1);
    
    gtk_entry_set_text(entryPath, value);
}

static void stepSoundPrevious(GtkWidget *widget, gpointer data)
{   
    currSongIndex--;
    if(currSongIndex < 0)
    {
        currSongIndex = 0;
    }
    gtk_tree_selection_select_iter(GTK_TREE_SELECTION(songSelection), &actIter[currSongIndex]);

    gchar *value;
    gtk_tree_model_get(GTK_TREE_MODEL(lsSongs), &actIter[currSongIndex], 0, &value, -1);
    
    gtk_entry_set_text(entryPath, value);
}

static void addMusic2Playlist(GtkWidget *widget, gpointer data)
{
    const char* input = gtk_entry_get_text(GTK_ENTRY(entryPath));
    strcpy(title, initFolder);
    strcat(title, "/");
    strcat(title, input);
    enqueue(&playlist, title);
}

static void addMusic2PlaylistGPIO()
{
    const char* input = gtk_entry_get_text(GTK_ENTRY(entryPath));
    strcpy(title, initFolder);
    strcat(title, "/");
    strcat(title, input);
    enqueue(&playlist, title);
}

static void turnON(GtkWidget *widget, gpointer data)
{
  int LED = 23;
  char mode[] = "out";

  initPIN(LED);
  setupPIN(LED, mode);
  writePIN(LED, ON);
  deinitPIN(LED);

}

static void turnOFF(GtkWidget *widget, gpointer data)
{
  int LED = 23;
  char mode[] = "out";

  initPIN(LED);
  setupPIN(LED, mode);
  writePIN(LED, OFF);
  deinitPIN(LED);
}

static void musicPlayer()
{
    //puts("Music initialization");
    initMusic();
    
    while(run)
    {
        if(nrOfElements(&playlist) == 0)
        {
            sleep(1);
        }
        else
        {   
            playMusic(dequeue(&playlist));
        }
        
    }
    
    //puts("Music closed");
    closeMusic();
}

static void gpioHandling()
{
    puts("Start, polling");
    struct gpiohandle_request req;
    int ctrlPINS[5] = {23, 24, 25, 26, 27};
    int ctrlPINSValues[5] = {0, 0, 0, 0, 0};
    int regPINSValeus[5] = {0,0,0,0,0};
    int i;
    //int sum = 0;
    printf("GPIO init: %d\n", initGPIO(&req, ctrlPINS, 5, INPUT));

    while (run)
    {   
        int pushed = -1;
        int released = -1;
        do  //Polling
        {
            readGPIO(&req, ctrlPINSValues);
            usleep(100000);
            if(addArrayElements(ctrlPINSValues, sizeof(ctrlPINSValues)/sizeof(int)) != 0)
            {
                regPINSValeus[0] = ctrlPINSValues[0];
                regPINSValeus[1] = ctrlPINSValues[1];
                regPINSValeus[2] = ctrlPINSValues[2];
                regPINSValeus[3] = ctrlPINSValues[3];
                regPINSValeus[4] = ctrlPINSValues[4];
                pushed = 1;
                do
                {
                    readGPIO(&req, ctrlPINSValues);
                    usleep(100000);
                    if(addArrayElements(ctrlPINSValues, sizeof(ctrlPINSValues)/sizeof(int)) == 0)
                    {
                        released = 1;
                    }

                } while (released != 1);
            }

        } while (pushed != 1 && released != 1);
        
        int sum = 0;
        for(i = 0; i < sizeof(ctrlPINS)/sizeof(int); i++)
        {
            regPINSValeus[i] *= (int)pow((double)2, (double)i);
            sum += regPINSValeus[i];
        }
        printf("Sum Value: %d\n", sum);
        switch(sum)
        {
            case 1:     //addMusicGPIO
            {
                const char* input = gtk_entry_get_text(GTK_ENTRY(entryPath));
                strcpy(title, initFolder);
                strcat(title, "/");
                strcat(title, input);
                enqueue(&playlist, title);

                break;
            }

            case 2:     //nextFolderGPIO
            {
                currFolderIndex++;
                strcpy(initFolder, mainFolder);
                strcat(initFolder, folderNames[currFolderIndex]);

                populateList(initFolder);

                break;
            }

            case 4:     //previousFolderGPIO
            {
                currFolderIndex--;
                strcpy(initFolder, mainFolder);
                strcat(initFolder, folderNames[currFolderIndex]);

                populateList(initFolder);

                break;
            }

            case 8:     //nextSongGPIO
            {
                currSongIndex++;
                gtk_tree_selection_select_iter(GTK_TREE_SELECTION(songSelection), &actIter[currSongIndex]);

                gchar *value;
                gtk_tree_model_get(GTK_TREE_MODEL(lsSongs), &actIter[currSongIndex], 0, &value, -1);

                gtk_entry_set_text(entryPath, value);

                break;
            }

            case 16:    //previousSongGPIO
            {
                currSongIndex--;
                if(currSongIndex < 0)
                {
                    currSongIndex = 0;
                }
                gtk_tree_selection_select_iter(GTK_TREE_SELECTION(songSelection), &actIter[currSongIndex]);

                gchar *value;
                gtk_tree_model_get(GTK_TREE_MODEL(lsSongs), &actIter[currSongIndex], 0, &value, -1);

                gtk_entry_set_text(entryPath, value);

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
}

static void addMusicGPIO()
{
    int pinNr = 23;
    //initPIN(pinNr);
    //setupPIN(pinNr, "IN");
    while(run)
    {
        if(pollGPIO(pinNr))
        //if(readPIN(pinNr))
        {
            const char* input = gtk_entry_get_text(GTK_ENTRY(entryPath));
            strcpy(title, initFolder);
            strcat(title, "/");
            strcat(title, input);
            enqueue(&playlist, title);
        }
    }
    //deinitPIN(pinNr);
}

static void nextFolderGPIO()
{
    int pinNr = 24;
    //initPIN(pinNr);
    //setupPIN(pinNr, "IN");
    while(run)
    {
        if(pollGPIO(pinNr))
        //if(readPIN(pinNr))
        {
            currFolderIndex++;
            strcpy(initFolder, mainFolder);
            strcat(initFolder, folderNames[currFolderIndex]);

            populateList(initFolder);
        }
    }
    //deinitPIN(pinNr);
}

static void previousFolderGPIO()
{
    int pinNr = 25;
    //initPIN(pinNr);
    //setupPIN(pinNr, "IN");
    while(run)
    {
        if(pollGPIO(pinNr))
        //if(readPIN(pinNr))
        {
            currFolderIndex--;
            strcpy(initFolder, mainFolder);
            strcat(initFolder, folderNames[currFolderIndex]);

            populateList(initFolder);
        }
    }
    //deinitPIN(pinNr);
}

static void nextSongGPIO()
{
    int pinNr = 26;
    //initPIN(pinNr);
    //setupPIN(pinNr, "IN");
    while(run)
    {
        if(pollGPIO(pinNr))
        //if(readPIN(pinNr))
        {
            currSongIndex++;
            gtk_tree_selection_select_iter(GTK_TREE_SELECTION(songSelection), &actIter[currSongIndex]);

            gchar *value;
            gtk_tree_model_get(GTK_TREE_MODEL(lsSongs), &actIter[currSongIndex], 0, &value, -1);
    
            gtk_entry_set_text(entryPath, value);
        }
    }
    //deinitPIN(pinNr);
}

static void previousSongGPIO()
{
    int pinNr = 27;
    //initPIN(pinNr);
    //setupPIN(pinNr, "IN");
    while(run)
    {
        if(pollGPIO(pinNr))
        //if(readPIN(pinNr))
        {
            currSongIndex--;
            if(currSongIndex < 0)
            {
                currSongIndex = 0;
            }
            gtk_tree_selection_select_iter(GTK_TREE_SELECTION(songSelection), &actIter[currSongIndex]);

            gchar *value;
            gtk_tree_model_get(GTK_TREE_MODEL(lsSongs), &actIter[currSongIndex], 0, &value, -1);
    
            gtk_entry_set_text(entryPath, value);
        }
    }
    //deinitPIN(pinNr);
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