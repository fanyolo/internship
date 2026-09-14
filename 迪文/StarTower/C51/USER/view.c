#include "view.h"
static U16 digit(U16 n,U8 place,U8 count){U8 i;U16 div=1;if(place>=count)return 0;for(i=place+1;i<count;i++)div*=10;return (n/div)%10;}
static U8 login_digit(U32 n,U8 place,U8 count){U8 i;U32 div=1;if(place>=count)return 0;for(i=place+1;i<count;i++)div*=10;return (U8)((n/div)%10);}
U8 tower_page(void){return g.page==P_BOOT?(U8)(11+g.boot_tick/4):g.page;}
U16 tower_vp(U16 vp){
 U16 value=0;U8 index=(U8)(vp&15),count=4;
 if(vp>=0x2000&&vp<0x2079)return tower_render((U8)(vp-0x2000));
 if(vp==0x1300)return g.message;
 if(vp==0x1310)return g.book_index;
 if(vp==0x1320)return g.return_page==P_MENU?0:1;
 if(vp==0x1330)return g.audio_level;
 if(vp==0x1340)return g.hp<=120&&((g.ticks/6)&1)?1:0;
 if(vp>=0x1350&&vp<=0x1353)return (g.key_flash&&g.last_key==K_UP+vp-0x1350)?1:0;
 if(vp>=0x1360&&vp<=0x1365)return index<g.login_account_digits?(U16)(login_digit(g.login_account,index,g.login_account_digits)+1):0;
 if(vp>=0x1370&&vp<=0x1375)return index<g.login_password_digits?11:0;
 if(vp==0x1380)return g.login_message;
 if(vp==0x1170)return g.sword;
 if(vp==0x1171)return g.shield;
 switch(vp&0xFFF0){
  case 0x1100:value=g.hp;break;case 0x1110:value=g.atk;break;case 0x1120:value=g.def;break;case 0x1130:value=g.gold;break;
  case 0x1140:value=g.xp;count=3;break;
  case 0x1150:if(index>=6)return 0;return digit(g.keys[index/2],index%2,2);
  case 0x1160:value=g.potions;count=2;break;
  case 0x1190:value=g.floor+1;count=2;break;
  case 0x11A0:value=(g.seconds/60)*100+g.seconds%60;break;
  case 0x11B0:value=g.kills;count=2;break;case 0x11C0:value=g.steps;break;
  case 0x1200:value=monsters[g.book_index].hp;break;case 0x1210:value=monsters[g.book_index].atk;break;
  case 0x1220:value=monsters[g.book_index].def;break;case 0x1230:value=monsters[g.book_index].gold;break;
  case 0x1240:value=tower_loss(g.book_index);break;default:return 0;
 }
 return digit(value,index,count);
}
