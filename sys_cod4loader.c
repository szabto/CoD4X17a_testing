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



#include "q_shared.h"
#include "qcommon_io.h"
#include "sys_main.h"
#include "sys_cod4defs.h"
#include "sys_patch.h"
#include "g_sv_shared.h"
#include "punkbuster.h"
#include "sys_net.h"
#include "scr_vm.h"
#include "server.h"
#include "scr_vm_functions.h"
#include "sys_thread.h"
#include "filesystem.h"
#include "misc.h"
#include "sys_cod4loader.h"
#include "sec_update.h"
#include "cmd.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


#define ELF_TYPEOFFSET 16
#define ELF_INITOFFSET 8612
#define ELF_FINIOFFSET 1842628
#define ELF_RELOCOFFSET 2280980
#define ELF_RELOC2OFFSET 2281020
#define ELF_SECTIONSTRINGOFFSET 0
#define ELF_SECTIONTYPEOFFSET 4
#define DLLMOD_FILESIZE 2281820
#define ELF_TEXTSECTIONLENGTH 1831332


void Sys_CoD4Linker();
void Com_PatchError(void);
void Cvar_PatchModifiedFlags();

static void __cdecl Cbuf_AddText_Wrapper_IW(int dummy, const char *text )
{
    Cbuf_AddText( text );
}

static void __cdecl Cbuf_InsertText_Wrapper_IW(int dummy, const char *text )
{
    Cbuf_InsertText( text );
}

