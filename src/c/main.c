#include <pebble.h>

#include "activities.h"

#define NUM_WINDOWS 4

int masterLog = 321;

static Window *s_main_window;
static MenuLayer *s_menu_layer;

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return NUM_WINDOWS;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  switch(cell_index->row) {
    case 0:
      menu_cell_basic_draw(ctx, cell_layer, "Activities", NULL, NULL);
      break;
    case 1:
      menu_cell_basic_draw(ctx, cell_layer, "Timeline", NULL, NULL);
      break;
    case 2:
//       menu_cell_basic_draw(ctx, cell_layer, "Goals", NULL, NULL);
      break;
    case 3:
//       menu_cell_basic_draw(ctx, cell_layer, "Setting", NULL, NULL);
      break;
    default:
      break;
  }
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  const int16_t cell_height = 44;
  return cell_height;
}

static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  switch(cell_index->row) {
    case 0:
      activities_window_push();
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    default:
      break;
  }
}

static void window_load(Window *window) {
  
  if(persist_exists(masterLog)){
    
  APP_LOG(APP_LOG_LEVEL_DEBUG, "key perserved, retriving log.");
  int logsize = persist_get_size(masterLog);
  int displaylog_key[logsize];
  displaylog_key[logsize] = persist_read_data(masterLog, (void *)displaylog_key, sizeof(displaylog_key) );
  APP_LOG(APP_LOG_LEVEL_DEBUG, "logkeys are %d, %d, %d", displaylog_key[0], displaylog_key[1], displaylog_key[2]);

  }else {
      // if no masterlog exists then it means it is the user's first time using the app.
      // masterlog will be initalized as a global variable, but nothing will be written to it yet.
    APP_LOG(APP_LOG_LEVEL_DEBUG, "key not perserved, creating new masterlog. Account is %d", masterLog);
  
  }
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
#if defined(PBL_COLOR)
  menu_layer_set_normal_colors(s_menu_layer, GColorBlack, GColorWhite);
  menu_layer_set_highlight_colors(s_menu_layer, GColorRed, GColorWhite);
#endif
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .get_cell_height = get_cell_height_callback,
      .select_click = select_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}