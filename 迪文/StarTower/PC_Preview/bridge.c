#include "../C51/USER/view.h"
#include "../C51/USER/audio.h"
static TowerAudio sound;
#define API __declspec(dllexport)
API void st_init(void){tower_init();audio_init(&sound);}
API void st_tick(unsigned short key){tower_tick(key);}
API unsigned short st_vp(unsigned short vp){return tower_vp(vp);}
API unsigned short st_page(void){return tower_page();}
API unsigned short st_save_bytes(void){return SAVE_BYTES;}
API unsigned short st_requests(void){return g.save_request|(g.load_request<<1);}
API void st_saved(int ok,unsigned short seq){tower_saved((U8)ok,seq);}
API void st_preferences(unsigned char *b){if(b&&tower_valid(b)){g.audio_level=tower_saved_audio(b);g.sequence=((U16)b[4]<<8)|b[5];g.save_valid=1;}}
API unsigned short st_audio(int playing){audio_step(&sound,(U8)playing);return sound.volume|((U16)sound.track<<8)|(sound.play?0x8000:0)|(sound.change_volume?0x4000:0);}
API void st_save(unsigned char *b,unsigned short seq){tower_save(b,seq);}
API int st_valid(unsigned char *b){return tower_valid(b);}
API int st_load(unsigned char *b){g.load_request=0;if(b&&tower_load(b))return 1;g.message=M_NO_SAVE;g.message_ticks=48;return 0;}
