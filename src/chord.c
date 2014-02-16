#include "chord.h"

#include <pebble.h>
#include "buffer_scanner.h"

Window* chord_window;

// static Window chord_window;
static char chord_name[64];

// store the fingering of the chord read from resources/src/raw/chords 
static char chord_fingering[7];

// displays name of chord
TextLayer* text_layer;
// static TextLayer text_layer;

// used to display fretboard image
BitmapLayer* bl;
GBitmap* fretboard;
// static BmpContainer background_container;

// layer that fingerings, open strings, and closed strings are drawn on
Layer* fingering_layer;
// static Layer fingering_layer;

// function prototypes
void setup_chord_window();
void chord_window_load(Window *window);
void get_chord_fingering(const char *chord);
void chord_fingering_update_proc(Layer *layer, GContext *context);

// draw routine for fingering_layer
void chord_fingering_update_proc(Layer *layer, GContext *context)
{
	// set draw and fill colors to block
	graphics_context_set_stroke_color(context, GColorBlack);
	graphics_context_set_fill_color(context, GColorBlack);
	
	chord_fingering[6] = '\0';
	APP_LOG(APP_LOG_LEVEL_INFO, "fingering: %s", chord_fingering);

	// loop through each char in chord_fingering
	for (int i = 0; i < 6; i++)
	{    
		char number = chord_fingering[i];

		if (number != 'x' && number != '0') // current char is a number, draw a circle to indicate fingering
		{    
			graphics_fill_circle(context, (GPoint){11 + (i * 24), 20 + (int)(number - '0') * 24}, 10);
		}
		else if (number == 'x') // char is an 'x', draw X on fretboard to indicate blocked string
		{   
			graphics_draw_line(context, (GPoint){11 + (i * 24) - 6, 30}, (GPoint){11 + (i * 24) + 6, 18});
			graphics_draw_line(context, (GPoint){11 + (i * 24) - 6, 18}, (GPoint){11 + (i * 24) + 6, 30});
		}
		else if (number == '0') // char is a '0', draw O on fretboard to indicate open string
		{
			graphics_draw_circle(context, (GPoint){11 + (i * 24), 24}, 6);
		}
	}
}

// init and add layers on window load
void chord_window_load(Window *window)
{
	Layer *root_layer = window_get_root_layer(window);

	fretboard = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
	// add fretboard at lowest layer
	bl = bitmap_layer_create(layer_get_bounds(root_layer));
	// bmp_init_container(RESOURCE_ID_BACKGROUND, &background_container); /* deinit on window unload */
	// layer_set_frame((Layer *)&background_container.layer, GRect(0, 0, 144, 152));
	bitmap_layer_set_bitmap(bl, fretboard);
	layer_add_child(root_layer, bitmap_layer_get_layer(bl));

	// add text layer. y is set to -2 to even things out
	text_layer = text_layer_create(GRect(0, -2, 144, 16));  
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	text_layer_set_text(text_layer, chord_name);
	layer_add_child(root_layer, text_layer_get_layer(text_layer));

	// add fingering layer
	fingering_layer = layer_create(GRect(0, 0, 144, 152));
	layer_set_update_proc(fingering_layer, chord_fingering_update_proc);
	layer_add_child(root_layer, fingering_layer);
}

// need to deinit bitmap layers loaded with bmp_init_container
void chord_window_unload(Window *window)
{
	// bmp_deinit_container(&background_container);
	bitmap_layer_destroy(bl);
	gbitmap_destroy(fretboard);
	text_layer_destroy(text_layer);
	layer_destroy(fingering_layer);
	window_destroy(chord_window);
}

// lookup chord fingering, setup window, and push to stack
void display_chord(const char *chord)
{
	// copy chord name into chord_name var
	strncpy(chord_name, chord, 64);

	// get chord fingering using chord_name as a key for resources/src/raw/chords 
	get_chord_fingering(chord_name);
	setup_chord_window();
	window_stack_push(chord_window, true /* Animated */);
}

void get_chord_fingering(const char *chord)
{
	BufferScanner buffer_scanner;
	ResHandle res = resource_get_handle(RESOURCE_ID_CHORDS);
	uint8_t* buffer = malloc(resource_size(res));
	// string to buffer to as we read file
	char string[32];
	
	// load resource into a buffer
	resource_load(res, buffer, resource_size(res));

	// create buffer scanner with our chords buffer, using new line as a delimiter
	buffer_scanner_init(&buffer_scanner, (char *)buffer, resource_size(res), '\n');

	// scan though our buffer and break when we find a match to our chord name
	while (buffer_scanner.position < (int)resource_size(res))
	{
		buffer_scanner_scan_next_string(&buffer_scanner, string, 32);     
 
		if (strncmp(string, chord, 10) == 0)
		{
			// match was found- grab the next line (which will be the fingering) and put it in chord_fingering
			buffer_scanner_scan_next_string(&buffer_scanner, chord_fingering, 6);
			break;
		}
	}
	free(buffer);
}

// init window and setup handlers
void setup_chord_window()
{
	chord_window = window_create();
	window_set_window_handlers(chord_window, (WindowHandlers)
			{
				.load = chord_window_load,
				.unload = chord_window_unload
			});
}
