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

Dim smartcard , app, regCards, readers, lcid

'
' Init
'
Sub Init()
'  on error resume next

  Set smartcard = WScript.CreateObject("BVR20983.Smartcard")

End Sub

Sub Smartcard_OnInserted
  WScript.Echo "inserted"

  WScript.Echo "CardID=" & smartcard.ATR
End Sub

Sub Smartcard_OnRemoved
  WScript.Echo "removed"
End Sub

Sub ShowStatus
  If smartcard.CardPresent  Then
    WScript.Echo "smartcard.CardPresent=true"
  Else
    WScript.Echo "smartcard.CardPresent=false"
  End If

End Sub

'
' starts here
'
WScript.Echo "Starting Test of BVR20983.Smartcard..."

Init

ShowStatus

WScript.ConnectObject smartcard, "Smartcard_"

smartcard.WaitForInsertEvent( 30000 )
WScript.Echo "history=" & smartcard.ATR.history

For Each app in smartcard.applications
  If IsObject(app) Then
    WScript.Echo "app=" & VarType(app) & " " & TypeName(app)

    WScript.Echo "app.Balance=" & app.Balance
  End If
Next 

For Each regCards in smartcard.registeredCards
  WScript.Echo "regCards=" & regCards
Next 

For Each readers in smartcard.readers
  WScript.Echo "readers=" & readers
Next 

WScript.Sleep 10000

WScript.DisconnectObject smartcard

WScript.Echo "Test of BVR20983.Smartcard  stopped."
'=======================================END-OF-FILE==========================