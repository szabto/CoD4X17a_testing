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
#include "sec_update.h"
#include "sec_crypto.h"
#include "sec_common.h"
#include "sec_main.h"
#include "sys_main.h"
#include "msg.h"
#include "sys_net.h"
#include "netchan.h"
#include "filesystem.h"
#include "cmd.h"
#include "cvar.h"
#include "sys_cod4defs.h"

#include <unistd.h>
#include <errno.h>

#define BUFSIZE 10240

cvar_t *canupdate;

char *Sec_StrTok(char *str,char *tokens,int id){
    static char *mem[100] = {NULL};
    char *ptr,*ptr2;
    if(id<0||id>99||tokens==NULL)
	return NULL;
    if(str==NULL){
	if(mem[id]==NULL){
	    return NULL;
	}
	
	for(ptr=mem[id];*ptr != 0;++ptr){
	    //printf("---%c\n",*ptr);
	    for(ptr2=tokens;*ptr2!=0;++ptr2){
		//printf("------%c\n",*ptr2);
		if(*ptr == *ptr2){
		    //printf("DEBUG!!!!!!!! %p:\"%s\", %p:\"%s\",%p:\"%s\".\n\n",ptr,ptr,ptr2,ptr2,mem[id],mem[id]);
		    *ptr=0;
		    ptr2=mem[id];
		    mem[id]=ptr+1;
		    //printf("DEBUG!!!!!!!! %p:\"%s\", %p:\"%s\".\n\n",ptr,ptr,ptr2,ptr2);
		    //__asm__("int $3");
		    return ptr2;
		}
	    }
	}
	if(ptr!=mem[id]){
	    ptr = mem[id];
	}
	else
	    ptr = NULL;
	mem[id]=NULL;
	return ptr;
    }
    else{
    //printf("Debugging: \"%s\"\n",str);
	//mem[id]=str;
	for(ptr=str;*ptr != 0 && mem[id]==NULL;++ptr){
	    for(ptr2=tokens;*ptr2!=0 && mem[id]==NULL;++ptr2){
		if(*ptr != *ptr2){
		    mem[id]=ptr;
		}
	    }
	}
	if(mem[id] == NULL) return NULL;
	return Sec_StrTok(NULL,tokens,id); // BECAUSE I CAN.
    }	
}

#define TCP_TIMEOUT 12

int Sec_GetHTTPPacket(int sock, sec_httpPacket_t *out){
    byte buff[8192]; // Would say 'risky', but it is only 0.8% of the stack in most cases :)
    msg_t msg_c, msg_h;
    int status;
    char* line;
    unsigned int time;

    memset(out,0,sizeof(sec_httpPacket_t));

    status = 1;
    out->contentLength = -1;

    time = Sys_Milliseconds() + TCP_TIMEOUT*1000;

    MSG_Init(&msg_h, buff, sizeof(buff));

    do{
        while(status > 0 && status != 2 && time > Sys_Milliseconds())
        {
            status = NET_ReceiveData(sock, &msg_h);
            usleep(10000);
        }
        if(status < 1 && msg_h.cursize - msg_h.readcount < 1){

		Com_PrintError("Sec_GetHTTPPacket: Failed to receive data!\n");
		return -1;
	}

	if(out->code < 1)
	{
		line = MSG_ReadStringLine(&msg_h);
		if(Q_stricmpn(line,"HTTP/1.",7) || isInteger(line + 9, 4) == qfalse){
			Com_PrintError("Sec_GetHTTPPacket: Packet is corrupt!\nDebug: %s\n", line);
			return -3;
		}
		out->code = atoi(line + 9);
	}
	line = MSG_ReadStringLine(&msg_h);
	if(out->contentLength < 0)
	{
		if(!Q_stricmpn("Content-Length:", line, 15))
		{
			if(isInteger(line + 15, 0) == qfalse)
			{
				Com_PrintError("Sec_GetHTTPPacket: Packet is corrupt!\nDebug: %s\n", line);
				return -5;
			}
			out->contentLength = atoi(line + 15);
		}
	}else if(line[0] == '\r'){
            /* We are done with header */
            break;
	}
    }while(line[0] != '\0');


    if(line[0] == '\0')
    {
	Com_PrintError("Sec_GetHTTPPacket: Packet has unexpected end!\n");
	return -2;
    }

    if(out->contentLength < 0)
    {
        out->contentLength = 0;
	Com_PrintError("Sec_GetHTTPPacket: Packet is corrupt!\n");
	return -4;
    }

    out->headerLength = msg_h.readcount;
    out->header = Sec_SMalloc(out->headerLength + 1);
    Com_Memcpy(out->header, msg_h.data, out->headerLength);
    out->header[out->headerLength] = 0; // A valid C string :)
    out->content = Sec_SMalloc(out->contentLength + 1);

    MSG_Init(&msg_c, (byte*)out->content, out->contentLength);
    MSG_WriteData(&msg_c, msg_h.data + msg_h.readcount, msg_h.cursize - msg_h.readcount);

    while(status > 0 && time > Sys_Milliseconds() && msg_c.cursize != out->contentLength)
    {
        status = NET_ReceiveData(sock, &msg_c);
        usleep(10000);
    }

    out->content[out->contentLength] = '\0';

    if(status > 0)
    {
        NET_TcpCloseSocket(sock);
    }
    if(msg_c.cursize != out->contentLength){
	    Com_PrintError("Sec_GetHTTPPacket: Packet has invalid length. Length is: %d Expected %d!\n", msg_c.cursize, out->contentLength);
	    return -7;
    }
    return out->code;
}


