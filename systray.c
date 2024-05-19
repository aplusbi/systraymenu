#include <gtk/gtk.h>

void resize_systray(GtkStatusIcon*, gint, gpointer);
void on_click(GtkStatusIcon*, gpointer);
GtkMenu* create_menu();

void resize_systray(GtkStatusIcon* icon, gint _size, gpointer dummy) {
  gint size = gtk_status_icon_get_size (icon);
  GdkPixbuf* pix = gtk_icon_theme_load_icon(
      gtk_icon_theme_get_default(),
      g_strdup("face-smile"),
      size,
      GTK_ICON_LOOKUP_USE_BUILTIN,
      NULL);
  gtk_status_icon_set_from_pixbuf(icon, pix);
}

void on_click(GtkStatusIcon* icon, gpointer menu) {
  gtk_menu_popup_at_pointer(menu, NULL);
}

GtkMenu* create_menu() {
  GtkMenu* menu = GTK_MENU (gtk_menu_new());
  GtkWidget *shutdown = gtk_menu_item_new_with_label("Shutdown");
  GtkWidget *restart = gtk_menu_item_new_with_label("Restart");
  gtk_widget_show(shutdown);
  gtk_widget_show(restart);
  gtk_menu_shell_append(GTK_MENU_SHELL (menu), restart);
  gtk_menu_shell_append(GTK_MENU_SHELL (menu), shutdown);

  return menu;
}

void create_systray() {
  GtkStatusIcon *icon = gtk_status_icon_new();
  GtkMenu* menu = create_menu();
  gtk_status_icon_set_visible(icon, TRUE);
  resize_systray(icon, 0, NULL);
  g_signal_connect(
      G_OBJECT (icon),
      "size-changed",
      G_CALLBACK (resize_systray),
      NULL);
  g_signal_connect(
      G_OBJECT (icon),
      "activate",
      G_CALLBACK (on_click),
      menu);
}

int main(int argc, char** argv) {
  gtk_init(&argc, &argv);
  create_systray();
  gtk_main();

  return 0;
}

