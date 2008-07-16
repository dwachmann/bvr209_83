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

Const msiOpenDatabaseModeReadOnly = 0
Const msiOpenDatabaseModeTransact = 1

Dim installer : Set installer = Nothing
Dim database  : Set database = Nothing
Dim fso       : Set fso = Nothing

'
' Init
'
Sub Init
  On Error Resume Next
  
  Set installer = Wscript.CreateObject("WindowsInstaller.Installer") : CheckError
  Set fso       = Wscript.CreateObject("Scripting.FileSystemObject")
End Sub

'
' OpenDB
'
Sub OpenDB(databasePath,dbOpenMode)
  On Error Resume Next
  
  Set database  = installer.OpenDatabase(databasePath, dbOpenMode) : CheckError
End Sub

'
' QueryDB
'
Sub QueryDB(query)
  On Error Resume Next
  Dim view, record, record1, column

  Set view = database.OpenView(query) : CheckError
  view.Execute : CheckError
  
  Set record  = view.ColumnInfo(0)
  Set record1 = view.ColumnInfo(1)
  
  WScript.StdOut.Write "  "

  For column = 1 To record.FieldCount
    WScript.StdOut.Write record.StringData(column) & "[" & record1.StringData(column) & "]   "
  Next

  WScript.StdOut.WriteLine

  Do
    Set record = view.Fetch
    If record Is Nothing Then Exit Do
    
    For column = 1 To record.FieldCount
      WScript.StdOut.Write record.StringData(column) & "  "
    Next
    
    WScript.StdOut.WriteLine
  Loop
  
  WScript.StdOut.WriteLine
End Sub

'
' AddProperty
'
Sub AddProperty(args)
  On Error Resume Next
  Dim insertStmt,updateStmt, insertRec, i, propValue
  
  Set insertRec  = installer.CreateRecord(2)
  Set insertStmt = database.OpenView("INSERT INTO Property(Property,Value) VALUES(?,?)") : CheckError
  Set updateStmt = database.OpenView("UPDATE Property SET `Value`=? WHERE `Property`=?") : CheckError
  
  For i=0 to args.length-1 Step 2
    propValue = GetProperty(args(i))
    
    If IsNull(propValue) Then
      insertRec.StringData(1) = args(i)
      insertRec.StringData(2) = args(i+1)
      insertStmt.Execute insertRec : CheckError
    Else
      insertRec.StringData(2) = args(i)
      insertRec.StringData(1) = args(i+1)
      updateStmt.Execute insertRec : CheckError
    End If
  Next
End Sub

'
' AddProperty
'
Function GetProperty(propName)
  On Error Resume Next
  Dim queryStmt, queryParam, result
  
  Set queryParam = installer.CreateRecord(1)
  Set queryStmt  = database.OpenView("SELECT Value FROM Property WHERE Property=?") : CheckError
  
  queryParam.StringData(1) = propName
  
  queryStmt.Execute queryParam : CheckError
  
  Set result = queryStmt.Fetch : CheckError
  
  If result Is Nothing Then
    GetProperty = Null
  Else
    GetProperty = result.StringData(1)
  End If
End Function


'
' CheckError
'
Sub CheckError
  Dim message, errRec
  
  If Err = 0 Then Exit Sub
  
  message = Err.Source & " " & Hex(Err) & ": " & Err.Description
  If Not installer Is Nothing Then
    Set errRec = installer.LastErrorRecord
    If Not errRec Is Nothing Then message = message & vbLf & errRec.FormatText
  End If
  
  Fail message
End Sub

'
' Fail
'
Sub Fail(message)
  Wscript.Echo message
  Wscript.Quit 2
End Sub
