' $Id$
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
Option explicit
Dim fso,xmlDoc
Dim projectPath,searchPath()
Dim argsKey,argsValue

Const ForReading    = 1
Const tmpFileSuffix = ".tmp"

'
' evaluate the patch element and patch file according to the defined regexp pattern
'
Sub PatchIt(f,selectCriteria)
  Dim attrValue
  Dim objNodeList,o,objPatternList,o1
  Dim key,val
  Dim key1,val1
  Dim re(),va(),reI,tmpl,filename
  
  xmlDoc.load(f)
  
  If xmlDoc.parseError.errorCode<>0 Then
    WScript.Echo xmlDoc.parseError.reason
  Else
    'WScript.Echo xmlDoc.xml
  End If
  
  Set objNodeList = xmlDoc.documentElement.selectNodes(selectCriteria & ".//v:patch")
  
  If objNodeList.length>0 Then
    For Each o in objNodeList
    
      If TypeName(o)="IXMLDOMElement" and TypeName(o.GetAttributeNode("filename"))<>"Nothing" Then
        Set objPatternList = o.selectNodes(".//v:pattern")
        
        Redim re(objPatternList.length)
        Redim va(objPatternList.length)
        
        reI = 0
        
        For Each o1 in objPatternList
          Set key = o1.firstChild.firstChild
          Set val = o1.firstChild.nextSibling.firstChild
          
          key1 = GetValue(key,key.text)
          val1 = GetValue(val,val.text)
          
          Set re(reI) = New RegExp
          
          va(reI) = val1
          
          re(reI).Pattern   = key1
          re(reI).Global    = True
          
          reI = reI + 1
        Next
        
        Set tmpl     = o.GetAttributeNode("template")
        Set filename = o.GetAttributeNode("filename")
        
        If TypeName(tmpl)<>"Nothing" Then
          PatchFile tmpl.value,filename.value,re,va
        Else
          PatchFile filename.value,filename.value & tmpFileSuffix,re,va
          
          MoveFile filename.value
        End If
      End If
    Next
  End If
End Sub

