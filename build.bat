
call "%VS120COMNTOOLS%\vsvars32.bat"
set SDK_INCLUDE_DIR="C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include"

cl.exe notepad_wrapper.cpp user32.lib shell32.lib

pause
