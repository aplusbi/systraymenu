#include <gtk/gtk.h>

typedef struct {
  char* name;
  char* command;
  gboolean confirm;
} MenuItem;

void callback(GtkWidget*, gpointer);
void callback_dialog(GtkDialog*, gint, gpointer);
void resize_systray(GtkStatusIcon*, gint, gpointer);
void on_click(GtkStatusIcon*, gpointer);
GtkMenu* create_menu();
GtkWidget* create_menu_item(MenuItem*);

static MenuItem menu_items[] = {
  { "Lock", "slock", FALSE },
  { "Logout", "pkill dwm", TRUE },
  { "Suspend", "systemctl suspend", TRUE },
  { "Hibernate", "systemctl hibernate", TRUE },
  { "Restart", "systemctl reboot", TRUE },
  { "Shutdown", "sudo -A -k shutdown -h now", FALSE },
};

void callback(GtkWidget* widget, gpointer data) {
  MenuItem* menu_item = (MenuItem*)(data);
  if (menu_item->confirm == TRUE) {
    GtkWidget* dialog = gtk_message_dialog_new(
        NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_OK_CANCEL,
        "Are you sure you want to %s?",
        menu_item->name);
    g_signal_connect(
        dialog,
        "response",
        G_CALLBACK (callback_dialog),
        menu_item);
    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_widget_destroy(dialog);
  } else {
    GError *error = NULL;
    g_spawn_command_line_async(menu_item->command, &error);
  }
}

void callback_dialog(GtkDialog* dialog, gint response_id, gpointer data) {
  if (response_id == GTK_RESPONSE_OK) {
    MenuItem* menu_item = (MenuItem*) (data);
    GError *error = NULL;
    g_spawn_command_line_async(menu_item->command, &error);
  }
}

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
  size_t num_menu_items = sizeof(menu_items) / sizeof(MenuItem);
  for (int i = 0; i < num_menu_items; ++i) {
    GtkWidget *menu_item =
      create_menu_item(&menu_items[i]);
    gtk_menu_shell_append(GTK_MENU_SHELL (menu), menu_item);
  }

  return menu;
}

GtkWidget* create_menu_item(MenuItem* menu_info) {
  GtkWidget *menu_item = gtk_menu_item_new_with_label(menu_info->name);
  g_signal_connect(
      G_OBJECT (menu_item),
      "activate",
      G_CALLBACK (callback),
      menu_info);
  gtk_widget_show(menu_item);

  return menu_item;
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

