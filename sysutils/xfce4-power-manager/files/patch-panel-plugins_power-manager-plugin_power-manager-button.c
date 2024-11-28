--- panel-plugins/power-manager-plugin/power-manager-button.c.orig	2024-06-09 15:35:08 UTC
+++ panel-plugins/power-manager-plugin/power-manager-button.c
@@ -535,6 +535,7 @@ power_manager_button_update_device_icon_and_details (P
     battery_device->img = gtk_image_new_from_surface (battery_device->surface);
     g_object_ref (battery_device->img);
 
+        g_object_ref (battery_device->img);
 G_GNUC_BEGIN_IGNORE_DEPRECATIONS
     gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(battery_device->menu_item), battery_device->img);
 G_GNUC_END_IGNORE_DEPRECATIONS
