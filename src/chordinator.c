#include <pebble.h>
#include "instrument_menu.h"

void init()
{
	setup_instrument_menu_window();
}

void deinit()
{
	// window_destroy(instrument_menu_window);
	// // for (int i = 0; i < 12; ++i)
	// // {
	// // 	free(root_instrument_menu_items[i]);
	// // }
	// free(root_instrument_menu_items);
	// free(root_instrument_menu_sections);
	// simple_menu_layer_destroy(root_instrument_menu_layer);
}

int main()
{
	init();

	app_event_loop();

	deinit();
}
