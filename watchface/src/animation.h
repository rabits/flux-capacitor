#ifndef ANIMATION_H
#define ANIMATION_H

void commonAnimationForeground();

void lightningAnimation(void *context);

void startLightningAnimation(int lightning_charge);

void tapHandler(AccelAxisType axis, int32_t direction);

void tickHandler(struct tm *tick_time, TimeUnits units_changed);

#endif // ANIMATION_H
