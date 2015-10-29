#include "main.h"
#include "config.h"
#include "animation.h"

// FOREGROUND

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

// LIGHTNING

static int s_lightning_animation = -1;
static int s_lightning_charge = 0;
static AppTimer *s_animation_timer = NULL;

#define S_LIGHTNING_SMALL_NUM 11
static const uint8_t s_lighting_small[] = {
    RESOURCE_ID_LIGHTNING_S_T_1,
    RESOURCE_ID_LIGHTNING_S_T_2,
    RESOURCE_ID_LIGHTNING_S_T_3,
    RESOURCE_ID_LIGHTNING_S_R_1,
    RESOURCE_ID_LIGHTNING_S_R_2,
    RESOURCE_ID_LIGHTNING_S_R_3,
    RESOURCE_ID_LIGHTNING_S_R_4,
    RESOURCE_ID_LIGHTNING_S_L_1,
    RESOURCE_ID_LIGHTNING_S_L_2,
    RESOURCE_ID_LIGHTNING_S_L_3,
    RESOURCE_ID_LIGHTNING_S_L_4
};
#define S_LIGHTNING_MEDIUM_NUM 4
static const uint8_t s_lighting_medium[] = {
    RESOURCE_ID_LIGHTNING_M_1,
    RESOURCE_ID_LIGHTNING_M_2,
    RESOURCE_ID_LIGHTNING_M_3,
    RESOURCE_ID_LIGHTNING_M_4
};
#define S_LIGHTNING_BIG_NUM 4
static const uint8_t s_lighting_big[] = {
    RESOURCE_ID_LIGHTNING_B_1,
    RESOURCE_ID_LIGHTNING_B_2,
    RESOURCE_ID_LIGHTNING_B_3,
    RESOURCE_ID_LIGHTNING_B_4
};

void commonAnimationForeground()
{
    time_t unixtime = time(NULL);

    BatteryChargeState charge_state = battery_state_service_peek();
    uint8_t battery_state = (charge_state.charge_percent - 1) / 10;

    loadBackgroundImage(RESOURCE_ID_BACKGROUND);

    if( charge_state.is_charging ) {
        static int last_state = 2;
        battery_state = unixtime % 10;
        if( battery_state < 2 )
            battery_state = 2;
        if( battery_state == 9 && last_state != 9 ) {
            last_state = 9;
            startLightningAnimation(0);
            return;
        }
        last_state = battery_state;
    }

    if( persist_read_int(KEY_ANIMATION) > 0 ) {
        if( persist_read_int(KEY_ANIMATION) == 1 )
            unixtime /= 60;
        if( unixtime % 2 )
            loadForegroundImage(s_foregrounds_odd[battery_state]);
        else
            loadForegroundImage(s_foregrounds_even[battery_state]);
    } else
        loadForegroundImage(s_foregrounds[battery_state]);
}

void cleanAnimation()
{
    s_lightning_animation = -1;
    commonAnimationForeground();
}

void supersmallLightningAnimation(void *context)
{
    if( s_lightning_charge == 0 ) {
        if( s_lightning_animation < 5 ) {
            if( ! (rand() % 3) ) {
                loadForegroundImage(s_lighting_small[rand() % S_LIGHTNING_SMALL_NUM]);
            } else
                commonAnimationForeground();

            s_lightning_animation++;
            s_animation_timer = app_timer_register(100, supersmallLightningAnimation, NULL);
        }
        else
            cleanAnimation();
    }
}

void smallLightningAnimation(void *context)
{
    if( s_lightning_charge == 1 ) {
        if( s_lightning_animation < 30 ) {
            if( s_lightning_animation == 0 || (! (rand() % 3)) ) {
                loadForegroundImage(s_lighting_small[rand() % S_LIGHTNING_SMALL_NUM]);
                static const uint32_t const segments[] = { 50 };
                static VibePattern pat = {
                  .durations = segments,
                  .num_segments = ARRAY_LENGTH(segments),
                };
                vibes_enqueue_custom_pattern(pat);
            } else
                commonAnimationForeground();

            s_lightning_animation++;
            s_animation_timer = app_timer_register(100, smallLightningAnimation, NULL);
        }
        else
            cleanAnimation();
    }
}

