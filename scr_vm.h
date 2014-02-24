/*
===========================================================================
    Copyright (C) 2010-2013  Ninja and TheKelm of the IceOps-Team

    This file is part of CoD4X17a-Server source code.

    CoD4X17a-Server source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CoD4X17a-Server source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/



#ifndef __SCR_VM_H__
#define __SCR_VM_H__

#define SCRSTRUCT_ADDR 0x895bf08
#define STRINGINDEX_ADDR 0x836fe20
#define stringIndex (*((stringIndex_t*)(STRINGINDEX_ADDR)))

#include "q_shared.h"
#include "q_math.h"
#include "entity.h"
#include "g_hud.h"
#include "filesystem.h"

typedef struct{
	short   emptystring;
	short   active;
	short   j_spine4;
	short   j_helmet;
	short   j_head;
	short   all;
	short   allies;
	short   axis;
	short   bad_path;
	short   begin_firing;
	short   cancel_location;
	short   confirm_location;
	short   crouch;
	short   current;
	short   damage;
	short   dead;
	short   death;
	short   detonate;
	short   direct;
	short   dlight;
	short   done;
	short   empty;
	short   end_firing;
	short   entity;
	short   explode;
	short   failed;
	short   fraction;
	short   free;
	short   goal;
	short   goal_changed;
	short   goal_yaw;
	short   grenade;
	short   grenade_danger;
	short   grenade_fire;
	short   grenade_pullback;
	short   info_notnull;
	short   invisible;
	short   key1;
	short   key2;
	short   killanimscript;
	short   left;
	short   light;
	short   movedone;
	short   noclass;
	short   none;
	short   normal;
	short   player;
	short   position;
	short   projectile_impact;
	short   prone;
	short   right;
	short   reload;
	short   reload_start;
	short   rocket;
	short   rotatedone;
	short   script_brushmodel;
	short   script_model;
	short   script_origin;
	short   snd_enveffectsprio_level;
	short   snd_enveffectsprio_shellshock;
	short   snd_channelvolprio_holdbreath;
	short   snd_channelvolprio_pain;
	short   snd_channelvolprio_shellshock;
	short   stand;
	short   suppression;
	short   suppression_end;
	short   surfacetype;
	short   tag_aim;
	short   tag_aim_animated;
	short   tag_brass;
	short   tag_butt;
	short   tag_clip;
	short   tag_flash;
	short   tag_flash_11;
	short   tag_flash_2;
	short   tag_flash_22;
	short   tag_flash_3;
	short   tag_fx;
	short   tag_inhand;
	short   tag_knife_attach;
	short   tag_knife_fx;
	short   tag_laser;
	short   tag_origin;
	short   tag_weapon;
	short   tag_player;
	short   tag_camera;
	short   tag_weapon_right;
	short   tag_gasmask;
	short   tag_gasmask2;
	short   tag_sync;
	short   target_script_trigger;
	short   tempEntity;
	short   top;
	short   touch;
	short   trigger;
	short   trigger_use;
	short   trigger_use_touch;
	short   trigger_damage;
	short   trigger_lookat;
	short   truck_cam;
	short   weapon_change;
	short   weapon_fired;
	short   worldspawn;
	short   flashbang;
	short   flash;
	short   smoke;
	short   night_vision_on;
	short   night_vision_off;
	short   MOD_UNKNOWN;
	short   MOD_PISTOL_BULLET;
	short   MOD_RIFLE_BULLET;
	short   MOD_GRENADE;
	short   MOD_GRENADE_SPLASH;
	short   MOD_PROJECTILE;
	short   MOD_PROJECTILE_SPLASH;
	short   MOD_MELEE;
	short   MOD_HEAD_SHOT;
	short   MOD_CRUSH;
	short   MOD_TELEFRAG;
	short   MOD_FALLING;
	short   MOD_SUICIDE;
	short   MOD_TRIGGER_HURT;
	short   MOD_EXPLOSIVE;
	short   MOD_IMPACT;
	short   script_vehicle;
	short   script_vehicle_collision;
	short   script_vehicle_collmap;
	short   script_vehicle_corpse;
	short   turret_fire;
	short   turret_on_target;
	short   turret_not_on_target;
	short   turret_on_vistarget;
	short   turret_no_vis;
	short   turret_rotate_stopped;
	short   turret_deactivate;
	short   turretstatechange;
	short   turretownerchange;
	short   reached_end_node;
	short   reached_wait_node;
	short   reached_wait_speed;
	short   near_goal;
	short   veh_collision;
	short   veh_predictedcollision;
	short   auto_change;
	short   back_low;
	short   back_mid;
	short   back_up;
	short   begin;
	short   call_vote;
	short   freelook;
	short   head;
	short   intermission;
	short   j_head_dup;
	short   manual_change;
	short   menuresponse;
	short   neck;
	short   pelvis;
	short   pistol;
	short   plane_waypoint;
	short   playing;
	short   spectator;
	short   vote;
	short   sprint_begin;
	short   sprint_end;
	short   tag_driver;
	short   tag_passenger;
	short   tag_gunner;
	short   tag_wheel_front_left;
	short   tag_wheel_front_right;
	short   tag_wheel_back_left;
	short   tag_wheel_back_right;
	short   tag_wheel_middle_left;
	short   tag_wheel_middle_right;
	short   script_vehicle_collision_dup;
	short   script_vehicle_collmap_dup;
	short   script_vehicle_corpse_dup;
	short   tag_detach;
	short   tag_popout;
	short   tag_body;
	short   tag_turret;
	short   tag_turret_base;
	short   tag_barrel;
	short   tag_engine_left;
	short   tag_engine_right;
	short   front_left;
	short   front_right;
	short   back_left;
	short   back_right;
	short   tag_gunner_pov;
}stringIndex_t;

typedef void (*xfunction_t)();

typedef struct scr_function_s
{
	struct scr_function_s	*next;
	char			*name;
	xfunction_t		function;
	qboolean		developer;
} scr_function_t;



typedef byte PrecacheEntry[8192];
typedef unsigned int sval_u;

#ifndef SCR_ENTREF_DEFINED
#define SCR_ENTREF_DEFINED
typedef int scr_entref_t;
#endif
/**************** Additional *************************/

