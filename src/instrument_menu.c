#include <pebble.h>

#include "note_menu.h"
#include "instrument_menu.h"

int instrument = 0;

static char *instruments[3] = {"Guitar", "Ukulele", "Piano"};

Window* instrument_menu_window;
SimpleMenuLayer* instrument_menu_layer;
SimpleMenuSection* instrument_menu_sections;
SimpleMenuItem* instrument_menu_items;

// function prototypes
void setup_instrument_menu_window();
void setup_instrument_menu_sections();

// callback for menu items being selected
void instrument_menu_item_selected_callback(int index, void *context)
{
	APP_LOG(APP_LOG_LEVEL_INFO, "index: %d - %s", index, instruments[index]);
	instrument = index;
	setup_chord_menu_window();
}

// builds menu items and sections for menu layer
void setup_instrument_menu_sections()
{
	// creates items based titled from instruments variable 
	instrument_menu_items = malloc(sizeof(SimpleMenuItem) * 3);
	for (int i = 0; i < 3; i++)
	{
		// instrument_menu_items[i] = malloc(sizeof(SimpleMenuItem));
		// { .title = instruments[i], .callback = instrument_menu_item_selected_callback }
		instrument_menu_items[i].title = instruments[i];
		instrument_menu_items[i].callback = instrument_menu_item_selected_callback;
		instrument_menu_items[i].subtitle = NULL;
		instrument_menu_items[i].icon = NULL;
	}

	// setup menu section with items created above
	instrument_menu_sections = malloc(sizeof(SimpleMenuSection));
	instrument_menu_sections->title = "Select instrument";
	instrument_menu_sections->items = instrument_menu_items;
	instrument_menu_sections->num_items = 3;
	//(SimpleMenuSection){ .title = "Select Root instrument", .items = instrument_menu_items, .num_items = sizeof(instrument_menu_items) / sizeof(instrument_menu_items[0]) };
}

// on window load init menu layer and add it as a sub layer
// it's important to set the frame of the menu layer here,
// because at this point we know the correct size of our
// window's root layer. It may not account for the action bar layer
// prior to this point.
void instrument_menu_window_load(Window *window)
{
	Layer *layer = window_get_root_layer(instrument_menu_window);

	instrument_menu_layer = simple_menu_layer_create(
				layer_get_bounds(layer), 
				instrument_menu_window,
				instrument_menu_sections,
				1,
				NULL
			);

	layer_add_child(layer, simple_menu_layer_get_layer(instrument_menu_layer));
}
void instrument_menu_window_unload(Window *window)
{
	APP_LOG(APP_LOG_LEVEL_INFO, "free()ing things");
	free(instrument_menu_sections);
	// for (int i = 0; i < (int)(sizeof(types) / sizeof(types[0])); i++)
	// {
	// 	free((char*) chord_type_menu_items[i].title);
	// }
	free(instrument_menu_items);
	simple_menu_layer_destroy(instrument_menu_layer);
	window_destroy(instrument_menu_window);
}

// init, configure, and push instrument menu window
void setup_instrument_menu_window()
{
	setup_instrument_menu_sections();

	instrument_menu_window = window_create();
	window_set_window_handlers(instrument_menu_window, (WindowHandlers){ .load = instrument_menu_window_load, .unload = instrument_menu_window_unload });

  window_stack_push(instrument_menu_window, true);
}

