#include <pebble.h>

#include "chord_type_menu.h"

// Used to build the menu and build the key used to find the fingering
// in /resources/src/raw/chords
static char *notes[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

Window* chord_menu_window;
SimpleMenuLayer* root_note_menu_layer;
SimpleMenuSection* root_note_menu_sections;
SimpleMenuItem* root_note_menu_items;

// static Window chord_menu_window;
// static SimpleMenuLayer root_note_menu_layer;
// static SimpleMenuSection root_note_menu_sections[1];
// static SimpleMenuItem root_note_menu_items[12];

// function prototypes
void setup_chord_menu_window();
void setup_menu_sections();

// callback for menu items being selected
void note_menu_item_selected_callback(int index, void *context)
{
	// push chord type (Minor / Major / etc) window with selected root note
	APP_LOG(APP_LOG_LEVEL_INFO, "index: %d - %s", index, notes[index]);
	display_chord_type_menu(notes[index]);
}

// builds menu items and sections for menu layer
void setup_menu_sections()
{
	// creates items based titled from notes variable 
	root_note_menu_items = malloc(sizeof(SimpleMenuItem) * 12);
	for (int i = 0; i < 12; i++)
	{
		// root_note_menu_items[i] = malloc(sizeof(SimpleMenuItem));
		// { .title = notes[i], .callback = note_menu_item_selected_callback }
		root_note_menu_items[i].title = notes[i];
		root_note_menu_items[i].callback = note_menu_item_selected_callback;
		root_note_menu_items[i].subtitle = NULL;
		root_note_menu_items[i].icon = NULL;
	}

	// setup menu section with items created above
	root_note_menu_sections = malloc(sizeof(SimpleMenuSection));
	root_note_menu_sections->title = "Select Root Note";
	root_note_menu_sections->items = root_note_menu_items;
	root_note_menu_sections->num_items = 12;
	//(SimpleMenuSection){ .title = "Select Root Note", .items = root_note_menu_items, .num_items = sizeof(root_note_menu_items) / sizeof(root_note_menu_items[0]) };
}

// on window load init menu layer and add it as a sub layer
// it's important to set the frame of the menu layer here,
// because at this point we know the correct size of our
// window's root layer. It may not account for the action bar layer
// prior to this point.
void chord_menu_window_load(Window *window)
{
	Layer *root_layer = window_get_root_layer(chord_menu_window);

	root_note_menu_layer = simple_menu_layer_create(
				layer_get_bounds(root_layer), 
				chord_menu_window,
				root_note_menu_sections,
				1,
				NULL
			);

	layer_add_child(root_layer, simple_menu_layer_get_layer(root_note_menu_layer));
}
void chord_menu_window_unload(Window *window)
{
	APP_LOG(APP_LOG_LEVEL_INFO, "free()ing things");
	free(root_note_menu_sections);
	// for (int i = 0; i < (int)(sizeof(types) / sizeof(types[0])); i++)
	// {
	// 	free((char*) chord_type_menu_items[i].title);
	// }
	free(root_note_menu_items);
	simple_menu_layer_destroy(root_note_menu_layer);
	window_destroy(chord_menu_window);
}

// init, configure, and push chord menu window
void setup_chord_menu_window()
{
	setup_menu_sections();

	chord_menu_window = window_create();
	window_set_window_handlers(chord_menu_window, (WindowHandlers){ .load = chord_menu_window_load, .unload = chord_menu_window_unload });

  window_stack_push(chord_menu_window, true);
}
