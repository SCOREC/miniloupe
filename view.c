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
double old_x, old_y;

enum gui_state {
  IDLE,
  SPIN_TILT,
  PAN,
  ZOOM
};

enum gui_state state = IDLE;

enum mouse_buttons {
  LEFT_BUTTON = 1,
  MIDDLE_BUTTON = 2,
  RIGHT_BUTTON = 3
};

static void close_window(GtkWidget* widget, gpointer data)
{
  gtk_main_quit();
}

static void render(void)
{
  send_code(serv.fd, PROTO_RENDER);
  blocking_recv(serv.fd, pixels, PIX_BYTES);
  gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
}

static void accepted(GtkWidget* widget, gpointer data)
{
  server_accept(&serv);
  render();
}

static gboolean pressed(GtkWidget* w, GdkEventButton* event, gpointer u)
{
  if (state != IDLE) {
    return FALSE;
  }
  switch (event->button) {
    case LEFT_BUTTON:
      state = SPIN_TILT;
      break;
    case MIDDLE_BUTTON:
      state = PAN;
      break;
    case RIGHT_BUTTON:
      state = ZOOM;
      break;
  }
  old_x = event->x;
  old_y = event->y;
  return FALSE;
}

static gboolean released(GtkWidget* w, GdkEventButton* event, gpointer u)
{
  double dx, dy;
  dx = event->x - old_x;
  dy = event->y - old_y;
  switch (state) {
    case IDLE:
      return FALSE;
    case ZOOM:
      send_code(serv.fd, PROTO_ZOOM);
      send_double(serv.fd, dy);
      break;
    case PAN:
      send_code(serv.fd, PROTO_PAN);
      send_double(serv.fd, dx);
      send_double(serv.fd, dy);
      break;
    case SPIN_TILT:
      send_code(serv.fd, PROTO_SPIN);
      send_double(serv.fd, dx);
      send_code(serv.fd, PROTO_TILT);
      send_double(serv.fd, dy);
      break;
  }
  state = IDLE;
  render();
  return FALSE;
}

int main(int argc, char** argv)
{
  GtkWidget* window;
  GtkWidget* button;
  GtkWidget* vbox;
  GtkWidget* hbox;
  GtkWidget* event_box;
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
  g_signal_connect(button, "clicked", G_CALLBACK(accepted), NULL);
  memset(pixels, 0, PIX_BYTES);
  pixbuf = gdk_pixbuf_new_from_data(pixels, GDK_COLORSPACE_RGB,
      FALSE, 8, WIDTH, HEIGHT, WIDTH * 3, NULL, NULL);
  image = gtk_image_new_from_pixbuf(pixbuf);
  event_box = gtk_event_box_new();
  gtk_widget_add_events(event_box, GDK_BUTTON_PRESS_MASK |
                                   GDK_BUTTON_RELEASE_MASK);
  g_signal_connect(event_box, "button-press-event", G_CALLBACK(pressed), 0);
  g_signal_connect(event_box, "button-release-event", G_CALLBACK(released), 0);
  vbox = gtk_vbox_new(FALSE, 0);
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
  gtk_container_add(GTK_CONTAINER(event_box), image);
  gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), event_box, FALSE, FALSE, 4);
  gtk_container_add(GTK_CONTAINER(window), hbox);
  gtk_widget_show_all(window);
  gtk_main();
  if (serv.fd != -1) {
    send_code(serv.fd, PROTO_STOP);
    server_close(&serv);
  }
  return 0;
}
