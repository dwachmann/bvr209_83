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

OBJS= \
$(OUTDIR)\bvr20983res.res

!ifdef clean
all: clean
!else
all: $(OUTDIR) $(RESULTDIR) $(SLNDIR) $(SLNDIR)\$(BVR20983RES_RESULT).dll
!endif

$(OUTDIR)\bvr20983res.res: $(RESDIR)\bvr20983res.rc
  $(rc) $(rcflags) $(rcvars) /fo $@ $(RESDIR)\bvr20983res.rc

$(OUTDIR)\$(BVR20983RES_RESULT).dll: $(OBJS) $(INCDIR)\bvr20983res.def
  $(link) -noentry -dll -machine:X86 -def:$(INCDIR)\bvr20983res.def -out:$@ $(OBJS)

$(SLNDIR)\$(BVR20983RES_RESULT).dll $(SLNDIR)\$(BVR20983RES_RESULT).lib: $(OUTDIR)\$(BVR20983RES_RESULT).dll
  @copy $(OUTDIR)\$(BVR20983RES_RESULT).dll $(SLNDIR)
  @copy $(OUTDIR)\$(BVR20983RES_RESULT).lib $(SLNDIR)

clean:
  if exist $(OUTDIR)\$(NULL) rd /s /q $(OUTDIR)
  if exist $(SLNDIR)\$(BVR20983RES_RESULT).dll del /q $(SLNDIR)\$(BVR20983RES_RESULT).dll
  if exist $(SLNDIR)\$(BVR20983RES_RESULT).lib del /q $(SLNDIR)\$(BVR20983RES_RESULT).lib

!include <../../inc/Makefile.rule>
