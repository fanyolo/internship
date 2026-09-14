#include "audio.h"
U8 tower_music_track(void){
 if(g.page==P_BOOT)return 0;
 if(g.page==P_WIN)return 3;
 if(g.floor==FLOORS-1&&g.page!=P_MENU&&g.page!=P_HELP&&!(g.page==P_DIALOG&&g.return_page==P_MENU))return 2;
 return 1;
}
U8 tower_music_volume(void){
 U8 v=0;if(g.audio_level==1)v=32;
 if(g.audio_level==2)v=52;
 if(g.audio_level==3)v=64;
 if(g.page==P_PAUSE)v/=2;return v;
}
void audio_init(TowerAudio *s){s->track=255;s->volume=255;s->wait=0;s->play=0;s->change_volume=0;}
void audio_step(TowerAudio *s,U8 playing){
 U8 track=tower_music_track(),volume=tower_music_volume();
 s->play=0;s->change_volume=(volume!=s->volume);
 if(!volume){s->volume=0;s->track=255;s->wait=0;return;}
 if(track!=s->track){s->track=track;s->play=1;s->wait=6;}
 else if(s->wait)s->wait--;
 else if((track==1||track==2)&&!playing){s->play=1;s->wait=6;}
 s->volume=volume;
}