void Sec_FreeHttpPacket(sec_httpPacket_t *packet){
    Sec_Free(packet->content);
    Sec_Free(packet->header);
}
void Sec_FreeFileStruct(sec_file_t *file){
    if(file->next != NULL)
	Sec_FreeFileStruct(file->next);
    Sec_Free(file);
}

void Sec_Update( qboolean getbasefiles ){
    char buff[SEC_UPDATE_INITIALBUFFSIZE];
    char *ptr,*ptr2, *testfile;
    char baseurl[1024];
    char commandline[1024];
    char name1[256],name2[256];
    int sock,status;
    sec_file_t files, *currFile = &files;
    sec_httpPacket_t packet,packet2;
    qboolean dlExec = qfalse;
    int len;
    char hash[128];
    long unsigned size;
    
    if(!Sec_Initialized()){
	return;
    }
    
#ifdef CAN_UPDATE
    Com_Printf("\n-----------------------------\n");
    Com_Printf(" CoD4X Auto Update\n");
    Com_Printf(" Current version: %g\n",SEC_VERSION);
    Com_Printf(" Current build: %d\n",BUILD_NUMBER);
    Com_Printf(" Current type: %s\n",SEC_TYPE == 's' ? "stable      " : "experimental");
    Com_Printf("-----------------------------\n\n");

    canupdate = Cvar_RegisterBool("allowupdating", qtrue, 0, "This enables autoupdating of CoD4 server with new versions.");

    if(getbasefiles == qtrue)
    {

        Com_sprintf(buff, sizeof(buff), SEC_UPDATE_GETGROUNDVERSION);

    }else{

        if(canupdate->boolean == qfalse)
            return;

        Com_sprintf(buff, sizeof(buff), SEC_UPDATE_GETVERSION);
    }
#else
    if(getbasefiles == qtrue)
    {
        Com_sprintf(buff, sizeof(buff), SEC_UPDATE_GETGROUNDVERSION);
    }else{
        return;
    }
#endif
    sock = NET_TcpClientConnect(SEC_UPDATE_HOST);

    if(sock < 0){
	return;
    }

    status = NET_TcpSendData(sock, buff, strlen(buff));

    if(status < 0){
	return;
    }
    /* Need to catch errors */
    FS_WriteFile("tmp.txt", va("%d", status), 1);

    // TODO: Do something with the status?

    status = Sec_GetHTTPPacket(sock, &packet);

    FS_WriteFile("tmp2.txt", packet.header, packet.headerLength);
    FS_WriteFile("tmp3.txt", packet.content, packet.contentLength);

    if(status <= 0){
	Com_PrintError("Receiving data. Error code: %d.\n", status);
	Sec_FreeHttpPacket(&packet);
	return;
    }
    if(status == 204){
	Com_Printf("\nServer is up to date.\n\n");
	Sec_FreeHttpPacket(&packet);
	return;
    }
    else if(status != 200){
	Com_PrintWarning("The update server's malfunction.\nStatus code: %d.\n",status);
	Sec_FreeHttpPacket(&packet);
	return;
    }

    Com_Memset(&files, 0, sizeof(files));

    /* We need to parse filenames etc */
    ptr = Sec_StrTok(packet.content,"\n",42); // Yes, 42.
    if(ptr == NULL || Q_stricmpn("baseurl: ", ptr, 9))
    {
	    Com_PrintWarning("Sec_Update: Corrupt data from update server. Update aborted.\n");
	    Sec_FreeHttpPacket(&packet);
	    return;
    }
    Q_strncpyz(baseurl, ptr +9, sizeof(baseurl));

    ptr = Sec_StrTok(NULL,"\n",42); // Yes, 42 again.

    while(ptr != NULL){
	
	currFile->next = Sec_GMalloc(sec_file_t,1);
	currFile = currFile->next;
	Com_Memset(currFile,0,sizeof(sec_file_t));
	ptr2 = strchr(ptr,' ');
	if(ptr2 == NULL){
	    Com_PrintWarning("Sec_Update: Corrupt data from update server. Update aborted.\nDebug:\"%s\"\n",ptr);
	    Sec_FreeHttpPacket(&packet);
	    return;
	}
	*ptr2++ = 0;
	Q_strncpyz(currFile->path,ptr,sizeof(currFile->path));
	ptr = ptr2;
	ptr2 = strchr(ptr,' ');
	if(ptr2 == NULL){
	    Com_PrintWarning("Sec_Update: Corrupt data from update server. Update aborted.\nDebug:\"%s\"\n",ptr);
	    Sec_FreeHttpPacket(&packet);
	    return;
	}
	*ptr2++ = 0;
	if(!isInteger(ptr, 0)){
	    Com_PrintWarning("Sec_Update: Corrupt data from update server - size is not a number. Update aborted.\nDebug:\"%s\"\n",ptr);
	    Sec_FreeHttpPacket(&packet);
	    return;
	}
	currFile->size = atoi(ptr);
	Q_strncpyz(currFile->hash,ptr2,sizeof(currFile->hash));
	Q_strncpyz(currFile->name,currFile->path, sizeof(currFile->name));
	//printf("DEBUG: File to download: link: \"%s\", name: \"%s\", size: %d, hash: \"%s\"\n\n",file.path,file.name,file.size,file.hash);

        sock = NET_TcpClientConnect(SEC_UPDATE_HOST);

        if(sock < 0){
		Sec_FreeHttpPacket(&packet);
		return;
        }

	Com_sprintf(buff, sizeof(buff), SEC_UPDATE_DOWNLOAD(baseurl, currFile->path));
	status = NET_TcpSendData(sock, buff, strlen(buff));

	Q_strncpyz(buff,currFile->name, sizeof(buff));
	Q_strcat(buff, sizeof(buff),".new");

	Com_Printf("Downloading file: \"%s\"\n",currFile->name);

	status = Sec_GetHTTPPacket(sock, &packet2);
	if(status != 200){
	    Com_PrintError("Downloading has failed! Error code: %d. Update aborted.\n", status);
	    Sec_FreeHttpPacket(&packet);
	    Sec_FreeHttpPacket(&packet2);
	    return;
	}

	len = FS_SV_BaseWriteFile(buff, packet2.content, packet2.contentLength);
	if(len != packet2.contentLength){

		len = FS_SV_HomeWriteFile(buff, packet2.content, packet2.contentLength);
		if(len != packet2.contentLength)
		{
		    Com_PrintError("Opening \"%s\" for writing! Update aborted.\n",buff);
		    Sec_FreeHttpPacket(&packet);
		    Sec_FreeHttpPacket(&packet2);
		    return;
		}
	}

	ptr = Sec_StrTok(NULL,"\n",42); // Yes, 42 again.

	size = sizeof(hash);
	
	if(!Sec_HashMemory(SEC_HASH_SHA256,packet2.content, packet2.contentLength, hash, &size,qfalse)){
	    Com_PrintError("Hashing the file \"%s\". Error code: %s.\nUpdate aborted.\n",currFile->name,Sec_CryptErrStr(SecCryptErr));
	    Sec_FreeHttpPacket(&packet);
	    Sec_FreeHttpPacket(&packet2);
	    return;
	}

	Sec_FreeHttpPacket(&packet);
	Sec_FreeHttpPacket(&packet2);

	if(!Q_strncmp(hash, currFile->hash, size)){
	    Com_Printf("Successfully downloaded file \"%s\".\n", currFile->name);
	}
	else{
	    Com_PrintError("File \"%s\" is corrupt!\nUpdate aborted.\n",currFile->name);
	    Com_DPrintf("Hash: \"%s\", correct hash: \"%s\".\n",hash,currFile->hash);

	    return;
	}
	
    }

    Com_Printf("All files downloaded successfully. Applying update...\n");

    currFile = files.next;
    do{
	Com_Printf("Updating file %s...\n", currFile->name);
	Q_strncpyz(name1, currFile->name, sizeof(name1));

	Q_strcat(name1, sizeof(name1), ".old");

	Q_strncpyz(name2, currFile->name, sizeof(name2));

	Q_strcat(name2, sizeof(name2), ".new");

	testfile = FS_SV_GetFilepath(name1);
	if(testfile != NULL)
	{ // Old file exists, back it up
		FS_SV_BaseRemove( name1 );
		FS_SV_HomeRemove( name1 );
		testfile = FS_SV_GetFilepath(name1);
		if(testfile != NULL)
		{
			Com_PrintWarning("Couldn't remove backup file: %s\n", testfile);
		}
		if(FS_SV_HomeFileExists(name1) == qtrue)
		{
			Com_PrintError("Couldn't remove backup file from fs_homepath: %s\n", name1);
		}
	}
	// Check if an old file exists with this name
	testfile = FS_SV_GetFilepath(currFile->name);
	if(testfile != NULL)
	{ // Old file exists, back it up
		FS_SV_Rename(currFile->name, name1);
	}
	testfile = FS_SV_GetFilepath(currFile->name);
	// We couldn't back it up. Now we try to just delete it.
	if(testfile != NULL)
	{
		FS_SV_BaseRemove( currFile->name );
		FS_SV_HomeRemove( currFile->name );
		testfile = FS_SV_GetFilepath( currFile->name );
		if(testfile != NULL)
		{
			Com_PrintWarning("Couldn't remove file: %s\n", testfile);
		}
		if(FS_SV_HomeFileExists(currFile->name) == qtrue)
		{
			Com_PrintError("Couldn't remove file from fs_homepath: %s\n", currFile->name);
			Com_PrintError("Update has failed!\n");
			return;
		}
	}

	if(Q_strncmp(currFile->name, EXECUTABLE_NAME, 15)){
		/* This is not the executable file */
		FS_SV_Rename(name2, currFile->name);
		testfile = FS_SV_GetFilepath(currFile->name);
		if(testfile == NULL)
		{
			Com_PrintError("Failed to rename file %s to %s\n", name2,currFile->name);
			Com_PrintError("Update has failed!\n");
			return;
		}
		Com_Printf("Update on file %s successfully applied.\n",currFile->name);

	}else{
		/* This is the executable file */
		testfile = FS_SV_GetFilepath(name2);
		if(testfile == NULL)
		{
			Com_PrintError("Can not find file %s\n", name2);
			Com_PrintError("Update has failed!\n");
			return;
		}
		if(FS_SetPermissionsExec(name2) == qfalse)
		{
			Com_PrintError("CRITICAL ERROR: failed to change mode of the file \"%s\"! Aborting, manual installation might be required.\n", name2);
			return;
		}
		FS_RenameOSPath(Sys_ExeFile(), va("%s.dead", Sys_ExeFile()));
		FS_RemoveOSPath(va("%s.dead", Sys_ExeFile()));
		FS_RemoveOSPath(Sys_ExeFile());
		if(FS_FileExistsOSPath(Sys_ExeFile()))
		{
			Com_PrintError("Failed to delete file %s\n", Sys_ExeFile());
			Com_PrintError("Update has failed!\n");
			return;
		}
		FS_RenameOSPath(testfile, Sys_ExeFile());
		if(!FS_FileExistsOSPath(Sys_ExeFile()))
		{
			Com_PrintError("Failed to rename file %s\n", testfile);
			Com_PrintError("Update has failed! Manual reinstallation of file %s is required. This server is now broken!\n", Sys_ExeFile());
			return;
		}
		Com_Printf("Update on file %s successfully applied.\n", Sys_ExeFile());
		dlExec = qtrue;
	}
	currFile = currFile->next;

    }while(currFile != NULL);

    Sec_FreeFileStruct(files.next);
    Com_Printf("Finalizing update...\n");


    if(dlExec == qtrue)
    {
        Com_sprintf(commandline, sizeof(commandline), "%s %s", Sys_ExeFile(), Sys_GetCommandline());
        Com_Printf("Restart commandline is: %s\n", commandline);
        Sys_SetExitCmdline(commandline);

        Cbuf_ExecuteText(EXEC_NOW, "quit\n");
        Com_Printf("If you can see this, call 911! Something went terribly wrong...\n");
        Com_PrintError("execv's error code: %d, error string: \"%s\".\n",errno,strerror(errno));
        exit(87);

    }else{
        FS_Restart( 0 );
    }
}