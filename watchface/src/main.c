#include <pebble.h>

#define KEY_ANIMATION 0
#define KEY_DESTINATION 1

static Window *s_main_window;

static const uint8_t s_foregrounds[] = {
    RESOURCE_ID_FOREGROUND_10,
    RESOURCE_ID_FOREGROUND_20,
    RESOURCE_ID_FOREGROUND_30,
    RESOURCE_ID_FOREGROUND_40,
    RESOURCE_ID_FOREGROUND_50,
    RESOURCE_ID_FOREGROUND_60,
    RESOURCE_ID_FOREGROUND_70,
    RESOURCE_ID_FOREGROUND_80,
    RESOURCE_ID_FOREGROUND_90,
    RESOURCE_ID_FOREGROUND_100
};
static const uint8_t s_foregrounds_odd[] = {
    RESOURCE_ID_FOREGROUND_O_10,
    RESOURCE_ID_FOREGROUND_O_20,
    RESOURCE_ID_FOREGROUND_O_30,
    RESOURCE_ID_FOREGROUND_O_40,
    RESOURCE_ID_FOREGROUND_O_50,
    RESOURCE_ID_FOREGROUND_O_60,
    RESOURCE_ID_FOREGROUND_O_70,
    RESOURCE_ID_FOREGROUND_O_80,
    RESOURCE_ID_FOREGROUND_O_90,
    RESOURCE_ID_FOREGROUND_O_100
};
static const uint8_t s_foregrounds_even[] = {
    RESOURCE_ID_FOREGROUND_E_10,
    RESOURCE_ID_FOREGROUND_E_20,
    RESOURCE_ID_FOREGROUND_E_30,
    RESOURCE_ID_FOREGROUND_E_40,
    RESOURCE_ID_FOREGROUND_E_50,
    RESOURCE_ID_FOREGROUND_E_60,
    RESOURCE_ID_FOREGROUND_E_70,
    RESOURCE_ID_FOREGROUND_E_80,
    RESOURCE_ID_FOREGROUND_E_90,
    RESOURCE_ID_FOREGROUND_E_100
};

static BitmapLayer *s_background_layer;
static BitmapLayer *s_foreground_layer;
static GBitmap *s_background_bitmap;
static GBitmap *s_foreground_bitmap;

static GFont s_time_font_small;
static GFont s_time_font_big;

static TextLayer *s_dtime_layer_M;
static TextLayer *s_dtime_layer_D;
static TextLayer *s_dtime_layer_Y;
static TextLayer *s_dtime_layer_h;
static TextLayer *s_dtime_layer_m;

static TextLayer *s_dtime_layer_arrived_1;
static TextLayer *s_dtime_layer_arrived_2;

static TextLayer *s_ptime_layer_M;
static TextLayer *s_ptime_layer_D;
static TextLayer *s_ptime_layer_Y;
static TextLayer *s_ptime_layer_h;
static TextLayer *s_ptime_layer_m;

static TextLayer *s_btime_layer_h;
static TextLayer *s_btime_layer_m;

static void updateDdate()
{
    int32_t dtime = persist_read_int(KEY_DESTINATION);
    struct tm *tick_time = localtime(&dtime);

    // Create long-live text buffer
    static char dbuffer_M[] = "###";
    static char dbuffer_D[] = "00";
    static char dbuffer_Y[] = "0000";
    static char dbuffer_h[] = "00";
    static char dbuffer_m[] = "00";

    // Write the current hours into the buffer
    if(clock_is_24h_style() == true)
        strftime(dbuffer_h, sizeof("00"), "%H", tick_time);
    else
        strftime(dbuffer_h, sizeof("00"), "%I", tick_time);
    text_layer_set_text(s_dtime_layer_h, dbuffer_h);

    // Write the current minutes into the buffer
    strftime(dbuffer_m, sizeof("00"), "%M", tick_time);
    text_layer_set_text(s_dtime_layer_m, dbuffer_m);

    // Month
    strftime(dbuffer_M, sizeof("###"), "%b", tick_time);
    text_layer_set_text(s_dtime_layer_M, dbuffer_M);
    // Day
    strftime(dbuffer_D, sizeof("00"), "%d", tick_time);
    text_layer_set_text(s_dtime_layer_D, dbuffer_D);
    // Year
    strftime(dbuffer_Y, sizeof("0000"), "%Y", tick_time);
    text_layer_set_text(s_dtime_layer_Y, dbuffer_Y);

    layer_set_hidden(text_layer_get_layer(s_dtime_layer_arrived_1), true );
    layer_set_hidden(text_layer_get_layer(s_dtime_layer_arrived_2), true );
}

static void updatePdate()
{
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    // Create long-live text buffer
    static char buffer_M[] = "###";
    static char buffer_D[] = "00";
    static char buffer_Y[] = "0000";

    // Month
    strftime(buffer_M, sizeof("###"), "%b", tick_time);
    text_layer_set_text(s_ptime_layer_M, buffer_M);
    // Day
    strftime(buffer_D, sizeof("00"), "%d", tick_time);
    text_layer_set_text(s_ptime_layer_D, buffer_D);
    // Year
    strftime(buffer_Y, sizeof("0000"), "%Y", tick_time);
    text_layer_set_text(s_ptime_layer_Y, buffer_Y);
}