static void Sys_PatchImageData( void )
{


static byte patchblock_01[] = { 0xAE, 0xA, 0x5, 0x8, 
	0x89, 0x3C, 0x24, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90
};

static byte patchblock_Pmove_GetSpeed[] = { 0xC6, 0x5E, 0xA, 0x8,
	0x89, 0x3C, 0x24, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC
};

static byte patchblock_Pmove_GetGravity[] = { 0xC4, 0x68, 0xA, 0x8, 
	0x8B, 0x98, 0x5C, 0x1, 0x0, 0x0, 0x89, 0x5D, 0xB4, 0x89, 0x1C, 0x24, 0xE8, 0xCC, 0xCC, 0xCC,
	0xCC, 0x89, 0x43, 0x58, 0x90, 0x90, 0xC7, 0x83, 0x9C, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x8B, 0x55, 0x8, 0xC6, 0x82, 0x6E, 0x1, 0x0, 0x0, 0xB, 0x8B, 0x4D, 0xB4
};

static byte patchblock_ClientConnect_NoPassword[] = { 0x76, 0x85, 0xA, 0x8,
	0xEB, 0x2C
};

static byte patchblock_G_RegisterCvars[] = { 0x57, 0x49, 0xB, 0x8,
	0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x90, 0x90, 0x90, 0x90, 0x90
};

static byte patchblock_Scr_AddSourceBuffer_Ignore_fs_game[] = { 0x74, 0xFC, 0x14, 0x8,
	0xEB, 0x4A
};

static byte patchblock_Scr_AddSourceBuffer_Ignore_fs_game2[] = { 0x22, 0xFD, 0x14, 0x8,
	0x89, 0xd0, 0x90
};

static byte patchblock_SV_SpawnServer[] = { 0x7E, 0x4A, 0x17, 0x8,
	0xeb, 0x19
};

/*static byte patchblock_SV_SendServerCommand[] = { 0x56, 0x74, 0x17, 0x8,
	0xeb, 0x5c
};*/
//NET_OutOfBandPrint prototype got changed from netadr_t to netadr_t* The remaining hooks should get fixed up by this:
static byte patchblock_NET_OOB_CALL1[] = { 0x75, 0x50, 0x17, 0x8,
	0xC7, 0x44, 0x24, 0x8, 0xE8, 0x18, 0x23, 0x8, 0x8D, 0x43, 0x20, 0x89, 0x44, 0x24, 0x4, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
};

static byte patchblock_NET_OOB_CALL2[] = { 0xB3, 0x69, 0x17, 0x8,
	0xC7, 0x44, 0x24, 0x8, 0xEA, 0x86, 0x21, 0x8, 0x8B, 0x5D, 0x8, 0x8D, 0x43, 0x20, 0x89, 0x44, 
	0x24, 0x4, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
};

static byte patchblock_NET_OOB_CALL3[] = { 0x41, 0xB5, 0x17, 0x8,
	0xC7, 0x44, 0x24, 0x8, 0xEA, 0x86, 0x21, 0x8, 0x8D, 0x46, 0x20, 0x89, 0x44, 0x24, 0x4, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
};

static byte patchblock_NET_OOB_CALL4[] = { 0x9B, 0x53, 0x17, 0x8,
	0x8D, 0x45, 0x9C, 0x89, 0x44, 0x24, 0x8, 0x8D, 0x43, 0x20, 0x89, 0x44, 0x24, 0x4, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
};


	Sys_PatchImageWithBlock(patchblock_01, sizeof(patchblock_01));
	Sys_PatchImageWithBlock(patchblock_Pmove_GetSpeed, sizeof(patchblock_Pmove_GetSpeed));
	Sys_PatchImageWithBlock(patchblock_Pmove_GetGravity, sizeof(patchblock_Pmove_GetGravity));
	Sys_PatchImageWithBlock(patchblock_ClientConnect_NoPassword, sizeof(patchblock_ClientConnect_NoPassword));
	Com_Memset((void*)0x80b4872, 0x90, 41); //In G_RegisterCvars()
	Com_Memset((void*)0x80b478b, 0x90, 108); //In G_RegisterCvars() (sv_maxclients)
	Sys_PatchImageWithBlock(patchblock_G_RegisterCvars, sizeof(patchblock_G_RegisterCvars));
	Sys_PatchImageWithBlock(patchblock_Scr_AddSourceBuffer_Ignore_fs_game, sizeof(patchblock_Scr_AddSourceBuffer_Ignore_fs_game));  //Maybe script unlock ?
	Sys_PatchImageWithBlock(patchblock_Scr_AddSourceBuffer_Ignore_fs_game2, sizeof(patchblock_Scr_AddSourceBuffer_Ignore_fs_game2));  //Script unlock
	Sys_PatchImageWithBlock(patchblock_SV_SpawnServer, sizeof(patchblock_SV_SpawnServer));  //Skip useless check for cvar: sv_dedicated
	Com_Memset((void*)0x8174da9, 0x90, 5); //In SV_SpawnServer()  Don't overwrite sv.frameusec  (was before unknown write only variable)
	Com_Memset((void*)0x81753ea, 0x90, 5); //In SV_SpawnServer()  Removing the call of NET_Sleep() I don't know for what this can be usefull to have here O_o
	Com_Memset((void*)0x8174db5, 0x90, 42); //In SV_SpawnServer()  Don't set cvar cl_paused as well as nextmap
	Com_Memset((void*)0x8174b9b, 0x90, 116); //In SV_SpawnServer()  Removal of sv_maxclients amd ui_maxclients Cvar_Register()
	Com_Memset((void*)0x8204acf, 0x90, 16); //In ???() Skip useless check for cvar: sv_dedicated
	Com_Memset((void*)0x8204ce9, 0x90, 16); //In ???() Skip useless check for cvar: sv_dedicated
	Sys_PatchImageWithBlock(patchblock_NET_OOB_CALL1, sizeof(patchblock_NET_OOB_CALL1));
	Sys_PatchImageWithBlock(patchblock_NET_OOB_CALL2, sizeof(patchblock_NET_OOB_CALL2));
	Sys_PatchImageWithBlock(patchblock_NET_OOB_CALL3, sizeof(patchblock_NET_OOB_CALL3));
	Sys_PatchImageWithBlock(patchblock_NET_OOB_CALL4, sizeof(patchblock_NET_OOB_CALL4));

	Com_Memset((void*)0x81747b5, 0x90, 116); //In SV_???()  Removal of sv_maxclients amd ui_maxclients Cvar_Register()
	Com_Memset((void*)0x817498c, 0x90, 116); //In SV_???()  Removal of sv_maxclients amd ui_maxclients Cvar_Register()

	#ifndef PUNKBUSTER
		Com_Memset((void*)0x8175255, 0x90, 5);
		Com_Memset((void*)0x81751fe, 0x90, 5);
		
	#endif
	
	SetCall(0x8050ab1, Jump_CalcHeight);
	SetJump(0x8050786, Jump_IsPlayerAboveMax);
	SetJump(0x80507c6, Jump_ClampVelocity);
	SetJump(0x805072a, Jump_GetStepHeight);
	SetCall(0x80a4b5f, StopFollowingOnDeath);
	SetJump(0x80a559e, StuckInClient);
	SetCall(0x80a5ec9, Pmove_GetSpeed);
	SetCall(0x80a68d0, Pmove_GetGravity);
	SetJump(0x80a7b60, ClientSpawn);
	SetJump(0x80ae962, G_Say);
	SetJump(0x80adbf2, Cmd_CallVote_f);
	SetCall(0x80b4957, G_RegisterCvarsCallback);
	SetJump(0x80c0b5a, GScr_LoadScripts);
	SetJump(0x80bc03e, ExitLevel); //ToDo Maybe build GScr_ExitLevel directly
#ifdef PUNKBUSTER
	SetJump(0x810e6ea, PbSvGameQuery);
	SetJump(0x810e5dc, PbSvSendToClient);
	SetJump(0x810e5b0, PbSvSendToAddrPort);
#endif
	SetJump(0x817e988, SV_ClipMoveToEntity);
	SetJump(0x81d5a14, Sys_Error);
	SetJump(0x8122724, Com_PrintMessage);
	SetJump(0x8122d94, Com_DPrintfWrapper);
	SetJump(0x817452e, SV_Shutdown);
	SetJump(0x813d086, NET_OutOfBandPrint);
	SetJump(0x80bfef4, Scr_GetMethod);
	SetJump(0x80bd238, Scr_GetFunction);
	SetJump(0x814bef0, Scr_LoadScript);
	SetJump(0x8170a26, SV_DropClient);
	SetJump(0x8179120, SV_SendMessageToClient);
	SetJump(0x817a23e, SV_UpdateServerCommandsToClient);
	SetJump(0x816f828, SV_WriteDownloadToClient);
	SetJump(0x817a392, SV_WriteSnapshotToClient);
	SetJump(0x8178da2, SV_Netchan_TransmitNextFragment);
	SetJump(0x81d76ca, FS_GetBasepath); //Prior: GetCurrentWorkingDir
	SetJump(0x808b764, ClientScr_SetSessionTeam);
	SetJump(0x80b43c4, G_LogPrintf);
	SetJump(0x80a8068, ClientUserinfoChanged);
	SetJump(0x81aa0be, BigInfo_SetValueForKey);
	SetJump(0x81d6fca, Sys_Milliseconds);
	SetJump(0x81d6f7c, Sys_MillisecondsRaw);
	SetJump(0x81a9f8a, va);
	SetJump(0x8131200, MSG_Init);
	SetJump(0x8131320, MSG_InitReadOnly);
	SetJump(0x8131294, MSG_InitReadOnlySplit);
	SetJump(0x8140e9c, Sys_GetValue);
	SetJump(0x8140efe, Sys_IsMainThread);
	SetJump(0x8140f42, Sys_IsDatabaseThread);
	SetJump(0x81d6be4, Sys_EnterCriticalSection);
	SetJump(0x81d6bc8, Sys_LeaveCriticalSection);
	SetJump(0x81d7282, Sys_ListFiles);
	SetJump(0x81d6f06, Sys_FreeFileList);
	SetJump(0x8177402, SV_SendServerCommand_IW);
	SetJump(0x818e73c, FS_Restart);
	SetJump(0x818726c, FS_FCloseFile);

	SetJump(0x81a2944, Cvar_RegisterString);
	SetJump(0x81a2d94, Cvar_RegisterBool);
	SetJump(0x81a2cc6, Cvar_RegisterInt);
	SetJump(0x81a2860, Cvar_RegisterEnum);
	SetJump(0x81a2e6c, Cvar_RegisterFloat);
	SetJump(0x81a2bea, Cvar_RegisterVec2);
	SetJump(0x81a2b08, Cvar_RegisterVec3);
	SetJump(0x81a2550, Cvar_RegisterColor);

	SetJump(0x81a14fa, Cvar_SetString);
	SetJump(0x81a1c6c, Cvar_SetBool);
	SetJump(0x81a20c4, Cvar_SetInt);
	SetJump(0x81a1fe0, Cvar_SetFloat);
	SetJump(0x81a14c2, Cvar_SetColor);
	SetJump(0x81a3422, Cvar_SetStringByName);
	SetJump(0x81a3020, Cvar_SetFloatByName);
	SetJump(0x81a3178, Cvar_SetIntByName);
	SetJump(0x81a32dc, Cvar_SetBoolByName);
	SetJump(0x81a3f66, Cvar_Set);

	SetJump(0x819e7c0, Cvar_GetBool);
	SetJump(0x819e810, Cvar_GetString);
	SetJump(0x819e90a, Cvar_GetInt);

	SetJump(0x819e6d0, Cvar_FindVar);
	SetJump(0x819f328, Cvar_ForEach);
	SetJump(0x81264f4, Cvar_InfoString_IW_Wrapper);
	SetJump(0x81a1cc4, Com_LoadDvarsFromBuffer);

	SetJump(0x8110ff8, Cbuf_AddText_Wrapper_IW);
	SetJump(0x8110f3e, Cbuf_InsertText_Wrapper_IW);
	SetJump(0x8111bea, Cmd_ExecuteSingleCommand);
	
	*(char*)0x8215ccc = '\n'; //adds a missing linebreak
	*(char*)0x8222ebc = '\n'; //adds a missing linebreak
	*(char*)0x8222ebd = '\0'; //adds a missing linebreak

	FS_PatchFileHandleData();
	Com_PatchError();
	Cvar_PatchModifiedFlags();
}


