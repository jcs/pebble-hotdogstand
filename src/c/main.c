/*
 * Copyright (c) 2015, 2026 joshua stein <jcs@jcs.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  static char buffer[] = "  :     ";
  static char dbuffer[] = "  /  /  ";

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  if (clock_is_24h_style() == true) {
    strftime(buffer, sizeof(buffer), "%H:%M", tick_time);
    text_layer_set_text(s_time_layer, buffer);
  }
  else {
    strftime(buffer, sizeof(buffer), "%I:%M %p", tick_time);
    text_layer_set_text(s_time_layer, buffer);
  }

  strftime(dbuffer, sizeof(dbuffer), "%m/%d/%y", tick_time);
  text_layer_set_text(s_date_layer, dbuffer);
}

#if defined(PBL_PLATFORM_EMERY)
#define TIME_Y     88
#define TIME_FONT  FONT_KEY_BITHAM_42_MEDIUM_NUMBERS
#define DATE_Y     129
#define DATE_FONT  FONT_KEY_GOTHIC_28_BOLD
#else
#define TIME_Y     65
#define TIME_FONT  FONT_KEY_BITHAM_34_MEDIUM_NUMBERS
#define DATE_Y     95
#define DATE_FONT  FONT_KEY_GOTHIC_24_BOLD
#endif

static void main_window_load(Window *window) {
  GRect bounds = layer_get_bounds(window_get_root_layer(window));

  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  /* time */
  s_time_layer = text_layer_create(GRect(0, TIME_Y, bounds.size.w, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, fonts_get_system_font(TIME_FONT));
  if (clock_is_24h_style() == true)
    text_layer_set_text(s_time_layer, "  :  ");
  else
    text_layer_set_text(s_time_layer, "  :    ");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

  /* date */
  s_date_layer = text_layer_create(GRect(0, DATE_Y, bounds.size.w, 50));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_font(s_date_layer, fonts_get_system_font(DATE_FONT));
  text_layer_set_text(s_date_layer, "  /  /  ");
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));

  update_time();
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
