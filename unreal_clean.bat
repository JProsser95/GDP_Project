@ECHO OFF

SET ProjectName=GDP_Project

ECHO Cleaning up files which may cause problems with launching Unreal.

ECHO.

setlocal enabledelayedexpansion 
SET FilesDeleted=0
SET FileNames[0]=ERROR: Nothing to clean up

ECHO Successfully cleaned up the following:

ECHO.

if exist %~dp0Binaries (
rd /s /q %~dp0Binaries
SET /A FilesDeleted=FilesDeleted+1
SET FileNames[%FilesDeleted%]=Binaries )

if EXIST %~dp0.vs (
rd /s /q %~dp0.vs
SET /A FilesDeleted=%FilesDeleted%+1
SET FileNames[%FilesDeleted%]=.vs Directory )

if EXIST %~dp0Saved (
rd /s /q %~dp0Saved
SET /A FilesDeleted=%FilesDeleted%+1
SET FileNames[%FilesDeleted%]=Saved Directory )

if EXIST %~dp0Intermediate (
rd /s /q %~dp0Intermediate
SET /A FilesDeleted=%FilesDeleted%+1
SET FileNames[%FilesDeleted%]=Intermediate Directory )

if EXIST %~dp0%ProjectName%.sln (
DEL %~dp0%ProjectName%.sln
SET /A FilesDeleted=%FilesDeleted%+1
SET FileNames[%FilesDeleted%]=%ProjectName%.sln )

if EXIST %~dp0%ProjectName%.VC.db (
DEL %~dp0%ProjectName%.VC.db
SET /A FilesDeleted=%FilesDeleted%+1
SET FileNames[%FilesDeleted%]=%ProjectName%.VC.db )

SET /A FilesDeleted=%FilesDeleted%-1

if %FilesDeleted% gtr 0 (
	for /l %%n in (0,1,%FilesDeleted%) do ( 
	   echo !FileNames[%%n]!
	)
) ELSE (
	echo %FileNames[0]%
)

ECHO.

PAUSE