'
' Patch the file f0 using the regular expressions
'
Sub PatchFile(tmpl,f,re,va)
  Dim f0,f1,r0
  Dim fullTmplPath,fullPath
  Dim i

  fullTmplPath = FindFile(tmpl)
  If IsNull(fullTmplPath) Then Exit Sub End If
  
  fullPath = Left(fullTmplPath,InStrRev(fullTmplPath,"\")) & f
  If IsNull(fullPath) Then Exit Sub End If

  WScript.Echo "Patching file " & tmpl & " --> " & f

  For i=LBound(re) to UBound(re)-1
    WScript.Echo "  |" & re(i).Pattern & "| --> |" & va(i) & "|"
  Next

  Set f0 = fso.OpenTextFile(fullTmplPath, ForReading)
  Set f1 = fso.CreateTextFile(fullPath,True)
  
  Do While Not f0.AtEndOfStream 
    r0 = f0.ReadLine
    
    For i=LBound(re) to UBound(re)-1
      r0 = re(i).Replace(r0,va(i))
    Next
    
    f1.WriteLine(r0)
  Loop
  
  f0.Close
  f1.Close
End Sub

'
' ReadXml selects a xml NODE using a XPATH selection criteria
'
Sub ReadXml(f,selectCriteria)
  Dim attrValue
  Dim objNodeList,o
  
  xmlDoc.load(f)
  
  If xmlDoc.parseError.errorCode<>0 Then
    WScript.Echo xmlDoc.parseError.reason
  Else
    'WScript.Echo xmlDoc.xml
  End If
  
  Set objNodeList = xmlDoc.documentElement.selectNodes(selectCriteria)
  
  If objNodeList.length>0 Then
    For Each o in objNodeList
      Select Case TypeName(o)
        Case "IXMLDOMText"
          WScript.Echo "[text   ] {" & GetValue(o,o.text) & "}"

        Case "IXMLDOMElement"
          WScript.Echo "[element] {" & o.xml & "}"

        Case "IXMLDOMAttribute"
          WScript.Echo "[attrib ] {" & GetValue(o,o.value) & "}"

        Case else
          WScript.Echo "[" & TypeName(o) & "] {" & o.xml & "}"
      End Select
    Next
  End If
End Sub

'
' implements value substitution 
' e.g. "abc ${name} ${name1}"
'
Function GetValue(e,s)
  Dim s0,i,i0,i1,i2,v,evalExp

  'WScript.Echo "GetValue(" & s & ")"
  
  evalExp = False
  
  If TypeName(s)="String" Then
    s0 = ""
    i0 = 0
    i1 = 0
    i2 = 0
    
    For i=1 to Len(s)
      If Mid(s,i,2)="${" or Mid(s,i,2)="#{" Then
        
        If Mid(s,i,2)="#{" Then
          evalExp = True
        End If
        
        If i0=0 Then
          i0 = i+2
        End If
        
        i2 = i2+1
        i  = i+1
      ElseIf Mid(s,i,1)="}" Then
        i2 = i2-1

        If i2<=0 and i0>0 Then
          i1 = i-1
          
          v = Mid(s,i0,i1-i0+1)
          
          If InStr(v,"${")>0 or Mid(s,i,2)="#{" Then
            v = GetValue(e,v)
          End If

          s0 = s0 & GetVarValue(e,v,evalExp)
          
          i0 = 0
          i1 = 0
          
          evalExp = False
        End If
      ElseIf i2>0 Then
      Else
        s0 = s0 & Mid(s,i,1)
      End If
    Next

    GetValue = s0
  else
    GetValue = s
  End If
End Function

'
' evaluate variable value
' first using parameter elements in xml document
' then, if not successful, use s as XPath expression
'
Function GetVarValue(e,s,evalExp)
  on error resume next
  Dim result,selectQuery,node,i
  
  If TypeName(s)="String" Then
    
    For i=0 to WScript.Arguments.Unnamed.length-1
      If WScript.Arguments.Unnamed(i)=s and i+1<=WScript.Arguments.Unnamed.length-1 Then
        GetVarValue = WScript.Arguments.Unnamed(i+1)
        Exit Function
      End If
    Next
    
    If IsArray(argsKey) Then
      For i=LBound(argsKey) to UBound(argsKey)-1
        If argsKey(i)=s Then
          GetVarValue = argsValue(i)
          Exit Function
        End If
      Next
    End If
    
    selectQuery = "/v:versions/v:parameter[@name='" & s &"']/text()"

    Set node = e.selectSingleNode(selectQuery)
    
    If Not IsNull(node) and TypeName(node)="IXMLDOMText" Then
      result = GetValue(e,node.text)
    Else
      selectQuery = s
      
      'WScript.Echo "selectQuery=" & selectQuery
      
      Set node = e.selectSingleNode(selectQuery)
      
      If Not IsNull(node) and TypeName(node)<>"Nothing" Then
        Select Case TypeName(node)
          Case "IXMLDOMText"
            result = GetValue(node,node.text)
  
          Case "IXMLDOMElement"
            result = GetValue(node,node.xml)
  
          Case "IXMLDOMAttribute"
            result = GetValue(node,node.value)
  
          Case else
            WScript.Echo "Result of selectSingleNode(): " & TypeName(node)
        End Select
      Else
        result = selectQuery
      End If
    End If
  End If
  
  If evalExp Then
    'WScript.Echo "Eval(" & result & ")"

    result = Eval(result)
  End If

  GetVarValue = result
End Function

'
' Padding a string with leading zeros
'
Function ZeroFill(s,l)
  Dim result,i

  result = s
  
  If Not IsNull(s) Then
    For i=Len(s)+1 to l
      result = "0" & result
    Next
    
    'WScript.Echo "ZeroFill(" & s & "," & l & "):" & result
  End If
    
  ZeroFill = result
End Function

'
' Init
'
Sub Init
  Dim f
  Set xmlDoc = CreateObject("Msxml2.DOMDocument.5.0")
  
  xmlDoc.validateOnParse = True
  xmlDoc.async = False
  xmlDoc.setProperty "SelectionNamespaces", "xmlns:v='http://bvr20983.berlios.de'"
  xmlDoc.setProperty "SelectionLanguage", "XPath"

  Set fso = CreateObject("Scripting.FileSystemObject")
  
  projectPath = Left(WScript.ScriptFullName,InStrRev(WScript.ScriptFullName,"\")-1)
  projectPath = Left(projectPath,InStrRev(projectPath,"\"))
  
  Redim searchPath(2)
  
  searchPath(0) = "inc\"
  searchPath(1) = "inc\ver\"
  searchPath(1) = "inc\com\"
  searchPath(1) = "html\"
End Sub

'
'
' 
Function FindFile(filename)
  Dim fullFileName,s,result
  
  result = null
  
  For Each s In searchPath
    fullFileName = projectPath & s & filename
    
    'WScript.Echo fullFileName
      
    If fso.FileExists(fullFileName) Then
      result = fullFileName
      
      Exit For
    End If
  Next
  
  FindFile = result
End Function

'
' GetInfo starts external program and returns stdout
'
Function ExecuteProgram(cmd)
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
  
  ExecuteProgram = cmdOut
End Function

'
' GetSvnInfo
'
Function GetSvnInfo(f)
  xmlDoc.loadXml(ExecuteProgram("svn info --xml " & f))
  
  Set GetSvnInfo = xmlDoc.documentElement
End Function


'
' Get subversion revision of commit date of file f
'
Sub GetRevisionAndDate(f)
  Dim doc,d,rev

  Set doc = GetSvnInfo(WScript.Arguments.Named.Item("File"))
  
  rev = doc.selectSingleNode("/info/entry/commit/@revision").nodeValue
  d   = doc.selectSingleNode("/info/entry/commit/date/text()").text
  
  argsKey   = Array("rev","builddate")
  argsValue = Array(rev,d)
End Sub

'
' Move File
'
Sub MoveFile(f)
  Dim f0,filename
  
  filename = FindFile(f)
  
  WScript.Echo "MoveFile(f="&filename&")"

  Set f0 = fso.GetFile(filename)
  f0.Delete(True)
  
  Set f0 = fso.GetFile(filename & tmpFileSuffix)
  f0.Move(filename)
End Sub
'=======================================END-OF-FILE==========================
