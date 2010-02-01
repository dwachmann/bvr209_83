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
Const msiOpenDatabasePatchFile    = 32

Const msiInstallStateNotUsed      = -7
Const msiInstallStateBadConfig    = -6
Const msiInstallStateIncomplete   = -5
Const msiInstallStateSourceAbsent = -4
Const msiInstallStateInvalidArg   = -2
Const msiInstallStateUnknown      = -1
Const msiInstallStateBroken       =  0
Const msiInstallStateAdvertised   =  1
Const msiInstallStateRemoved      =  1
Const msiInstallStateAbsent       =  2
Const msiInstallStateLocal        =  3
Const msiInstallStateSource       =  4
Const msiInstallStateDefault      =  5


Dim installer : Set installer = Nothing
Dim database  : Set database = Nothing

'
' InitMsi
'
Sub InitMsi
  On Error Resume Next
  
  Set installer = Wscript.CreateObject("WindowsInstaller.Installer") : CheckError
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
' GetLastSequenceNo retrieves this information from the media table
'
Function GetLastSequenceNo()
  On Error Resume Next
  Dim view, record,lastSeqNo,seqNo
  
  lastSeqNo = -1

  Set view = database.OpenView("select LastSequence from Media") : CheckError
  view.Execute : CheckError

  Do
    Set record = view.Fetch
    If record Is Nothing Then Exit Do
    
    seqNo = record.IntegerData(1)
    
    If seqNo>lastSeqNo Then
      lastSeqNo = seqNo
    End If
  Loop
  
  GetLastSequenceNo = lastSeqNo
End Function

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
' List Product
'
Sub ListProduct(productName)
  Dim product, products, info, productList, version
  Dim productCode, property, value, message
  Dim feature, features, parent, state, featureInfo

  Set products = installer.Products : CheckError

  For Each product In products
    version = DecodeVersion(installer.ProductInfo(product, "Version")) : CheckError

    If productName="*" or InStr(UCase(installer.ProductInfo(product, "ProductName")),UCase(productName))>0 Then
      Wscript.Echo product & " = " & installer.ProductInfo(product, "ProductName") & "[" & version & "]" : CheckError
    End If
  Next

  ' If Product name supplied, need to search for product code
  If Left(productName, 1) = "{" And Right(productName, 1) = "}" Then
    If installer.ProductState(productName) <> msiInstallStateUnknown Then 
      productCode = UCase(productName)
    End If
  Else
    For Each productCode In installer.Products : CheckError
      If LCase(installer.ProductInfo(productCode, "ProductName")) = LCase(productName) Then Exit For
    Next
  End If

  If IsEmpty(productCode) Then 
    Wscript.Echo "Product is not registered: " & productName
  Else

    Wscript.Echo "  " & "ProductCode = " & productCode

    For Each property In Array(_
                              "Language",_
                              "ProductName",_
                              "PackageCode",_
                              "Transforms",_
                              "AssignmentType",_
                              "PackageName",_
                              "InstalledProductName",_
                              "VersionString",_
                              "RegCompany",_
                              "RegOwner",_
                              "ProductID",_
                              "ProductIcon",_
                              "InstallLocation",_
                              "InstallSource",_
                              "InstallDate",_
                              "Publisher",_
                              "LocalPackage",_
                              "HelpLink",_
                              "HelpTelephone",_
                              "URLInfoAbout",_
                              "URLUpdateInfo") : CheckError

      value = installer.ProductInfo(productCode, property) : CheckError
      If Err <> 0 Then Err.Clear : value = Empty

      If (property = "Version") Then value = DecodeVersion(value)

      If value <> Empty Then Wscript.Echo "  " & property & " = " & value
    Next

    Set features = installer.Features(productCode)
    
    Wscript.Echo 
    Wscript.Echo "---Features in product " & productCode & "---"

    For Each feature In features
      parent = installer.FeatureParent(productCode, feature) : CheckError

      If Len(parent) Then parent = " {" & parent & "}"

      state = installer.FeatureState(productCode, feature)

      Select Case(state)
        Case msiInstallStateBadConfig:    state = "Corrupt"
        Case msiInstallStateIncomplete:   state = "InProgress"
        Case msiInstallStateSourceAbsent: state = "SourceAbsent"
        Case msiInstallStateBroken:       state = "Broken"
        Case msiInstallStateAdvertised:   state = "Advertised"
        Case msiInstallStateAbsent:       state = "Uninstalled"
        Case msiInstallStateLocal:        state = "Local"
        Case msiInstallStateSource:       state = "Source"
        Case msiInstallStateDefault:      state = "Default"
        Case Else:                        state = "Unknown"
      End Select
      Wscript.Echo "  " & feature & parent & " = " & state
    Next

    Dim component, components, client, clients, path
    Set components = installer.Components : CheckError

    Wscript.Echo 
    Wscript.Echo "---Components in product " & productCode & "---"

    For Each component In components
      Set clients = installer.ComponentClients(component) : CheckError
      For Each client In Clients
        If client = productCode Then
          path = installer.ComponentPath(productCode, component) : CheckError
          Wscript.Echo "  " & component & " = " & path
          Exit For
        End If
      Next
    Next

  End If
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
' DecodeVersion
'
Function DecodeVersion(version)    
  version = CLng(version)    
  DecodeVersion = version\65536\256 & "." & (version\65535 MOD 256) & "." & (version Mod 65536)
