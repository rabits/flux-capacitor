#ifndef MAIN_H
#define MAIN_H

#include <pebble.h>

void setTime(TimeUnits units_changed);

void switchTimeOffset();

void setTimeOffset(time_t offset, bool update);

void setTimeSecondsHidden(bool hidden);

void setBigTimeHidden(bool hidden);

void loadForegroundImage(uint8_t resource);

void loadBackgroundImage(uint8_t resource);

void updateDdate();

void updatePdate();

void updatePtime();

void checkDtime();

// pivotal_gmtime lib
typedef long long time64_t;
time64_t mktime64(struct tm *t);
struct tm *localtime64_r(const time64_t *t, struct tm *p);

#endif // MAIN_H
