@echo off
cd /d "%~dp0"
py -3 build.py %*
exit /b %errorlevel%
