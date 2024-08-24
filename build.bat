@echo off

SET CommonCompilerFlags=/W4 /WX /nologo /GS- /GR- /EHa- /Oi
SET DebugCompilerFlags=/Od /Zi /MTd
SET ReleaseCompilerFlags=/O2 /MT
SET CommonLinkerFlags=/OPT:REF /SUBSYSTEM:CONSOLE Kernel32.lib User32.lib

IF NOT EXIST out mkdir out
pushd out

SET CompilerFlags=%CommonCompilerFlags% %DebugCompilerFlags%
::SET CompilerFlags=%CommonCompilerFlags% %ReleaseCompilerFlags%

SET src=..\\src
SET files=%src%\\chunk.c %src%\\compiler.c %src%\\debug.c %src%\\memory.c %src%\\object.c %src%\\scanner.c %src%\\value.c %src%\\vm.c %src%\\table.c

cl %CompilerFlags% %src%\\clox.c %files% /I %src% /link %CommonLinkerFlags% /OUT:clox.exe

SET LastError=%ERRORLEVEL%
popd
