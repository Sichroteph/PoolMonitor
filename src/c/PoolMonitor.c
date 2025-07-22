#include <pebble.h>

#define KEY_TEMP 0
#define KEY_PH 1
#define KEY_ORP 2
#define MESSAGE_KEY_READY 3
#define KEY_RADIO_UNITS 4

#ifdef PBL_COLOR
#define IS_COLOR true
#else
#define IS_COLOR false
#endif

static Window *s_window;
static TextLayer *s_text_temp_layer;
static TextLayer *s_text_ORP_layer;
static TextLayer *s_text_PH_layer;
static TextLayer *s_text_TITLE_layer;
static BitmapLayer *s_bar1_layer;
static BitmapLayer *s_bar2_layer;
static BitmapLayer *s_bar3_layer;
static BitmapLayer *s_arrow1_layer;
static BitmapLayer *s_arrow2_layer;
static BitmapLayer *s_arrow3_layer;

static GBitmap *s_bar;
static GBitmap *s_arrow;
int unit_value;


int offset_Y1 = 27;
int offset_Y2 = 19;
int offset_Y3 = 11;

int arrowPosition(float value, int min, int max)
{
  int bar_size = 144;

  if (value <= min)
  {
    value = min;
  }
  if (value >= max)
  {
    value = max;
  }

  float factor = (value - min) / (max - min);

  int result = (int)((factor * bar_size));

  return result - 6;
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMP);
  Tuple *ph_tuple = dict_find(iterator, KEY_PH);
  Tuple *orp_tuple = dict_find(iterator, KEY_ORP);
  Tuple *unit_tuple = dict_find(iterator, KEY_RADIO_UNITS);

  if (temp_tuple && ph_tuple && orp_tuple)
  {
    static char temp_buffer[30];
    static char ph_buffer[30];
    static char orp_buffer[30];

    float temp_value = temp_tuple->value->int32;
    float orp_value = orp_tuple->value->int32;
    float ph_value = ph_tuple->value->int32;

    snprintf(temp_buffer, sizeof(temp_buffer), "%d.%01d °C", (int)(temp_tuple->value->int32 / 10), (int)(temp_tuple->value->int32 % 10));
    snprintf(ph_buffer, sizeof(ph_buffer), "%d.%02d pH", (int)(ph_tuple->value->int32 / 100), (int)(ph_tuple->value->int32 % 100));
    snprintf(orp_buffer, sizeof(orp_buffer), "%d mV", (int)orp_tuple->value->int32);
    
    unit_value = unit_tuple->value->int32;

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Unit value: %d", unit_value);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "temp_buffer : %s", temp_buffer);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "ph_buffer : %s", ph_buffer);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "orp_buffer : %s", orp_buffer);

    text_layer_set_text(s_text_temp_layer, temp_buffer);
    text_layer_set_text(s_text_ORP_layer, orp_buffer);
    text_layer_set_text(s_text_PH_layer, ph_buffer);


    layer_set_frame(bitmap_layer_get_layer(s_arrow1_layer), GRect(arrowPosition(temp_value, 120, 370), 35 + offset_Y1, 13, 7));
    layer_set_frame(bitmap_layer_get_layer(s_arrow2_layer), GRect(arrowPosition(orp_value, 400, 1050), 91 + offset_Y2, 13, 7));
    layer_set_frame(bitmap_layer_get_layer(s_arrow3_layer), GRect(arrowPosition(ph_value, 640, 850), 147 + offset_Y3, 13, 7));
    
    // layer_set_frame(bitmap_layer_get_layer(s_arrow1_layer), GRect(, 35, 13, 7));
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "arrow position : %d", arrowPosition(temp_value, 12, 37));
    vibes_double_pulse();

  }
}

static void send_ready_message()
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending message");

  DictionaryIterator *out_iter;
  AppMessageResult result = app_message_outbox_begin(&out_iter);

  if (result == APP_MSG_OK)
  {
    dict_write_uint8(out_iter, MESSAGE_KEY_READY, 1);

    result = app_message_outbox_send();
    if (result != APP_MSG_OK)
    {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Erreur lors de l'envoi du message : %d", result);
    }
  }
  else
  {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Erreur lors de la préparation du message : %d", result);
  }
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context)
{
  // send_ready_message();
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context)
{
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context)
{
}

static void prv_click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
}

