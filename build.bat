@echo off
REM --- SFML 遊戲編譯與連結 (修正版) ---

REM 編譯 main.cpp (輸出 main.o)
echo Compiling main.cpp...
g++ -std=c++17 -IC:\SFML-3.0.2\include -c main.cpp -o main.o
if errorlevel 1 goto error

REM 編譯 Constants.hpp (無需編譯，僅供參考)

REM 編譯 Logger.cpp (輸出 Logger.o)
echo Compiling Logger.cpp...
g++ -std=c++17 -IC:\SFML-3.0.2\include -c Logger.cpp -o Logger.o
if errorlevel 1 goto error

REM 編譯 Utils.cpp (輸出 Utils.o)
echo Compiling Utils.cpp...
g++ -std=c++17 -IC:\SFML-3.0.2\include -c Utils.cpp -o Utils.o

REM 編譯 Shape.cpp (輸出 Shape.o)
echo Compiling Shape.cpp...
g++ -std=c++17 -IC:\SFML-3.0.2\include -c Shape.cpp -o Shape.o
if errorlevel 1 goto error

REM 連結所有物件檔 (.o) - 輸出 game.exe
echo Linking game.exe...
g++ -LC:\SFML-3.0.2\lib .\Logger.o .\Utils.o .\Shape.o .\main.o -o game.exe -lmingw32 -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system -mwindows
if errorlevel 1 goto error

REM 清理物件檔 (可選，但在開發階段很有用)
del .\main.o
del .\Logger.o
del .\Utils.o
del .\Shape.o

REM 執行 (Execute)
echo Running game.exe...
.\game.exe

goto end

:error
echo.
echo *** ERROR: Build failed. Check the errors above. ***

:end