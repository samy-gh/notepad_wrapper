
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

cl.exe notepad_wrapper.cpp user32.lib shell32.lib

pause
