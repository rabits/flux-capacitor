#include "main.h"
#include "config.h"

void setConfigDefaults()
{
    if( ! persist_exists(KEY_ANIMATION) )
        persist_write_int(KEY_ANIMATION, 0); // No animation
    if( ! persist_exists(KEY_DESTINATION) )
        persist_write_int(KEY_DESTINATION, 1445470140); // 21 Oct 2015 16:29:00 GMT-7
    if( ! persist_exists(KEY_TIMEMACHINE) )
        persist_write_int(KEY_TIMEMACHINE, 1); // Only to the future
}

void inboxReceivedHandler(DictionaryIterator *iter, void *context)
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
        int32_t destination = value_t->value->int32;
        persist_write_int(KEY_DESTINATION, destination);

        // Set destination date
        setTimeOffset(0, true);
    }

    // TimeMachine
    value_t = dict_find(iter, KEY_TIMEMACHINE);
    if( value_t ) {
        // Apply
        uint32_t timemachine = value_t->value->int32;
        persist_write_int(KEY_TIMEMACHINE, timemachine);
    }
}

void inboxDroppedHandler(AppMessageResult reason, void *context)
{
    // Incoming message dropped
    app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "AppMessage Dropped: %d", reason);
}
