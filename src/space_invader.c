#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x0A, 0x3C, 0x3A, 0x9F, 0x32, 0xA2, 0x44, 0xD5, 0xAC, 0x43, 0x54, 0xB7, 0x3D, 0xCD, 0x07, 0x64 }
PBL_APP_INFO(MY_UUID,
             "Space Invader", "Roy",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
BmpContainer background_image;
TextLayer text_time_layer;

void update_display(PblTm *tick_time) {
	static char time_text[] = "00:00";
	char *time_format;

	if (clock_is_24h_style()) {
		time_format = "%R";
	} 
	else {
		time_format = "%I:%M";
	}

	string_format_time(time_text, sizeof(time_text), time_format, tick_time);
	if (!clock_is_24h_style() && (time_text[0] == '0')) {
		memmove(time_text, &time_text[1], sizeof(time_text) - 1);
	}

	text_layer_set_text(&text_time_layer, time_text);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	(void)ctx;

	update_display(t->tick_time);
}

void handle_init(AppContextRef ctx) {
	(void)ctx;

	window_init(&window, "window");
	window_stack_push(&window, true /* Animated */);
	resource_init_current_app(&APP_RESOURCES);

	bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image);
	layer_add_child(&window.layer, &background_image.layer.layer);

	GRect time_rect;
	text_layer_init(&text_time_layer, window.layer.frame);
	text_layer_set_text_color(&text_time_layer, GColorBlack);
	text_layer_set_background_color(&text_time_layer, GColorClear);
	layer_set_frame(&text_time_layer.layer, GRect(0, 101, 144, 168-54));
	text_layer_set_text_alignment(&text_time_layer, GTextAlignmentCenter);
	text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGI_47)));
	layer_add_child(&window.layer, &text_time_layer.layer);

	PblTm tick_time;
	get_time(&tick_time);
	update_display(&tick_time);
}

void handle_deinit(AppContextRef ctx) {
	(void)ctx;

	bmp_deinit_container(&background_image);
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.deinit_handler = &handle_deinit,

		.tick_info = {
			.tick_handler = &handle_minute_tick,
			.tick_units = MINUTE_UNIT
		}
	};
	app_event_loop(params, &handlers);
}
