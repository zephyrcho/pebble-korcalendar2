#include <pebble.h>

#include "draw_proc.h"

//static int s_watch_type = 0;

//#define   TEST_VIEW

static Window *s_window;

Layer *s_bg_layer;             // Background Layer

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  static int s_min = -1;
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  if (s_min != t->tm_min) {
    s_min = t->tm_min;
    
    layer_mark_dirty(window_get_root_layer(s_window));
  }
  
  #ifdef TEST_VIEW
    layer_mark_dirty(window_get_root_layer(s_window));
  #endif
}

void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  batt_save_level(state.charge_percent);
  
  // Update meter
  layer_mark_dirty(window_get_root_layer(s_window));
}

void bluetooth_callback(bool connected) {
  // Show icon if disconnected
  bt_set_connected(connected);

  if(!connected) {
    // Issue a vibrating alert
    vibes_long_pulse();
  }
  
  // Update meter
  layer_mark_dirty(window_get_root_layer(s_window));
}
  
void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  load_resource();

  // Background layer setting
  s_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_bg_layer);
}

void window_unload(Window *window) {
  layer_destroy(s_bg_layer);

  unload_resource();
}

void init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);

  battery_state_service_subscribe(battery_callback);
  
  // Ensure battery level is displayed from the start
  battery_callback(battery_state_service_peek());
  
  // Register for bluetooth connection updates
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });
  
  // Show the correct state of the BT connection from the start
  bluetooth_callback(connection_service_peek_pebble_app_connection());
}

void deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