static void updatePtime()
{
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    // Create long-live text buffer
    static char buffer_h[] = "00";
    static char buffer_m[] = "00";

    // Write the current hours into the buffer
    if(clock_is_24h_style() == true)
        strftime(buffer_h, sizeof("00"), "%H", tick_time);
    else
        strftime(buffer_h, sizeof("00"), "%I", tick_time);
    text_layer_set_text(s_ptime_layer_h, buffer_h);

    // Write the current minutes into the buffer
    strftime(buffer_m, sizeof("00"), "%M", tick_time);
    text_layer_set_text(s_ptime_layer_m, buffer_m);

    text_layer_set_text(s_btime_layer_h, buffer_h);
    text_layer_set_text(s_btime_layer_m, buffer_m);
}

static void inboxReceivedHandler(DictionaryIterator *iter, void *context)
{
    Tuple *value_t = NULL;

    // Animation
    value_t = dict_find(iter, KEY_ANIMATION);
    if( value_t ) {
        // Apply
        uint32_t animation = value_t->value->int32;
        persist_write_int(KEY_ANIMATION, animation);
    }

    // Destination date
    value_t = dict_find(iter, KEY_DESTINATION);
    if( value_t ) {
        // Apply
        time_t destination = value_t->value->int32;
        persist_write_int(KEY_DESTINATION, destination);

        // Set destination date
        updateDdate();
    }
}

void inboxDroppedHandler(AppMessageResult reason, void *context)
{
    // Incoming message dropped
    app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "AppMessage Dropped: %d", reason);
}

static void loadForegroundImage(uint8_t resource)
{
    static uint8_t last_foreground_image = 0;

    if( resource != last_foreground_image ) {
        if( s_foreground_bitmap )
            gbitmap_destroy(s_foreground_bitmap);

        s_foreground_bitmap = gbitmap_create_with_resource(resource);
        bitmap_layer_set_bitmap(s_foreground_layer, s_foreground_bitmap);
        layer_mark_dirty(bitmap_layer_get_layer(s_foreground_layer));
        last_foreground_image = resource;
    }
}

static void updateForeground()
{
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    BatteryChargeState charge_state = battery_state_service_peek();
    uint8_t current_percent = charge_state.charge_percent;
    if( charge_state.is_charging ) {
        static uint8_t last_percent = 2;
        if( last_percent > 8 )
            last_percent = 2;
        current_percent = ++last_percent;
    } else {
        current_percent = current_percent > 99 ? 9 : current_percent / 10;
    }

    if( persist_read_int(KEY_ANIMATION) > 0 ) {
        time_t unixtime = mktime(tick_time);
        if( persist_read_int(KEY_ANIMATION) == 1 )
            unixtime /= 60;
        if( unixtime % 2 )
            loadForegroundImage(s_foregrounds_odd[current_percent]);
        else
            loadForegroundImage(s_foregrounds_even[current_percent]);
    } else
        loadForegroundImage(s_foregrounds[current_percent]);
}

static void checkDtime()
{
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    // Check destination time
    time_t dtime = persist_read_int(KEY_DESTINATION);
    time_t ptime = mktime(tick_time);
    if( ptime > (dtime - 30 ) && ptime < (dtime + 90) ) {
        bool hidden = ptime > (dtime + 30);
        layer_set_hidden(text_layer_get_layer(s_dtime_layer_arrived_1), hidden );
        layer_set_hidden(text_layer_get_layer(s_dtime_layer_arrived_2), hidden );
        if( ! hidden ) {
            static const uint32_t const segments[] = { 300, 100, 400, 100, 300, 100, 400, 100, 300 };
            VibePattern pat = {
              .durations = segments,
              .num_segments = ARRAY_LENGTH(segments),
            };
            vibes_enqueue_custom_pattern(pat);
        }
    }
}

static void tickHandler(struct tm *tick_time, TimeUnits units_changed)
{
    updateForeground();

    if( units_changed & DAY_UNIT )
        updatePdate();

    if( units_changed & MINUTE_UNIT ) {
        updatePtime();
        checkDtime();
    }
}

static TextLayer* newTextLayer(int x, int y, int w, int h, GColor color, const char *text)
{
    TextLayer *layer = text_layer_create(GRect(x, y, w, h));
    text_layer_set_background_color(layer, GColorClear);
    text_layer_set_text_color(layer, color);
    text_layer_set_font(layer, s_time_font_small);
    text_layer_set_text_alignment(layer, GTextAlignmentRight);
    text_layer_set_text(layer, text);
    layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(layer));

    return layer;
}

