#ifndef GUIHANDLER_H
#define GUIHANDLER_H

#include <gtk/gtk.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct gui
{
    GObject *folderName;
    GtkListStore *lsSongs;
    GtkScrolledWindow *songSW;
    GtkTreeView *tvwSongs;
    GtkTreeSelection *songSelection;
    GtkTreeIter *actIter;
    int currSongIndex;
    
    void (*stepFolderNext)(struct GUI*);
    void (*stepFolderPrevious)(struct GUI*);
    void (*stepSoundNext)(struct GUI*);
    void (*stepSoundPrevious)(struct GUI*);
    void (*addMusic2Playlist)(struct GUI*);
}GUI;


enum {SONG_COLUMN, N_COLUMNS};

GUI* initGUI();
GUI* initGUIDashboard(GUI* self, const char* path, int argc, char** argv);
GUI* updateGUIAlbum(GUI* self, const char* title);
GUI* updateGUITrack(GUI* self, const char* title);
GUI* updateGUITrackList(GUI* self, GList* trackList);
void scroll_to_selection(GtkTreeSelection *selection,gpointer user_data);
void freeGUI(GUI* self);

#endif  /*  GUIHANDLER_H    */