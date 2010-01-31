# $Id$
# 
# Copyright (C) 2008 Dorothea Wachmann
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see http://www.gnu.org/licenses/.

!include <../../inc/Makefile.inc>

signvars    = sign /f $(signkey) /d "$(BVR20983DESC)" /du "https://bvr20983.berlios.de/" /t http://timestamp.verisign.com/scripts/timstamp.dll
msicab      = $(SLNDIR)\$(MSICAB_RESULT).exe
msidb       = msidb
msival      = msival2
msimsp      = msimsp
msiinfo     = msiinfo
msiexec     = msiexec
cscript     = cscript

!ifndef PROJECTDIR
all: warning
!elseifdef clean
all: clean
!else
all: distribute
!endif

distribute:

msicab:
   @if not exist "$(MSIDIR)\$(NULL)"                  mkdir $(MSIDIR)
   $(msicab) -msicab $(INCDIR)\ver\versions.xml $(PROJECTDIR)\comp\msi\msiid.xml $(PROJECTDIR)\comp\msi\msipackage.xml $(SLNDIR) $(MSIDIR)\$(BVR20983_CABMSI)

msidistribute: $(DISTDIR) 
  @echo signkey=$(signkey) 
  @if not exist "$(signkey)" exit 1
  @if "$(signpwd)"=="" exit 1
  @if not exist "$(DISTDIR)\ProgramMenuFolder\$(NULL)"                    mkdir $(DISTDIR)\ProgramMenuFolder
  @if not exist "$(DISTDIR)\ProgramMenuFolder\StartupMenuFolder\$(NULL)"  mkdir $(DISTDIR)\ProgramMenuFolder\StartupMenuFolder
  @if not exist "$(DISTDIR)\ProgramFilesFolder\$(NULL)"                   mkdir $(DISTDIR)\ProgramFilesFolder
  @if not exist "$(DISTDIR)\ProgramFilesFolder\BVRDIR\$(NULL)"            mkdir $(DISTDIR)\ProgramFilesFolder\BVRDIR
  @if not exist "$(DISTDIR)\ProgramFilesFolder\BVRDIR\bin\$(NULL)"        mkdir $(DISTDIR)\ProgramFilesFolder\BVRDIR\bin
  @if not exist "$(DISTDIR)\ProgramFilesFolder\BVRDIR\samples\$(NULL)"    mkdir $(DISTDIR)\ProgramFilesFolder\BVRDIR\samples
  @xcopy /Q /Y  $(HTMLDIR)\led.*                         $(DISTDIR)\ProgramFilesFolder\BVRDIR\samples
  @xcopy /Q /Y  $(HTMLDIR)\*.jpg                         $(DISTDIR)\ProgramFilesFolder\BVRDIR\samples
  @xcopy /Q /Y  $(SIGNDIR)\$(BVR20983MSGS_RESULT).dll    $(DISTDIR)\ProgramFilesFolder\BVRDIR\bin
  @xcopy /Q /Y  $(SIGNDIR)\$(BVR20983UPDATE_RESULT).dll  $(DISTDIR)\ProgramFilesFolder\BVRDIR\bin
  @xcopy /Q /Y  $(SIGNDIR)\$(BVR20983SC_RESULT).dll      $(DISTDIR)\ProgramFilesFolder\BVRDIR\bin
  @xcopy /Q /Y  $(SIGNDIR)\$(BVR20983CC_RESULT).dll      $(DISTDIR)\ProgramFilesFolder\BVRDIR\bin
  @xcopy /Q /Y  $(SIGNDIR)\$(DIGICLOCK_RESULT).exe       $(DISTDIR)\ProgramFilesFolder\BVRDIR\bin
  @xcopy /Q /Y  $(SIGNDIR)\$(LSSTG_RESULT).exe           $(DISTDIR)\ProgramFilesFolder\BVRDIR\bin
  @xcopy /Q /Y  $(SIGNDIR)\$(LSTYPEINFO_RESULT).exe      $(DISTDIR)\ProgramFilesFolder\BVRDIR\bin
  @xcopy /Q /Y $(DOCDIR)\gpl-3.0.*.htm                  $(DISTDIR)\ProgramFilesFolder\BVRDIR
  $(SLNDIR)\$(MSICAB_RESULT).exe -msicab $(INCDIR)\ver\versions.xml $(PROJECTDIR)\comp\msi\msiid.xml $(PROJECTDIR)\comp\msi\msipackage.xml $(DISTDIR) $(MSIDIR)\$(BVR20983_CABMSI)
  @$(sign) $(signvars) /p $(signpwd) $(MSIDIR)\$(BVR20983_CABMSI)
 
