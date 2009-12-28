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
Dim argsKey,argsValue,vbTab

Const ForReading    = 1
Const ForWriting    = 2
Const ForAppending  = 8
Const tmpFileSuffix = ".tmp"

vbTab = chr(9)

'
' increment build attribute
'
Sub IncrementBuildCount(f)
  Dim objNodeList,selectCriteria,buildAttr,oldValue
  
  selectCriteria = "/v:versions/v:product[1]/v:versionhistory/v:version[1]"
  
  WScript.Echo "IncrementBuildCount(f=" & f & ",selectCriteria=" & selectCriteria & ")"
  
  xmlDoc.load(f)
  
  If xmlDoc.parseError.errorCode<>0 Then
    WScript.Echo xmlDoc.parseError.reason
  Else
    'WScript.Echo xmlDoc.xml

    Set objNodeList = xmlDoc.documentElement.selectNodes(selectCriteria)

    If TypeName(objNodeList(0))="IXMLDOMElement" Then
      Set buildAttr = objNodeList(0).GetAttributeNode("build")
      
      If TypeName(buildAttr)<>"Nothing" Then
        oldValue = buildAttr.value
        
        buildAttr.value = buildAttr.value + 1
        
        WScript.Echo "build: " & oldValue & "-->" & buildAttr.value
        
        xmlDoc.save(f)
      End If
    End If
  End If
End Sub

