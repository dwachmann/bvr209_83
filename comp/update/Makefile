# $Id$
# 
# Copyright (C) 2008-2009 Dorothea Wachmann
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

OBJS= \
$(OUTDIR)\os.obj \
$(OUTDIR)\bvr20983update.obj \
$(OUTDIR)\bvr20983update.res \
$(OUTDIR)\bitsprogressdlg.obj \
$(OUTDIR)\autoupdate.obj \
$(OUTDIR)\cabinetexception.obj \
$(OUTDIR)\cabinetfci.obj \
$(OUTDIR)\cabinetfdi.obj \
$(SLNDIR)\libbvr20983.lib

!ifdef clean
all: clean
!else
all: $(OUTDIR) $(RESULTDIR) $(SLNDIR) $(SLNDIR)\$(BVR20983UPDATE_RESULT).dll
!endif

$(OUTDIR)\bvr20983update.res: $(RESDIR)\bvr20983update.rc
  $(rc) $(rcflags) $(rcvars) /fo $@ $(RESDIR)\bvr20983update.rc

$(OUTDIR)\$(BVR20983UPDATE_RESULT).dll: $(OBJS)
  $(link) $(ldebug) $(dlllflags) $(lvars) -def:$(INCDIR)\bvr20983update.def -out:$@ -pdb:$(@R).pdb $(olelibsmt) bits.lib comctl32.lib wintrust.lib FCI.LIB FDI.LIB MSI.LIB $(OBJS)

$(SLNDIR)\$(BVR20983UPDATE_RESULT).dll: $(OUTDIR)\$(BVR20983UPDATE_RESULT).dll
  @copy $(OUTDIR)\$(BVR20983UPDATE_RESULT).dll $(SLNDIR)
  @copy $(OUTDIR)\$(BVR20983UPDATE_RESULT).lib $(SLNDIR)

clean:
  if exist $(OUTDIR)\$(NULL) rd /s /q $(OUTDIR)
  if exist $(SLNDIR)\$(BVR20983UPDATE_RESULT).dll del /q $(SLNDIR)\$(BVR20983UPDATE_RESULT).dll
  if exist $(SLNDIR)\$(BVR20983UPDATE_RESULT).lib del /q $(SLNDIR)\$(BVR20983UPDATE_RESULT).lib

!include <../../inc/Makefile.rule>
