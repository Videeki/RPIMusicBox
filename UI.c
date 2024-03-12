// Compiling: gcc `pkg-config --cflags gtk+-3.0` -o Builds/UI UI.c `pkg-config --libs gtk+-3.0` -IGPIOHandler "./GPIOHandler/GPIOHandler.c" -IQueue "./Queue/Queue.c" -IpRegex "./pRegex/pRegex.c" -IConfigFile "./ConfigFile/ConfigFile.c" -IPlayMusic "./PlayMusic/PlayMusic.c" -lmpg123 -lao -lpthread -lm


// Standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <limits.h>


#include <gtk/gtk.h>        // UI
#include <pthread.h>        // Thread
#include <dirent.h>         // Folder structure
#include <regex.h>          // Regex


#include "pRegex.h"         // Find Match function
#include "ConfigFile.h"     // Config
#include "GPIOHandler.h"    // GPIO
#include "PlayMusic.h"      // Sound
#include "Queue.h"          // Queue


#define ON 1
#define OFF 0
#define BITS 8
#define QUEUE_SIZE 255
#define PATH_LENGTH 1024

//GtkBuilder *builder;
//GObject *window;
//GObject *btnFolderUP, *btnFolderDOWN, *btnSongUP, *btnSongDOWN, *btnAddMusic;
//GObject *entryPath;

typedef struct _MusicBoxUI{
    GObject *folderName;
    GtkListStore *lsSongs;
    GtkScrolledWindow *songSW;
    GtkTreeView *tvwSongs;
    GtkTreeSelection *songSelection;
    GtkTreeIter *actIter;

    config* configINI;

    int currFolderIndex;
    int currSongIndex;
    int nrOfFolders;
    int nrOfSongs;

    char** folderNames;
    char mainFolder[PATH_LENGTH];
    char initFolder[PATH_LENGTH];
    char title[PATH_LENGTH];
    char musicPath[PATH_LENGTH];
}MusicBoxUI;


//GObject *folderName;

//GtkListStore *lsSongs;
//GtkScrolledWindow *songSW;
//GtkAdjustment *listAdjustment;

//GtkTreeView *tvwSongs;
//GtkTreeViewColumn *tvwcTitle;
//GtkCellRenderer *rndrSong;
//GtkTreeSelection *songSelection;
//GtkTreeIter iter, nulliter;

//GtkTreeIter actIter[255];

//int currFolderIndex = 0;
//int currSongIndex = 0;
//int nrOfFolders = 0;
//int nrOfSongs = 0;

//char** folderNames;
//char mainFolder[PATH_LENGTH];
//char initFolder[PATH_LENGTH];
//char title[PATH_LENGTH];
//char musicPath[PATH_LENGTH];
enum {SONG_COLUMN, N_COLUMNS};

string_Queue_t playlist;
int run = 1;

int populateList(MusicBoxUI* UIData);

static void scroll_to_selection(GtkTreeSelection *selection,gpointer user_data);
static void stepFolderNext(MusicBoxUI* UIData);
static void stepFolderPrevious(MusicBoxUI* UIData);
static void stepSoundNext(MusicBoxUI* UIData);
static void stepSoundPrevious(MusicBoxUI* UIData);
static void addMusic2Playlist(MusicBoxUI* UIData);

//static void* musicPlayer(GObject* entryPath);
static void* musicPlayer(void *entryPath);
//static void* gpioHandling();
static void* gpioHandling(MusicBoxUI* UIData);

int readFile(const char* argv);
int getFolders(MusicBoxUI* UIData);
//int addArrayElements(int *intArray, int arraySize);