static void prv_window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(s_window, GColorBlack);
  s_text_temp_layer = text_layer_create(GRect(4, -3+offset_Y1, bounds.size.w, 30));
  text_layer_set_text(s_text_temp_layer, "Loading");
  text_layer_set_text_alignment(s_text_temp_layer, GTextAlignmentLeft);
  text_layer_set_font(s_text_temp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_color(s_text_temp_layer, GColorWhite);
  text_layer_set_background_color(s_text_temp_layer, GColorBlack);
  layer_add_child(window_layer, text_layer_get_layer(s_text_temp_layer));

  s_text_ORP_layer = text_layer_create(GRect(4, 53+offset_Y2, bounds.size.w, 30));
  text_layer_set_text(s_text_ORP_layer, "Loading");
  text_layer_set_text_alignment(s_text_ORP_layer, GTextAlignmentLeft);
  text_layer_set_font(s_text_ORP_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_color(s_text_ORP_layer, GColorWhite);
  text_layer_set_background_color(s_text_ORP_layer, GColorBlack);
  layer_add_child(window_layer, text_layer_get_layer(s_text_ORP_layer));

  s_text_PH_layer = text_layer_create(GRect(4, 109+offset_Y3, bounds.size.w, 30));
  text_layer_set_text(s_text_PH_layer, "Loading");
  text_layer_set_text_alignment(s_text_PH_layer, GTextAlignmentLeft);
  text_layer_set_font(s_text_PH_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_color(s_text_PH_layer, GColorWhite);
  text_layer_set_background_color(s_text_PH_layer, GColorBlack);
  layer_add_child(window_layer, text_layer_get_layer(s_text_PH_layer));

  s_text_TITLE_layer = text_layer_create(GRect(0, -10, 144, 30));
  text_layer_set_text(s_text_TITLE_layer, "POOL MONITOR");
  text_layer_set_text_alignment(s_text_TITLE_layer, GTextAlignmentCenter);
  text_layer_set_font(s_text_TITLE_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_color(s_text_TITLE_layer, GColorWhite);
  text_layer_set_background_color(s_text_TITLE_layer, GColorBlack);
  layer_add_child(window_layer, text_layer_get_layer(s_text_TITLE_layer));

  // Charger l'icône
  if (IS_COLOR)
    s_bar = gbitmap_create_with_resource(RESOURCE_ID_BAR_C);
  else
    s_bar = gbitmap_create_with_resource(RESOURCE_ID_BAR_W);

  // Créer un BitmapLayer pour afficher l'icône
  s_bar1_layer = bitmap_layer_create(GRect(0, -6+offset_Y1, 144, 75)); 
  bitmap_layer_set_bitmap(s_bar1_layer, s_bar);

  s_bar2_layer = bitmap_layer_create(GRect(0, 50+offset_Y2, 144, 75)); 
  bitmap_layer_set_bitmap(s_bar2_layer, s_bar);

  s_bar3_layer = bitmap_layer_create(GRect(0, 106+offset_Y3, 144, 75)); 
  bitmap_layer_set_bitmap(s_bar3_layer, s_bar);

  // -------------------------------------------------------------

  s_arrow = gbitmap_create_with_resource(RESOURCE_ID_ARROW);

  s_arrow1_layer = bitmap_layer_create(GRect(66, 35+offset_Y1, 13, 7)); 
  bitmap_layer_set_bitmap(s_arrow1_layer, s_arrow);

  s_arrow2_layer = bitmap_layer_create(GRect(66, 91+offset_Y2, 13, 7)); 
  bitmap_layer_set_bitmap(s_arrow2_layer, s_arrow);

  s_arrow3_layer = bitmap_layer_create(GRect(66, 147+offset_Y3, 13, 7)); 
  bitmap_layer_set_bitmap(s_arrow3_layer, s_arrow);

  layer_add_child(window_layer, bitmap_layer_get_layer(s_bar1_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bar2_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bar3_layer));

  layer_add_child(window_layer, bitmap_layer_get_layer(s_arrow1_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_arrow2_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_arrow3_layer));
}

static void prv_window_unload(Window *window)
{
  text_layer_destroy(s_text_temp_layer);
  text_layer_destroy(s_text_ORP_layer);
  text_layer_destroy(s_text_PH_layer);
  text_layer_destroy(s_text_TITLE_layer);

  // Détruire le BitmapLayer et l'icône
  bitmap_layer_destroy(s_bar1_layer);
  bitmap_layer_destroy(s_bar2_layer);
  bitmap_layer_destroy(s_bar3_layer);
  bitmap_layer_destroy(s_arrow1_layer);
  bitmap_layer_destroy(s_arrow2_layer);
  bitmap_layer_destroy(s_arrow3_layer);
  gbitmap_destroy(s_bar);
  gbitmap_destroy(s_arrow);
}

static void prv_init(void)
{
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = prv_window_load,
                                           .unload = prv_window_unload,
                                       });

  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void prv_deinit(void)
{
  window_destroy(s_window);
}

int main(void)
{
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}