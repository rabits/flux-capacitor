#include "main.h"
#include "config.h"
#include "animation.h"

static time_t s_current_time = 0;
static time_t s_present_time = 0;
static int32_t s_destination_time = 0;
static int32_t s_current_time_offset = 0;

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
static Layer *s_dtime_layer_apm;
static bool s_dtime_pm;

static TextLayer *s_dtime_layer_arrived_1;
static TextLayer *s_dtime_layer_arrived_2;
static TextLayer *s_dtime_layer_outatime;

static TextLayer *s_ptime_layer_M;
static TextLayer *s_ptime_layer_D;
static TextLayer *s_ptime_layer_Y;
static TextLayer *s_ptime_layer_h;
static TextLayer *s_ptime_layer_m;
static Layer *s_ptime_layer_apm;
static bool s_ptime_pm;

static TextLayer *s_btime_layer_h;
static TextLayer *s_btime_layer_m;

void setTime(TimeUnits units_changed)
{
    s_current_time = time(NULL);
    s_present_time = s_current_time - s_current_time_offset;

    if( s_current_time_offset != 0 ) {
        s_destination_time = s_current_time;
        if( units_changed & MINUTE_UNIT )
            updateDdate();
    }
}

void switchTimeOffset()
{
    if( s_current_time_offset != 0 )
        setTimeOffset(0, true);
    else {
        time_t offset_time = time(NULL) - (time(NULL) % 60) - s_destination_time;
        setTimeOffset(offset_time, true);
    }
}

void setTimeOffset(int32_t offset, bool update)
{
    if( offset == 0 )
        s_destination_time = persist_read_int(KEY_DESTINATION);

    s_current_time_offset = offset;

    if( update ) {
        layer_set_hidden(text_layer_get_layer(s_dtime_layer_outatime), offset == 0 );
        updateDdate();
        tickHandler(NULL, MINUTE_UNIT | DAY_UNIT);
    }
}

void setBigTimeHidden(bool hidden)
{
    layer_set_hidden(text_layer_get_layer(s_btime_layer_h), hidden );
    layer_set_hidden(text_layer_get_layer(s_btime_layer_m), hidden );
}