void mediumLightningAnimation(void *context)
{
    if( s_lightning_charge == 2 ) {
        if( s_lightning_animation < 15 ) {
            if( s_lightning_animation == 0 || (! (rand() % 2)) ) {
                loadForegroundImage(s_lighting_medium[rand() % S_LIGHTNING_MEDIUM_NUM]);
                static const uint32_t const segments[] = { 100 };
                static VibePattern pat = {
                  .durations = segments,
                  .num_segments = ARRAY_LENGTH(segments),
                };
                vibes_enqueue_custom_pattern(pat);
            } else
                commonAnimationForeground();

            s_lightning_animation++;
            s_animation_timer = app_timer_register(200, mediumLightningAnimation, NULL);
        }
        else
            cleanAnimation();
    }
}

void bigLightningAnimation(void *context)
{
    if( s_lightning_charge == 3 ) {
        if( s_lightning_animation < 10 ) {
            loadForegroundImage(RESOURCE_ID_FOREGROUND_E_10);
            loadBackgroundImage(s_lighting_big[s_lightning_animation % S_LIGHTNING_BIG_NUM]);
            static uint32_t segments[] = { 20 };
            segments[0] = 20 * (s_lightning_animation+1);
            static VibePattern pat = {
              .durations = segments,
              .num_segments = ARRAY_LENGTH(segments),
            };
            vibes_enqueue_custom_pattern(pat);

            s_lightning_animation++;
            s_animation_timer = app_timer_register(200, bigLightningAnimation, NULL);
        }
        else {
            cleanAnimation();
            switchTimeOffset();
            setBigTimeHidden(false);
            s_lightning_charge = 0;
        }
    }
}

void startLightningAnimation(int lightning_charge)
{
    // Do nothing if time travel in progress
    if( s_lightning_charge == 3 )
        return;

    if( s_animation_timer ) {
        app_timer_cancel(s_animation_timer);
        cleanAnimation();
    }

    if( persist_read_int(KEY_TIMEMACHINE) < 2 && lightning_charge == 3 )
        lightning_charge = 2;

    s_lightning_charge = lightning_charge;
    s_lightning_animation = 0;

    switch( lightning_charge ) {
        case 1:
            s_animation_timer = app_timer_register(1, smallLightningAnimation, NULL);
            break;
        case 2:
            s_animation_timer = app_timer_register(1, mediumLightningAnimation, NULL);
            break;
        case 3:
            setBigTimeHidden(true);
            s_animation_timer = app_timer_register(1, bigLightningAnimation, NULL);
            break;
        default:
            s_animation_timer = app_timer_register(1, supersmallLightningAnimation, NULL);
            break;
    }
}

void tapHandler(AccelAxisType axis, int32_t direction)
{
    static time_t lightning_last_time = 0;

    int lightning_charge = s_lightning_charge;

    if( axis == ACCEL_AXIS_X ) {
        if( persist_read_int(KEY_ANIMATION) == 2 ) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Charging time machine");

            time_t ctime = time(NULL);

            if( lightning_charge > 3 )
                lightning_charge = 0;
            else if( lightning_last_time + 4 > ctime )
                lightning_charge++;
            else
                lightning_charge = 0;

            lightning_last_time = ctime;

            startLightningAnimation(lightning_charge);
            return;
        }
    }

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Lightning supersmall");
    if( persist_read_int(KEY_ANIMATION) == 2 )
        startLightningAnimation(0);
}

void tickHandler(struct tm *tick_time, TimeUnits units_changed)
{
    setTime(units_changed);

    if( s_lightning_animation == -1 )
        commonAnimationForeground();

    if( units_changed & DAY_UNIT )
        updatePdate();

    if( units_changed & MINUTE_UNIT ) {
        updatePtime();
        checkDtime();
    }
}