static qboolean Sys_PatchImage()
{
	Sys_PatchImageData( );
	return qtrue;
}


void Sys_ImageRunInitFunctions()
{

    int i;

    void (*functions[])() = { (void*)0x81d8c1e, (void*)0x81b5d3c, (void*)0x81b104c, (void*)0x81a6040, (void*)0x8197bd4, (void*)0x8191cf4,
                              (void*)0x818efac, (void*)0x80f32cc, (void*)0x80f1354, (void*)0x80893b0, (void*)0x80803cc, (void*)0x807fe7c,
		                      (void*)0x807e95c, (void*)0x8076ee4, NULL };

    for(i = 0; functions[i] != NULL; i++)
    {
        functions[i]();
    }
#ifdef PUNKBUSTER	
	void (*PbSv_Initializer)() = (void*)0x810e59c;
	PbSv_Initializer();
#endif

}

/*
=============
Sys_LoadImage

=============
*/
qboolean Sys_LoadImage( ){

    byte *fileimage;
    int len;

    /* Is this file here ? */
    len = FS_FOpenFileRead(BIN_FILENAME, NULL);
    if(len != DLLMOD_FILESIZE)
    {/* Nope !*/

        Sec_Update( qtrue );
        len = FS_FOpenFileRead(BIN_FILENAME, NULL);
        if(len != DLLMOD_FILESIZE)
        {/* Nope !*/
            Com_PrintError("Failed to load the CoD4 Game. Can not startup the game\n");
            return qfalse;
        }
    }
    Sec_Update( qfalse );

    len = FS_ReadFile(BIN_FILENAME, (void**)&fileimage);
    if(!fileimage)
    {
	Com_PrintError("Couldn't open "BIN_FILENAME". CoD4 can not startup.\n");
	return qfalse;
    }
    if(len != DLLMOD_FILESIZE)
    {
	Com_PrintError(BIN_FILENAME" is corrupted! CoD4 can not startup.\n");
	FS_FreeFile(fileimage);
	return qfalse;
    }

    Com_Memcpy(BIN_SECT_TEXT_START, fileimage + BIN_SECT_TEXT_FOFFSET, BIN_SECT_TEXT_LENGTH);
    Com_Memcpy(BIN_SECT_RODATA_START, fileimage + BIN_SECT_RODATA_FOFFSET, BIN_SECT_RODATA_LENGTH);
    Com_Memcpy(BIN_SECT_DATA_START, fileimage + BIN_SECT_DATA_FOFFSET, BIN_SECT_DATA_LENGTH);
    Com_Memset(BIN_SECT_PLT_START, 0xCC, BIN_SECT_PLT_LENGTH);

    Sys_CoD4Linker();

    FS_FreeFile(fileimage);

    if(!Sys_PatchImage())
    {
        return qfalse;
    }
    if(Sys_MemoryProtectExec(BIN_SECT_PLT_START, BIN_SECT_PLT_LENGTH) == qfalse)
    {
        FS_FreeFile(fileimage);
        return qfalse;
    }
    if(Sys_MemoryProtectExec(BIN_SECT_TEXT_START, BIN_SECT_TEXT_LENGTH) == qfalse)
    {
        FS_FreeFile(fileimage);
        return qfalse;
    }
    if(Sys_MemoryProtectReadonly(BIN_SECT_RODATA_START, BIN_SECT_RODATA_LENGTH) == qfalse)
    {
        FS_FreeFile(fileimage);
        return qfalse;
    }
    if(Sys_MemoryProtectWrite(BIN_SECT_DATA_START, BIN_SECT_DATA_LENGTH) == qfalse)
    {
        FS_FreeFile(fileimage);
        return qfalse;
    }
    if(Sys_MemoryProtectWrite(BIN_SECT_BSS_START, BIN_SECT_BSS_LENGTH) == qfalse)
    {
        FS_FreeFile(fileimage);
        return qfalse;
    }

    Sys_ImageRunInitFunctions();

    return qtrue;
}