typedef enum{
    SCR_CB_NEW_SAY,
    SCR_CB_NEW_SEQMSG,
    SCR_CB_NEW_SEQPLAYERMSG
}script_CallBacks_new_t;

int script_CallBacks_new[8];
qboolean say_forwardAll;

typedef int fieldtype_t;

typedef struct
{
    char* name;
    int val1;
    fieldtype_t type;
    void (*setfun)();
    void (*getfun)();

}client_fields_t;


void __cdecl Scr_InitVariables(void);			//VM
void __cdecl Scr_Init(void);			//VM_Init
void __cdecl Scr_Settings(int, int, int);
void __cdecl Scr_AddEntity(gentity_t* ent);
void __cdecl Scr_Cleanup(void);
void __cdecl GScr_Shutdown(void);
short __cdecl Scr_AllocArray();
int __cdecl Scr_GetNumParam( void );
int __cdecl Scr_GetInt( unsigned int );
float __cdecl Scr_GetFloat( unsigned int );
char* __cdecl Scr_GetString( unsigned int );
gentity_t* __cdecl Scr_GetEntity( unsigned int );
short __cdecl Scr_GetConstString( unsigned int );
unsigned int __cdecl Scr_GetType( unsigned int );
void __cdecl Scr_GetVector( unsigned int, vec3_t* );
void __cdecl Scr_Error( const char *string);
void __cdecl Scr_SetLoading( qboolean );
void __cdecl Scr_ParamError( int, const char *string);
void __cdecl Scr_ObjectError( const char *string);
void __cdecl Scr_AddInt(int value);
void __cdecl Scr_AddFloat(float);
void __cdecl Scr_AddBool(qboolean);
void __cdecl Scr_AddString(const char *string);
void __cdecl Scr_AddUndefined(void);
void __cdecl Scr_AddVector( vec3_t vec );
void __cdecl Scr_AddArray( void );
void __cdecl Scr_MakeArray( void );
void __cdecl Scr_Notify( gentity_t*, unsigned short, unsigned int);
void __cdecl Scr_NotifyNum( int, unsigned int, unsigned int, unsigned int);
/*Not working :(  */
void __cdecl Scr_PrintPrevCodePos( int printDest, const char* unk, qboolean unk2 );
int __cdecl Scr_GetFunctionHandle( const char* scriptName, const char* labelName);
short __cdecl Scr_ExecEntThread( gentity_t* ent, int callbackHook, unsigned int numArgs);
short __cdecl Scr_ExecThread( int callbackHook, unsigned int numArgs);
void __cdecl Scr_FreeThread( short threadId);
unsigned int __cdecl Scr_CreateCanonicalFilename( const char* name );
//Unknown real returntype
unsigned int __cdecl FindVariable( unsigned int, unsigned int );
unsigned int __cdecl FindObject( unsigned int );
unsigned int __cdecl GetNewVariable( unsigned int, unsigned int );
void * __cdecl TempMalloc( int );
void __cdecl ScriptParse( sval_u* , byte);
unsigned int __cdecl GetObjectA( unsigned int );
unsigned int __cdecl GetVariable( unsigned int, unsigned int );
void __cdecl ScriptCompile( sval_u, unsigned int, unsigned int, PrecacheEntry*, int);
void* __cdecl Scr_AddSourceBuffer( const char*, const char*, const char*, byte );
void __cdecl Scr_InitAllocNode( void );
void __cdecl Scr_BeginLoadScripts( void );
void __cdecl Scr_SetClassMap( unsigned int );
void __cdecl Scr_AddFields( unsigned int, const char*, unsigned int );
void __cdecl Scr_SetGenericField( void*, fieldtype_t, int );
void __cdecl Scr_GetGenericField( void*, fieldtype_t, int );
/*
void __cdecl GScr_AddFieldsForEntity( void );
tGScr_AddFieldsForEntity GScr_AddFieldsForEntity = (tGScr_AddFieldsForEntity(0x80c7808);
*/
void __cdecl GScr_AddFieldsForHudElems( void );
void __cdecl GScr_AddFieldsForRadiant( void );
void __cdecl Scr_AddHudElem( game_hudelem_t* );
void __cdecl Scr_FreeHudElem( game_hudelem_t* );
void __cdecl Scr_EndLoadScripts( void );
void __cdecl Scr_ConstructMessageString( int, int, const char*, char*, unsigned int );

