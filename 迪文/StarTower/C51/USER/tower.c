#include "tower.h"
#define LOGIN_CREDENTIAL 123456UL
Tower EXT g;
const Monster ROM monsters[6]={
 {24,13,2,8,2,0},{36,19,5,12,3,0},{65,27,9,18,5,0},
 {78,30,13,24,6,1},{125,42,19,38,9,0},{1600,180,100,300,80,0}
};
static void msg(U8 m){g.message=m;g.message_ticks=48;}
static void loading(U8 target){g.page=P_BOOT;g.boot_target=target;g.boot_tick=0;g.key_flash=0;}
static U16 capadd(U16 a,U16 b){U32 v=(U32)a+b;return v>9999?9999:(U16)v;}
static U8 cleared(U8 f,U8 p){return g.cleared[f*16+(p>>3)]&(1<<(p&7));}
static void take(U8 p){g.cleared[g.floor*16+(p>>3)]|=(1<<(p&7));}
U8 tower_tile(U8 f,U8 p){
 U8 map,t;if(f>=FLOORS||p>=CELLS)return T_WALL;
 map=f%MAPS;t=levels[map][p];if(cleared(f,p))return T_FLOOR;
 if(p==routes[map][0])return f?T_DOWN:T_FLOOR;
 if(p==routes[map][48])return f==FLOORS-1?T_EXIT:T_UP;
 if(f==FLOORS-1&&p==routes[map][47])return T_BOSS;
 if(t==T_GATE&&(g.switches&((U16)1<<f)))return T_FLOOR;
 if(t>=T_SLIME&&t<=T_GUARD){t+=(U8)(f/3);if(t>T_GUARD)t=T_GUARD;}
 return t;
}
void tower_init(void){
 U16 i;U8 *p=(U8*)&g;for(i=0;i<sizeof(Tower);i++)p[i]=0;
 g.page=P_BOOT;g.return_page=P_MAP;g.pos=12;g.hp=400;g.atk=18;g.def=8;
 g.audio_level=2;g.boot_target=P_LOGIN;
}
void tower_new(void){
 U8 valid=g.save_valid,audio=g.audio_level;U16 seq=g.sequence;tower_init();g.save_valid=valid;g.sequence=seq;g.audio_level=audio;
 g.page=P_MAP;g.pos=routes[0][0];g.visited=1;g.potions=1;g.save_request=1;msg(M_READY);
}
U16 tower_loss(U8 id){
 U16 hit,ehit,turns,def;U32 loss;
 if(id>=6||g.atk<=monsters[id].def)return 9999;
 hit=g.atk-monsters[id].def;turns=(monsters[id].hp+hit-1)/hit;
 def=monsters[id].kind?g.def/2:g.def;
 ehit=monsters[id].atk>def?monsters[id].atk-def:0;
 loss=(U32)(turns-1)*ehit;return loss>9999?9999:(U16)loss;
}
static void travel(U8 floor,U8 pos){g.floor=floor;g.pos=pos;g.visited|=(U16)1<<floor;g.save_request=1;msg(M_STAIRS);loading(P_MAP);}
static void move(U8 dir){
 U8 pos,t,id;U16 loss;
 g.dir=dir;pos=g.pos;
 if(dir==0){if(pos<11)return;pos-=11;}
 if(dir==1){if(pos>109)return;pos+=11;}
 if(dir==2){if(pos%11==0)return;pos--;}
 if(dir==3){if(pos%11==10)return;pos++;}
 t=tower_tile(g.floor,pos);
 if(t==T_WALL)return;
 if(t>=T_YDOOR&&t<=T_RDOOR){id=t-T_YDOOR;if(!g.keys[id]){msg(M_NO_KEY);return;}g.keys[id]--;take(pos);msg(M_DOOR);}
 else if(t>=T_SLIME&&t<=T_BOSS){
  id=t-T_SLIME;g.book_index=id;loss=tower_loss(id);
  if(loss==9999){msg(M_TOO_STRONG);return;}if(loss>=g.hp){msg(M_LOW_HP);return;}
  g.battle_ticks=16;g.battle_pos=pos;g.battle_type=id;g.battle_loss=loss;msg(M_BATTLE);return;
 }
 else if(t>=T_YKEY&&t<=T_RKEY){id=t-T_YKEY;if(g.keys[id]<99)g.keys[id]++;take(pos);msg(M_PICK);}
 else if(t==T_RED||t==T_BLUE){g.hp=capadd(g.hp,t==T_RED?50:180);take(pos);msg(M_PICK);}
 else if(t==T_ATK){g.atk=capadd(g.atk,3);take(pos);msg(M_PICK);}
 else if(t==T_DEF){g.def=capadd(g.def,2);take(pos);msg(M_PICK);}
 else if(t==T_SWORD){g.atk=capadd(g.atk,8);g.sword=1;take(pos);msg(M_PICK);}
 else if(t==T_SHIELD){g.def=capadd(g.def,8);g.shield=1;take(pos);msg(M_PICK);}
 else if(t==T_PACK){if(g.potions<99)g.potions++;take(pos);msg(M_PACK);}
 else if(t==T_CHEST){g.gold=capadd(g.gold,40);take(pos);msg(M_GOLD);}
 else if(t==T_SWITCH){g.switches|=(U16)1<<g.floor;take(pos);msg(M_SWITCH);}
 else if(t==T_GATE){msg(M_SEALED);return;}
 else if(t==T_SHOP){g.page=P_SHOP;return;}
 else if(t==T_SAGE){g.page=P_DIALOG;g.return_page=P_MAP;msg(M_SAGE);if(!g.quest){g.quest=1;g.potions++;g.save_request=1;}return;}
 else if(t==T_UP){if(g.floor<FLOORS-1){g.steps++;travel(g.floor+1,routes[(g.floor+1)%MAPS][1]);}return;}
 else if(t==T_DOWN){if(g.floor){g.steps++;travel(g.floor-1,routes[(g.floor-1)%MAPS][47]);}return;}
 else if(t==T_EXIT){g.pos=pos;g.steps++;g.page=P_WIN;msg(M_COMPLETE);g.save_request=1;return;}
 g.pos=pos;g.steps++;
}
static void purchase(U16 key){
 U16 price=0;if(key==K_BUY_ATK)price=35;if(key==K_BUY_DEF)price=35;if(key==K_BUY_HP)price=20;
 if(!price)return;if(g.gold<price){msg(M_NO_GOLD);return;}
 g.gold-=price;if(key==K_BUY_ATK)g.atk=capadd(g.atk,3);if(key==K_BUY_DEF)g.def=capadd(g.def,3);if(key==K_BUY_HP)g.hp=capadd(g.hp,180);
 msg(M_PICK);g.save_request=1;
}
static void login_input(U16 key){
 U8 digit;
 if(key>=K_LOGIN_0&&key<=K_LOGIN_9){
  digit=(U8)(key-K_LOGIN_0);
  if(!g.login_stage&&g.login_account_digits<6){g.login_account=g.login_account*10+digit;g.login_account_digits++;}
  if(g.login_stage&&g.login_password_digits<6){g.login_password=g.login_password*10+digit;g.login_password_digits++;}
  g.login_message=g.login_stage?L_PASSWORD:L_ACCOUNT;return;
 }
 if(key==K_LOGIN_CLEAR){
  if(g.login_stage){g.login_password=0;g.login_password_digits=0;}else{g.login_account=0;g.login_account_digits=0;}
  g.login_message=g.login_stage?L_PASSWORD:L_ACCOUNT;return;
 }
 if(key==K_LOGIN_DELETE){
  if(g.login_stage){
   if(g.login_password_digits){g.login_password/=10;g.login_password_digits--;}
   else g.login_stage=0;
  }else if(g.login_account_digits){g.login_account/=10;g.login_account_digits--;}
  g.login_message=g.login_stage?L_PASSWORD:L_ACCOUNT;return;
 }
 if(key!=K_LOGIN_SUBMIT)return;
 if(!g.login_stage){
  if(g.login_account_digits!=6){g.login_message=L_INCOMPLETE;return;}
  g.login_stage=1;g.login_message=L_PASSWORD;return;
 }
 if(g.login_password_digits!=6){g.login_message=L_INCOMPLETE;return;}
 if(g.login_account==LOGIN_CREDENTIAL&&g.login_password==LOGIN_CREDENTIAL){
  g.login_account=g.login_password=0;g.login_account_digits=g.login_password_digits=0;g.page=P_MENU;return;
 }
 g.login_account=g.login_password=0;g.login_account_digits=g.login_password_digits=g.login_stage=0;g.login_message=L_INVALID;
}
void tower_tick(U16 key){
 g.ticks++;
 if(g.page==P_BOOT){g.boot_tick++;if(g.boot_tick>=60)g.page=g.boot_target;return;}
 if(g.key_flash)g.key_flash--;
 if(g.home_request)return;
 if(g.page==P_LOGIN){login_input(key);return;}
 if(key==K_AUDIO&&(g.page==P_MENU||g.page==P_PAUSE)){g.audio_level=(g.audio_level+1)%4;if(g.page==P_PAUSE)g.save_request=1;return;}
 if(g.page>=P_MAP&&g.page<=P_BAG){g.subtick++;if(g.subtick>=20){g.subtick=0;if(g.seconds<5999)g.seconds++;}}
 if(g.message_ticks){g.message_ticks--;if(!g.message_ticks)g.message=M_READY;}
 if(g.page==P_MENU){
  if(key==K_NEW){g.page=P_DIALOG;g.return_page=P_MENU;msg(M_NEW_CONFIRM);}
  if(key==K_LOAD)g.load_request=1;
  if(key==K_HELP){g.return_page=P_MENU;g.page=P_HELP;}return;
 }
 if(g.page==P_DIALOG){
  if(g.return_page==P_MENU&&key==K_CONFIRM_NEW){tower_new();return;}
  if(g.return_page!=P_MENU&&key==K_CONFIRM_NEW){g.page=P_MAP;return;}
  if(key==K_BACK)g.page=g.return_page;return;
 }
 if(key==K_HOME&&(g.page==P_PAUSE||g.page==P_WIN)){g.home_request=1;g.save_request=1;return;}
 if(key==K_BACK){if(g.page==P_HELP)g.page=g.return_page;else if(g.page!=P_WIN)g.page=P_MAP;return;}
 if(g.page==P_PAUSE){if(key==K_RESUME)g.page=P_MAP;if(key==K_SAVE)g.save_request=1;return;}
 if(g.page==P_BOOK){if(key==K_BOOK_NEXT)g.book_index=(g.book_index+1)%6;if(key==K_BOOK_PREV)g.book_index=(g.book_index+5)%6;return;}
 if(g.page==P_SHOP){purchase(key);return;}
 if(g.page==P_BAG){
  if(key==K_USE_POTION){if(g.potions){g.potions--;g.hp=capadd(g.hp,180);msg(M_PICK);g.save_request=1;}else msg(M_NO_PACK);}
  if(key==K_FLOOR_UP||key==K_FLOOR_DOWN){
   if(key==K_FLOOR_UP&&g.floor<FLOORS-1&&(g.visited&((U16)1<<(g.floor+1))))travel(g.floor+1,routes[(g.floor+1)%MAPS][1]);
   else if(key==K_FLOOR_DOWN&&g.floor)travel(g.floor-1,routes[(g.floor-1)%MAPS][47]);else msg(M_FLOOR_LOCK);
  }return;
 }
 if(g.page!=P_MAP)return;
 if(g.battle_ticks){
  g.battle_ticks--;
  if(!g.battle_ticks){g.hp-=g.battle_loss;g.gold=capadd(g.gold,monsters[g.battle_type].gold);g.xp=capadd(g.xp,monsters[g.battle_type].xp);g.kills++;take(g.battle_pos);g.pos=g.battle_pos;g.steps++;msg(M_WIN);}return;
 }
 if(key>=K_UP&&key<=K_RIGHT){g.last_key=(U8)key;g.key_flash=4;}
 if(key==K_UP)move(0);if(key==K_DOWN)move(1);if(key==K_LEFT)move(2);if(key==K_RIGHT)move(3);
 if(key==K_BOOK)g.page=P_BOOK;if(key==K_BAG)g.page=P_BAG;if(key==K_PAUSE){g.page=P_PAUSE;msg(M_PAUSED);}
 if(key==K_SAVE)g.save_request=1;
}
U8 tower_render(U8 pos){
 U8 t;if(pos==g.pos)return (U8)(30+g.dir);
 if(g.battle_ticks&&pos==g.battle_pos&&((g.battle_ticks/2)&1))return 34;
 t=tower_tile(g.floor,pos);return t;
}
U16 tower_crc(U8 *b,U16 n){U16 c=0xFFFF;U8 j;while(n--){c^=*b++;for(j=0;j<8;j++)c=(c&1)?(c>>1)^0xA001:c>>1;}return c;}
static void put(U8 *b,U16 offset,U16 n){b[offset]=(U8)(n>>8);b[offset+1]=(U8)n;}
static U16 get(U8 *b,U16 offset){return ((U16)b[offset]<<8)|b[offset+1];}
void tower_save(U8 *b,U16 sequence){
 U16 i;for(i=0;i<SAVE_BYTES;i++)b[i]=0;
 put(b,0,0x5354);put(b,2,2);put(b,4,sequence);
 b[6]=g.floor;b[7]=g.pos;b[8]=g.dir;
 put(b,10,g.hp);put(b,12,g.atk);put(b,14,g.def);put(b,16,g.gold);put(b,18,g.xp);
 b[20]=g.keys[0];b[21]=g.keys[1];b[22]=g.keys[2];
 b[24]=g.sword;b[25]=g.shield;b[26]=g.potions;b[27]=g.quest;
 put(b,28,g.kills);put(b,30,g.steps);put(b,32,g.seconds);b[34]=(g.page==P_WIN);
 b[35]=0xA7;b[36]=g.audio_level;put(b,38,g.visited);put(b,40,g.switches);
 for(i=0;i<CLEARED_BYTES;i++)b[42+i]=g.cleared[i];put(b,SAVE_BYTES-2,tower_crc(b,SAVE_BYTES-2));
}
U8 tower_valid(U8 *b){
 U16 visited=get(b,38),switches=get(b,40);
 if(get(b,0)!=0x5354||get(b,2)!=2||get(b,SAVE_BYTES-2)!=tower_crc(b,SAVE_BYTES-2))return 0;
 if(b[6]>=FLOORS||b[7]>=CELLS||b[8]>3||!(visited&((U16)1<<b[6]))||visited>0x7FFF||switches>0x7FFF)return 0;
 if(!get(b,10)||get(b,10)>9999||!get(b,12)||get(b,12)>9999||get(b,14)>9999||get(b,16)>9999||get(b,18)>9999)return 0;
 if(b[20]>99||b[21]>99||b[22]>99||b[24]>1||b[25]>1||b[26]>99||b[27]>1||b[34]>1)return 0;
 if(get(b,32)>5999)return 0;
 if(b[35]!=0xA7||b[36]>3)return 0;
 if(levels[b[6]%MAPS][b[7]]==T_WALL)return 0;
 return 1;
}
U8 tower_load(U8 *b){
 U8 i;if(!tower_valid(b))return 0;tower_init();g.page=b[34]?P_WIN:P_MAP;
 g.sequence=get(b,4);g.floor=b[6];g.pos=b[7];g.dir=b[8];g.visited=get(b,38);g.hp=get(b,10);g.atk=get(b,12);g.def=get(b,14);g.gold=get(b,16);g.xp=get(b,18);
 for(i=0;i<3;i++)g.keys[i]=b[20+i];g.switches=get(b,40);g.sword=b[24];g.shield=b[25];g.potions=b[26];g.quest=b[27];g.kills=get(b,28);g.steps=get(b,30);g.seconds=get(b,32);
 for(i=0;i<CLEARED_BYTES;i++)g.cleared[i]=b[42+i];g.audio_level=tower_saved_audio(b);g.save_valid=1;msg(M_LOADED);return 1;
}
U8 tower_saved_audio(U8 *b){return b[35]==0xA7&&b[36]<=3?b[36]:2;}
void tower_saved(U8 ok,U16 sequence){
 g.save_request=0;if(ok){g.sequence=sequence;g.save_valid=1;msg(M_SAVED);}else msg(M_SAVE_FAIL);
 if(g.home_request){g.home_request=0;if(ok)loading(P_MENU);}
}
