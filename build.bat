@echo off
REM --- SFML 遊戲編譯與連結 (修正版) ---

REM 編譯 main.cpp (輸出 main.o)
echo Compiling main.cpp...
g++ -std=c++17 -IC:\SFML-3.0.2\include -c main.cpp -o main.o
if errorlevel 1 goto error

REM 編譯 Constants.cpp (輸出 Constants.o)
echo Compiling Constants.cpp...
g++ -std=c++17 -IC:\SFML-3.0.2\include -c Constants.cpp -o Constants.o
if errorlevel 1 goto error

REM 編譯 Logger.cpp (輸出 Logger.o)
echo Compiling Logger.cpp...
g++ -std=c++17 -IC:\SFML-3.0.2\include -c Logger.cpp -o Logger.o
if errorlevel 1 goto error

REM 編譯 Utils.cpp (輸出 Utils.o)
echo Compiling Utils.cpp...
g++ -std=c++17 -IC:\SFML-3.0.2\include -c Utils.cpp -o Utils.o
if errorlevel 1 goto error

REM 編譯 Shape.cpp (輸出 Shape.o)
echo Compiling Shape.cpp...
g++ -std=c++17 -IC:\SFML-3.0.2\include -c Shape.cpp -o Shape.o
if errorlevel 1 goto error

REM 編譯 Astar.cpp (輸出 Astar.o)
echo Compiling Astar.cpp...
g++ -std=c++17 -IC:\SFML-3.0.2\include -c Astar.cpp -o Astar.o
if errorlevel 1 goto error

REM 編譯 Object.cpp (輸出 Object.o)
echo Compiling Object.cpp...
g++ -std=c++17 -IC:\SFML-3.0.2\include -c Object.cpp -o Object.o
if errorlevel 1 goto error

REM 編譯 Stage.cpp (輸出 Stage.o)
echo Compiling Stage.cpp...
g++ -std=c++17 -IC:\SFML-3.0.2\include -c Stage.cpp -o Stage.o
if errorlevel 1 goto error

REM 連結所有物件檔 (.o) - 輸出 game.exe
echo Linking game.exe...
g++ -LC:\SFML-3.0.2\lib .\Constants.o .\Logger.o .\Utils.o .\Shape.o .\Astar.o .\Object.o .\Stage.o .\main.o -o game.exe -lmingw32 -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system -mwindows
if errorlevel 1 goto error

REM 清理物件檔 (可選，但在開發階段很有用)
del .\main.o
del .\Constants.o
del .\Logger.o
del .\Utils.o
del .\Shape.o
del .\Astar.o
del .\Object.o
del .\Stage.o

REM 執行 (Execute)
echo Running game.exe...
.\game.exe

goto end

:error
echo.
echo *** ERROR: Build failed. Check the errors above. ***

:end