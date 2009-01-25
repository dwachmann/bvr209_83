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

TEMPLATEDIR = $(MAKEDIR)\template
msicab      = $(SLNDIR)\$(MSICAB_RESULT).exe
msidb       = msidb
msival      = msival2
msiinfo     = msiinfo

!ifdef clean
all: clean
!else
all: distribute
!endif

distribute: msi

msi: clean $(RESULTDIR) $(MSIDIR) $(MSIDIR)\$(BVR20983_CABMSI)
   @copy $(TEMPLATEDIR)\Registry0.tmpl /B + $(TEMPLATEDIR)\Registry1.tmpl /B $(TEMPLATEDIR)\Registry.idt /B
   $(msidb) -c -d $(MSIDIR)\$(BVR20983_MSI) -f $(TEMPLATEDIR) -i *
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

$(MSIDIR)\$(BVR20983_CABMSI): ..\..\inc\ver\versions.xml
   @copy $(TEMPLATEDIR)\File.tmpl $(TEMPLATEDIR)\File.idt
   @copy $(TEMPLATEDIR)\Component.tmpl $(TEMPLATEDIR)\Component.idt
   @copy $(TEMPLATEDIR)\Media.tmpl $(TEMPLATEDIR)\Media.idt
   if not exist "$(SLNDIR)\documentation/$(NULL)" mkdir $(SLNDIR)\documentation
   if not exist "$(SLNDIR)\samples/$(NULL)" mkdir $(SLNDIR)\samples
   @xcopy ..\..\res\html\* $(SLNDIR)\samples /S
   @copy ..\..\scripts\bvr20983.vbs $(SLNDIR)\samples
   @copy ..\..\scripts\test1.vbs    $(SLNDIR)\samples
   @xcopy ..\..\doc\* $(SLNDIR)\documentation /S
   cd $(SLNDIR)
   rundll32.exe $(BVR20983MSGS_RESULT).dll,DllRegistrationInfo $(TEMPLATEDIR)\Registry1.tmpl
   rundll32.exe $(BVR20983SC_RESULT).dll,DllRegistrationInfo $(TEMPLATEDIR)\Registry1.tmpl
   rundll32.exe $(BVR20983CC_RESULT).dll,DllRegistrationInfo $(TEMPLATEDIR)\Registry1.tmpl
   $(msicab) -msicab ..\..\inc\ver\versions.xml $(SLNDIR) $(MSIDIR)\$(BVR20983_CABMSI) $(TEMPLATEDIR)

validate: 
   $(msival) $(MSIDIR)\$(BVR20983_MSI) "c:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Bin\darice.cub" -f

distsrc: 
   svn export ..\.. $(DISTSRCDIR)
   $(msicab) c $(RESULTDIR)\$(BVR20983_RESULT)-src.cab $(DISTSRCDIR)

clean:
  if exist $(MSIDIR)\$(BVR20983_CABMSI) del /q $(MSIDIR)\$(BVR20983_CABMSI)
  if exist $(MSIDIR)\$(BVR20983_MSI) del /q $(MSIDIR)\$(BVR20983_MSI)
  if exist $(TEMPLATEDIR)\Media.idt del /q $(TEMPLATEDIR)\Media.idt
  if exist $(TEMPLATEDIR)\File.idt del /q $(TEMPLATEDIR)\File.idt
  if exist $(TEMPLATEDIR)\Component.idt del /q $(TEMPLATEDIR)\Component.idt
  if exist $(TEMPLATEDIR)\Registry1.tmpl del /q $(TEMPLATEDIR)\Registry1.tmpl
  if exist $(TEMPLATEDIR)\Registry.idt del /q $(TEMPLATEDIR)\Registry.idt
  if exist $(DISTSRCDIR)/$(NULL) rd /s /q $(DISTSRCDIR)
  if exist $(SLNDIR)\documentation/$(NULL) rd /s /q $(SLNDIR)\documentation
  if exist $(SLNDIR)\samples/$(NULL) rd /s /q $(SLNDIR)\samples
  if exist $(RESULTDIR)\$(BVR20983_RESULT)-src.cab del /q $(RESULTDIR)\$(BVR20983_RESULT)-src.cab

install:
  msiexec /i $(MSIDIR)\$(BVR20983_MSI) /log $(MSIDIR)\msiinstall.log

uninstall:
  msiexec /x $(MSIDIR)\$(BVR20983_MSI) /log $(MSIDIR)\msiuninstall.log

!include <../../inc/Makefile.rule>
