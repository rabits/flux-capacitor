#ifndef CONFIG_H
#define CONFIG_H

#define KEY_ANIMATION 0
#define KEY_DESTINATION 1
#define KEY_DESTINATION_USE64 11
#define KEY_DESTINATION_H 12
#define KEY_TIMEMACHINE 2
#define KEY_TIMEMACHINE_LOCK 21

void setConfigDefaults();

void inboxReceivedHandler(DictionaryIterator *iter, void *context);

void inboxDroppedHandler(AppMessageResult reason, void *context);

#endif // CONFIG_H
