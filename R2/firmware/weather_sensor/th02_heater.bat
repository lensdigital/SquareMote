@echo off
color 1F
rem ===========================================================================
rem === Author: LensDigital (xronosclock.com)
rem ==== INSTRUCTIONS =====
rem ---- Turns on TH02 heater on Squaremote R2 to fix "stuck" humidity value
rem ---- After uploading let it run for about 15 minutes. Observe temeprature reading increasing via terminal monitor
rem ===========================================================================
SET FWVER=1
SET ATMEGA=328
rem ============================================================================
rem ++++ DO NOT MODIFY LINE BELOW!!!! +++++
rem ============================================================================
:START
cls
@echo =======================================================
@echo --- Squaremote R2 Compatible  firmware updater ----
@echo =======================================================
SET /P COMPORT=Enter COM port number (enter 0 to exit) and press ENTER:
IF %COMPORT%==0 GOTO EXIT
SET /P FREQ=Enter Frequency (915 or 433) and press ENTER:
IF %FREQ%==0 GOTO EXIT
:UPDATE
rem SET FIRMEWARE=xronos%FWVER%_%ATMEGA%.hex
@echo Ready to Upload %CD%\th02_heater.hex via COM%COMPORT% 
pause
call update_squaremote.bat %COMPORT% %CD%\th02_heater.hex
@echo 
goto exit
:EXIT