qboolean Scr_PlayerSay(gentity_t*, int mode, const char* text);
void GScr_LoadGameTypeScript(void);
unsigned int Scr_LoadScript(const char* scriptname, PrecacheEntry *precache, int iarg_02);
qboolean Scr_ExecuteMasterResponse(char* s);
void Scr_AddStockFunctions(void);
void Scr_AddStockMethods(void);

qboolean Scr_AddFunction( const char *cmd_name, xfunction_t function, qboolean developer);
qboolean Scr_RemoveFunction( const char *cmd_name );
void Scr_ClearFunctions( void );
__cdecl void* Scr_GetFunction( const char** v_functionName, qboolean* v_developer );
qboolean Scr_AddMethod( const char *cmd_name, xfunction_t function, qboolean developer);
qboolean Scr_RemoveMethod( const char *cmd_name );
void Scr_ClearMethods( void );
__cdecl void* Scr_GetMethod( const char** v_functionName, qboolean* v_developer );



#define MAX_SCRIPT_FILEHANDLES 10

typedef enum{
    SCR_FH_FILE,
    SCR_FH_PARALIST,
    SCR_FH_INDEXPARALIST
}scr_fileHandleType_t;


typedef struct{
    FILE* fh;
    scr_fileHandleType_t type;
    char filename[MAX_QPATH];
    int baseOffset;
    int fileSize;
}scr_fileHandle_t;

qboolean Scr_FS_CloseFile( scr_fileHandle_t* f );
int Scr_FS_ReadLine( void *buffer, int len, fileHandle_t f );
qboolean Scr_FS_AlreadyOpened( char* qpath, char* filename, size_t fnamelen);
qboolean Scr_FS_FOpenFile( const char *filename, fsMode_t mode, scr_fileHandle_t* f );
fileHandle_t Scr_OpenScriptFile( char* qpath, scr_fileHandleType_t ft, fsMode_t mode);
qboolean Scr_CloseScriptFile( fileHandle_t fh);
int Scr_FS_Read( void *buffer, int len, fileHandle_t f );
int Scr_FS_Write( const void *buffer, int len, fileHandle_t h );
int Scr_FS_Seek( fileHandle_t f, long offset, int origin );

void GScr_MakeCvarServerInfo(void);
void GScr_SetCvar();
void GScr_GetCvarFloat();
void  GScr_GetCvarInt();
void  GScr_GetCvar();

#endif
