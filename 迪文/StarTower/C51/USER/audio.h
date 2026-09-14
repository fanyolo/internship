#ifndef AUDIO_H
#define AUDIO_H
#include "tower.h"
typedef struct {U8 track,volume,wait,play,change_volume;} TowerAudio;
void audio_init(TowerAudio *state);
void audio_step(TowerAudio *state,U8 playing);
U8 tower_music_track(void);
U8 tower_music_volume(void);
#endif