msitransform: 
   @echo signkey=$(signkey) 
   @if not exist "$(signkey)" exit 1
   @if "$(signpwd)"=="" exit 1
   @if not exist "$(MSIDIR)\templates\$(NULL)"        mkdir $(MSIDIR)\templates
   @if not exist "$(MSIDIR)\templates\Binary\$(NULL)" mkdir $(MSIDIR)\templates\Binary
   @if not exist "$(MSIDIR)\templates\Icon\$(NULL)"   mkdir $(MSIDIR)\templates\Icon
   @xcopy /Q /Y $(MSITEMPLATEDIR)\*.idt    $(MSIDIR)\templates
   @xcopy /Q /Y $(MSITEMPLATEDIR)\Binary\* $(MSIDIR)\templates\Binary
   @xcopy /Q /Y $(MSITEMPLATEDIR)\Icon\*   $(MSIDIR)\templates\Icon
   @xcopy /Q /Y $(SLNDIR)\$(BVR20983RES_RESULT).dll $(MSIDIR)\templates\Icon\bvr20983res.exe.ibd
   $(cscript) //job:transform $(SCRIPTSDIR)\msi.wsf /File:$(PROJECTDIR)\comp\msi\msipackage.xml /MSIDir:$(MSIDIR)\templates
   $(msidb) -c -d $(MSIDIR)\$(BVR20983_MSI) -f $(MSIDIR)\templates -i *
   $(msidb) -d $(MSIDIR)\$(BVR20983_MSI) -a $(MSIDIR)\$(BVR20983_CABMSI)
   $(msiinfo) $(MSIDIR)\$(BVR20983_MSI) \
             -T "Installation Database" \
             -J "$(BVR20983_RESULT)" \
             -A "$(BVR20983_MSI_COMPANY)" \
             -K "Installer, MSI, Database" \
             -O "$(BVR20983DESC)" \
             -P "Intel;1033" \
             -V {$(BVR20983_MSI_PACKAGECODE)} \
             -G 200 \
             -W 2 \
             -N msidb \
             -U 0 \
             -L dw
   @$(sign) $(signvars) /p $(signpwd) $(MSIDIR)\$(BVR20983_MSI)

validate: 
   $(msival) $(MSIDIR)\$(BVR20983_MSI) "c:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Bin\darice.cub" -f

createpatch: $(MSIPATCHDIR)
  $(cscript) //job:msipatch $(SCRIPTSDIR)\patch.wsf /File:$(INCDIR)\ver\versions.xml /MSIDir:$(MSIDIR) /MSIPatchDir:$(MSIPATCHDIR)  /MSIPatchTemplateDir:$(MSIPATCHTMPLDIR)

applypatch: 
  $(msiexec) /p $(MSIDIR)\$(BVR20983_RESULT).msp REINSTALL=ALL REINSTALLMODE=omus /log $(MSIDIR)\$(BVR20983_RESULT)-applypatch.log

clean:
  if exist $(MSIDIR)\$(BVR20983_CABMSI) del /q $(MSIDIR)\$(BVR20983_CABMSI)
  if exist $(MSIDIR)\$(BVR20983_MSI) del /q $(MSIDIR)\$(BVR20983_MSI)
  if exist $(MSITEMPLATEDIR)\MsiFileHash.idt del /q $(MSITEMPLATEDIR)\MsiFileHash.idt
  if exist $(MSITEMPLATEDIR)\Media.idt del /q $(MSITEMPLATEDIR)\Media.idt
  if exist $(MSITEMPLATEDIR)\File.idt del /q $(MSITEMPLATEDIR)\File.idt
  if exist $(MSITEMPLATEDIR)\Component.idt del /q $(MSITEMPLATEDIR)\Component.idt
  if exist $(MSITEMPLATEDIR)\Registry1.tmpl del /q $(MSITEMPLATEDIR)\Registry1.tmpl
  if exist $(MSITEMPLATEDIR)\Registry.idt del /q $(MSITEMPLATEDIR)\Registry.idt
  if exist $(MSITEMPLATEDIR)\Icon\bvr20983res.exe.ibd del /q $(MSITEMPLATEDIR)\Icon\bvr20983res.exe.ibd
  if exist $(DISTSRCDIR)/$(NULL) rd /s /q $(DISTSRCDIR)
  if exist $(MSIPATCHDIR)/$(NULL) rd /s /q $(MSIPATCHDIR)
  if exist $(SLNDIR)\documentation/$(NULL) rd /s /q $(SLNDIR)\documentation
  if exist $(SLNDIR)\samples/$(NULL) rd /s /q $(SLNDIR)\samples
  if exist $(RESULTDIR)\$(BVR20983_RESULT)-src.cab del /q $(RESULTDIR)\$(BVR20983_RESULT)-src.cab

install:
  msiexec /i $(MSIDIR)\$(BVR20983_MSI) /log $(MSIDIR)\msiinstall.log

uninstall:
  msiexec /x $(MSIDIR)\$(BVR20983_MSI) /log $(MSIDIR)\msiuninstall.log
  
warning:
  @echo macro PROJECTDIR is not defined

!include <../../inc/Makefile.rule>
