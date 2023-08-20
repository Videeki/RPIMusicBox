// Compiling: gcc `pkg-config --cflags gtk+-3.0` -o Builds/UI UI.c `pkg-config --libs gtk+-3.0` -lmpg123 -lao -lpthread


// Standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// UI
#include <gtk/gtk.h>

// Sound
#include <ao/ao.h>
#include <mpg123.h>

// Thread
#include <pthread.h>

// Folder structure
#include <dirent.h>

#define ON 1
#define OFF 0
#define BITS 8

int currFolderIndex = 0;
char** folderNames;
enum {SONG_COLUMN, N_COLUMNS};

int initPIN(int pinNr);
int setupPIN(int pinNr, char *mode);
int writePIN(int pinNr, int value);
int readPIN(int pinNr);
int deinitPIN(int pinNr);
int playMusic(char *argv);
int readFile(char* argv);
int countFolders(char* path);
int getFolders(char* path);

GObject *entryPath;

GtkListStore* populateList(char* path)
{
    GtkListStore* varListStore;
    GtkTreeIter iter;

    struct dirent *pDirent;
    DIR *pDir;

    //rewinddir(pDir);

    varListStore = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);

    // Ensure we can open directory.
    pDir = opendir (path);
    if(pDir == NULL)
    {
        printf("Cannot open directory '%s'\n", path);
    }
    else
    {
        while ((pDirent = readdir(pDir)) != NULL)
        {   
            if(strstr(pDirent->d_name, ".mp3"))
            {
                gtk_list_store_append(varListStore, &iter);
                gtk_list_store_set(varListStore, &iter, SONG_COLUMN, pDirent->d_name, -1);
            }
        }

        rewinddir(pDir);
        
        closedir (pDir);
    }
    
    return varListStore;
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

static void play(GtkWidget *widget, gpointer data)
{
  //buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
  const char *input = gtk_entry_get_text(GTK_ENTRY(entryPath));
  pthread_t tid;
  pthread_create(&tid, NULL, playMusic, input);
  
  //playMusic("/media/videeki/Adatok/Zene/Vegyes/02_hirado.mp3");
  printf("%s\n", input);
  //playMusic(input);
}


int main(int argc, char *argv[])
{
    readFile("config.ini");

    GtkBuilder *builder;
    GObject *window;
    GObject *btnFolderUP, *btnFolderDOWN, *btnSongUP, *btnSongDOWN, *btnAddMusic, *btnTurnON, *btnTurnOFF;

    GtkListStore *lsSongs;
    
    GtkTreeView *tvwSongs;
    GtkTreeViewColumn *tvwcTitle;
    GtkCellRenderer *rndrSong;
    GtkTreeSelection *selectSong;

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
    gtk_entry_set_placeholder_text(GTK_ENTRY(entryPath),"02 Write the main Music folder");


    char* mainFolder = "/media/videeki/Adatok/Zene/";
    int nrOfFolders = countFolders(mainFolder);
    printf("Number of folders: %d\n", nrOfFolders);
    
    getFolders(mainFolder);

    char initFolder[255];
    strcpy(initFolder, mainFolder);

    strcat(initFolder, folderNames[currFolderIndex]);
    printf("Selected folder name: %s\n", folderNames[currFolderIndex]);
    puts(initFolder);

    lsSongs = populateList("/media/videeki/Adatok/Zene/Vegyes");
    //lsSongs = populateList(mainFolder);
    //lsSongs = populateList(initFolder);
    
    tvwSongs = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tvwSongs"));
    rndrSong = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "rndrSong"));
    tvwcTitle = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "tvwcTitle"));
    gtk_tree_view_column_add_attribute(tvwcTitle, rndrSong, "text", 0);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tvwSongs), tvwcTitle);

    gtk_tree_view_set_model(GTK_TREE_VIEW(tvwSongs), GTK_TREE_MODEL(lsSongs));

    g_object_unref(lsSongs);


    btnFolderUP = gtk_builder_get_object (builder, "btnFolderUP");
    //g_signal_connect(btnFolderUP, "clicked", G_CALLBACK(), NULL);
  
    btnFolderDOWN = gtk_builder_get_object (builder, "btnFolderDOWN");
    //g_signal_connect(btnFolderDOWN, "clicked", G_CALLBACK(), NULL);

    btnSongUP = gtk_builder_get_object (builder, "btnSongUP");
    //g_signal_connect(btnSongUP, "clicked", G_CALLBACK(), NULL);  

    btnSongDOWN = gtk_builder_get_object (builder, "btnSongDOWN");
    //g_signal_connect(btnSongDOWN, "clicked", G_CALLBACK(), NULL);  


    btnAddMusic = gtk_builder_get_object (builder, "btnAddMusic");
    g_signal_connect(btnAddMusic, "clicked", G_CALLBACK(play), NULL);

    btnTurnON = gtk_builder_get_object (builder, "btnTurnON");
    g_signal_connect(btnTurnON, "clicked", G_CALLBACK(turnON), NULL);

    btnTurnOFF = gtk_builder_get_object (builder, "btnTurnOFF");
    g_signal_connect(btnTurnOFF, "clicked", G_CALLBACK(turnOFF), NULL);


    gtk_main ();

    return 0;
}

