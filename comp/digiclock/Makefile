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
$(OUTDIR)\os.obj \
$(OUTDIR)\digiclock.obj \
$(OUTDIR)\digiclock.res \
$(SLNDIR)\libbvr20983.lib

!ifdef clean
all: clean
!else
all: $(OUTDIR) $(SLNDIR) $(SLNDIR)\$(DIGICLOCK_RESULT).exe
!endif

$(OUTDIR)\$(DIGICLOCK_RESULT).exe: $(OBJS)
  $(link) $(ldebug) $(guilflags) $(lvars) -out:$@ -pdb:$(@R).pdb $(olelibsmt) comctl32.lib $(BVR20983CC_RESULT).lib $(OBJS)

$(SLNDIR)\$(DIGICLOCK_RESULT).exe: $(OUTDIR)\$(DIGICLOCK_RESULT).exe
  @copy $(OUTDIR)\$(DIGICLOCK_RESULT).exe $(SLNDIR)

clean:
  if exist $(OUTDIR)\$(NULL) rd /s /q $(OUTDIR)
  if exist $(SLNDIR)\$(DIGICLOCK_RESULT).exe del /q $(SLNDIR)\$(DIGICLOCK_RESULT).exe

!include <../../inc/Makefile.rule>
