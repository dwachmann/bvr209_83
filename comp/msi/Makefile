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

msicab      = $(SLNDIR)\$(MSICAB_RESULT).exe
msicab1     = $(SLNDIR)\$(MSICAB1_RESULT).exe
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

createmsi: clean $(RESULTDIR) $(MSIDIR) $(MSIDIR)\$(BVR20983_CABMSI)
   @copy $(MSITEMPLATEDIR)\Registry0.tmpl /B + $(MSITEMPLATEDIR)\Registry1.tmpl /B $(MSITEMPLATEDIR)\Registry.idt /B
   @copy $(SLNDIR)\$(BVR20983RES_RESULT).dll $(MSITEMPLATEDIR)\Icon\bvr20983res.exe.ibd
   $(msidb) -c -d $(MSIDIR)\$(BVR20983_MSI) -f $(MSITEMPLATEDIR) -i *
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

$(MSIDIR)\$(BVR20983_CABMSI): $(INCDIR)\ver\versions.xml
   @copy $(MSITEMPLATEDIR)\File.tmpl $(MSITEMPLATEDIR)\File.idt
   @copy $(MSITEMPLATEDIR)\Component.tmpl $(MSITEMPLATEDIR)\Component.idt
   @copy $(MSITEMPLATEDIR)\Media.tmpl $(MSITEMPLATEDIR)\Media.idt
   @copy $(MSITEMPLATEDIR)\MsiFileHash.tmpl $(MSITEMPLATEDIR)\MsiFileHash.idt
   if not exist "$(SLNDIR)\documentation/$(NULL)" mkdir $(SLNDIR)\documentation
   if not exist "$(SLNDIR)\samples/$(NULL)" mkdir $(SLNDIR)\samples
   @xcopy $(HTMLDIR)\* $(SLNDIR)\samples /S
   @copy $(SCRIPTSDIR)\bvr20983.vbs $(SLNDIR)\samples
   @copy $(SCRIPTSDIR)\test1.vbs    $(SLNDIR)\samples
   @xcopy $(DOCDIR)\* $(SLNDIR)\documentation /S
   cd $(SLNDIR)
   rundll32.exe $(BVR20983MSGS_RESULT).dll,DllRegistrationInfo $(MSITEMPLATEDIR)\Registry1.tmpl
   rundll32.exe $(BVR20983SC_RESULT).dll,DllRegistrationInfo $(MSITEMPLATEDIR)\Registry1.tmpl
   rundll32.exe $(BVR20983CC_RESULT).dll,DllRegistrationInfo $(MSITEMPLATEDIR)\Registry1.tmpl
   $(msicab) -msicab $(INCDIR)\ver\versions.xml $(SLNDIR) $(MSIDIR)\$(BVR20983_CABMSI) $(MSITEMPLATEDIR)

msicab1:
   $(msicab1) -msicab $(INCDIR)\ver\versions.xml $(SLNDIR) $(MSIDIR)\$(BVR20983_CABMSI)

validate: 
   $(msival) $(MSIDIR)\$(BVR20983_MSI) "c:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Bin\darice.cub" -f

distsrc: 
   svn export $(PROJECTDIR) $(DISTSRCDIR)
   $(msicab) c $(RESULTDIR)\$(BVR20983_RESULT)-src.cab $(DISTSRCDIR)

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