int initPIN(int pinNr)
{
    char initPin[33];
    FILE *export;
    export = fopen("/sys/class/gpio/export", "w");

    if (export == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(export, "%d", pinNr);
    fclose(export);

    sleep(1);

    return 0;
}

int setupPIN(int pinNr, char *mode)
{
    
    char setupPin[50];
    FILE *direction;
    sprintf(setupPin, "/sys/class/gpio/gpio%d/direction", pinNr);
    direction = fopen(setupPin, "w");

    if (direction == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(direction, "%s", mode);
    fclose(direction);
       
    return 0;
}

int writePIN(int pinNr, int value)
{
    char pinPath[50];
    FILE *writePin;
    sprintf(pinPath, "/sys/class/gpio/gpio%d/value", pinNr);
    writePin = fopen(pinPath, "w");

    if (writePin == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(writePin, "%d", value);
    fclose(writePin);
    
    return 0;
}

int readPIN(int pinNr)
{
    int value;
    char pinPath[50];
    FILE *readPin;
    sprintf(pinPath, "/sys/class/gpio/gpio%d/value", pinNr);
    
    readPin = fopen(pinPath, "r");
    
    if (readPin == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fscanf(readPin, "%d", &value);
    
    fclose(readPin);
   
    return value;
}

int deinitPIN(int pinNr)
{
    char deinitPin[35];
    FILE *unexport;
    unexport = fopen("/sys/class/gpio/unexport", "w");

    if (unexport == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(unexport, "%d", pinNr);
    fclose(unexport);

    return 0;
}

int playMusic(char *argv)
{
    mpg123_handle *mh;
    unsigned char *buffer;
    size_t buffer_size;
    size_t done;
    int err;

    int driver;
    ao_device *dev;

    ao_sample_format format;
    int channels, encoding;
    long rate;

    /* initializations */
    ao_initialize();
    driver = ao_default_driver_id();
    mpg123_init();
    mh = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

    /* open the file and get the decoding format */
    mpg123_open(mh, argv);
    mpg123_getformat(mh, &rate, &channels, &encoding);

    /* set the output format and open the output device */
    format.bits = mpg123_encsize(encoding) * BITS;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    dev = ao_open_live(driver, &format, NULL);

    /* decode and play */
    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
        ao_play(dev, buffer, done);

    /* clean up */
    free(buffer);
    ao_close(dev);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    ao_shutdown();

    return 0;
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

int countFolders(char* path)
{
    struct dirent *pDirent;
    DIR *pDir;
    int count = 0;
    // Ensure we can open directory.

    pDir = opendir (path);
    if (pDir == NULL) {
        printf ("Cannot open directory '%s'\n", path);
        return -1;
    }

    // Process each entry.

    while ((pDirent = readdir(pDir)) != NULL)
    {
        count++;
    }

    rewinddir(pDir);

    closedir (pDir);
    
    return count;
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
    
    folderNames = calloc(i, sizeof(char *));  

    for(int j = 0; j < i; j++)
    {
        pDirent = readdir(pDir);
        char dirName[255];
        
        strcpy(dirName, pDirent->d_name);
        int foundDot = strcspn(dirName, ".");
        if(foundDot == strlen(dirName) && foundDot != 1)
        {
            folderNames[j] = calloc(1, sizeof(char *)*strlen(dirName));
            strcpy(folderNames[j], dirName);
        }
        
    }

    closedir (pDir);
    return i;
}