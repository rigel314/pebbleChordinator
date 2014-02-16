#include "chord_type_menu.h"
#include "chord.h"

#include <pebble.h>

Window* chord_type_window;
SimpleMenuLayer* chord_type_menu_layer;
SimpleMenuSection* chord_type_menu_sections;
SimpleMenuItem* chord_type_menu_items;

// static Window chord_type_window;
// static SimpleMenuLayer chord_type_menu_layer;
// static SimpleMenuSection chord_type_menu_sections[1];
// static SimpleMenuItem chord_type_menu_items[3];

// Holds titles built for use in menu items. Because there is no
// dynamic memory allocation, we have to get this space now
// static char menu_item_names[128];

// Holds root note passed in from note selection screen
static char *root_note;

// Types of supported chords. Used to build menu titles
static char *types[3] = {"Major", "Minor", "Major 7"};

// function prototypes
void setup_chord_type_window();
void chord_type_window_load(Window *window);
void setup_chord_type_menu_sections();
void chord_type_menu_item_selected_callback(int index, void *context);

// sets up items and sections for our menu
void setup_chord_type_menu_sections()
{
	// builds title strings and creates items with them. We store
	// the built title strings in menu_item_names.
	// APP_LOG(APP_LOG_LEVEL_INFO, "malloc()ing %d bytes", sizeof(SimpleMenuItem) * (sizeof(types) / sizeof(types[0])));
	chord_type_menu_items = malloc(sizeof(SimpleMenuItem) * (sizeof(types) / sizeof(types[0])));
	for (int i = 0; i < (int)(sizeof(types) / sizeof(types[0])); i++)
	{
		char title[16];
		int len;

		// chord_type_menu_items[i] = malloc(sizeof(SimpleMenuItem));

		len = snprintf(title, sizeof(title), "%s %s", root_note, types[i]);
		len++;
		// APP_LOG(APP_LOG_LEVEL_INFO, "malloc()ing %d bytes", len);
		chord_type_menu_items[i].title = malloc(len);
		strncpy((char*)chord_type_menu_items[i].title, title, len);
		
		chord_type_menu_items[i].callback = chord_type_menu_item_selected_callback;
	}

	// make our section with the items generated above
	// APP_LOG(APP_LOG_LEVEL_INFO, "malloc()ing %d bytes", sizeof(SimpleMenuSection));
	chord_type_menu_sections = malloc(sizeof(SimpleMenuSection));
	chord_type_menu_sections->title = "Select Type";
	chord_type_menu_sections->items = chord_type_menu_items;
	chord_type_menu_sections->num_items = (sizeof(types) / sizeof(types[0]));
	//(SimpleMenuSection){ .title = "Select Type", .items = chord_type_menu_items, .num_items = sizeof(chord_type_menu_items) / sizeof(chord_type_menu_items[0]) };
}

// callback for when a menu item is selected. Display chord window
void chord_type_menu_item_selected_callback(int index, void *context)
{
	// Pass in menu item title which will be used for chord lookup in
	// resources/src/raw/chords
	APP_LOG(APP_LOG_LEVEL_INFO, "index: %d - %s", index, chord_type_menu_items[index].title);
	display_chord(chord_type_menu_items[index].title);
}

// setup our menu layer when the window loads
void chord_type_window_load(Window *window)
{
	Layer *root_layer = window_get_root_layer(chord_type_window);

	chord_type_menu_layer = simple_menu_layer_create(
				layer_get_bounds(root_layer),
				chord_type_window,
				chord_type_menu_sections,
				1,
				NULL
			);

	layer_add_child(root_layer, simple_menu_layer_get_layer(chord_type_menu_layer));
}
void chord_type_window_unload(Window *window)
{
	APP_LOG(APP_LOG_LEVEL_INFO, "free()ing things");
	free(chord_type_menu_sections);
	for (int i = 0; i < (int)(sizeof(types) / sizeof(types[0])); i++)
	{
		free((char*) chord_type_menu_items[i].title);
	}
	free(chord_type_menu_items);
	simple_menu_layer_destroy(chord_type_menu_layer);
	window_destroy(chord_type_window);
}

// configures window and menu sections, and pushes it to the window stack
void display_chord_type_menu(char *root_note_)
{
	root_note = root_note_;
	setup_chord_type_window();
	setup_chord_type_menu_sections();
	window_stack_push(chord_type_window, true /* Animated */);
}

// setup window and it's handlers
void setup_chord_type_window()
{
	chord_type_window = window_create();
	window_set_window_handlers(chord_type_window, (WindowHandlers) { .load = chord_type_window_load, .unload = chord_type_window_unload});
}
