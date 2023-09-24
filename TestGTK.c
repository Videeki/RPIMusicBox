/****************************************************************************************************************
*   Compiling:                                                                                                  *
*                                                                                                               *
*    Linux:                                                                                                     *
*    gcc `pkg-config --cflags gtk+-3.0` -o Builds/TestGTK TestGTK.c `pkg-config --libs gtk+-3.0`                *
*                                                                                                               *
****************************************************************************************************************/
 

// Standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>

/* Define the columns of the list */
enum { NAME_COLUMN, OFFSET_COLUMN, SIZE_COLUMN, N_COLUMNS };


struct songTreeView
{
    GtkWidget* songView;
    GtkListStore* songListStore;
};

/* Create a new GtkListStore and fill it with some data */
void end_program(GtkWidget *wid, gpointer ptr)
{
    gtk_main_quit();
}

GtkListStore* init_list_store(GtkWidget *view)
{
    GtkListStore* store;
    GtkTreeIter iter;

    store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, NAME_COLUMN, "test name", OFFSET_COLUMN, 0, SIZE_COLUMN, 10, -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, NAME_COLUMN, "another name", OFFSET_COLUMN, 20, SIZE_COLUMN, 15, -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, NAME_COLUMN, "test name", OFFSET_COLUMN, 0, SIZE_COLUMN, 10, -1);

    /* Set the model of the tree view to the list store */
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(store));

    g_object_unref(store);

    return store;
}

GtkListStore* previous_list_store(GtkWidget *widget, gpointer data)
{
    GtkListStore* store;
    GtkTreeIter iter;

    store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, NAME_COLUMN, "test name", OFFSET_COLUMN, 0, SIZE_COLUMN, 10, -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, NAME_COLUMN, "another name", OFFSET_COLUMN, 20, SIZE_COLUMN, 15, -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, NAME_COLUMN, "test name", OFFSET_COLUMN, 0, SIZE_COLUMN, 10, -1);

    /* Set the model of the tree view to the list store */
    gtk_tree_view_set_model(GTK_TREE_VIEW(data), GTK_TREE_MODEL(store));

    g_object_unref(store);

    return store;
}

GtkListStore* next_list_store(GtkWidget *widget, gpointer data)
{
    GtkTreeIter iter;
    GtkListStore* store;
    store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
    
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, NAME_COLUMN, "test name 2", OFFSET_COLUMN, 42, SIZE_COLUMN, 24, -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, NAME_COLUMN, "another name 2", OFFSET_COLUMN, 69, SIZE_COLUMN, 96, -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, NAME_COLUMN, "test name 2", OFFSET_COLUMN, 666, SIZE_COLUMN, 666, -1);
  
    /* Set the model of the tree view to the list store */
    gtk_tree_view_set_model(GTK_TREE_VIEW(data), GTK_TREE_MODEL(store));

    g_object_unref(store);

    return store;
}

/* Create a new GtkTreeView and add some columns to it */
GtkWidget* create_tree_view()
{
    GtkWidget* view; GtkCellRenderer* renderer; GtkTreeViewColumn* column;

    view = gtk_tree_view_new();
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", NAME_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Offset", renderer, "text", OFFSET_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Size", renderer, "text", SIZE_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    return view;
}

int main(int argc, char* argv[])
{ 
    GtkWidget* window;
    GtkWidget* vbox;
    GtkWidget* label;
    GtkWidget* sw;
    GtkWidget* button;

    GtkWidget* view;
    GtkListStore* store;

    /* Initialize GTK */
    gtk_init(&argc, &argv);

    /* Create the main window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK List Example");
    

    /* Connect the destroy signal to quit the application */
    g_signal_connect(window, "destroy", G_CALLBACK(end_program), NULL);

    gtk_container_set_border_width(GTK_CONTAINER(window), 8);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    label = gtk_label_new("This is the test list");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
    
    sw = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_ETCHED_IN);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), sw, TRUE, TRUE, 0);


    /* Create the list store and the tree view */
    view = create_tree_view();
    store = init_list_store(view);

    /* Add the tree view to the window */
    gtk_container_add(GTK_CONTAINER(sw), view);

    button = gtk_button_new_with_label("Next");
    g_signal_connect(button, "clicked", G_CALLBACK(next_list_store), view);
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

    button = gtk_button_new_with_label("Previous");
    g_signal_connect(button, "clicked", G_CALLBACK(previous_list_store), view);
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

    gtk_window_set_default_size(GTK_WINDOW(window), 300, 500);

    /* Show the window and start the main loop */
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}