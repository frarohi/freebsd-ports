--- src/xfpm-button.c.orig	2024-06-09 15:35:08 UTC
+++ src/xfpm-button.c
@@ -39,6 +39,7 @@
 
 #include <X11/X.h>
 #include <X11/XF86keysym.h>
+#include <keybinder.h>
 
 #include <gdk/gdkx.h>
 #include <gtk/gtk.h>
@@ -54,12 +55,6 @@ static void xfpm_button_finalize   (GObject *object);
 
 static void xfpm_button_finalize   (GObject *object);
 
-static struct
-{
-  XfpmButtonKey    key;
-  guint            key_code;
-} xfpm_key_map [NUMBER_OF_BUTTONS] = { {0, 0}, };
-
 struct XfpmButtonPrivate
 {
   GdkScreen   *screen;
@@ -74,189 +69,125 @@ enum
   LAST_SIGNAL
 };
 
+static struct
+{
+  char            *keysymbol;
+  XfpmButtonKey    key;
+} xfpm_symbol_map [] = {
+  {"XF86MonBrightnessUp"  , BUTTON_MON_BRIGHTNESS_UP  },
+  {"XF86MonBrightnessDown", BUTTON_MON_BRIGHTNESS_DOWN},
+  {"XF86KbdBrightnessUp"  , BUTTON_KBD_BRIGHTNESS_UP  },
+  {"XF86KbdBrightnessDown", BUTTON_KBD_BRIGHTNESS_DOWN},
+  {"XF86PowerOff"         , BUTTON_POWER_OFF          },
+  {"XF86Hibernate"        , BUTTON_HIBERNATE          },
+  {"XF86Suspend"          , BUTTON_HIBERNATE          },
+  {"XF86Sleep"            , BUTTON_SLEEP              },
+  {"XF86XK_Battery"       , BUTTON_BATTERY            },
+  {NULL, BUTTON_UNKNOWN}
+};
+
 #define DUPLICATE_SHUTDOWN_TIMEOUT 4.0f
 
 static guint signals[LAST_SIGNAL] = { 0 };
 
 G_DEFINE_TYPE_WITH_PRIVATE (XfpmButton, xfpm_button, G_TYPE_OBJECT)
 
