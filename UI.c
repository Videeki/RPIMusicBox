//compiling: gcc `pkg-config --cflags gtk+-3.0` -o Builds/UI UI.c `pkg-config --libs gtk+-3.0`


#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define ON 1
#define OFF 0

int initPIN(int pinNr);
int setupPIN(int pinNr, char *mode);
int writePIN(int pinNr, int value);
int readPIN(int pinNr);
int deinitPIN(int pinNr);

static void print_hello(GtkWidget *widget, gpointer data)
{
  g_print("Hello World\n");
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

static void activate(GtkApplication *app, gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *button;

  /* create a new window, and set its title */
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Window");
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new();

  /* Pack the container in the window */
  gtk_container_add(GTK_CONTAINER(window), grid);

  button = gtk_button_new_with_label("Turn ON");
  //g_signal_connect(button, "clicked", G_CALLBACK(turnON), NULL);
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

  /* Place the first button in the grid cell (0, 0), and make it fill
   * just 1 cell horizontally and vertically (ie no spanning)
   */
  gtk_grid_attach(GTK_GRID(grid), button, 0, 0, 1, 1);

  button = gtk_button_new_with_label("Turn OFF");
  //g_signal_connect(button, "clicked", G_CALLBACK(turnOFF), NULL);
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

  /* Place the second button in the grid cell (1, 0), and make it fill
   * just 1 cell horizontally and vertically (ie no spanning)
   */
  gtk_grid_attach(GTK_GRID(grid), button, 1, 0, 1, 1);

  button = gtk_button_new_with_label("Quit");
  g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), window);

  /* Place the Quit button in the grid cell (0, 1), and make it
   * span 2 columns.
   */
  gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 2, 1);

  /* Now that we are done packing our widgets, we show them all
   * in one go, by calling gtk_widget_show_all() on the window.
   * This call recursively calls gtk_widget_show() on all widgets
   * that are contained in the window, directly or indirectly.
   */
  gtk_widget_show_all(window);

}

int main(int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new("MusicBox", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
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