void updateDdate()
{
    struct tm *tick_time = localtime(&s_destination_time);

    // Create long-live text buffer
    static char dbuffer_M[] = "###";
    static char dbuffer_D[] = "00";
    static char dbuffer_Y[] = "0000";
    static char dbuffer_h[] = "00";
    static char dbuffer_m[] = "00";

    // Write the current hours into the buffer
    if( clock_is_24h_style() == true )
        strftime(dbuffer_h, sizeof("00"), "%H", tick_time);
    else
        strftime(dbuffer_h, sizeof("00"), "%I", tick_time);
    text_layer_set_text(s_dtime_layer_h, dbuffer_h);

    if( s_dtime_pm != ((tick_time->tm_hour) > 11) ) {
        s_dtime_pm = (tick_time->tm_hour) > 11;
        layer_mark_dirty(s_dtime_layer_apm);
    }

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

void updatePdate()
{
    struct tm *tick_time = localtime(&s_present_time);

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

void updatePtime()
{
    struct tm *tick_time = localtime(&s_present_time);

    // Create long-live text buffer
    static char buffer_h[] = "00";
    static char buffer_m[] = "00";

    // Write the current hours into the buffer
    if(clock_is_24h_style() == true)
        strftime(buffer_h, sizeof("00"), "%H", tick_time);
    else
        strftime(buffer_h, sizeof("00"), "%I", tick_time);
    text_layer_set_text(s_ptime_layer_h, buffer_h);

    if( s_ptime_pm != ((tick_time->tm_hour) > 11) ) {
        s_ptime_pm = (tick_time->tm_hour) > 11;
        layer_mark_dirty(s_ptime_layer_apm);
    }

    // Write the current minutes into the buffer
    strftime(buffer_m, sizeof("00"), "%M", tick_time);
    text_layer_set_text(s_ptime_layer_m, buffer_m);

    text_layer_set_text(s_btime_layer_h, buffer_h);
    text_layer_set_text(s_btime_layer_m, buffer_m);
}

void loadForegroundImage(uint8_t resource)
{
    static uint8_t last_foreground_image = RESOURCE_ID_FOREGROUND_100;

    if( resource != last_foreground_image ) {
        if( s_foreground_bitmap )
            gbitmap_destroy(s_foreground_bitmap);

        s_foreground_bitmap = gbitmap_create_with_resource(resource);
        bitmap_layer_set_bitmap(s_foreground_layer, s_foreground_bitmap);
        layer_mark_dirty(bitmap_layer_get_layer(s_foreground_layer));
        last_foreground_image = resource;
    }
}

void loadBackgroundImage(uint8_t resource)
{
    static uint8_t last_background_image = RESOURCE_ID_BACKGROUND;

    if( resource != last_background_image ) {
        if( s_background_bitmap )
            gbitmap_destroy(s_background_bitmap);

        s_background_bitmap = gbitmap_create_with_resource(resource);
        bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
        layer_mark_dirty(bitmap_layer_get_layer(s_background_layer));
        last_background_image = resource;
    }
}

void checkDtime()
{
    if( persist_read_int(KEY_TIMEMACHINE) > 0 ) {
        int32_t dtime = persist_read_int(KEY_DESTINATION);
        if( s_present_time > (dtime - 30 ) && s_present_time < (dtime + 90) ) {
            bool hidden = s_present_time > (dtime + 30);
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

static void ptimeApmDraw(Layer *layer, GContext *ctx)
{
    graphics_context_set_fill_color(ctx, GColorDarkGray);
    graphics_fill_circle(ctx, GPoint(2, 2), 2);
    graphics_fill_circle(ctx, GPoint(2, 7), 2);

    graphics_context_set_fill_color(ctx, GColorYellow);
    if( s_ptime_pm ) {
        graphics_context_set_fill_color(ctx, GColorYellow);
        graphics_fill_circle(ctx, GPoint(2, 2), 1);
    } else {
        graphics_context_set_fill_color(ctx, GColorRed);
        graphics_fill_circle(ctx, GPoint(2, 7), 1);
    }
}

static void dtimeApmDraw(Layer *layer, GContext *ctx)
{
    graphics_context_set_fill_color(ctx, GColorDarkGray);
    graphics_fill_circle(ctx, GPoint(2, 2), 2);
    graphics_fill_circle(ctx, GPoint(2, 7), 2);

    if( s_dtime_pm ) {
        graphics_context_set_fill_color(ctx, GColorYellow);
        graphics_fill_circle(ctx, GPoint(2, 2), 1);
    } else {
        graphics_context_set_fill_color(ctx, GColorRed);
        graphics_fill_circle(ctx, GPoint(2, 7), 1);
    }
}

static void mainWindowLoad(Window *window)
{
    // Create GBitmap, then set to created BitmapLayer
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
    s_foreground_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FOREGROUND_100);

    s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);

    s_foreground_layer = bitmap_layer_create(GRect(26, 10, 92, 112));
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

    s_dtime_layer_outatime = newTextLayer(0, 12, 144, 16, GColorWhite, "OUTATIME");
    text_layer_set_text_alignment(s_dtime_layer_outatime, GTextAlignmentCenter);
    layer_set_hidden(text_layer_get_layer(s_dtime_layer_outatime), true );

    s_dtime_layer_arrived_2 = newTextLayer(0, 12, 144, 16, GColorWhite, "the destination time!");
    text_layer_set_background_color(s_dtime_layer_arrived_2, GColorDarkCandyAppleRed);
    text_layer_set_text_alignment(s_dtime_layer_arrived_2, GTextAlignmentCenter);
    s_dtime_layer_arrived_1 = newTextLayer(0, 0, 144, 16, GColorWhite, "You are arrived at");
    text_layer_set_background_color(s_dtime_layer_arrived_1, GColorDarkCandyAppleRed);
    text_layer_set_text_alignment(s_dtime_layer_arrived_1, GTextAlignmentCenter);

    s_dtime_layer_apm = layer_create(GRect(90, 127, 5, 10));
    layer_set_update_proc(s_dtime_layer_apm, dtimeApmDraw);
    layer_add_child(window_get_root_layer(s_main_window), s_dtime_layer_apm);


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

    s_ptime_layer_apm = layer_create(GRect(91, 152, 5, 10));
    layer_set_update_proc(s_ptime_layer_apm, ptimeApmDraw);
    layer_add_child(window_get_root_layer(s_main_window), s_ptime_layer_apm);

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

    layer_destroy(s_ptime_layer_apm);
    text_layer_destroy(s_ptime_layer_m);
    text_layer_destroy(s_ptime_layer_h);
    text_layer_destroy(s_ptime_layer_Y);
    text_layer_destroy(s_ptime_layer_D);
    text_layer_destroy(s_ptime_layer_M);

    text_layer_destroy(s_dtime_layer_outatime);
    text_layer_destroy(s_dtime_layer_arrived_1);
    text_layer_destroy(s_dtime_layer_arrived_2);

    layer_destroy(s_dtime_layer_apm);
    text_layer_destroy(s_dtime_layer_m);
    text_layer_destroy(s_dtime_layer_h);
    text_layer_destroy(s_dtime_layer_Y);
    text_layer_destroy(s_dtime_layer_D);
    text_layer_destroy(s_dtime_layer_M);

    // Unload GFont
    fonts_unload_custom_font(s_time_font_small);

    // Destroy BitmapLayer
    bitmap_layer_destroy(s_foreground_layer);
    bitmap_layer_destroy(s_background_layer);

    // Destroy GBitmaps
    gbitmap_destroy(s_foreground_bitmap);
    gbitmap_destroy(s_background_bitmap);
}

static void init()
{
    // Set defaults
    setConfigDefaults();

    // Preseed random
    srand(time(NULL));

    // Set time offset
    setTimeOffset(0, false);

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

    // Register tap handler
    accel_tap_service_subscribe(tapHandler);
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
