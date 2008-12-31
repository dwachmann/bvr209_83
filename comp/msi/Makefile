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

distribute: clean $(RESULTDIR) $(MSIDIR) $(MSIDIR)\$(BVR20983_CABMSI)
   $(msidb) -c -d $(MSIDIR)\$(BVR20983_MSI) -f $(TEMPLATEDIR) -i *
   $(msidb) -d $(MSIDIR)\$(BVR20983_MSI) -a $(MSIDIR)\$(BVR20983_CABMSI)

$(MSIDIR)\$(BVR20983_CABMSI): ..\..\inc\ver\versions.xml
   @copy $(TEMPLATEDIR)\File.tmpl $(TEMPLATEDIR)\File.idt
   @copy $(TEMPLATEDIR)\Component.tmpl $(TEMPLATEDIR)\Component.idt
   @copy $(TEMPLATEDIR)\Registry.tmpl $(TEMPLATEDIR)\Registry.idt
   cd $(SLNDIR)
   rundll32.exe $(BVR20983MSGS_RESULT).dll,DllRegistrationInfo $(TEMPLATEDIR)\Registry.idt
   rundll32.exe $(BVR20983SC_RESULT).dll,DllRegistrationInfo $(TEMPLATEDIR)\Registry.idt
   rundll32.exe $(BVR20983CC_RESULT).dll,DllRegistrationInfo $(TEMPLATEDIR)\Registry.idt
   $(msicab) -msicab ..\..\inc\ver\versions.xml $(SLNDIR) $(MSIDIR)\$(BVR20983_CABMSI) $(TEMPLATEDIR)

validate: 
   $(msival) $(MSIDIR)\$(BVR20983_MSI) "c:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Bin\darice.cub" -f

clean:
  if exist $(MSIDIR)\$(BVR20983_CABMSI) del /q $(MSIDIR)\$(BVR20983_CABMSI)
  if exist $(MSIDIR)\$(BVR20983_MSI) del /q $(MSIDIR)\$(BVR20983_MSI)

!include <../../inc/Makefile.rule>
