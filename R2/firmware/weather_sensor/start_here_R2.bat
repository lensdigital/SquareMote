@echo off
color 1F
rem ===========================================================================
rem === Author: LensDigital (xronosclock.com)
rem     Updates Squaremote R2 based Temperature and Humidity sensor Firmware
rem ==== INSTRUCTIONS =====
rem ---- First enter COM number, then Frequency (915 or 433 Mhz)
rem ===========================================================================
SET FWVER=2
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
@echo Ready to Upload %CD%\sensor_R%FWVER%_%FREQ%.hex via COM%COMPORT% 
pause
call update_squaremote.bat %COMPORT% %CD%\sensor_R%FWVER%_%FREQ%.hex
@echo 
goto exit
:EXIT