
#include "activities.h"


Window *s_main_window;
static MenuLayer *s_menu_layer;
static TextLayer *s_list_message_layer;

extern int masterLog;

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return LIST_MESSAGE_WINDOW_NUM_ROWS;
}



static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  switch(cell_index->row) {
    case 0:
      menu_cell_basic_draw(ctx, cell_layer, "01:00PM Swimming", NULL, NULL);
      break;
    case 1:
      menu_cell_basic_draw(ctx, cell_layer, "02:00PM Writing", NULL, NULL);
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    
    default:
      break;
  }
}

static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  switch(cell_index->row) {
    case 0:
      break;
    case 1:
      break;
    case 2:
//       transport_window_push();
      break;
    case 3:
//       eat_window_push();
      break;
    case 4:
//       programming_window_push();
      break;
  }
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ?
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    LIST_MESSAGE_WINDOW_CELL_HEIGHT);
}

static void window_load(Window *window) {
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
  
  const GEdgeInsets message_insets = {.top = 140};
  s_list_message_layer = text_layer_create(grect_inset(bounds, message_insets));
  text_layer_set_text_alignment(s_list_message_layer, GTextAlignmentCenter);
  text_layer_set_text(s_list_message_layer, LIST_MESSAGE_WINDOW_HINT_TEXT);
  layer_add_child(window_layer, text_layer_get_layer(s_list_message_layer));
  
  
//   int logsize = persist_get_size(masterLog);
//   int arraySize = logsize/sizeof(int);
//   int displaylog[arraySize];
//   displaylog[arraySize] = persist_read_data(masterLog, (void *)displaylog, logsize );
  
//     //ceating a new masterLog
//   int newMasterLog[arraySize];
    
//   //incrementing to copy over the data
//   for ( int i = 0 ; i <= arraySize; i++) {
//       newMasterLog[i] = displaylog[i];
//   };

// // for (int i =0; i <= arraySize; i++) {

//   int lastkey = newMasterLog[arraySize];
//   int actLogSize = persist_get_size(lastkey);
//   int actArraySize = actLogSize/sizeof(int);
//   int actLog[actArraySize];
//   actLog[actArraySize] = persist_read_data(lastkey, (void *)actLog, actLogSize);

//   for (int i = 0; i < actArraySize; i++){
//       APP_LOG(APP_LOG_LEVEL_DEBUG, "the actLog is %d", actLog[i]);
//   }
// // }
  
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  text_layer_destroy(s_list_message_layer);

  window_destroy(window);
  s_main_window = NULL;
}

void timeline_window_push() {
  if(!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}
