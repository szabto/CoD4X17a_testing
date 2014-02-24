@set path=%LOCALAPPDATA%\nasm;%path%
@echo off

echo Compiling C-code...
gcc -m32 -Wall -O0 -g -fno-omit-frame-pointer -mtune=prescott -D WINVER=0x501 -c ./win32/sys_win32.c
gcc -m32 -Wall -O0 -g -fno-omit-frame-pointer -mtune=prescott -D WINVER=0x501 -c ./win32/win_syscon.c
gcc -m32 -Wall -O0 -g -fno-omit-frame-pointer -mtune=prescott -D WINVER=0x501 -c ./win32/sys_cod4linker_win32.c
gcc -m32 -Wall -O0 -g -fno-omit-frame-pointer -mtune=prescott -D WINVER=0x501 -c ./win32/pe32_parser.c
gcc -m32 -Wall -O0 -g -fno-omit-frame-pointer -mtune=prescott -D WINVER=0x501 -Ilib_tomcrypt/headers -Ilib_tomcrypt/math/tommath -c *.c


echo Compiling NASM...
nasm -f coff qcommon_hooks.asm --prefix _
nasm -f coff cmd_hooks.asm --prefix _
nasm -f coff filesystem_hooks.asm --prefix _
nasm -f coff xassets_hooks.asm --prefix _
nasm -f coff trace_hooks.asm --prefix _
nasm -f coff misc_hooks.asm --prefix _
nasm -f coff scr_vm_hooks.asm --prefix _
nasm -f coff g_sv_hooks.asm --prefix _
nasm -f coff server_hooks.asm --prefix _
nasm -f coff msg_hooks.asm --prefix _

echo Linking...
gcc -Wl,--dynamic-list=pluginExports.ld -g -o cod4x17a_dedrun *.o win32/win_cod4.res -L./ -ltomcrypt_win32 -ltommath_win32 -lm -lws2_32 -lwsock32 -lgdi32 -mwindows -lwinmm 
echo Cleaning up...
del *.o

echo Creating plugin export lib...
pexports cod4x17a_dedrun.exe > cod4x17a_dedrun.def
rename cod4x17a_dedrun.exe _____________________________________________cod4x17a_dedrun.exe
dlltool -D _____________________________________________cod4x17a_dedrun.exe -d cod4x17a_dedrun.def -l plugins/libcom_plugin.a
rename _____________________________________________cod4x17a_dedrun.exe cod4x17a_dedrun.exe
echo Done!
pause
REM ./version_make_progress.sh

