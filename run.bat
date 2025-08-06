@echo off

set EXE_NAME=Chessboard

if /I "%~1"=="-d" (
    set BUILD_MODE=Debug
) else if /I "%~1"=="-r" (
    set BUILD_MODE=Release
) else (
    set BUILD_MODE=Debug
)

echo Building project(%BUILD_MODE%)...
cd build
cmake ..
cmake --build . --config=%BUILD_MODE%

if %errorlevel% neq 0 (
    echo Build failed with error %errorlevel%
    pause
    exit /b %errorlevel%
)

echo Running %EXE_NAME%.exe...
cd %BUILD_MODE%
"%EXE_NAME%.exe"