-static guint
-xfpm_button_get_key (unsigned int keycode)
+static
+void xfpm_key_handler (const char *keystring, void *data)
 {
-  XfpmButtonKey key = BUTTON_UNKNOWN;
-  guint i;
+  XfpmButton *button = (XfpmButton *) data;
 
-  for ( i = 0; i < G_N_ELEMENTS (xfpm_key_map); i++)
-  {
-    if ( xfpm_key_map [i].key_code == keycode )
-      key = xfpm_key_map [i].key;
+  XFPM_DEBUG ("Key symbol received: %s", keystring );
+  for (int idx=0; xfpm_symbol_map[idx].keysymbol; idx++) {
+    if ( strcmp( keystring, xfpm_symbol_map[idx].keysymbol ) == 0) {
+      g_signal_emit (G_OBJECT (button), signals[BUTTON_PRESSED], 0, xfpm_symbol_map[idx].key );
+      XFPM_DEBUG ("Key press signalled" );
+      break;
+    }
   }
-
-  return key;
 }
 
-static GdkFilterReturn
-xfpm_button_filter_x_events (GdkXEvent *xevent, GdkEvent *ev, gpointer data)
-{
-  XfpmButtonKey key;
-  XfpmButton *button;
+static char *modifiers[] = {
+   "",
+   "<Ctrl>",
+   "<Alt>",
+   "<Super>",
+   "<Shift>",
+   "<Ctrl><Shift>",
+   "<Ctrl><Alt>",
+   "<Ctrl><Super>",
+   "<Alt><Shift>",
+   "<Alt><Super>",
+   "<Shift><Super>",
+   "<Ctrl><Shift><Super>",
+   "<Ctrl><Shift><Alt>",
+   "<Ctrl><Alt><Super>",
+   "<Shift><Alt><Super>",
+   "<Ctrl><Shift><Alt><Super>",
+   NULL
+};
 
-  XEvent *xev = (XEvent *) xevent;
-
-  if ( xev->type != KeyPress )
-    return GDK_FILTER_CONTINUE;
-
-  key = xfpm_button_get_key (xev->xkey.keycode);
-
-  if ( key != BUTTON_UNKNOWN )
-  {
-    button = (XfpmButton *) data;
-
-    XFPM_DEBUG_ENUM (key, XFPM_TYPE_BUTTON_KEY, "Key press");
-
-    g_signal_emit (G_OBJECT(button), signals[BUTTON_PRESSED], 0, key);
-    return GDK_FILTER_REMOVE;
-  }
-
-  return GDK_FILTER_CONTINUE;
-}
-
-static gboolean
-xfpm_button_grab_keystring (XfpmButton *button, guint keycode)
+static void
+xfpm_bind_keysym (XfpmButton *button,
+                  char *keysym,
+                  XfpmKeys key )
 {
-  Display *display;
-  GdkDisplay *gdisplay;
-  guint ret;
-  guint modmask = AnyModifier;
+  char buffer[100];
 
-  display = gdk_x11_get_default_xdisplay ();
-  gdisplay = gdk_display_get_default ();
+  if ((button->priv->mapped_buttons & key) == 0) {
+    for (int idx=0; modifiers[idx]; idx++) {
+      sprintf( buffer, "%s%s", modifiers[idx], keysym);
+      keybinder_bind (buffer, xfpm_key_handler, button);
+    }
+    button->priv->mapped_buttons |= key;
+    XFPM_DEBUG ("Button bound: %s", keysym );
 
-  gdk_x11_display_error_trap_push (gdisplay);
-
-  ret = XGrabKey (display, keycode, modmask,
-                  GDK_WINDOW_XID (button->priv->window), True,
-                  GrabModeAsync, GrabModeAsync);
-
-  if ( ret == BadAccess )
-  {
-    g_warning ("Failed to grab modmask=%u, keycode=%li", modmask, (long int) keycode);
-    return FALSE;
   }
-
-  ret = XGrabKey (display, keycode, LockMask | modmask,
-                  GDK_WINDOW_XID (button->priv->window), True,
-                  GrabModeAsync, GrabModeAsync);
-
-  if (ret == BadAccess)
-  {
-    g_warning ("Failed to grab modmask=%u, keycode=%li", LockMask | modmask, (long int) keycode);
-    return FALSE;
-  }
-
-  gdk_display_flush (gdisplay);
-  gdk_x11_display_error_trap_pop_ignored (gdisplay);
-
-  return TRUE;
 }
 
-
-static gboolean
-xfpm_button_xevent_key (XfpmButton *button, guint keysym , XfpmButtonKey key)
-{
-  guint keycode = XKeysymToKeycode (gdk_x11_get_default_xdisplay(), keysym);
-
-  if ( keycode == 0 )
-  {
-    XFPM_DEBUG ("could not map keysym %x to keycode\n", keysym);
-    return FALSE;
-  }
-
-  if ( !xfpm_button_grab_keystring(button, keycode))
-    return FALSE;
-
-  XFPM_DEBUG_ENUM (key, XFPM_TYPE_BUTTON_KEY, "Grabbed key %li ", (long int) keycode);
-
-  xfpm_key_map [key].key_code = keycode;
-  xfpm_key_map [key].key = key;
-
-  return TRUE;
-}
-
 static void
-xfpm_button_ungrab (XfpmButton *button, guint keysym, XfpmButtonKey key)
+xfpm_unbind_keysym (XfpmButton *button,
+                    char *keysym,
+                    XfpmKeys key )
 {
-  Display *display;
-  GdkDisplay *gdisplay;
-  guint modmask = AnyModifier;
-  guint keycode = XKeysymToKeycode (gdk_x11_get_default_xdisplay(), keysym);
+  char buffer[100];
 
-  if (keycode == 0)
-  {
-    XFPM_DEBUG ("could not map keysym %x to keycode\n", keysym);
-    return;
+  if ((button->priv->mapped_buttons & key) != 0) {
+    for (int idx=0; modifiers[idx]; idx++) {
+      sprintf( buffer, "%s%s", modifiers[idx], keysym);
+      keybinder_unbind (buffer, xfpm_key_handler);
+    }
+    button->priv->mapped_buttons &= ~(key);
+    XFPM_DEBUG ("Button unbound: %s", keysym );
   }
-
-  display = gdk_x11_get_default_xdisplay ();
-  gdisplay = gdk_display_get_default ();
-
-  gdk_x11_display_error_trap_push (gdisplay);
-
-  XUngrabKey (display, keycode, modmask,
-              GDK_WINDOW_XID (button->priv->window));
-  XUngrabKey (display, keycode, LockMask | modmask,
-              GDK_WINDOW_XID (button->priv->window));
-
-  gdk_display_flush (gdisplay);
-  gdk_x11_display_error_trap_pop_ignored (gdisplay);
-
-  XFPM_DEBUG_ENUM (key, XFPM_TYPE_BUTTON_KEY, "Ungrabbed key %li ", (long int) keycode);
-
-  xfpm_key_map [key].key_code = 0;
-  xfpm_key_map [key].key = 0;
 }
 
+
 static void
 xfpm_button_setup (XfpmButton *button)
 {
   button->priv->screen = gdk_screen_get_default ();
   button->priv->window = gdk_screen_get_root_window (button->priv->screen);
 
-  if ( xfpm_button_xevent_key (button, XF86XK_PowerOff, BUTTON_POWER_OFF) )
-    button->priv->mapped_buttons |= POWER_KEY;
+  keybinder_init();
 
 #ifdef HAVE_XF86XK_HIBERNATE
-  if ( xfpm_button_xevent_key (button, XF86XK_Hibernate, BUTTON_HIBERNATE) )
-    button->priv->mapped_buttons |= HIBERNATE_KEY;
+  xfpm_bind_keysym (button, "XF86Hibernate", HIBERNATE_KEY);
 #endif
-
 #ifdef HAVE_XF86XK_SUSPEND
-  if ( xfpm_button_xevent_key (button, XF86XK_Suspend, BUTTON_HIBERNATE) )
-    button->priv->mapped_buttons |= HIBERNATE_KEY;
+  xfpm_bind_keysym (button, "XF86Suspend", HIBERNATE_KEY);
 #endif
+  xfpm_bind_keysym (button, "XF86PowerOff", POWER_KEY);
+  xfpm_bind_keysym (button, "XF86Sleep", SLEEP_KEY);
+  xfpm_bind_keysym (button, "XF86Battery", BATTERY_KEY);
+  xfpm_bind_keysym (button, "XF86KbdBrightnessUp", KBD_BRIGHTNESS_KEY_UP);
+  xfpm_bind_keysym (button, "XF86KbdBrightnessDown", KBD_BRIGHTNESS_KEY_DOWN);
 
-  if ( xfpm_button_xevent_key (button, XF86XK_Sleep, BUTTON_SLEEP) )
-    button->priv->mapped_buttons |= SLEEP_KEY;
-
   if (button->priv->handle_brightness_keys)
   {
-    if ( xfpm_button_xevent_key (button, XF86XK_MonBrightnessUp, BUTTON_MON_BRIGHTNESS_UP) )
-      button->priv->mapped_buttons |= BRIGHTNESS_KEY_UP;
-
-    if (xfpm_button_xevent_key (button, XF86XK_MonBrightnessDown, BUTTON_MON_BRIGHTNESS_DOWN) )
-      button->priv->mapped_buttons |= BRIGHTNESS_KEY_DOWN;
+    xfpm_bind_keysym (button, "XF86MonBrightnessUp", BRIGHTNESS_KEY_UP);
+    xfpm_bind_keysym (button, "XF86MonBrightnessDown", BRIGHTNESS_KEY_DOWN);
   }
-
-  if (xfpm_button_xevent_key (button, XF86XK_Battery, BUTTON_BATTERY))
-    button->priv->mapped_buttons |= BATTERY_KEY;
-
-  if ( xfpm_button_xevent_key (button, XF86XK_KbdBrightnessUp, BUTTON_KBD_BRIGHTNESS_UP) )
-    button->priv->mapped_buttons |= KBD_BRIGHTNESS_KEY_UP;
-
-  if (xfpm_button_xevent_key (button, XF86XK_KbdBrightnessDown, BUTTON_KBD_BRIGHTNESS_DOWN) )
-    button->priv->mapped_buttons |= KBD_BRIGHTNESS_KEY_DOWN;
-
-  gdk_window_add_filter (button->priv->window, xfpm_button_filter_x_events, button);
 }
 
 static void
@@ -333,25 +264,13 @@ xfpm_button_set_handle_brightness_keys (XfpmButton *bu
 
     if (handle_brightness_keys)
     {
-      if (xfpm_button_xevent_key (button, XF86XK_MonBrightnessUp, BUTTON_MON_BRIGHTNESS_UP))
-        button->priv->mapped_buttons |= BRIGHTNESS_KEY_UP;
-
-      if (xfpm_button_xevent_key (button, XF86XK_MonBrightnessDown, BUTTON_MON_BRIGHTNESS_DOWN))
-        button->priv->mapped_buttons |= BRIGHTNESS_KEY_DOWN;
+      xfpm_bind_keysym (button, "XF86MonBrightnessUp", BRIGHTNESS_KEY_UP);
+      xfpm_bind_keysym (button, "XF86MonBrightnessDown", BRIGHTNESS_KEY_DOWN);
     }
     else
     {
-      if ((button->priv->mapped_buttons & BRIGHTNESS_KEY_UP) != 0)
-      {
-        xfpm_button_ungrab (button, XF86XK_MonBrightnessUp, BUTTON_MON_BRIGHTNESS_UP);
-        button->priv->mapped_buttons &= ~(BRIGHTNESS_KEY_UP);
-      }
-
-      if ((button->priv->mapped_buttons & BRIGHTNESS_KEY_DOWN) != 0)
-      {
-        xfpm_button_ungrab (button, XF86XK_MonBrightnessDown, BUTTON_MON_BRIGHTNESS_DOWN);
-        button->priv->mapped_buttons &= ~(BRIGHTNESS_KEY_DOWN);
-      }
+      xfpm_unbind_keysym (button, "XF86MonBrightnessUp", BRIGHTNESS_KEY_UP);
+      xfpm_unbind_keysym (button, "XF86MonBrightnessDown", BRIGHTNESS_KEY_DOWN);
     }
   }
 }
