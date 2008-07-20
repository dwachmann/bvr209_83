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

ansi=1

!include <../inc/Makefile.inc>

OBJS= \
$(OUTDIR)\os.obj \
$(OUTDIR)\msicab.obj \
$(OUTDIR)\msicab.res \
$(SLNDIR)\libbvr20983a.lib

!ifdef clean
all: clean
!else
all: $(OUTDIR) $(SLNDIR) $(SLNDIR)\$(MSICAB_RESULT).exe
!endif

$(OUTDIR)\$(MSICAB_RESULT).exe: $(OBJS)
  $(link) $(ldebug) $(conlflags) $(lvars) -out:$@ -pdb:$(@R).pdb $(olelibsmt) FCI.LIB $(OBJS)

$(SLNDIR)\$(MSICAB_RESULT).exe: $(OUTDIR)\$(MSICAB_RESULT).exe
  @copy $(OUTDIR)\$(MSICAB_RESULT).exe $(SLNDIR)

clean:
  if exist $(OUTDIR)\$(NULL) rd /s /q $(OUTDIR)
  if exist $(SLNDIR)\$(MSICAB_RESULT).exe del /q $(SLNDIR)\$(MSICAB_RESULT).exe

!include <../inc/Makefile.rule>
