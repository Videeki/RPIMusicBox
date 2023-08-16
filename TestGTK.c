// Compiling: gcc `pkg-config --cflags gtk+-3.0` -o Builds/TestGTK TestGTK.c `pkg-config --libs gtk+-3.0`

// Standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>

/* Define the columns of the list */
enum { NAME_COLUMN, OFFSET_COLUMN, SIZE_COLUMN, N_COLUMNS };

/* Create a new GtkListStore and fill it with some data */
void end_program(GtkWidget *wid, gpointer ptr)
{
    gtk_main_quit();
}

GtkListStore* create_list_store()
{
    GtkListStore* store; GtkTreeIter iter;

    store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, NAME_COLUMN, "test name", OFFSET_COLUMN, 0, SIZE_COLUMN, 10, -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, NAME_COLUMN, "another name", OFFSET_COLUMN, 20, SIZE_COLUMN, 15, -1);

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
    GtkWidget* window; GtkWidget* view; GtkListStore* store;

    /* Initialize GTK */
    gtk_init(&argc, &argv);

    /* Create the main window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK List Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    /* Connect the destroy signal to quit the application */
    g_signal_connect(window, "destroy", G_CALLBACK(end_program), NULL);

    /* Create the list store and the tree view */
    store = create_list_store();
    view = create_tree_view();

    /* Set the model of the tree view to the list store */
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(store));

    /* Unreference the list store to avoid memory leaks */
    g_object_unref(store);

    /* Add the tree view to the window */
    gtk_container_add(GTK_CONTAINER(window), view);

    /* Show the window and start the main loop */
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}