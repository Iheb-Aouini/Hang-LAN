#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "console_hangman/headers.h"
#include <gtk/gtk.h>

// Callback function for when the console mode button is clicked
void on_console_button_clicked(GtkWidget *widget, gpointer data) {
    system("make");
    system("cd console_hangman && ./hangman");
}

// Callback function for when the interface mode button is clicked
void on_interface_button_clicked(GtkWidget *widget, gpointer data) {
    // Interface mode
    system("cd desktop_hangman && python3 desktop_hangman.py");
}

// Callback function for when the web interface button is clicked
void on_web_interface_button_clicked(GtkWidget *widget, gpointer data) {
    system("cd web_hangman && python3 web_hangman.py");
}

// Callback function for when the quit button is clicked
void on_quit_button_clicked(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *console_button, *interface_button, *web_interface_button, *quit_button;
    GtkWidget *hbox;
    GtkWidget *vbox;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Choose Mode");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

    console_button = gtk_button_new_with_label("Console Mode");
    g_signal_connect(console_button, "clicked", G_CALLBACK(on_console_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), console_button, TRUE, TRUE, 0);

    interface_button = gtk_button_new_with_label("Interface Mode");
    g_signal_connect(interface_button, "clicked", G_CALLBACK(on_interface_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), interface_button, TRUE, TRUE, 0);

    web_interface_button = gtk_button_new_with_label("Web Interface");
    g_signal_connect(web_interface_button, "clicked", G_CALLBACK(on_web_interface_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), web_interface_button, TRUE, TRUE, 0);

    quit_button = gtk_button_new_with_label("Quit");
    g_signal_connect(quit_button, "clicked", G_CALLBACK(on_quit_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), quit_button, TRUE, TRUE, 0);

    // Adjust the window size while keeping the button size
    gtk_widget_set_size_request(console_button, 150, 33);
    gtk_widget_set_size_request(interface_button, 150, -1);
    gtk_widget_set_size_request(web_interface_button, 150, -1);
    gtk_widget_set_size_request(quit_button, 150, -1);
    gtk_widget_set_size_request(window, 300, 300);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

