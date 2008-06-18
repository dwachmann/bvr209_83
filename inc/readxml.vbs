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

Dim objArgs,fso
Dim reProdVer,reFileVer,reMakeDate
Dim prodRev,fileRev,fileDate
Dim xmlDoc

Const ForReading = 1

'
' Usage
'
Sub Usage()
  WScript.Echo "readxml <filename> <select criteria>"
End Sub

'
' GetInfo
'
Sub ListXml(f,selectCriteria)
  Dim attrValue
  Dim objNodeList,o
  
  xmlDoc.load(f)
  
  If xmlDoc.parseError.errorCode<>0 Then
    WScript.Echo xmlDoc.parseError.reason
  Else
    WScript.Echo xmlDoc.xml
  End If
  
  Set objNodeList = xmlDoc.documentElement.selectNodes(selectCriteria)
  
  If objNodeList.length>0 Then
    For Each o in objNodeList
      WScript.Echo "[" & TypeName(o) & "] " & o.xml
    Next
  End If
End Sub


'
' Init
'
Sub Init
'  on error resume next

  Set xmlDoc = CreateObject("Msxml2.DOMDocument.5.0")
  
  xmlDoc.validateOnParse = True
  xmlDoc.async = False
  xmlDoc.setProperty "SelectionNamespaces", "xmlns:xsl='http://www.w3.org/1999/XSL/Transform'"
  xmlDoc.setProperty "SelectionLanguage", "XPath"
End Sub

Set objArgs = WScript.Arguments

If objArgs.Count>=2 Then
  Set fso = CreateObject("Scripting.FileSystemObject")
  
  If fso.FileExists( objArgs(0) ) Then
    Init
    
    ListXml  objArgs(0),objArgs(1) 
  Else
    WScript.Echo "File " & objArgs(0) & " doesnt exist"
  End If
Else
  Usage
End If
'=======================================END-OF-FILE==========================