'
' create git tag for build no
'
Sub TagBuildNo(f)
  Dim objNodeList,selectCriteria,buildAttr,gitOutput
  
  selectCriteria = "/v:versions/v:product[1]/v:versionhistory/v:version[1]"
  
  xmlDoc.load(f)
  
  If xmlDoc.parseError.errorCode<>0 Then
    WScript.Echo xmlDoc.parseError.reason
  Else
    'WScript.Echo xmlDoc.xml

    Set objNodeList = xmlDoc.documentElement.selectNodes(selectCriteria)

    If TypeName(objNodeList(0))="IXMLDOMElement" Then
      Set buildAttr = objNodeList(0).GetAttributeNode("build")
      
      If TypeName(buildAttr)<>"Nothing" Then
        If ExecuteProgram("git tag -afm ""tagging build no " & buildAttr.value & """ build." & buildAttr.value,gitOutput,False,False)=0 Then
          WScript.Echo "git tag build."&buildAttr.value&" created."
        End If
      End If
    End If
  End If
End Sub

'
' create git tag for msi version
'
Sub TagMSIVersion(f)
  Dim objNodeList,selectCriteria,buildAttr,majorAttr,minorAttr,fixAttr,msiVersion,gitOutput
  
  selectCriteria = "/v:versions/v:product[1]/v:versionhistory/v:version[1]"
  
  xmlDoc.load(f)
  
  If xmlDoc.parseError.errorCode<>0 Then
    WScript.Echo xmlDoc.parseError.reason
  Else
    'WScript.Echo xmlDoc.xml

    Set objNodeList = xmlDoc.documentElement.selectNodes(selectCriteria)

    If TypeName(objNodeList(0))="IXMLDOMElement" Then
      Set majorAttr = objNodeList(0).GetAttributeNode("major")
      Set minorAttr = objNodeList(0).GetAttributeNode("minor")
      Set fixAttr   = objNodeList(0).GetAttributeNode("fix")
      Set buildAttr = objNodeList(0).GetAttributeNode("build")
      
      If TypeName(majorAttr)<>"Nothing" and TypeName(minorAttr)<>"Nothing" and TypeName(fixAttr)<>"Nothing" and TypeName(buildAttr)<>"Nothing" Then
        msiVersion = majorAttr.value & "." & minorAttr.value & "." & fixAttr.value & "." & buildAttr.value
        
        If ExecuteProgram("git tag -afm ""tagging MSI version " & buildAttr.value & """ msi." & msiVersion,gitOutput,False,False)=0 Then
          WScript.Echo "git tag msi." & msiVersion & " created."
        End If
      End If
    End If
  End If
End Sub

'
' evaluate the patch element and patch file according to the defined regexp pattern
'
Sub PatchIt(f,selectCriteria)
  Dim objNodeList
  
  WScript.Echo "PatchIt(f=" & f & ",selectCriteria=" & selectCriteria & ")"
  
  xmlDoc.load(f)
  
  If xmlDoc.parseError.errorCode<>0 Then
    WScript.Echo xmlDoc.parseError.reason
  Else
    'WScript.Echo xmlDoc.xml

    Set objNodeList = xmlDoc.documentElement.selectNodes(selectCriteria)
  
    ExecutePatchPattern objNodeList
  End If
End Sub


'
' execute patch pattern on the selected file
'
Sub ExecutePatchPattern(objNodeList)
  Dim attrValue
  Dim o,objPatternList,o1
  Dim key,val
  Dim key1,val1
  Dim re(),va(),reI,tmpl,filename

  WScript.Echo "ExecutePatchPattern() objNodeList.length=" & objNodeList.length
  
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
  Dim i,lineno

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
  
  lineno = 0
  
  Do While Not f0.AtEndOfStream 
    r0 = f0.ReadLine
    
    For i=LBound(re) to UBound(re)-1
      r0 = re(i).Replace(r0,va(i))
    Next
    
    f1.WriteLine(r0)
    
    lineno = lineno + 1
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
      For i=LBound(argsKey) to UBound(argsKey)
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
  Set xmlDoc = CreateObject("Msxml2.DOMDocument.6.0")
  
  xmlDoc.resolveExternals = True
  xmlDoc.validateOnParse = True
  xmlDoc.async = False
  xmlDoc.setProperty "SelectionNamespaces", "xmlns:v='urn:bvr20983'"
  xmlDoc.setProperty "SelectionLanguage", "XPath"

  Set fso = CreateObject("Scripting.FileSystemObject")
  
  projectPath = Left(WScript.ScriptFullName,InStrRev(WScript.ScriptFullName,"\")-1)
  projectPath = Left(projectPath,InStrRev(projectPath,"\"))
  
  Redim searchPath(7)
  
  searchPath(0) = "inc\"
  searchPath(1) = "inc\ver\"
  searchPath(2) = "inc\com\"
  searchPath(3) = "res\"
  searchPath(4) = "res\html\"
  searchPath(5) = "comp\msi\template\"
  searchPath(6) = "comp\msi\patch\"
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
' starts external program and returns stdout
'
Function ExecuteProgram(cmd,ByRef cmdOut,printInfo,quitOnError)
  Dim WshShell, oExec

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
  
  If printInfo Then
    WScript.Echo cmd
    WScript.Echo oExec.ExitCode & ":" & cmdOut
  End If
  
  If quitOnError and oExec.ExitCode<>0 Then
    WScript.Quit(oExec.ExitCode)
  End If
  
  ExecuteProgram = oExec.ExitCode
End Function

'
' GetEnvironment
'
Function GetEnvironment(name)
  Dim WshShell, WshSysEnv

  Set WshShell  = CreateObject("WScript.Shell")
  Set WshSysEnv = WshShell.Environment("PROCESS")
  
  If WshSysEnv(name)<>"" Then
    GetEnvironment = WshSysEnv(name)
  Else
    GetEnvironment = null
  End If
End Function

'
'
' 
Function GetActRevision()
  Dim fullFileName,f0
  
  fullFileName   = ".\actrevision"
  GetActRevision = null
  
  If fso.FileExists(fullFileName) Then
    Set f0 = fso.OpenTextFile(fullFileName, ForReading)

    If Not f0.AtEndOfStream Then
      GetActRevision = f0.ReadLine
    End If
    
    f0.Close
  End If
End Function


'
' GetSvnInfo
'
Function GetSvnInfo(f)
  Dim svnOutput
  
  WScript.Echo "Retrieving subversion info..."
  
  If ExecuteProgram("svn info --xml -r HEAD " & f,svnOutput,False,False)=0 Then
    xmlDoc.loadXml(svnOutput)
    
    Set GetSvnInfo = xmlDoc.documentElement
  Else
    Set GetSvnInfo = Nothing
  End If
End Function

'
' GetGitLastCommit
'
Function GetGitLastCommit()
  Dim gitOutput
  
  GetGitLastCommit = Null
  
  WScript.Echo "Retrieving git info..."
  
  If ExecuteProgram("git --no-pager log -1 --pretty=format:%H",gitOutput,False,False)=0 Then
    GetGitLastCommit = gitOutput
  End If
End Function

'
' Get subversion revision of commit date of file f
'
Sub GetRevisionAndDate(f)
  Dim doc,d,rev
  
  If GetEnvironment("GIT_SSH")<>"" Then
    rev = GetActRevision
  Else
    Set doc = GetSvnInfo(WScript.Arguments.Named.Item("File"))
    
    If not doc is Nothing Then
      rev = doc.selectSingleNode("/info/entry/@revision").nodeValue
      'd   = doc.selectSingleNode("/info/entry/wc-info/text-updated/text()").text
    Else
      rev = -1
    End If
  End If
  
  d = FormatDateTime(Date, 2) & " " & FormatDateTime(Now,4)
  
  WScript.Echo "rev="       & rev
  WScript.Echo "builddate=" & d
  
  argsKey   = Array("revision","builddate","commit")
  argsValue = Array(rev+1,d,"df7cf0120dd2d222aaa157cc5bdef35630ed2a6f")
End Sub

'
' set external variables that are used for the patch process
'
Sub GetExternalPatchValues()
  Dim d,commit,signpubkeytok,msvcrtlib_name,msvcrtlib_ver,msvcrtlib_pubtok,debugver
  
  signpubkeytok    = "93425facf1ef717a"
  debugver         = "1"
  commit           = GetGitLastCommit()
  d                = FormatDateTime(Date, 2) & " " & FormatDateTime(Now,4)
  msvcrtlib_name   = "Microsoft.VC80.DebugCRT"
  msvcrtlib_ver    = "8.0.50727.762"
  msvcrtlib_pubtok = "1fc8b3b9a1e18e3b"

  If GetEnvironment("NODEBUG")<>"" Then  
    msvcrtlib_name   = "Microsoft.VC80.CRT"
    debugver         = "0"
  End If  

  If GetEnvironment("MSVCRTLIB90")<>"" Then  
    msvcrtlib_name   = "Microsoft.VC90.DebugCRT"
    
    If GetEnvironment("NODEBUG")<>"" Then  
      msvcrtlib_name   = "Microsoft.VC90.CRT"
    End If  
    
    msvcrtlib_ver    = "9.0.21022.8"
    msvcrtlib_pubtok = "1fc8b3b9a1e18e3b"
  End If
  
  WScript.Echo "signpubkeytok    = <" & signpubkeytok & ">"
  WScript.Echo "msvcrtlib_name   = <" & msvcrtlib_name & ">"
  WScript.Echo "msvcrtlib_ver    = <" & msvcrtlib_ver & ">"
  WScript.Echo "msvcrtlib_pubtok = <" & msvcrtlib_pubtok & ">"
  WScript.Echo "debugver         = <" & debugver & ">"
  WScript.Echo "commit           = <" & commit & ">"
  WScript.Echo "builddate        = <" & d & ">"
  
  argsKey   = Array("signpubkeytok","msvcrtlib_name","msvcrtlib_ver","msvcrtlib_pubtok","debugver","builddate","commit")
  argsValue = Array(signpubkeytok,msvcrtlib_name,msvcrtlib_ver,msvcrtlib_pubtok,debugver,d,commit)
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
