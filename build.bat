cls
set src=src/main.cpp 
@REM set src=src/*.cpp

g++ -O0 -march=native -fno-exceptions -fno-rtti -DNDEBUG -std=c++23 ^
-I D:\Programmig\C++\Libraries\headers -I header ^
-o wallpaper-changer.exe %src%
