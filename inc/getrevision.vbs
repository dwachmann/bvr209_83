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
  WScript.Echo "getrevision <filename> <prodrev> <filerev> <filedate>"
End Sub

'
' GetInfo
'
Function GetInfo(cmd)
  Dim WshShell, oExec,cmdOut

  Set WshShell = CreateObject("WScript.Shell")
  Set oExec    = WshShell.Exec(cmd)

  cmdOut = ""
  
  Do While True
    If Not oExec.StdOut.AtEndOfStream Then
      cmdOut = cmdOut & oExec.StdOut.ReadAll
    Else
      Exit Do
    End If

    WScript.Sleep 10
  Loop
  
  Do While oExec.Status = 0
    WScript.Sleep 100
  Loop
  
  'WScript.Echo "cmdOut=" & cmdOut
  
  GetInfo = cmdOut
End Function

'
' GetInfo
'
Function GetXmlInfo(f,selectCriteria)
  Dim attrValue
  Dim objNodeList
  
  xmlDoc.loadXml(GetInfo("svn info --xml " & f))
  
  Set objNodeList = xmlDoc.documentElement.selectNodes(selectCriteria)
  
  If objNodeList.length>0 Then
    'WScript.Echo "type=" & TypeName(objNodeList.Item(0))
    
    attrValue = objNodeList.Item(0).nodeValue
  End If

  WScript.Echo "attrValue=" & attrValue
  
  GetXmlInfo = attrValue
End Function


'
' Init
'
Sub Init(f0,f1,f2)
'  on error resume next

  Set xmlDoc = CreateObject("Msxml2.DOMDocument.5.0")
  
  xmlDoc.validateOnParse = False
  xmlDoc.async = False
  xmlDoc.setProperty "SelectionNamespaces", "xmlns:xsl='http://www.w3.org/1999/XSL/Transform'"
  xmlDoc.setProperty "SelectionLanguage", "XPath"

  Set reProdVer       = New RegExp
  Set reFileVer       = New RegExp
  Set reMakeDate      = New RegExp
  
  reProdVer.Pattern   = "(^#define.+verProdVer.+" & "[0-9]+[\.,][0-9]+[\.,][0-9]+[\.,])([0-9]+)(.*)"
  reProdVer.Global    = True

  reFileVer.Pattern   = "(^#define.+verFileVer.+" & "[0-9]+[\.,][0-9]+[\.,][0-9]+[\.,])([0-9]+)(.*)"
  reFileVer.Global    = True

  reMakeDate.Pattern  = "(^#define.+verMakeDate.+"")(.+)("")"
  reMakeDate.Global   = True

  prodRev             = GetXmlInfo(f0,"/info/entry/commit/@revision")
  fileRev             = GetXmlInfo(f1,"/info/entry/@revision")
  fileDate            = GetXmlInfo(f2,"/info/entry/commit/date/text()")
End Sub

'
' Patch the file f0 using the regular expressions
'
Sub PatchFile(f)
  Dim f0,f1,r0

  Set f0 = fso.OpenTextFile(objArgs(0), ForReading)
  Set f1 = fso.CreateTextFile(objArgs(0)&".new",True)
  
  Do While Not f0.AtEndOfStream 
    r0 = f0.ReadLine
    r0 = reProdVer.Replace(r0,"$1" & prodRev & "$3")
    r0 = reFileVer.Replace(r0,"$1" & fileRev & "$3")
    r0 = reMakeDate.Replace(r0,"$1" & fileDate & "$3")
    
    f1.WriteLine(r0)
  Loop
  
  f0.Close
  f1.Close
End Sub

'
' Move File
'
Sub MoveFile(f)
  Dim f0

  Set f0 = fso.GetFile(f)
  f0.Delete(True)
  
  Set f0 = fso.GetFile(f & ".new")
  f0.Move(f)
End Sub

Set objArgs = WScript.Arguments

If objArgs.Count>=4 Then
  Set fso = CreateObject("Scripting.FileSystemObject")
  
  If fso.FileExists( objArgs(0) ) Then
    Init objArgs(1),objArgs(2),objArgs(3)
    
    PatchFile objArgs(0)
    
    MoveFile objArgs(0)
  End If
Else
  Usage
End If
'=======================================END-OF-FILE==========================
