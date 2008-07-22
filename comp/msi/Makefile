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

TEMPLATEDIR = template
RESULT      = bvr20983.Msi
msidb       = msidb
msival      = msival2

distribute: 
   $(msidb) -c -d $(RESULT) -f $(MAKEDIR)\$(TEMPLATEDIR) -i *
   $(msidb) -d $(RESULT) -a bvr20983.2.cab

validate: 
   $(msival) $(RESULT) "c:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Bin\darice.cub" -f
