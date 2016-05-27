#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_hour_layer;
static TextLayer *s_minutes_layer;
static TextLayer *s_minute_tens_layer;
static TextLayer *s_minute_ones_layer;
bool displayTime = true;
int flicks = 0;
int secondsSinceFlick = 0;
char *colorBinary = "000000";
/**/

void tobin6str(int value, char* output){
  int i;
  output[6] = '\0';
  for (i = 5; i >= 0; --i, value >>= 1)
  {
    output[i] = (value & 1) + '0';
  }
}

void displayTextTime(char *hours, char *minutes, char *dayWeek, char *month, char *day){
  static char *textHours[] = {
    "one","two","three","four","five","six",
    "seven","eight","nine","ten","eleven","twelve"
  };
  static char *minuteTensArray[] = {
    "ten","twenty","thirty","forty","fifty"
  };
  static char *minuteOnesArray[] = {
    "one","two","three","four","five","six","seven","eight","nine"
  };
  
  int minutesNumber = atoi(minutes);
  int minuteTens = minutesNumber / 10;
  int minuteOnes = minutesNumber % 10;
  char *minuteTensText = "";
  char *minuteOnesText = "";
  
  int colorNumber = minutesNumber + 1;
  if(colorNumber >= 21)
    colorNumber += 1;
  if(colorNumber >= 42)
    colorNumber += 1;
  
  tobin6str(colorNumber, colorBinary);
  int rValue = ((colorBinary[0]-48) * 170) + ((colorBinary[1]-48) * 85);
  int gValue = ((colorBinary[2]-48) * 170) + ((colorBinary[3]-48) * 85);
  int bValue = ((colorBinary[4]-48) * 170) + ((colorBinary[5]-48) * 85);
  int colorTotal = rValue + gValue + bValue;
  
  window_set_background_color(s_main_window, GColorFromRGB(rValue, gValue, bValue));
  if(colorTotal <= 170)
    text_layer_set_text_color(s_hour_layer, GColorWhite);
  else
    text_layer_set_text_color(s_hour_layer, GColorBlack);
  
  switch(minutesNumber){
    case 0: break;
    case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:
      minuteTensText = minuteOnesArray[minuteOnes-1];
    break;
    case 11: minuteTensText = "eleven"; break;
    case 12: minuteTensText = "twelve"; break;
    case 13: minuteTensText = "thirteen"; break;
    case 14: minuteTensText = "fourteen"; break;
    case 15: minuteTensText = "fifteen"; break;
    case 16: minuteTensText = "sixteen"; break;
    case 17: minuteTensText = "seventeen"; break;
    case 18: minuteTensText = "eighteen"; break;
    case 19: minuteTensText = "nineteen"; break;
    default:
      minuteTensText = minuteTensArray[minuteTens-1];
      if (minuteOnes != 0){
        minuteOnesText = minuteOnesArray[minuteOnes-1];
      }
    break;
  }
  
  if(displayTime){
    text_layer_set_text(s_hour_layer, textHours[atoi(hours)-1]);
    text_layer_set_text(s_minute_tens_layer, minuteTensText);
    text_layer_set_text(s_minute_ones_layer, minuteOnesText);  
  }
  else {
    text_layer_set_text(s_hour_layer, dayWeek);
    text_layer_set_text(s_minute_tens_layer, month);
    text_layer_set_text(s_minute_ones_layer, day); 
  }
}

static void main_window_load(Window *window) {
  window_set_background_color(s_main_window, GColorWhite);

  s_hour_layer = text_layer_create(GRect(3, 78, 139, 47));
  text_layer_set_background_color(s_hour_layer, GColorClear); 
  text_layer_set_text_color(s_hour_layer, GColorBlack);
  
  s_minutes_layer = text_layer_create(GRect(0, 126, 144, 42));
  text_layer_set_background_color(s_minutes_layer, GColorWhite);
  text_layer_set_text_color(s_minutes_layer, GColorBlack);
  
  s_minute_tens_layer = text_layer_create(GRect(5, 124, 139, 42));
  text_layer_set_background_color(s_minute_tens_layer, GColorClear);
  text_layer_set_text_color(s_minute_tens_layer, GColorBlack);
  s_minute_ones_layer = text_layer_create(GRect(5, 144, 139, 42));
  text_layer_set_background_color(s_minute_ones_layer, GColorClear);
  text_layer_set_text_color(s_minute_ones_layer, GColorBlack);

  text_layer_set_font(s_hour_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_L_B_40)));
  text_layer_set_font(s_minute_tens_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_L_R_20)));
  text_layer_set_font(s_minute_ones_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_L_R_20)));

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minutes_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minute_tens_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minute_ones_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_minutes_layer);
  text_layer_destroy(s_minute_tens_layer);
  text_layer_destroy(s_minute_ones_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  static char hourBuffer[] = "00";
  static char minuteBuffer[] = "00";
  static char dayWeekBuffer[] = "Mon";
  static char monthBuffer[] = "September";
  static char dayBuffer[] = "00";
  
  strftime(hourBuffer, sizeof(hourBuffer), "%I", tick_time);
  strftime(minuteBuffer, sizeof(minuteBuffer), "%M", tick_time);
  strftime(dayWeekBuffer, sizeof(dayWeekBuffer), "%a", tick_time);
  strftime(monthBuffer, sizeof(monthBuffer), "%B", tick_time);
  strftime(dayBuffer, sizeof(dayBuffer), "%d", tick_time);
  
  secondsSinceFlick++;
  if(secondsSinceFlick > 5){
    flicks = 0;
    displayTime = true;
  }

  displayTextTime(hourBuffer, minuteBuffer, dayWeekBuffer, monthBuffer, dayBuffer);
}

static void updateTime(){
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  tick_handler(tick_time, SECOND_UNIT);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  flicks++;
  secondsSinceFlick = 0;
  if(flicks>1){
    displayTime = !displayTime;
    updateTime();
  }
}
  
static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  accel_tap_service_subscribe(tap_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}