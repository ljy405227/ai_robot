#ifndef MY_LOUDSPEAKER_HPP
#define MY_LOUDSPEAKER_HPP

#include <Arduino.h>
#include "Audio.h"

#define LOUDSPEAKE_LRC 42
#define LOUDSPEAKE_BLCK 1
#define LOUDSPEAKE_DIN 2

extern Audio audio;

void loudspeaker_init(void);
void playCurrentSong(void);
#endif