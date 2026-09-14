#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#undef assert
#define assert(x) do { if(!(x)) {fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);} } while(0)
#include "../C51/USER/tower.h"
#include "../C51/USER/view.h"
#include "../C51/USER/audio.h"
static U8 savebuf[SAVE_BYTES],bad[SAVE_BYTES];
static void finish_battle(void){int n=0;while(g.battle_ticks&&n++<100)tower_tick(0);assert(n<100);}
static void finish_loading(void){int n=0;while(g.page==P_BOOT&&n++<61)tower_tick(0);assert(n<=60);}
static void enter_digits(const char *s){while(*s)tower_tick((U16)(K_LOGIN_0+(*s++-'0')));}
static void login_ok(void){enter_digits("123456");tower_tick(K_LOGIN_SUBMIT);enter_digits("123456");tower_tick(K_LOGIN_SUBMIT);assert(g.page==P_MENU);}
static void step_to(U8 p){
 int diff=(int)p-g.pos;U16 key=0;
 if(diff==1)key=K_RIGHT;if(diff==-1)key=K_LEFT;if(diff==11)key=K_DOWN;if(diff==-11)key=K_UP;
 assert(key);tower_tick(key);finish_battle();finish_loading();
}
int main(void){
 int i,f;U16 hp,gold;Tower original;TowerAudio sound;
 tower_init();for(i=0;i<60;i++)tower_tick(0);assert(g.page==P_LOGIN);
 enter_digits("12345");tower_tick(K_LOGIN_SUBMIT);assert(g.login_message==L_INCOMPLETE&&g.login_stage==0);
 enter_digits("6");tower_tick(K_LOGIN_SUBMIT);assert(g.login_stage==1&&tower_vp(0x1360)==2&&tower_vp(0x1365)==7);
 enter_digits("123455");assert(tower_vp(0x1370)==11&&tower_vp(0x1375)==11);tower_tick(K_LOGIN_SUBMIT);assert(g.page==P_LOGIN&&g.login_message==L_INVALID&&!g.login_account_digits&&!g.login_password_digits);
 login_ok();
 tower_tick(K_NEW);assert(g.page==P_DIALOG);tower_tick(K_CONFIRM_NEW);assert(g.page==P_MAP&&g.hp==400);
 tower_tick(K_LEFT);assert(g.pos==12); /* wall */
 g.atk=2;assert(tower_loss(0)==9999);g.atk=18;assert(tower_loss(0)==5);
 g.pos=routes[0][8];g.keys[0]=0;tower_tick(K_DOWN);assert(g.keys[0]==0); /* door cannot underflow */
 tower_new();
 for(f=0;f<FLOORS;f++){
  for(i=f?2:1;i<49;i++){
   U8 before=g.floor;U8 target=routes[f%MAPS][i];step_to(target);
   if(i<48&&g.pos!=target){printf("BLOCKED floor %d map %d index %d tile %u message %u hp %u atk %u def %u\n",f+1,f%MAPS+1,i,tower_tile(g.floor,target),g.message,g.hp,g.atk,g.def);return 1;}
   if(i==48&&f<FLOORS-1)assert(g.floor==before+1);
  }
  printf("Floor %d cleared: HP=%u ATK=%u DEF=%u gold=%u kills=%u\n",f+1,g.hp,g.atk,g.def,g.gold,g.kills);
 }
 assert(g.page==P_WIN&&g.floor==FLOORS-1&&g.kills==61&&g.sword&&g.shield&&g.visited==0x7FFF);
 assert(g.hp>0);original=g;tower_save(savebuf,17);assert(tower_valid(savebuf));tower_init();assert(tower_load(savebuf));assert(g.hp==original.hp&&g.floor==FLOORS-1&&g.page==P_WIN&&g.sequence==17);
 assert(memcmp(g.cleared,original.cleared,CLEARED_BYTES)==0);
 for(i=0;i<SAVE_BYTES;i++){memcpy(bad,savebuf,SAVE_BYTES);bad[i]^=1;assert(!tower_valid(bad));} /* every byte protected */
 tower_new();g.page=P_SHOP;g.gold=34;hp=g.atk;tower_tick(K_BUY_ATK);assert(g.atk==hp&&g.gold==34);g.gold=35;tower_tick(K_BUY_ATK);assert(g.atk==hp+3&&g.gold==0);
 g.page=P_BAG;g.potions=0;hp=g.hp;tower_tick(K_USE_POTION);assert(g.hp==hp);g.potions=1;tower_tick(K_USE_POTION);assert(g.hp==hp+180&&g.potions==0);
 g.page=P_PAUSE;hp=g.seconds;for(i=0;i<100;i++)tower_tick(0);assert(g.seconds==hp);
 tower_new();gold=g.gold;g.pos=routes[0][5];tower_tick(K_DOWN);assert(g.gold==gold+40);tower_tick(K_UP);tower_tick(K_DOWN);assert(g.gold==gold+40); /* no respawn */
 tower_new();g.pos=routes[0][8];step_to(routes[0][9]);assert(g.pos==routes[0][8]&&g.message==M_NO_KEY);g.keys[0]=1;step_to(routes[0][9]);assert(g.pos==routes[0][9]&&g.keys[0]==0);
 tower_new();g.floor=2;g.visited=7;g.pos=routes[2][23];step_to(routes[2][24]);assert(g.pos==routes[2][23]&&g.message==M_SEALED);g.switches=4;step_to(routes[2][24]);assert(g.pos==routes[2][24]);
 tower_new();g.page=P_BAG;tower_tick(K_FLOOR_UP);assert(g.floor==0&&g.message==M_FLOOR_LOCK);g.visited=3;tower_tick(K_FLOOR_UP);assert(g.floor==1&&g.pos==routes[1][1]&&g.page==P_BOOT);hp=g.seconds;for(i=0;i<59;i++)tower_tick(K_RIGHT);assert(g.page==P_BOOT&&g.pos==routes[1][1]&&g.seconds==hp);tower_tick(0);assert(g.page==P_MAP);tower_tick(K_BAG);tower_tick(K_FLOOR_DOWN);assert(g.floor==0&&g.pos==routes[0][47]);finish_loading();assert(g.page==P_MAP);
 tower_new();g.pos=routes[0][13];tower_tick(K_DOWN);assert(g.page==P_DIALOG&&g.potions==2);tower_tick(K_CONFIRM_NEW);tower_tick(K_DOWN);assert(g.potions==2);
 tower_new();g.page=P_SHOP;g.gold=100;g.atk=9998;tower_tick(K_BUY_ATK);assert(g.atk==9999&&g.gold==65);
 tower_new();g.hp=1;g.pos=routes[0][6];tower_tick(K_RIGHT);assert(!g.battle_ticks&&g.message==M_LOW_HP);
 tower_save(savebuf,1);memcpy(bad,savebuf,SAVE_BYTES);bad[6]=FLOORS;hp=tower_crc(bad,SAVE_BYTES-2);bad[SAVE_BYTES-2]=(U8)(hp>>8);bad[SAVE_BYTES-1]=(U8)hp;assert(!tower_valid(bad));
 tower_new();g.hp=1987;assert(tower_vp(0x1100)==1&&tower_vp(0x1103)==7);g.seconds=125;assert(tower_vp(0x11A1)==2&&tower_vp(0x11A3)==5);g.floor=FLOORS-1;assert(tower_vp(0x1190)==1&&tower_vp(0x1191)==5);g.floor=0;assert(tower_vp(0x2000+g.pos)==30);
 tower_init();for(i=0;i<60;i++){assert(tower_page()==11+i/4);tower_tick(0);}assert(tower_page()==10);login_ok();
 tower_new();tower_tick(K_RIGHT);tower_tick(K_PAUSE);tower_tick(K_HOME);assert(g.home_request&&g.page==P_PAUSE&&g.save_request);tower_saved(0,2);assert(g.page==P_PAUSE&&!g.home_request&&g.message==M_SAVE_FAIL);tower_tick(K_HOME);tower_save(savebuf,2);tower_saved(1,2);assert(g.page==P_BOOT);finish_loading();assert(g.page==P_MENU);assert(tower_load(savebuf)&&g.pos==13);
 tower_tick(K_PAUSE);tower_tick(K_AUDIO);assert(g.audio_level==3&&g.save_request);tower_save(savebuf,3);tower_init();assert(tower_load(savebuf)&&g.audio_level==3);savebuf[35]=0;hp=tower_crc(savebuf,SAVE_BYTES-2);savebuf[SAVE_BYTES-2]=(U8)(hp>>8);savebuf[SAVE_BYTES-1]=(U8)hp;assert(!tower_load(savebuf)); /* incompatible save marker is rejected */
 tower_tick(K_RIGHT);assert(tower_vp(0x1353)==1);for(i=0;i<4;i++)tower_tick(0);assert(tower_vp(0x1353)==0);g.hp=100;g.ticks=6;assert(tower_vp(0x1340)==1);g.hp=121;assert(tower_vp(0x1340)==0);
 tower_init();audio_init(&sound);audio_step(&sound,0);assert(sound.play&&sound.track==0&&sound.volume==52);for(i=0;i<10;i++){audio_step(&sound,0);assert(!sound.play);}finish_loading();audio_step(&sound,0);assert(sound.play&&sound.track==1);for(i=0;i<6;i++){audio_step(&sound,0);assert(!sound.play);}audio_step(&sound,1);assert(!sound.play);audio_step(&sound,0);assert(sound.play);g.page=P_PAUSE;audio_step(&sound,1);assert(sound.volume==26&&sound.change_volume&&!sound.play);g.audio_level=0;audio_step(&sound,1);assert(sound.volume==0&&!sound.play);g.audio_level=2;audio_step(&sound,0);assert(sound.play);g.floor=FLOORS-1;g.page=P_MAP;audio_step(&sound,1);assert(sound.play&&sound.track==2);g.page=P_WIN;audio_step(&sound,1);assert(sound.play&&sound.track==3);
 puts("PASS: boot/login validation/new confirmation, 15-floor three-map-cycle victory, damage, saves/320 corruption cases, shop, potions, pause, no duplicate loot");
 puts("PASS: locked doors/gates, teleport restrictions, sage once, stat cap, lethal refusal, semantic save validation, VP digit mapping, 15 boot frames");
 puts("PASS: floor loading/input lock, save-before-home success/failure, audio preference/v2 save rejection, low-HP/direction feedback, music switching/loop/mute/pause");
 return 0;
}
