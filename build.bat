@echo off

SET CommonCompilerFlags=/W4 /WX /nologo /GS- /GR- /EHa- /Oi
SET DebugCompilerFlags=/Od /Zi /MTd
SET ReleaseCompilerFlags=/O2 /MT
SET CommonLinkerFlags=/OPT:REF /SUBSYSTEM:CONSOLE Kernel32.lib User32.lib

IF NOT EXIST out mkdir out
pushd out

SET CompilerFlags=%CommonCompilerFlags% %ReleaseCompilerFlags%

::cl %CompilerFlags% ..\\tests\\arena_tests.c /I ..\\src /link %CommonLinkerFlags% /OUT:arena_test.exe
cl %CompilerFlags% ..\\tests\\scanner_tests.c /I ..\\src /link %CommonLinkerFlags% /OUT:scanner_test.exe
::cl %CompilerFlags% ..\\src\\clox.c /I ..\\src /link %CommonLinkerFlags% /OUT:clox.exe

SET LastError=%ERRORLEVEL%
popd