End Function

'
' Fail
'
Sub Fail(message)
  Wscript.Echo message
  Wscript.Quit 2
End Sub

'
' extracts the filename extension
'
Function GetExtension(path)
  Dim dot    : dot    = InStrRev(path, ".")
  Dim bslash : bslash = InStrRev(path, "\")
  
  If dot > bslash Then
    GetExtension = Mid(path, dot)
  Else
    GetExtension = Empty
  End If
End Function

'
' evaluate the versioninfo elements and create msi template files to create msi patch package
'
Sub MsiPatchInfo(f,msidir,msipatchdir,msipatchtemplatedir)
  Dim objNodeList,o,ch
  Dim major,minor,fix,msipackagecode,msiproductcode,msiupgradecode,msipatchcode
  Dim major0,minor0,fix0,msipackagecode0,msiproductcode0,msiupgradecode0,msipatchcode0
  Dim major1,minor1,fix1,msipackagecode1,msiproductcode1,msiupgradecode1
  Dim versionCount,productId,msifilename,msicmdout,msitargetdir,basename
  Dim upgradedImages,targetImages,imageFamilies,properties,patchMetadata
  
  xmlDoc.load(f)
  
  If xmlDoc.parseError.errorCode<>0 Then
    WScript.Echo xmlDoc.parseError.reason
  Else
    'WScript.Echo xmlDoc.xml
  End If
  
  Set objNodeList = xmlDoc.documentElement.selectNodes("/v:versions//v:versionhistory/v:version")
  Set productId   = xmlDoc.documentElement.selectSingleNode("/v:versions//v:product/@id")
  
  WScript.Echo "        productId="& LCase(productId.text)
  WScript.Echo "           msidir="& msidir
  WScript.Echo "      msipatchdir="& msipatchdir
  WScript.Echo "  msipatchtmpldir="& msipatchtemplatedir
  
  fso.CopyFile msipatchtemplatedir & "\*.idt", msipatchdir
  
  Set upgradedImages = fso.OpenTextFile(msipatchdir&"\UpgradedImages.idt", ForAppending)
  Set targetImages   = fso.OpenTextFile(msipatchdir&"\TargetImages.idt", ForAppending)
  Set imageFamilies  = fso.OpenTextFile(msipatchdir&"\ImageFamilies.idt", ForAppending)
  Set properties     = fso.OpenTextFile(msipatchdir&"\Properties.idt", ForAppending)
  Set patchMetadata  = fso.OpenTextFile(msipatchdir&"\PatchMetadata.idt", ForAppending)
  
  If objNodeList.length>0 Then
    versionCount = 0
    
    For Each o in objNodeList
      If TypeName(o)="IXMLDOMElement" Then
        major          = o.GetAttributeNode("major").text
        minor          = o.GetAttributeNode("minor").text
        fix            = o.GetAttributeNode("fix").text
        msipackagecode = Null
        msiproductcode = Null
        msiupgradecode = Null
        msipatchcode   = Null
        
        For Each ch in o.childNodes
          If TypeName(ch)="IXMLDOMElement" and ch.tagName="msipackagecode" Then
            msipackagecode = GetValue(ch.firstChild,ch.firstChild.text)
          ElseIf TypeName(ch)="IXMLDOMElement" and ch.tagName="msiproductcode" Then
            msiproductcode = GetValue(ch.firstChild,ch.firstChild.text)
          ElseIf TypeName(ch)="IXMLDOMElement" and ch.tagName="msiupgradecode" Then
            msiupgradecode = GetValue(ch.firstChild,ch.firstChild.text)
          ElseIf TypeName(ch)="IXMLDOMElement" and ch.tagName="msipatchcode" Then
            msipatchcode = GetValue(ch.firstChild,ch.firstChild.text)
          End If
        Next
        
        If versionCount=0 Then
          major0          = CInt(major)
          minor0          = CInt(minor)
          fix0            = CInt(fix)
          msipackagecode0 = msipackagecode
          msiproductcode0 = msiproductcode
          msiupgradecode0 = msiupgradecode
          msipatchcode0   = msipatchcode
          
          basename        = LCase(productId.text) & "." & major0 & "." & minor0 & "." & fix0

          msifilename  = fso.GetAbsolutePathName(msidir & "\" & basename)
          msitargetdir = fso.GetAbsolutePathName(msipatchdir & "\" & basename)

          If fso.FileExists(msifilename&".msi") and not fso.FolderExists(msitargetdir) Then
            ExecuteProgram "msiexec /a " & msifilename & ".msi /qb TARGETDIR=" & msitargetdir,msicmdout,True,True
          End If

          upgradedImages.WriteLine("U_" & major0 & "_" & minor0 & "_" & fix0&vbTab&msitargetdir&"\"&LCase(productId.text) & "." & major0 & "." & minor0 & "." & fix0&".msi"&vbTab&vbTab&vbTab&UCase(productId.text))
          
          properties.WriteLine("PatchGUID" & vbTab & "{" & msipatchcode0 & "}" )
        Else
          major1          = CInt(major)
          minor1          = CInt(minor)
          fix1            = CInt(fix)
          msipackagecode1 = msipackagecode
          msiproductcode1 = msiproductcode
          msiupgradecode1 = msiupgradecode

          If major0=major1 and minor0=minor1 Then
            WScript.Echo "Version " & major1 & "." & minor1 & "." & fix1 & " ==> " & major0 & "." & minor0 & "." & fix0
            WScript.Echo "  packagecode="& msipackagecode1 & " ==> " & msipackagecode0
            WScript.Echo "  productcode="& msiproductcode1 & " ==> " & msiproductcode0
            WScript.Echo "  upgradecode="& msiupgradecode1 & " ==> " & msiupgradecode0
            
            msifilename  = fso.GetAbsolutePathName(msidir & "\" & LCase(productId.text) & "." & major1 & "." & minor1 & "." & fix1)
            msitargetdir = fso.GetAbsolutePathName(msipatchdir & "\" & LCase(productId.text) & "." & major1 & "." & minor1 & "." & fix1)

            If fso.FileExists(msifilename&".msi") and not fso.FolderExists(msitargetdir) Then
              ExecuteProgram "msiexec /a " & msifilename & ".msi /qb TARGETDIR=" & msitargetdir,msicmdout,True,True

            End If
            
            targetImages.WriteLine("T_" & major1 & "_" & minor1 & "_" & fix1&vbTab&msitargetdir&"\"&LCase(productId.text) & "." & major1 & "." & minor1 & "." & fix1&".msi"&vbTab&vbTab&"U_" & major0 & "_" & minor0 & "_" & fix0&vbTab&"1"&vbTab&vbTab&"0")
            
            If ExecuteProgram(WScript.FullName & " /nologo /job:nextsequenceno " & fso.GetParentFolderName(WScript.ScriptFullName) & "\msi.wsf /File:" & msifilename & ".msi",msicmdout,True,True)=0 Then
              imageFamilies.WriteLine(UCase(productId.text)&vbTab&"MNPSrcPropName"&vbTab&"2"&vbTab&msicmdout&vbTab&vbTab)
            End If
          Else
            Exit For
            WScript.Echo 

            major0          = CInt(major)
            minor0          = CInt(minor)
            fix0            = CInt(fix)
            msipackagecode0 = msipackagecode
            msiproductcode0 = msiproductcode
            msiupgradecode0 = msiupgradecode
          End If
        End If

        versionCount = versionCount + 1
      End If
    Next

    properties.WriteLine("PatchOutputPath" & vbTab & msipatchdir & "\" & basename & ".msp" )
    
    patchMetadata.WriteLine(vbTab & "CreationTimeUTC" & vbTab & FormatDateTime(Date, 2) & " " & FormatDateTime(Now,4))

    upgradedImages.Close
    targetImages.Close
    imageFamilies.Close
    properties.Close
    patchMetadata.Close

    ExecuteProgram "msidb -c -d " & msipatchdir & "\" & basename & ".pcp -f " & msipatchdir & " -i *.idt",msicmdout,True,True
    ExecuteProgram "msimsp -s " & msipatchdir & "\" & basename & ".pcp -p " & msipatchdir & "\" & basename & ".msp -l " & msipatchdir & "\" & basename & "-createpatch.log",msicmdout,True,True
  End If
End Sub

'
' transform directory info in msipackage to idt file
'
Sub TransformMsiDirectoryInfo(xmlDoc,msidir,ByRef targetDirId)
  Dim objNodeList,o,idAttr,parentIdAttr,dirId,fileId,nameNode,parentNode,directoryIdt

  Set objNodeList = xmlDoc.documentElement.selectNodes("/msipackage/directories/directory")
  
  If objNodeList.length>0 Then
    Set directoryIdt = fso.CreateTextFile(msidir&"\Directory.idt", True)

    directoryIdt.WriteLine("Directory"&vbTab&"Directory_Parent"&vbTab&"DefaultDir")
    directoryIdt.WriteLine("s72"&vbTab&"S72"&vbTab&"l255")
    directoryIdt.WriteLine("Directory"&vbTab&"Directory")

    For Each o in objNodeList
      If TypeName(o)="IXMLDOMElement" Then
        Set idAttr       = o.GetAttributeNode("id")
        Set parentIdAttr = o.GetAttributeNode("parentid")
    
        If TypeName(idAttr)<>"Nothing" Then
          If TypeName(parentIdAttr)<>"Nothing" Then
            directoryIdt.Write(idAttr.value)
            directoryIdt.Write(vbTab)

            If parentIdAttr.value=targetDirId Then
              directoryIdt.Write("TARGETDIR")
            Else
              directoryIdt.Write(parentIdAttr.value)
            End If

            directoryIdt.Write(vbTab)

            If Right(idAttr.value,6)="Folder" Then
              directoryIdt.Write(".")
            Else
              directoryIdt.Write(o.selectSingleNode("./shortname/text()").nodeValue)
              directoryIdt.Write("|")
              directoryIdt.Write(o.selectSingleNode("./name/text()").nodeValue)
            End If
          Else
            directoryIdt.Write("TARGETDIR")
            directoryIdt.Write(vbTab)
            directoryIdt.Write(vbTab)
            directoryIdt.Write("SourceDir")

            targetDirId = idAttr.value
          End If

          directoryIdt.WriteLine()
        End If
      End If  
    Next

    directoryIdt.Close
  End If
End Sub

'
' transform directory info in msipackage to idt file
'
Sub TransformMsiShortcutInfo(xmlDoc,msidir)
  Dim objNodeList,o,idAttr,parentIdAttr,dirId,fileId,nameNode,parentNode,shortcutIdt

  Set objNodeList = xmlDoc.documentElement.selectNodes("/msipackage/shortcuts/shortcut")
  
  If objNodeList.length>0 Then
    Set shortcutIdt = fso.CreateTextFile(msidir&"\Shortcut.idt", True)

    shortcutIdt.WriteLine("Shortcut"&vbTab&"Directory_"&vbTab&"Name"&vbTab&"Component_"&vbTab&"Target"&vbTab&"Arguments"&vbTab&"Description"&vbTab&"Hotkey"&vbTab&"Icon_"&vbTab&"IconIndex"&vbTab&"ShowCmd"&vbTab&"WkDir")
    shortcutIdt.WriteLine("s72"&vbTab&"s72"&vbTab&"l128"&vbTab&"s72"&vbTab&"s72"&vbTab&"S255"&vbTab&"L255"&vbTab&"I2"&vbTab&"S72"&vbTab&"I2"&vbTab&"I2"&vbTab&"S72")
    shortcutIdt.WriteLine("Shortcut"&vbTab&"Shortcut")

    For Each o in objNodeList
      If TypeName(o)="IXMLDOMElement" Then
        shortcutIdt.Write(o.GetAttributeNode("id").value)
        shortcutIdt.Write(vbTab)

        shortcutIdt.Write(o.GetAttributeNode("directoryid").value)
        shortcutIdt.Write(vbTab)

        shortcutIdt.Write(o.GetAttributeNode("name").value)
        shortcutIdt.Write(vbTab)
        
        shortcutIdt.Write(o.GetAttributeNode("componentid").value)
        shortcutIdt.Write(vbTab)
        
        shortcutIdt.Write(o.GetAttributeNode("feature").value)
        shortcutIdt.Write(vbTab)

        ' shortcutIdt.Write("Arguments")
        shortcutIdt.Write(vbTab)

        ' shortcutIdt.Write("Description")
        shortcutIdt.Write(vbTab)

        ' shortcutIdt.Write("hotkey")
        shortcutIdt.Write(vbTab)

        shortcutIdt.Write(o.GetAttributeNode("icon").value)
        shortcutIdt.Write(vbTab)

        shortcutIdt.Write(o.GetAttributeNode("iconindex").value)
        shortcutIdt.Write(vbTab)

        ' shortcutIdt.Write("ShowCmd")
        shortcutIdt.Write(vbTab)

        ' shortcutIdt.Write("WkDir")

        shortcutIdt.WriteLine()
        End If  
    Next

    shortcutIdt.Close
  End If
End Sub

'
' transform file info in msipackage to idt file
'
Sub TransformMsiFileInfo(xmlDoc,msidir,targetDirId,componentIdt)
  Dim objNodeList,o,fileId,guidAttr,dirId,fileVersion,filesIdt

  Set objNodeList  = xmlDoc.documentElement.selectNodes("/msipackage/files/file")
  
  If objNodeList.length>0 Then
    Set filesIdt = fso.CreateTextFile(msidir&"\File.idt", True)

    filesIdt.WriteLine("File"&vbTab&"Component_"&vbTab&"FileName"&vbTab&"FileSize"&vbTab&"Version"&vbTab&"Language"&vbTab&"Attributes"&vbTab&"Sequence")
    filesIdt.WriteLine("s72"&vbTab&"s72"&vbTab&"l255"&vbTab&"i4"&vbTab&"S72"&vbTab&"S20"&vbTab&"I2"&vbTab&"i2")
    filesIdt.WriteLine("File"&vbTab&"File")

    For Each o in objNodeList
      If TypeName(o)="IXMLDOMElement" Then
        Set fileId   = o.GetAttributeNode("id")
        Set guidAttr = o.GetAttributeNode("guid")
        Set dirId    = o.GetAttributeNode("directoryid")
    
        If TypeName(guidAttr)<>"Nothing" Then
          Set fileVersion = o.selectSingleNode("./version/text()")

          filesIdt.Write(fileId.value & vbTab)
          filesIdt.Write(fileId.value & vbTab)
          filesIdt.Write(o.selectSingleNode("./shortname/text()").nodeValue)
          filesIdt.Write("|")
          filesIdt.Write(o.selectSingleNode("./name/text()").nodeValue)

          filesIdt.Write(vbTab)
          filesIdt.Write(o.GetAttributeNode("size").value & vbTab)

          If TypeName(fileVersion)<>"Nothing" Then
            filesIdt.Write(fileVersion.nodeValue)
          End If
          filesIdt.Write(vbTab)

          filesIdt.Write("1033" & vbTab)
          filesIdt.Write("0" & vbTab)

          filesIdt.Write(o.GetAttributeNode("no").value)

          filesIdt.WriteLine()

          componentIdt.Write(fileId.value&vbTab&"{"&guidAttr.value&"}"&vbTab)

          If dirId.value=targetDirId Then
            componentIdt.Write("TARGETDIR")
          Else
            componentIdt.Write(dirId.value)
          End If
          
          componentIdt.WriteLine(vbTab&"0"&vbTab&vbTab&fileId.value)
        End If
      End If  
    Next

    filesIdt.Close
  End If
End Sub

'
' transform registry info in msipackage to idt file
'
Sub DumpRegistryInfo(registryIdt,componentIdt,o)
  Dim guidAttr,idAttr,hiveNode,nameNode

  If TypeName(o)="IXMLDOMElement" Then
    Set guidAttr = o.GetAttributeNode("guid")
    Set idAttr   = o.GetAttributeNode("id")

    If TypeName(idAttr)<>"Nothing" Then
      Set hiveNode = o.selectSingleNode("./key/@hive")

      registryIdt.Write(idAttr.value & vbTab)

      If hiveNode.value="HKEY_LOCAL_MACHINE" Then
        registryIdt.Write("2")
      ElseIf hiveNode.value="HKEY_CLASSES_ROOT" Then
        registryIdt.Write("0")
      Else
        registryIdt.Write("-1")
      End If

      registryIdt.Write(vbTab)

      registryIdt.Write(o.selectSingleNode("./key/text()").nodeValue)
      registryIdt.Write(vbTab)

      Set nameNode = o.selectSingleNode("./name/text()")

      If TypeName(nameNode)<>"Nothing" Then
        registryIdt.Write(nameNode.nodeValue)
      End If
      
      registryIdt.Write(vbTab)
      registryIdt.Write(o.selectSingleNode("./value/text()").nodeValue)
      registryIdt.Write(vbTab)
      registryIdt.Write(idAttr.value)
      registryIdt.WriteLine()

      componentIdt.WriteLine(idAttr.value&vbTab&"{"&guidAttr.value&"}"&vbTab&"TARGETDIR"&vbTab&"4"&vbTab&vbTab&idAttr.value)
    End If
  End If
End Sub

'
' transform registry info in msipackage to idt file
'
Sub TransformMsiRegistryInfo(xmlDoc,msidir,componentIdt)
  Dim objNodeList,o,registryIdt

  Set registryIdt = fso.CreateTextFile(msidir&"\Registry.idt", True)

  registryIdt.WriteLine("Registry"&vbTab&"Root"&vbTab&"Key"&vbTab&"Name"&vbTab&"Value"&vbTab&"Component_")
  registryIdt.WriteLine("s72"&vbTab&"i2"&vbTab&"l255"&vbTab&"L255"&vbTab&"L0"&vbTab&"s72")
  registryIdt.WriteLine("Registry"&vbTab&"Registry")

  Set objNodeList = xmlDoc.documentElement.selectNodes("/msipackage//file/registryentries/registry")
  If objNodeList.length>0 Then
    For Each o in objNodeList
      DumpRegistryInfo registryIdt,componentIdt,o
    Next
  End If

  Set objNodeList = xmlDoc.documentElement.selectNodes("/msipackage/registryentries/registry")
  If objNodeList.length>0 Then
    For Each o in objNodeList
      DumpRegistryInfo registryIdt,componentIdt,o
    Next
  End If

  registryIdt.Close
End Sub

'
' transform feature info in msipackage to idt file
'
Sub TransformMsiFeatureInfo(xmlDoc,msidir)
  Dim objNodeList,o,displayAttr,parentIdAttr,featureIdt

  Set objNodeList = xmlDoc.documentElement.selectNodes("/msipackage/features/feature")
  If objNodeList.length>0 Then
    Set featureIdt = fso.CreateTextFile(msidir&"\Feature.idt", True)

    featureIdt.WriteLine("Feature"&vbTab&"Feature_Parent"&vbTab&"Title"&vbTab&"Description"&vbTab&"Display"&vbTab&"Level"&vbTab&"Directory_"&vbTab&"Attributes")
    featureIdt.WriteLine("s38"&vbTab&"S38"&vbTab&"L64"&vbTab&"L255"&vbTab&"I2"&vbTab&"i2"&vbTab&"S72"&vbTab&"i2")
    featureIdt.WriteLine("Feature"&vbTab&"Feature")

    displayAttr = 1

    For Each o in objNodeList
      If TypeName(o)="IXMLDOMElement" Then
        Set parentIdAttr = o.GetAttributeNode("parentid")

        featureIdt.Write(o.GetAttributeNode("id").value)
        featureIdt.Write(vbTab)

        if TypeName(parentIdAttr)<>"Nothing" Then
          featureIdt.Write(parentIdAttr.value)
        End If
        featureIdt.Write(vbTab)

        featureIdt.Write(o.GetAttributeNode("title").value)
        featureIdt.Write(vbTab)

        featureIdt.Write(o.GetAttributeNode("description").value)
        featureIdt.Write(vbTab)

        featureIdt.Write(displayAttr)
        featureIdt.Write(vbTab)
        displayAttr = displayAttr + 1

        featureIdt.Write(o.GetAttributeNode("level").value)
        featureIdt.Write(vbTab)

        featureIdt.Write(o.GetAttributeNode("directory").value)
        featureIdt.Write(vbTab)

        featureIdt.Write(o.GetAttributeNode("attribute").value)

        featureIdt.WriteLine()
      End If  
    Next

    featureIdt.Close
  End If
End Sub

'
' transform feature2comp info in msipackage to idt file
'
Sub TransformMsiFeature2ComponentInfo(xmlDoc,msidir)
  Dim objNodeList,objNodeList1,o,o1,parentNode,feature2compIdt

  Set feature2compIdt = fso.CreateTextFile(msidir&"\FeatureComponents.idt", True)

  Set objNodeList     = xmlDoc.documentElement.selectNodes("/msipackage//file/features/feature/text()")
  feature2compIdt.WriteLine("Feature_"&vbTab&"Component_")
  feature2compIdt.WriteLine("s38"&vbTab&"s72")
  feature2compIdt.WriteLine("FeatureComponents"&vbTab&"Feature_"&vbTab&"Component_")

  If objNodeList.length>0 Then
    For Each o in objNodeList
      If TypeName(o)="IXMLDOMText" Then
        feature2compIdt.Write(o.nodeValue)
        feature2compIdt.Write(vbTab)

        Set parentNode = o.parentNode.parentNode.parentNode
        feature2compIdt.WriteLine(parentNode.GetAttributeNode("id").value)

        Set objNodeList1 = parentNode.selectNodes(".//registry/@id")
        If objNodeList1.length>0 Then
          For Each o1 in objNodeList1
            feature2compIdt.Write(o.nodeValue)
            feature2compIdt.Write(vbTab)
            feature2compIdt.WriteLine(o1.value)
          Next
        End If  
      End If  
    Next
  End If

  Set objNodeList = xmlDoc.documentElement.selectNodes("/msipackage/registryentries/features/feature/text()")
  If objNodeList.length>0 Then
    For Each o in objNodeList
      If TypeName(o)="IXMLDOMText" Then
        Set objNodeList1 = o.selectNodes("../../../registry/@id")
        If objNodeList1.length>0 Then
          For Each o1 in objNodeList1
            feature2compIdt.Write(o.nodeValue)
            feature2compIdt.Write(vbTab)
            feature2compIdt.WriteLine(o1.value)
          Next
        End If  
      End If  
    Next
  End If

  feature2compIdt.Close
End Sub

'
' transform property info in msipackage to idt file
'
Sub TransformMsiPropertyInfo(xmlDoc,msidir)
  Dim objNodeList,o,keyAttr,parentIdAttr,propertyIdt

  Set objNodeList = xmlDoc.documentElement.selectNodes("/msipackage/properties/property")
  If objNodeList.length>0 Then
    Set propertyIdt = fso.OpenTextFile(msidir&"\Property.idt", ForAppending)

    For Each o in objNodeList
      If TypeName(o)="IXMLDOMElement" Then
        propertyIdt.Write(o.GetAttributeNode("key").value)
        propertyIdt.Write(vbTab)
        propertyIdt.Write(o.selectSingleNode("./text()").nodeValue)
        propertyIdt.WriteLine()
      End If  
    Next

    propertyIdt.Close
  End If
End Sub

'
' transform media info in msipackage to idt file
'
Sub TransformMsiMediaInfo(xmlDoc,msidir)
  Dim objNodeList,o,mediaIdt

  Set objNodeList = xmlDoc.documentElement.selectNodes("/msipackage/media")
  If objNodeList.length>0 Then
    Set mediaIdt = fso.CreateTextFile(msidir&"\Media.idt", True)

    mediaIdt.WriteLine("DiskId"&vbTab&"LastSequence"&vbTab&"DiskPrompt"&vbTab&"Cabinet"&vbTab&"VolumeLabel"&vbTab&"Source")
    mediaIdt.WriteLine("i2"&vbTab&"i2"&vbTab&"L64"&vbTab&"S255"&vbTab&"S32"&vbTab&"S72")
    mediaIdt.WriteLine("Media"&vbTab&"DiskId")

    For Each o in objNodeList
      If TypeName(o)="IXMLDOMElement" Then
        mediaIdt.Write(o.GetAttributeNode("diskid").value)

        mediaIdt.Write(vbTab)
        mediaIdt.Write(o.GetAttributeNode("lastSequence").value)

        mediaIdt.Write(vbTab)
        mediaIdt.Write("1")

        mediaIdt.Write(vbTab)
        mediaIdt.Write("#")
        mediaIdt.Write(o.selectSingleNode("./cabname/text()").nodeValue)

        mediaIdt.Write(vbTab)
        mediaIdt.Write("DISK1")

        mediaIdt.Write(vbTab)
        mediaIdt.WriteLine()
      End If  
    Next

    mediaIdt.Close
  End If
End Sub


'
' transform msipackage to idt files
'
Sub TransformMsiPackageDescription(f,msidir)
  Dim objNodeList,o,targetDirId,componentIdt

  WScript.Echo "TransformMsiPackageDescription(f=" & f & ")"
  
  xmlDoc.load(f)
  
  If xmlDoc.parseError.errorCode<>0 Then
    WScript.Echo xmlDoc.parseError.reason
  Else
    Set componentIdt = fso.CreateTextFile(msidir&"\Component.idt", True)
    componentIdt.WriteLine("Component"&vbTab&"ComponentId"&vbTab&"Directory_"&vbTab&"Attributes"&vbTab&"Condition"&vbTab&"KeyPath")
    componentIdt.WriteLine("s72"&vbTab&"S38"&vbTab&"s72"&vbTab&"i2"&vbTab&"S255"&vbTab&"S72")
    componentIdt.WriteLine("Component"&vbTab&"Component")

    TransformMsiDirectoryInfo xmlDoc,msidir,targetDirId
    TransformMsiFileInfo xmlDoc,msidir,targetDirId,componentIdt
    TransformMsiRegistryInfo xmlDoc,msidir,componentIdt
    TransformMsiFeatureInfo xmlDoc,msidir
    TransformMsiFeature2ComponentInfo xmlDoc,msidir
    TransformMsiPropertyInfo xmlDoc,msidir
    TransformMsiShortcutInfo xmlDoc,msidir
    TransformMsiMediaInfo xmlDoc,msidir

    componentIdt.Close
  End If
End Sub