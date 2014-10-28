#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "socks.h"
#include "proto.h"

static struct server serv;
static GtkWidget* image;
static GdkPixbuf* pixbuf;
#define WIDTH 200
#define HEIGHT 200
#define PIX_BYTES (WIDTH*HEIGHT*3)
unsigned char pixels[PIX_BYTES];

static void close_window(GtkWidget* widget, gpointer data)
{
  send_code(serv.fd, PROTO_STOP);
  gtk_main_quit();
}

static void click(GtkWidget* widget, gpointer data)
{
  server_accept(&serv);
  send_code(serv.fd, PROTO_START);
  blocking_recv(serv.fd, pixels, PIX_BYTES);
  gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
}

int main(int argc, char** argv)
{
  GtkWidget* window;
  GtkWidget* button;
  GtkWidget* vbox;
  if (argc != 2) {
    printf("usage: %s port\n",argv[0]);
    return 0;
  }
  server_init(&serv, atoi(argv[1]));
  server_listen(&serv);
  gtk_init(&argc, &argv);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);
  button = gtk_button_new_with_label("accept");
  g_signal_connect(button, "clicked", G_CALLBACK(click), NULL);
  memset(pixels, 0, PIX_BYTES);
  pixbuf = gdk_pixbuf_new_from_data(pixels, GDK_COLORSPACE_RGB,
      FALSE, 8, WIDTH, HEIGHT, WIDTH * 3, NULL, NULL);
  image = gtk_image_new_from_pixbuf(pixbuf);
  vbox = gtk_vbox_new(FALSE, 4);
  gtk_container_add(GTK_CONTAINER(vbox), button);
  gtk_container_add(GTK_CONTAINER(vbox), image);
  gtk_container_add(GTK_CONTAINER(window), vbox);
  gtk_widget_show_all(window);
  gtk_main();
  server_close(&serv);
  return 0;
}
