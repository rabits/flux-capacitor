#include <pebble.h>

static Window *s_main_window;

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

static TextLayer *s_ptime_layer_M;
static TextLayer *s_ptime_layer_D;
static TextLayer *s_ptime_layer_Y;
static TextLayer *s_ptime_layer_h;
static TextLayer *s_ptime_layer_m;

static TextLayer *s_btime_layer_h;
static TextLayer *s_btime_layer_m;

static void loadForegroundImage(uint8_t resource)
{
    if( s_foreground_bitmap )
        gbitmap_destroy(s_foreground_bitmap);
    s_foreground_bitmap = gbitmap_create_with_resource(resource);
    bitmap_layer_set_bitmap(s_foreground_layer, s_foreground_bitmap);
    layer_mark_dirty(bitmap_layer_get_layer(s_foreground_layer));
}

static void update_foreground(struct tm *tick_time)
{
    time_t unixtime = mktime(tick_time);
    if( unixtime % 2 )
        loadForegroundImage(RESOURCE_ID_FOREGROUND_100_O);
    else
        loadForegroundImage(RESOURCE_ID_FOREGROUND_100_E);
}

static void update_date(struct tm *tick_time)
{
    // Create a short-living buffer
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

static void update_time(struct tm *tick_time)
{
    // Create a short-living buffer
    static char buffer_h[] = "00";
    static char buffer_m[] = "00";

    // Write the current hours into the buffer
    if(clock_is_24h_style() == true)
        strftime(buffer_h, sizeof("00"), "%H", tick_time);
    else
        strftime(buffer_h, sizeof("00"), "%I", tick_time);
    text_layer_set_text(s_ptime_layer_h, buffer_h);
    text_layer_set_text(s_btime_layer_h, buffer_h);

    // Write the current minutes into the buffer
    strftime(buffer_m, sizeof("00"), "%M", tick_time);
    text_layer_set_text(s_ptime_layer_m, buffer_m);
    text_layer_set_text(s_btime_layer_m, buffer_m);
}

static TextLayer* new_text_layer(int x, int y, int w, int h, GColor color)
{
    TextLayer *layer = text_layer_create(GRect(x, y, w, h));
    text_layer_set_background_color(layer, GColorClear);
    text_layer_set_text_color(layer, color);
    text_layer_set_font(layer, s_time_font_small);
    text_layer_set_text_alignment(layer, GTextAlignmentRight);
    layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(layer));

    return layer;
}

static void main_window_load(Window *window)
{
    // Create GBitmap, then set to created BitmapLayer
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
    s_foreground_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FOREGROUND_100_F);

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
    s_dtime_layer_M = new_text_layer(17, 120, 23, 16, GColorRed);
    text_layer_set_text(s_dtime_layer_M, "Oct");
    s_dtime_layer_D = new_text_layer(43, 120, 14, 16, GColorRed);
    text_layer_set_text(s_dtime_layer_D, "21");
    s_dtime_layer_Y = new_text_layer(60, 120, 28, 16, GColorRed);
    text_layer_set_text(s_dtime_layer_Y, "2015");
    // Create destination time TextLayers
    s_dtime_layer_h = new_text_layer(94, 120, 14, 16, GColorRed);
    text_layer_set_text(s_dtime_layer_h, "16");
    s_dtime_layer_m = new_text_layer(112, 120, 14, 16, GColorRed);
    text_layer_set_text(s_dtime_layer_m, "29");

    // Create date TextLayers
    s_ptime_layer_M = new_text_layer(15, 146, 23, 16, GColorGreen);
    s_ptime_layer_D = new_text_layer(42, 146, 14, 16, GColorGreen);
    s_ptime_layer_Y = new_text_layer(61, 146, 28, 16, GColorGreen);
    // Create time TextLayers
    s_ptime_layer_h = new_text_layer(96, 146, 14, 16, GColorGreen);
    s_ptime_layer_m = new_text_layer(115, 146, 14, 16, GColorGreen);
    // Create big time TextLayers
    s_btime_layer_h = new_text_layer(35, 54, 30, 37, GColorWhite);
    text_layer_set_font(s_btime_layer_h, s_time_font_big);
    s_btime_layer_m = new_text_layer(75, 54, 30, 37, GColorWhite);
    text_layer_set_font(s_btime_layer_m, s_time_font_big);

    // Get a tm structure
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);

    update_date(tick_time);
    update_time(tick_time);
}

static void main_window_unload(Window *window)
{
    // Destroy TextLayers
    text_layer_destroy(s_btime_layer_m);
    text_layer_destroy(s_btime_layer_h);
    text_layer_destroy(s_ptime_layer_m);
    text_layer_destroy(s_ptime_layer_h);
    text_layer_destroy(s_ptime_layer_Y);
    text_layer_destroy(s_ptime_layer_D);
    text_layer_destroy(s_ptime_layer_M);

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

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
    if( units_changed & SECOND_UNIT )
        update_foreground(tick_time);

    if( units_changed & MINUTE_UNIT )
        update_time(tick_time);

    if( units_changed & DAY_UNIT )
        update_date(tick_time);
}

static void init()
{
    // Create main Window element and assign to pointer
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);

    // Register with TickTimerService
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
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