int main(int argc, char *argv[])
{
    //readFile("config.ini");
    MusicBoxUI UIData;

    UIData.currFolderIndex = 0;
    UIData.currSongIndex = 0;
    UIData.nrOfFolders = 0;
    UIData.nrOfSongs = 0;

    GtkTreeIter actIter[255];
    UIData.actIter = actIter;

    pthread_t musicTID;
    pthread_t gpioHandlingTID;
    obtain(&playlist, "MusicQueue", 100);
    GError *error = NULL;

    config configINI;
    UIData.configINI = &configINI;

    char* filename = "config.ini";
    char* configSection = "Default";

    char configPath[PATH_LENGTH];
    getcwd(configPath, sizeof(configPath));

    //char* configStr = openConfig(filename);
    strcat(configPath, "/");
    strcat(configPath, filename);
    puts(configPath);
    strcpy(configPath, "/home/videeki/Documents/GitRepos/RPIMusicBox/Builds/config.ini");
    puts(configPath);
    char* configStr = openConfig(configPath);
    parseConfig(configStr, UIData.configINI);
    closeConfig(configStr);


    gtk_init(&argc, &argv);

    /* Construct a GtkBuilder instance and load our UI description */
    GtkBuilder* builder = gtk_builder_new();
    if (gtk_builder_add_from_file (builder, readKey(UIData.configINI, "Default", "Theme"), &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
    }

    /* Connect signal handlers to the constructed widgets. */
    GObject* window = gtk_builder_get_object(builder, "window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GObject* entryPath = gtk_builder_get_object(builder, "entryPath");
    g_object_set_data(G_OBJECT(window), "entryPath", entryPath);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entryPath),"Music Title");

    UIData.folderName = gtk_builder_get_object(builder, "folderName");
    g_object_set_data(G_OBJECT(window), "folderName", UIData.folderName);

    UIData.songSW = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "songSW"));
    UIData.tvwSongs = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tvwSongs"));
    GtkCellRenderer* rndrSong = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "rndrSong"));
    GtkTreeViewColumn* tvwcTitle = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "tvwcTitle"));
    gtk_tree_view_column_add_attribute(tvwcTitle, rndrSong, "text", 0);
    //gtk_tree_view_append_column(GTK_TREE_VIEW(tvwSongs), tvwcTitle);
    
    UIData.songSelection = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "songSelection"));
    g_signal_connect(UIData.songSelection,"changed",G_CALLBACK(scroll_to_selection),UIData.tvwSongs);


    strcpy(UIData.mainFolder, readKey(&configINI, configSection, "MusicFolder"));
    getFolders(&UIData);

    strcpy(UIData.initFolder, UIData.mainFolder);
    strcat(UIData.initFolder, UIData.folderNames[UIData.currFolderIndex]);
    populateList(&UIData);
    

    GObject* btnFolderUP = gtk_builder_get_object (builder, "btnFolderUP");
    g_signal_connect(btnFolderUP, "clicked", G_CALLBACK(stepFolderNext), &UIData);
  
    GObject* btnFolderDOWN = gtk_builder_get_object (builder, "btnFolderDOWN");
    g_signal_connect(btnFolderDOWN, "clicked", G_CALLBACK(stepFolderPrevious), &UIData);

    GObject* btnSongUP = gtk_builder_get_object (builder, "btnSongUP");
    g_signal_connect(btnSongUP, "clicked", G_CALLBACK(stepSoundPrevious), &UIData);  

    GObject* btnSongDOWN = gtk_builder_get_object (builder, "btnSongDOWN");
    g_signal_connect(btnSongDOWN, "clicked", G_CALLBACK(stepSoundNext), &UIData);  

    GObject* btnAddMusic = gtk_builder_get_object (builder, "btnAddMusic");
    g_signal_connect(btnAddMusic, "clicked", G_CALLBACK(addMusic2Playlist), &UIData);


    pthread_create(&musicTID, NULL, musicPlayer, (void*)entryPath);
    pthread_create(&gpioHandlingTID, NULL, gpioHandling, (MusicBoxUI*)&UIData);


    gtk_main ();

    flush(&playlist);

    return 0;
}

