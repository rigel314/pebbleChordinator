#include "chord.h"

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "buffer_scanner.h"

static Window chord_window;
static char chord_fingering[6];
static char chord_name[64];
static TextLayer text_layer;
static BmpContainer background_container;
static Layer fingering_layer;

void setup_chord_window();
void chord_window_load(Window *window);
void get_chord_fingering(const char *chord);
void chord_fingering_update_proc(Layer *layer, GContext *context);

void chord_fingering_update_proc(Layer *layer, GContext *context) {

    graphics_context_set_stroke_color(context, GColorBlack);
    graphics_context_set_fill_color(context, GColorBlack);
    
    for (int i = 0; i < 6; i++) {
        
        char number = chord_fingering[i];
        if (number != 'x' && number != '0') {
            
            graphics_fill_circle(context, (GPoint){11 + (i * 24), 20 + (int)(number - '0') * 24}, 10);

        } else if (number == 'x') {
            
            graphics_draw_line(context, (GPoint){11 + (i * 24) - 6, 30}, (GPoint){11 + (i * 24) + 6, 18});
            graphics_draw_line(context, (GPoint){11 + (i * 24) - 6, 18}, (GPoint){11 + (i * 24) + 6, 30});

        } else if (number == '0') {

            graphics_draw_circle(context, (GPoint){11 + (i * 24), 24}, 6);
       
        }

    }

}

void chord_window_load(Window *window) {

    Layer *root_layer = window_get_root_layer(window);

    bmp_init_container(RESOURCE_ID_BACKGROUND, &background_container);
    layer_set_frame((Layer *)&background_container.layer, GRect(0, 0, 144, 152));
    layer_add_child(root_layer, (Layer *)&background_container.layer);

    text_layer_init(&text_layer, GRect(0, -2, 144, 16));  
    text_layer_set_text_alignment(&text_layer, GTextAlignmentCenter);
    text_layer_set_font(&text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
    text_layer_set_text(&text_layer, chord_name);
    layer_add_child(root_layer, (Layer *)&text_layer);

    layer_init(&fingering_layer, GRect(0, 0, 144, 152));
    layer_set_update_proc(&fingering_layer, chord_fingering_update_proc);
    layer_add_child(root_layer, &fingering_layer);

}

void chord_window_unload(Window *window) {

    bmp_deinit_container(&background_container);

}

void display_chord(const char *chord) {

    strncpy(chord_name, chord, 64);
    get_chord_fingering(chord);
    setup_chord_window();
    window_stack_push(&chord_window, true /* Animated */);

}

void get_chord_fingering(const char *chord) {

    ResHandle res = resource_get_handle(RESOURCE_ID_CHORDS);
    uint8_t buffer[resource_size(res)];
    resource_load(res, buffer, resource_size(res));

    BufferScanner buffer_scanner;
    buffer_scanner_init(&buffer_scanner, (char *)buffer, resource_size(res), '\n');

    char string[32];
    int match_found = 0;

    while (buffer_scanner.position < (int)resource_size(res)) {

        buffer_scanner_scan_next_string(&buffer_scanner, string, 32);     
        if (strncmp(string, chord, 10) == 0) {
                buffer_scanner_scan_next_string(&buffer_scanner, chord_fingering, 6);
                break;
        }
    }
}

void setup_chord_window() {

    window_init(&chord_window, "Chordinator");
    window_set_window_handlers(&chord_window, (WindowHandlers){
            .load = chord_window_load,
            .unload = chord_window_unload
    });

}
