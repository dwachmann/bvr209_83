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

$(DISTDIR):
  if not exist "$(DISTDIR)/$(NULL)" mkdir $(DISTDIR)

$(SIGNDIR):
  if not exist "$(SIGNDIR)/$(NULL)" mkdir $(SIGNDIR)

clean:
  if exist $(SLNDIR)/$(NULL) rd /s /q $(SLNDIR)
  if exist $(DISTDIR)/$(NULL) rd /s /q $(DISTDIR)
  if exist $(SIGNDIR)/$(NULL) rd /s /q $(SIGNDIR)

{$(SLNDIR)}.dll{$(SIGNDIR)}.dll:
  @if not exist "$(signkey)" echo NO signkey GIVEN
  @if "$(signpwd)"==""  echo NO signpwd GIVEN
	@copy $< $@
	@$(sign) $(signvars) /p $(signpwd) $@

{$(SLNDIR)}.exe{$(SIGNDIR)}.exe:
  @if not exist "$(signkey)" echo NO signkey GIVEN
  @if "$(signpwd)"==""  echo NO signpwd GIVEN
	@copy $< $@
	@$(sign) $(signvars) /p $(signpwd) $@

.SUFFIXES: .dll .exe

$(PROJECTS):
!include <./inc/build.inc>