static void mainWindowLoad(Window *window)
{
    // Create GBitmap, then set to created BitmapLayer
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
    s_foreground_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FOREGROUND_100);

    s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);

    s_foreground_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
    bitmap_layer_set_compositing_mode(s_foreground_layer, GCompOpSet);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_foreground_layer));
    bitmap_layer_set_bitmap(s_foreground_layer, s_foreground_bitmap);

    // Create mini GFont
    s_time_font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGITALREADOUT_16));
    // Create big GFont
    s_time_font_big = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGITALREADOUT_36));

    // Create destination date TextLayers
    s_dtime_layer_M = newTextLayer(17, 120, 23, 16, GColorRed, "Oct");
    s_dtime_layer_D = newTextLayer(43, 120, 14, 16, GColorRed, "21");
    s_dtime_layer_Y = newTextLayer(60, 120, 28, 16, GColorRed, "2015");
    // Create destination time TextLayers
    s_dtime_layer_h = newTextLayer(94, 120, 14, 16, GColorRed, "16");
    s_dtime_layer_m = newTextLayer(112, 120, 14, 16, GColorRed, "29");

    s_dtime_layer_arrived_2 = newTextLayer(0, 12, 144, 16, GColorWhite, "the destination time!");
    text_layer_set_background_color(s_dtime_layer_arrived_2, GColorDarkCandyAppleRed);
    text_layer_set_text_alignment(s_dtime_layer_arrived_2, GTextAlignmentCenter);
    s_dtime_layer_arrived_1 = newTextLayer(0, 0, 144, 16, GColorWhite, "You are arrived at");
    text_layer_set_background_color(s_dtime_layer_arrived_1, GColorDarkCandyAppleRed);
    text_layer_set_text_alignment(s_dtime_layer_arrived_1, GTextAlignmentCenter);

    // Create date TextLayers
    s_ptime_layer_M = newTextLayer(15, 146, 23, 16, GColorGreen, "###");
    s_ptime_layer_D = newTextLayer(42, 146, 14, 16, GColorGreen, "00");
    s_ptime_layer_Y = newTextLayer(61, 146, 28, 16, GColorGreen, "0000");
    // Create time TextLayers
    s_ptime_layer_h = newTextLayer(96, 146, 14, 16, GColorGreen, "00");
    s_ptime_layer_m = newTextLayer(115, 146, 14, 16, GColorGreen, "00");
    // Create big time TextLayers
    s_btime_layer_h = newTextLayer(35, 54, 30, 37, GColorWhite, "00");
    text_layer_set_font(s_btime_layer_h, s_time_font_big);
    s_btime_layer_m = newTextLayer(75, 54, 30, 37, GColorWhite, "00");
    text_layer_set_font(s_btime_layer_m, s_time_font_big);

    // Update graphics
    tickHandler(NULL, MINUTE_UNIT | DAY_UNIT);

    // Set destination date
    updateDdate();
}

static void mainWindowUnload(Window *window)
{
    // Destroy TextLayers
    text_layer_destroy(s_btime_layer_m);
    text_layer_destroy(s_btime_layer_h);
    text_layer_destroy(s_ptime_layer_m);
    text_layer_destroy(s_ptime_layer_h);
    text_layer_destroy(s_ptime_layer_Y);
    text_layer_destroy(s_ptime_layer_D);
    text_layer_destroy(s_ptime_layer_M);

    text_layer_destroy(s_dtime_layer_arrived_1);
    text_layer_destroy(s_dtime_layer_arrived_2);

    text_layer_destroy(s_dtime_layer_m);
    text_layer_destroy(s_dtime_layer_h);
    text_layer_destroy(s_dtime_layer_Y);
    text_layer_destroy(s_dtime_layer_D);
    text_layer_destroy(s_dtime_layer_M);

    // Unload GFont
    fonts_unload_custom_font(s_time_font_small);

    // Destroy GBitmaps
    gbitmap_destroy(s_foreground_bitmap);
    gbitmap_destroy(s_background_bitmap);

    // Destroy BitmapLayer
    bitmap_layer_destroy(s_foreground_layer);
    bitmap_layer_destroy(s_background_layer);
}

static void setDefaults()
{
    if( ! persist_exists(KEY_ANIMATION) )
        persist_write_int(KEY_ANIMATION, 0); // No animation
    if( ! persist_exists(KEY_DESTINATION) )
        persist_write_int(KEY_DESTINATION, 1445470140); // 21 Oct 2015 16:29:00 GMT-7
}

static void init()
{
    // Set defaults
    setDefaults();

    // Create main Window element and assign to pointer
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = mainWindowLoad,
        .unload = mainWindowUnload
    });

    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);

    app_message_register_inbox_received(inboxReceivedHandler);
    app_message_register_inbox_dropped(inboxDroppedHandler);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

    // Register with TickTimerService
    tick_timer_service_subscribe(SECOND_UNIT, tickHandler);
}

static void deinit()
{
    // Destroy Window
    window_destroy(s_main_window);
}

int main(void)
{
    init();
    app_event_loop();
    deinit();
}