int populateList(MusicBoxUI* UIData)
{
    GtkTreeIter iter;
    struct dirent *pDirent;
    DIR *pDir;
    int doNothing = 0;

    int i = 0;
    for(i =0; i < 255; i++)
    {
        UIData->actIter[i].stamp = 0;
        UIData->actIter[i].user_data = 0;
        UIData->actIter[i].user_data2 = 0;
        UIData->actIter[i].user_data3 = 0;
    }

    //rewinddir(pDir);
    if(UIData->lsSongs != NULL)
    {
        g_object_unref(UIData->lsSongs);
    }
    

    UIData->lsSongs = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);

    // Ensure we can open directory.
    pDir = opendir (UIData->initFolder);
    if(pDir == NULL)
    {
        printf("Cannot open directory '%s'\n", UIData->initFolder);
    }
    else
    {   
        int i = 0;
        while ((pDirent = readdir(pDir)) != NULL)
        {   
            if(strstr(pDirent->d_name, ".mp3"))
            {
                gtk_list_store_append(UIData->lsSongs, &iter);
                gtk_list_store_set(UIData->lsSongs, &iter, SONG_COLUMN, pDirent->d_name, -1);
                UIData->actIter[i] = iter;
                i++;
            }
        }
        UIData->nrOfSongs = i - 1;

        rewinddir(pDir);
        
        closedir(pDir);

        gtk_tree_view_set_model(GTK_TREE_VIEW(UIData->tvwSongs), GTK_TREE_MODEL(UIData->lsSongs));


        //gtk_tree_model_get_iter_first(GTK_TREE_MODEL(lsSongs), &nulliter);
        UIData->currSongIndex = 0;
        if(i != 0)
        {
            gtk_tree_selection_select_iter(GTK_TREE_SELECTION(UIData->songSelection), &UIData->actIter[UIData->currSongIndex]);

            gchar *value;
            GtkTreeIter* actIterTemp;
            memcpy(actIterTemp, &UIData->actIter[UIData->currSongIndex], sizeof(typeof(GtkTreeIter)));
            gtk_tree_model_get(GTK_TREE_MODEL(UIData->lsSongs), actIterTemp, 0, &value, -1);
            puts(value);

            strcpy(UIData->musicPath, value);

            gtk_scrolled_window_get_vadjustment(UIData->songSW);
        }

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

static void addMusic2Playlist(MusicBoxUI* UIData)
{
    strcpy(UIData->title, UIData->initFolder);
    strcat(UIData->title, "/");
    strcat(UIData->title, UIData->musicPath);
    enqueue(&playlist, UIData->title);
}

static void stepFolderNext(MusicBoxUI* UIData)
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

static void stepFolderPrevious(MusicBoxUI* UIData)
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

static void stepSoundNext(MusicBoxUI* UIData)
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

static void stepSoundPrevious(MusicBoxUI* UIData)
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

static void* musicPlayer(void *entryPath)
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

static void* gpioHandling(MusicBoxUI* UIData)
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
    int regPINSValeus[5] = {0,0,0,0,0};
    int i;

    initGPIO(&req, ctrlPINS, 5, INPUT);

    while (run)
    {   
        detectButtonAction(&req, regPINSValeus, 200);

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
                addMusic2Playlist(UIData);
                //G_CALLBACK(addMusic2Playlist);
                break;
            }

            case 2:     //stepFolderNext
            {
                stepFolderNext(UIData);
                //G_CALLBACK(stepFolderNext);
                break;
            }

            case 4:     //stepFolderPrevious
            {
                stepFolderPrevious(UIData);
                //G_CALLBACK(stepFolderPrevious);
                break;
            }

            case 8:     //stepSoundNext
            {
                stepSoundNext(UIData);
                //G_CALLBACK(stepSoundNext);
                break;
            }

            case 16:    //stepSoundPrevious
            {
                stepSoundPrevious(UIData);
                //G_CALLBACK(stepSoundPrevious);
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

int readFile(const char* argv)
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

int getFolders(MusicBoxUI* UIData)
{
    struct dirent *pDirent;
    DIR *pDir;

    //rewinddir(pDir);

    pDir = opendir (UIData->mainFolder);
    if (pDir == NULL)
    {
        printf ("Cannot open directory '%s'\n", UIData->mainFolder);
        return 1;
    }

    int i = 0;
    // Process each entry.
    while((pDirent = readdir(pDir)) != NULL)
    {
        if(pDirent->d_name[0] != '.')
        {
            i++;
        }
    }
    UIData->nrOfFolders = i;
    printf("Number of folders: %d\n", UIData->nrOfFolders);

    rewinddir(pDir);
    
    UIData->folderNames = (char**)malloc(i * sizeof(char*));  
    int j = 0;
    char dirName[255];
    while ((pDirent = readdir(pDir)) != NULL)
    {   
        strcpy(dirName, pDirent->d_name);
        int foundDot = strcspn(dirName, ".");
        if(pDirent->d_name[0] != '.')
        {
            UIData->folderNames[j] = (char*)malloc(strlen(dirName) * sizeof(char*));
            strcpy(UIData->folderNames[j], dirName);
            j++;
        }
    }
    
    closedir (pDir);
    return i;
}

/*int addArrayElements(int *intArray, int arraySize)
{
    int i;
    int sum = 0;

    for(i = 0; i < arraySize; i++)
    {
        sum += intArray[i];
    }

    return sum;
}
*/