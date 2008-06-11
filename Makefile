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

!include <./inc/Makefile.inc>

SLNDIR  =.\debug
DISTDIR =.\dist
SIGNDIR =.\sign
INCDIR  =.\inc
HTMLDIR =.\html
signvars=sign /f $(signkey) /d "BVR20983 Security Suite" /du "https://www.jondos.de/" /t http://timestamp.verisign.com/scripts/timstamp.dll 

PROJECTS =    \
libbvr20983\~ \
bvr20983cc\~  \
bvr20983\~    \
digiclock\~   \
lsstg\~       \
lstypeinfo\~  \

!ifdef clean
all: $(PROJECTS) clean
!else
all: $(PROJECTS)
!endif

distribute: $(SIGNDIR) $(DISTDIR) $(SIGNDIR)\digiclock.exe $(SIGNDIR)\bvr20983.cab
  @copy $(HTMLDIR)\led.*        $(DISTDIR)
  @copy $(HTMLDIR)\*.jpg        $(DISTDIR)
  @copy $(SIGNDIR)\bvr20983.cab $(DISTDIR)

$(SIGNDIR)\bvr20983.cab: $(SIGNDIR)\bvr20983.dll $(SIGNDIR)\bvr20983cc.dll $(INCDIR)\bvr20983.inf
	@$(cab) -s 6144 N $@ $?
	@$(sign) $(signvars) /p $(signpwd) $@

!include <./inc/bvr.inc>
