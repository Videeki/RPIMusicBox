#include <gtk/gtk.h>
#include <glib.h>
#include "GUIHandler.h"

GError *error = NULL;

GUI* initGUI()
{
    GUI* self = (GUI*)malloc(sizeof(GUI));
    return self;
}

GUI* initGUIDashboard(GUI* self, const char* path, int argc, char** argv)
{
    gtk_init(&argc, &argv);

    /* Construct a GtkBuilder instance and load our UI description */
    GtkBuilder* builder = gtk_builder_new();
    if (gtk_builder_add_from_file(builder, path, &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return NULL;
    }

    /* Connect signal handlers to the constructed widgets. */
    GObject* window = gtk_builder_get_object(builder, "window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GObject* entryPath = gtk_builder_get_object(builder, "entryPath");
    g_object_set_data(G_OBJECT(window), "entryPath", entryPath);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entryPath),"Music Title");

    self->folderName = gtk_builder_get_object(builder, "folderName");
    g_object_set_data(G_OBJECT(window), "folderName", self->folderName);

    self->songSW = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "songSW"));
    self->tvwSongs = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tvwSongs"));
    GtkCellRenderer* rndrSong = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "rndrSong"));
    GtkTreeViewColumn* tvwcTitle = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "tvwcTitle"));
    gtk_tree_view_column_add_attribute(tvwcTitle, rndrSong, "text", 0);
    //gtk_tree_view_append_column(GTK_TREE_VIEW(tvwSongs), tvwcTitle);
    
    self->songSelection = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "songSelection"));
    g_signal_connect(self->songSelection,"changed",G_CALLBACK(scroll_to_selection),self->tvwSongs);
    

    GObject* btnFolderUP = gtk_builder_get_object (builder, "btnFolderUP");
    g_signal_connect(btnFolderUP, "clicked", G_CALLBACK(self->stepFolderNext), self);
  
    GObject* btnFolderDOWN = gtk_builder_get_object (builder, "btnFolderDOWN");
    g_signal_connect(btnFolderDOWN, "clicked", G_CALLBACK(self->stepFolderPrevious), self);

    GObject* btnSongUP = gtk_builder_get_object (builder, "btnSongUP");
    g_signal_connect(btnSongUP, "clicked", G_CALLBACK(self->stepSoundPrevious), self);  

    GObject* btnSongDOWN = gtk_builder_get_object (builder, "btnSongDOWN");
    g_signal_connect(btnSongDOWN, "clicked", G_CALLBACK(self->stepSoundNext), self);  

    GObject* btnAddMusic = gtk_builder_get_object (builder, "btnAddMusic");
    g_signal_connect(btnAddMusic, "clicked", G_CALLBACK(self->addMusic2Playlist), self);

    return self;
}


GUI* updateGUITrackList(GUI* self, GList* trackList)
{
    GtkTreeIter iter;
    if(self->actIter != NULL)
    {
        free(self->actIter);
        self->actIter = NULL;
    }

    int nrOfSongs = g_list_length(trackList);

    self->actIter = (GtkTreeIter*)malloc(nrOfSongs * sizeof(typeof(GtkTreeIter)));

    if(self->lsSongs != NULL)
    {
        g_object_unref(self->lsSongs);
    }
    
    self->lsSongs = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);

    for(GList* track = trackList; track != NULL; track = trackList->next)
    {   
        gtk_list_store_append(self->lsSongs, self->actIter);
        gtk_list_store_set(self->lsSongs, self->actIter, SONG_COLUMN, (char*)track->data, -1);
        self->actIter++;
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(self->tvwSongs), GTK_TREE_MODEL(self->lsSongs));

    self->currSongIndex = 0;
    if(nrOfSongs != 0)
    {
        gtk_tree_selection_select_iter(GTK_TREE_SELECTION(self->songSelection), &self->actIter[self->currSongIndex]);

        gchar *value;
        GtkTreeIter* actIterTemp;
        memcpy(actIterTemp, &self->actIter[self->currSongIndex], sizeof(typeof(GtkTreeIter)));
        gtk_tree_model_get(GTK_TREE_MODEL(self->lsSongs), actIterTemp, 0, &value, -1);
        //puts(value);

        //strcpy(self->musicPath, value);

        //gtk_scrolled_window_get_vadjustment(self->songSW);
    }

    return self;
}


GUI* updateGUIAlbum(GUI* self, const char* title)
{
    return self;
}


GUI* updateGUITrack(GUI* self, const char* title)
{
    return self;
}


void scroll_to_selection(GtkTreeSelection *selection,gpointer user_data)
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

void freeGUI(GUI* self)
{
    free(self->actIter);
    self->actIter = NULL;
    free(self);
    self = NULL;
}
