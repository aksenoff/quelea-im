!define PRODUCT_NAME "Quelea"
!define PRODUCT_VERSION "1.0"
!define pkgdir "c:\Qt\quelea-installer\client"

!include "MUI.nsh"
!include "nsDialogs.nsh"
SetCompressor /SOLID lzma

!define MUI_ICON "C:\Qt\quelea-installer\icon-installer.ico"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${pkgdir}\license.txt"
!insertmacro MUI_PAGE_DIRECTORY
Page custom fnCustomInit fnCustomDestroy
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_LANGUAGE "Russian"

Name "${PRODUCT_NAME}"
Caption "Установка ${PRODUCT_NAME} ${PRODUCT_VERSION}"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
OutFile "${PRODUCT_NAME}-${PRODUCT_VERSION}.exe"

Var Dialog
Var shotcutState
Var CheckBox

Function fnCustomInit
	!insertmacro MUI_HEADER_TEXT "Ярлык" "Не ставьте тут галку, а то пожалеете"
	nsDialogs::Create 1018
	Pop $Dialog

	${NSD_CreateCheckBox} 0 5u 100u 10u "Ярлык на рабочем столе"
	Pop $CheckBox
	
	nsDialogs::Show
FunctionEnd

Function fnCustomDestroy
	${NSD_GetState} $CheckBox $shotcutState
FunctionEnd

UninstPage uninstConfirm
UninstPage instfiles

Section
  ${If} $INSTDIR == "$PROGRAMFILES\${PRODUCT_NAME}"
	SetOutPath "$INSTDIR"
  ${Else}
	SetOutPath "$INSTDIR\${PRODUCT_NAME}"
  ${EndIf}
  
  File  "${pkgdir}\mingwm10.dll"
  File  "${pkgdir}\libgcc_s_dw2-1.dll"
  File  "${pkgdir}\QtCore4.dll"
  File  "${pkgdir}\QtGui4.dll"
  File  "${pkgdir}\QtNetwork4.dll"
  File  "${pkgdir}\QueleaClient.exe"
  
  WriteUninstaller "$OUTDIR\Uninstall.exe"
  
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$OUTDIR\QueleaClient.exe" "" ""
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Удалить ${PRODUCT_NAME}.lnk" "$OUTDIR\Uninstall.exe"
  ${If} $shotcutState == "1"
	CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$OUTDIR\QueleaClient.exe" "" ""
  ${EndIF}
  
  WriteRegStr HKLM SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME} "DisplayName" "${PRODUCT_NAME}"
  WriteRegStr HKLM SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME} "DisplayIcon" "$OUTDIR\QueleaClient.exe"
  WriteRegStr HKLM SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME} "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME} "URLInfoAbout" "http://quelea-im.googlecode.com/"
  WriteRegStr HKLM SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME} "UninstallString" "$OUTDIR\Uninstall.exe"
  
  
SectionEnd

Section "Uninstall"
  Delete "$OUTDIR\Uninstall.exe"
  Delete "$OUTDIR\mingwm10.dll"
  Delete "$OUTDIR\libgcc_s_dw2-1.dll"
  Delete "$OUTDIR\QtCore4.dll"
  Delete "$OUTDIR\QtGui4.dll"
  Delete "$OUTDIR\QtNetwork4.dll"
  Delete "$OUTDIR\QueleaClient.exe"
  RMDir $OUTDIR
  
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Удалить ${PRODUCT_NAME}.lnk"
  RMDir $SMPROGRAMS\${PRODUCT_NAME}
  
  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
  
  DeleteRegKey HKLM  SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}
SectionEnd