#include <pebble.h>

#include "chord.h"
#include "instrument_menu.h"
#include "buffer_scanner.h"

Window* chord_window;

// static Window chord_window;
char chord_name[64];

// store the fingering of the chord read from resources/src/raw/chords 
static char chord_fingering[14];

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
	
	// chord_fingering[6] = '\0';
	APP_LOG(APP_LOG_LEVEL_INFO, "fingering: %s", chord_fingering);

	switch(instrument)
	{
		case 0:
			graphics_context_set_fill_color(context, GColorBlack);
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
			break;

		case 1:
			graphics_context_set_fill_color(context, GColorBlack);
			for (int i = 0; i < 4; i++)
			{    
				char number = chord_fingering[i];

				if (number != 'x' && number != '0') // current char is a number, draw a circle to indicate fingering
				{    
					graphics_fill_circle(context, (GPoint){(11+24) + (i * 24), 20 + (int)(number - '0') * 24}, 10);
				}
				else if (number == 'x') // char is an 'x', draw X on fretboard to indicate blocked string
				{   
					graphics_draw_line(context, (GPoint){(11+24) + (i * 24) - 6, 30}, (GPoint){11 + (i * 24) + 6, 18});
					graphics_draw_line(context, (GPoint){(11+24) + (i * 24) - 6, 18}, (GPoint){11 + (i * 24) + 6, 30});
				}
				else if (number == '0') // char is a '0', draw O on fretboard to indicate open string
				{
					graphics_draw_circle(context, (GPoint){(11+24) + (i * 24), 24}, 6);
				}
			}
			break;

		case 2:
			for (int i = 0; i < 13; i++)
			{    
				char number = chord_fingering[i];

				if(number == '1' && !isBlackKey(i))
				{
					graphics_context_set_fill_color(context, GColorBlack);
					graphics_fill_circle(context, (GPoint){12 + 17*whiteKeys(i), 117}, 6);
				}
				else if(number == '1' && isBlackKey(i))
				{
					graphics_context_set_fill_color(context, GColorWhite);
					graphics_fill_circle(context, (GPoint){blackKeyIndex(i), 80}, 4);
				}
			}
			break;
	}
}

// init and add layers on window load
void chord_window_load(Window *window)
{
	Layer *root_layer = window_get_root_layer(window);

	switch(instrument)
	{
		case 0:
			fretboard = gbitmap_create_with_resource(RESOURCE_ID_GUITAR_BACKGROUND);
			break;

		case 1:
			fretboard = gbitmap_create_with_resource(RESOURCE_ID_UKULELE_BACKGROUND);
			break;

		case 2:
			fretboard = gbitmap_create_with_resource(RESOURCE_ID_PIANO_BACKGROUND);
			break;
	}
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
	ResHandle res;
	uint8_t* buffer;
	// string to buffer to as we read file
	char string[32];
	
	switch(instrument)
	{
		case 0:
			res = resource_get_handle(RESOURCE_ID_GUITAR_CHORDS);
			break;

		case 1:
			res = resource_get_handle(RESOURCE_ID_UKULELE_CHORDS);
			break;

		case 2:
			res = resource_get_handle(RESOURCE_ID_PIANO_CHORDS);
			break;

		default:
			res = NULL;
			break;
	}

	buffer = malloc(resource_size(res));

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
			int x = instrument;
			// match was found- grab the next line (which will be the fingering) and put it in chord_fingering
			buffer_scanner_scan_next_string(&buffer_scanner, chord_fingering, 7 - (15*x)/2 + (11*x*x)/2 ); // the interpolation function maps 0->6, 1->4, and 2->13
			break;
		}
	}
	free(buffer);
}

int whiteKeys(int index)
{
	if(index <= 4)
		return index / 2;
	if(index <= 11)
		return (index / 2) + 1;
	return 7;
}
int blackKeyIndex(int index)
{
	switch(index)
	{
		case 1:
			return 21;
			break;
		case 3:
			return 38;
			break;
		case 6:
			return 72;
			break;
		case 8:
			return 89;
			break;
		case 10:
			return 106;
			break;
	}
	return 0;
}
bool isBlackKey(int index)
{
	switch(index)
	{
		case  1:
		case  3:
		case  6:
		case  8:
		case 10:
			return true;
	}
	return false;
}

void chord_increment()
{
	int offset = (chord_name[1] == ' ') ? 2 : 3;

	// APP_LOG(APP_LOG_LEVEL_INFO, "chord_name+offset: %s", chord_name+offset);

	if(!strncmp(chord_name+offset, "Major", 5))
	{
		strcpy(chord_name+offset, "Minor");
	}
	else if(!strncmp(chord_name+offset, "Minor", 5))
	{
		strcpy(chord_name+offset, "7");
	}
	else
	{
		strcpy(chord_name+offset, "Major");

		if(chord_name[1] == ' ')
		{
			memmove(chord_name+offset, chord_name+1, 7);
			chord_name[1] = '#';
		}
		else
		{
			chord_name[0]++;
			if(chord_name[0] > 'G')
				chord_name[0] = 'A';

			memmove(chord_name+1, chord_name+offset-1, 7);
		}
	}
}
void chord_decrement()
{
	int offset = (chord_name[1] == ' ') ? 2 : 3;

	if(!strncmp(chord_name+offset, "7", 1))
	{
		strcpy(chord_name+offset, "Minor");
	}
	else if(!strncmp(chord_name+offset, "Minor", 5))
	{
		strcpy(chord_name+offset, "Major");
	}
	else
	{
		strcpy(chord_name+offset, "7");

		if(chord_name[1] == ' ')
		{
			chord_name[0]--;
			if(chord_name[0] < 'A')
				chord_name[0] = 'G';

			memmove(chord_name+offset, chord_name+1, 3);
			chord_name[1] = '#';
		}
		else
		{
			memmove(chord_name+1, chord_name+offset-1, 3);
		}
	}
}

void swapWindow()
{
	window_stack_pop(true);
	display_chord(chord_name);
}

void chord_up(ClickRecognizerRef recognizer, void* context)
{
	// APP_LOG(APP_LOG_LEVEL_INFO, "chord_name: %s", chord_name);
	chord_decrement();
	// APP_LOG(APP_LOG_LEVEL_INFO, "chord_name: %s", chord_name);
	swapWindow();
}
void chord_down(ClickRecognizerRef recognizer, void* context)
{
	// APP_LOG(APP_LOG_LEVEL_INFO, "chord_name: %s", chord_name);
	chord_increment();
	// APP_LOG(APP_LOG_LEVEL_INFO, "chord_name: %s", chord_name);
	swapWindow();
}
void chord_longup(ClickRecognizerRef recognizer, void* context)
{
	chord_decrement();
	chord_decrement();
	chord_decrement();
	swapWindow();
}
void chord_longdown(ClickRecognizerRef recognizer, void* context)
{
	chord_increment();
	chord_increment();
	chord_increment();
	swapWindow();
}
void chord_click_config_provider(void* context)
{
	window_single_click_subscribe(BUTTON_ID_UP, chord_up);
	window_single_click_subscribe(BUTTON_ID_DOWN, chord_down);
	window_long_click_subscribe(BUTTON_ID_UP, 0, chord_longup, NULL);
	window_long_click_subscribe(BUTTON_ID_DOWN, 0, chord_longdown, NULL);
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
	window_set_click_config_provider(chord_window, (ClickConfigProvider) chord_click_config_provider);
}
