
#include <pebble.h>

#define  TIME_STORE_KEY 1
#define  TIMER_DEFAULT 0
  
static Window *sleep_main_window;
static TextLayer *s_label_layer;
static BitmapLayer *s_icon_layer;
static ActionBarLayer *s_action_bar_layer;

extern Window *s_main_window;

static GBitmap *s_icon_bitmap, *s_pause_bitmap, *s_cross_bitmap, *s_play_bitmap;

char tmp_sleep[16];
uint32_t time_stopwatch_sleep = TIMER_DEFAULT;
bool pause_sleep = false;

uint32_t time_begin_sleep, time_end_sleep;

static void timer_time_str(uint32_t timer_time, bool showHours, char* str, int str_len) {
  int hours = timer_time / 3600;
  int minutes = (showHours ? (timer_time % 3600) : timer_time) / 60;
  int seconds = (showHours ? (timer_time % 3600) : timer_time) % 60;
  if (showHours) {
    snprintf(str, str_len, "%02d:%02d:%02d", hours, minutes, seconds);
  }
  else {
    snprintf(str, str_len, "%02d:%02d", minutes, seconds);
  }
}


static void select_click_handler(ClickRecognizerRef recognizer, void *context) { 
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "I've clicked");
  

}
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  pause_sleep = !pause_sleep;

  //when it is paused, it logs the begin_sleepning time and the end_sleep time for this session and send_sleep the data to firebase
  if(pause_sleep){
    action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP, s_play_bitmap);
    time_end_sleep = (uint32_t)time(NULL);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "time_begin_sleep: %d time_end_sleep %d", (int)time_begin_sleep, (int)time_end_sleep);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "messagekey begin_sleep: %d ", (int)MESSAGE_KEY_BEGIN);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "messagekey end_sleep: %d ", (int)MESSAGE_KEY_END);
    
    //begin_sleepning to encode and send_sleep time data
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    
    dict_write_int32(iter, MESSAGE_KEY_BEGIN, (int)time_begin_sleep);
    dict_write_int32(iter, MESSAGE_KEY_END, (int)time_end_sleep);
    dict_write_cstring(iter, MESSAGE_KEY_ACTIVITY, "sleeping");
    
    app_message_outbox_send();

  }else {
    action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP, s_pause_bitmap);
    time_begin_sleep = (uint32_t)time(NULL);
  }
    
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  pause_sleep = true;
  time_end_sleep = (uint32_t)time(NULL);
  
  time_stopwatch_sleep = TIMER_DEFAULT;
  
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
        
  dict_write_int32(iter, MESSAGE_KEY_BEGIN, (int)time_begin_sleep);
  dict_write_int32(iter, MESSAGE_KEY_END, (int)time_end_sleep);
  dict_write_cstring(iter, MESSAGE_KEY_ACTIVITY, "sleeping");
    
  app_message_outbox_send();
  
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void update_time() {
  if(!pause_sleep ){
    time_stopwatch_sleep++;
  }
  
  timer_time_str(time_stopwatch_sleep, true, tmp_sleep, sizeof(tmp_sleep));
  
  text_layer_set_text(s_label_layer, tmp_sleep );
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send_sleep success!");
}

static void window_load(Window *window) {
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SLEEP);

  const GEdgeInsets icon_insets = {.top = 7, .right = 28, .bottom = 56, .left = 14};
  s_icon_layer = bitmap_layer_create(grect_inset(bounds, icon_insets));
  bitmap_layer_set_bitmap(s_icon_layer, s_icon_bitmap);
  bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_icon_layer));

  const GEdgeInsets label_insets = {.top = 112, .right = ACTION_BAR_WIDTH, .left = ACTION_BAR_WIDTH / 2};
  s_label_layer = text_layer_create(grect_inset(bounds, label_insets));
  
  //time updates here:
  text_layer_set_text(s_label_layer, tmp_sleep );
  text_layer_set_background_color(s_label_layer, GColorClear);
  text_layer_set_text_alignment(s_label_layer, GTextAlignmentCenter);
  text_layer_set_font(s_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));

  s_pause_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PAUSE);
  s_cross_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CROSS);
  s_play_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PLAY);
  
  s_action_bar_layer = action_bar_layer_create();
  action_bar_layer_set_click_config_provider(s_action_bar_layer, click_config_provider);
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP, s_pause_bitmap);
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_DOWN, s_cross_bitmap);
  action_bar_layer_add_to_window(s_action_bar_layer, window);
  
  //updating the time
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
    // Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
}

static void window_unload(Window *window) {

}

void sleep_window_push() {
  if(!sleep_main_window) {
    sleep_main_window = window_create();
    window_set_background_color(sleep_main_window, PBL_IF_COLOR_ELSE(GColorJaegerGreen, GColorWhite));
    window_set_window_handlers(sleep_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    
    app_message_register_outbox_sent(outbox_sent_callback);

    
  }
  window_stack_push(sleep_main_window, true);
}
