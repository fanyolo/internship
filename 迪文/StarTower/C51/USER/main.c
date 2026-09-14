#include "sys.h"
#include "view.h"
#include "audio.h"
static TowerAudio sound;
static u8 xdata save_a[SAVE_BYTES],save_b[SAVE_BYTES];
static u16 xdata old_vp[208];
static u8 active_slot=0,have_slot=0;
static u16 code address_groups[]={0x1100,0x1110,0x1120,0x1130,0x1140,0x1150,0x1160,0x1170,0x1190,0x11A0,0x11B0,0x11C0,0x1200,0x1210,0x1220,0x1230,0x1240,0x1300,0x1310,0x1320,0x1330,0x1340,0x1350,0x1360,0x1370,0x1380};
static u8 code group_count[]={4,4,4,4,3,6,2,2,2,4,2,4,4,4,4,4,4,1,1,1,1,1,4,6,6,1};
static void word(u16 vp,u16 v){u8 b[2];b[0]=(u8)(v>>8);b[1]=(u8)v;sys_write_vp(vp,b,1);}
static u16 readword(u16 vp){u8 b[2];sys_read_vp(vp,b,1);return ((u16)b[0]<<8)|b[1];}
static void jump(u8 p){u8 b[4];b[0]=0x5A;b[1]=1;b[2]=0;b[3]=p;sys_write_vp(0x0084,b,2);}
/* Official 0x0008 database interface, 160 words, separate 8 KB regions.
   Slots: NOR word 0x020000 and 0x021000. Transfer VP 0x3000..0x309F. */
static u8 flash(u8 write,u8 slot,u8 *buffer){
 u8 b[8];u16 wait;
 b[0]=write?0xA5:0x5A;b[1]=2;b[2]=slot?0x10:0;b[3]=0;
 b[4]=0x30;b[5]=0;b[6]=0;b[7]=SAVE_WORDS;
 if(write)sys_write_vp(0x3000,buffer,SAVE_WORDS);
 sys_write_vp(0x0008,b,4);
 for(wait=0;wait<300;wait++){
  sys_delay_ms(5);
  if((readword(0x0008)>>8)==0){if(!write)sys_read_vp(0x3000,buffer,SAVE_WORDS);return 1;}
 }
 return 0;
}
static u16 sequence(u8 *b){return ((u16)b[4]<<8)|b[5];}
static u8 scan_save(u8 apply){
 u8 a,b,slot;u16 delta;
 a=flash(0,0,save_a)&&tower_valid(save_a);b=flash(0,1,save_b)&&tower_valid(save_b);
 if(!a&&!b){g.save_valid=0;return 0;}
 slot=0;if(b&&!a)slot=1;
 if(a&&b){delta=sequence(save_b)-sequence(save_a);if(delta&&delta<0x8000)slot=1;}
 active_slot=slot;have_slot=1;g.sequence=sequence(slot?save_b:save_a);g.save_valid=1;
 if(!apply)g.audio_level=tower_saved_audio(slot?save_b:save_a);
 return apply?tower_load(slot?save_b:save_a):1;
}
static void persistence(void){
 u8 slot,ok;u16 seq,i;
 if(g.load_request){g.load_request=0;if(!scan_save(1)){g.message=M_NO_SAVE;g.message_ticks=48;}}
 if(!g.save_request)return;g.save_request=0;
 slot=have_slot?(1-active_slot):0;seq=g.sequence+1;tower_save(save_a,seq);
 ok=flash(1,slot,save_a)&&flash(0,slot,save_b)&&tower_valid(save_b);
 if(ok){for(i=0;i<SAVE_BYTES;i++)if(save_a[i]!=save_b[i]){ok=0;break;}}
 if(ok){active_slot=slot;have_slot=1;}tower_saved(ok,seq);
}
static void render(u8 force){
 u16 vp,v;u8 i,j,cache=0;
 if(g.page==P_MAP)for(i=0;i<121;i++){vp=0x2000+i;v=tower_vp(vp);if(force||old_vp[i]!=v){word(vp,v);old_vp[i]=v;}}
 cache=121;
 for(i=0;i<sizeof(group_count);i++)for(j=0;j<group_count[i];j++){
  vp=address_groups[i]+j;v=tower_vp(vp);if(force||old_vp[cache]!=v){word(vp,v);old_vp[cache]=v;}cache++;
 }
}
static void music(void){
 u8 b[4];audio_step(&sound,(readword(0x00A1)&255)==2);
 if(sound.play){b[0]=sound.track;b[1]=1;b[2]=sound.volume;b[3]=0;sys_write_vp(0x00A0,b,2);}
 else if(sound.change_volume)word(0x00A1,(u16)sound.volume<<8);
}
void main(void){
 u8 page,previous=255,force=1;u16 key;
 sys_init();sys_delay_ms(300);tower_init();word(0x1000,0);scan_save(0);audio_init(&sound);music();
 while(1){
  page=tower_page();
  if(page!=previous){jump(page);previous=page;force=1;if(g.page!=P_BOOT)sys_delay_ms(60);}
  if(g.page!=P_BOOT)render(force);force=0;
  sys_delay_ms(50);key=readword(0x1000);if(key)word(0x1000,0);
  tower_tick(key);persistence();music();
 }
}
