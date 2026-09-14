#ifndef TOWER_H
#define TOWER_H
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned long U32;
#ifdef __C51__
#define ROM code
#define EXT xdata
#else
#define ROM
#define EXT
#endif
#define FLOORS 15
#define MAPS 3
#define CELLS 121
#define SAVE_BYTES 320
#define SAVE_WORDS (SAVE_BYTES/2)
#define CLEARED_BYTES (FLOORS*16)
enum {P_BOOT=0,P_MENU,P_MAP,P_BOOK,P_SHOP,P_BAG,P_HELP,P_WIN,P_PAUSE,P_DIALOG,P_LOGIN};
enum {T_FLOOR=0,T_WALL,T_YDOOR,T_BDOOR,T_RDOOR,T_UP,T_DOWN,T_YKEY,T_BKEY,T_RKEY,T_RED,T_BLUE,T_ATK,T_DEF,T_SWORD,T_SHIELD,T_CHEST,T_SWITCH,T_SLIME,T_BAT,T_SKELETON,T_MAGE,T_GUARD,T_BOSS,T_SHOP,T_SAGE,T_GATE,T_EXIT,T_PACK};
enum {K_NONE=0,K_NEW=1,K_LOAD=2,K_HELP=3,K_HOME=4,K_BACK=5,K_UP=10,K_DOWN=11,K_LEFT=12,K_RIGHT=13,K_BOOK=20,K_BAG=21,K_PAUSE=22,K_SAVE=23,K_RESUME=24,K_CONFIRM_NEW=25,K_BUY_ATK=30,K_BUY_DEF=31,K_BUY_HP=32,K_USE_POTION=33,K_BOOK_PREV=34,K_BOOK_NEXT=35,K_FLOOR_UP=36,K_FLOOR_DOWN=37,K_AUDIO=38,K_LOGIN_0=40,K_LOGIN_1,K_LOGIN_2,K_LOGIN_3,K_LOGIN_4,K_LOGIN_5,K_LOGIN_6,K_LOGIN_7,K_LOGIN_8,K_LOGIN_9,K_LOGIN_CLEAR=50,K_LOGIN_DELETE=51,K_LOGIN_SUBMIT=52};
enum {M_READY=0,M_PICK,M_DOOR,M_NO_KEY,M_TOO_STRONG,M_LOW_HP,M_BATTLE,M_WIN,M_STAIRS,M_SWITCH,M_SEALED,M_GOLD,M_NO_GOLD,M_SAVED,M_SAVE_FAIL,M_LOADED,M_NO_SAVE,M_PACK,M_NO_PACK,M_FLOOR_LOCK,M_SAGE,M_NEW_CONFIRM,M_PAUSED,M_COMPLETE};
enum {L_ACCOUNT=0,L_PASSWORD,L_INCOMPLETE,L_INVALID};
typedef struct {U16 hp,atk,def,gold,xp;U8 kind;} Monster;
typedef struct {
 U8 page,return_page,floor,pos,dir,keys[3];
 U16 visited,switches;
 U8 sword,shield,potions,quest;
 U8 cleared[CLEARED_BYTES];
 U16 hp,atk,def,gold,xp,kills,steps,seconds,ticks;
 U8 subtick,message,message_ticks,book_index,battle_ticks,battle_pos,battle_type;
 U16 battle_loss,sequence;
 U8 save_request,load_request,save_valid,boot_tick;
 U8 audio_level,home_request,key_flash,last_key,boot_target;
 U32 login_account,login_password;
 U8 login_account_digits,login_password_digits,login_stage,login_message;
} Tower;
extern Tower EXT g;
extern const U8 ROM levels[MAPS][CELLS];
extern const U8 ROM routes[MAPS][49];
extern const Monster ROM monsters[6];
void tower_init(void);
void tower_new(void);
void tower_tick(U16 key);
U8 tower_tile(U8 floor,U8 pos);
U16 tower_loss(U8 monster_id);
U8 tower_render(U8 pos);
void tower_save(U8 *buffer,U16 sequence);
U8 tower_valid(U8 *buffer);
U8 tower_load(U8 *buffer);
U16 tower_crc(U8 *buffer,U16 len);
void tower_saved(U8 ok,U16 sequence);
U8 tower_saved_audio(U8 *buffer);
#endif
