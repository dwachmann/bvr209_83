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

!include <../inc/Makefile.inc>

SRCS= \
$(SRCDIR)\sc\digiclock.cpp

OBJS= \
$(OUTDIR)\os.obj \
$(OUTDIR)\digiclock.obj \
$(OUTDIR)\digiclock.res

all: $(OUTDIR) $(SLNDIR) $(SLNDIR)\digiclock.exe

$(OUTDIR)\digiclock.res: $(RESDIR)\digiclock.rc
    $(rc) $(rcflags) $(rcvars) /fo $@ $**

$(OUTDIR)\digiclock.exe: $(OBJS)
  $(link) $(ldebug) $(guilflags) $(lvars) -out:$@ -pdb:$(@R).pdb $(olelibsmt) bvr20983cc.lib libbvr20983.lib $**

$(SLNDIR)\digiclock.exe: $(OUTDIR)\digiclock.exe
  @copy $(OUTDIR)\digiclock.exe $(SLNDIR)

!include <../inc/Makefile.rule>
