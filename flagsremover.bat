@echo off
setlocal enabledelayedexpansion

@REM параметр1 - название программы, параметр2 - флаг для удаления

set "input_file=%cd%\build\CMakeFiles\%1.dir\flags.make"
set "output_file=%cd%\build\CMakeFiles\%1.dir\flags_temp.make"
set "search_string=CXX_FLAGS ="
set "prefix=FLAGSREMOVER:"

if not exist "%input_file%" (
    echo %prefix% Файл %input_file% не найден!
    exit /b 1
)

(
    for /f "usebackq delims=" %%A in ("%input_file%") do (
        set "line=%%A"
        if "!line!"=="!line:%search_string%=!" (
            echo !line!
        ) else (
            set "line=!line:%2=!"
            echo !line!
        )
    )
) > "%output_file%"

move /y "%output_file%" "%input_file%"

echo %prefix% флаг "%2" удален из "%input_file%"