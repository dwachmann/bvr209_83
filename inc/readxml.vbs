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
          WScript.Echo "[attrib ] {" & o.value & "}"

        Case else
          WScript.Echo "[" & TypeName(o) & "] {" & o.xml & "}"
      End Select
    Next
  End If
End Sub

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
  Dim result,selectQuery,node
  
  If TypeName(s)="String" Then
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
            result = GetValue(e,node.text)
  
          Case "IXMLDOMElement"
            result = GetValue(e,node.xml)
  
          Case "IXMLDOMAttribute"
            result = GetValue(e,node.value)
  
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
'  on error resume next

  Set xmlDoc = CreateObject("Msxml2.DOMDocument.5.0")
  
  xmlDoc.validateOnParse = True
  xmlDoc.async = False
  xmlDoc.setProperty "SelectionNamespaces", "xmlns:v='http://bvr20983.berlios.de'"
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
