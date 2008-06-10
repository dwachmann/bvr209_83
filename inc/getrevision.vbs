' $Id$
' 
' Smartcard Test Script.
' 
' Copyright (C) 2008 Dorothea Wachmann
' 
' This program is free software: you can redistribute it and/or modify
' it under the terms of the GNU General Public License as published by
' the Free Software Foundation, either version 3 of the License, or
' (at your option) any later version.
' 
' This program is distributed in the hope that it will be useful,
' but WITHOUT ANY WARRANTY; without even the implied warranty of
' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
' GNU General Public License for more details.
' 
' You should have received a copy of the GNU General Public License
' along with this program.  If not, see http://www.gnu.org/licenses/.
option explicit

Dim re,re1,objArgs,fso,f,r,Matches,Match

Const ForReading = 1

'
' Init
'
Sub Init()
'  on error resume next

  Set re  = New RegExp
  Set re1 = New RegExp
  
  re.Pattern = "verFileVer1"
  re.Global = True

  re1.Pattern = "[0-9]\.[0-9]\.[0-9]\.[0-9]"
  re1.Global  = True
End Sub


Init

Set objArgs = WScript.Arguments

If objArgs.Count>0 Then
  Set fso = CreateObject("Scripting.FileSystemObject")
  
  If fso.FileExists( objArgs(0) ) Then
    Set f = fso.OpenTextFile(objArgs(0), ForReading)
    
    Do While Not f.AtEndOfStream 
      r = f.ReadLine
      
      If re.Test(r) Then
        Set Matches = re1.Execute(r)

        For Each Match in Matches      
          WScript.Echo Match.Value
          
          WScript.Quit(0)
        Next
      End If
    Loop

    f.Close
  End If
End If
'=======================================END-OF-FILE==========================
