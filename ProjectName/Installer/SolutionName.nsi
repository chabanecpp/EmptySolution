SetCompressor /FINAL lzma #Set compression algorithm (lzma in this case); /FINAL : to ignore subsequent setCompressor

;Global constants
!define COMPANY_NAME "Company name"
!define APP_NAME "Solution name"
!define REGISTRY_KEY "SolutionName"
!define INSTALL_EXE_NAME "SetupSolutionName.exe"
!define UNINSTALL_EXE_NAME "Uninstall_SolutionName.exe"
!define INSTALL_ROOT_DIR "$COMMONFILES\${COMPANY_NAME}\${APP_NAME}"
!define VERSION "0.0.0.0"

VIProductVersion "${VERSION}"

;Add version informations into installer and uninstaller (can be viewed in Installer/Uninstaller detail tab)
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Test Application"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "A test comment"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Company name"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© Fake company"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Test Application"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "1.2.3"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "1.2.3"
VIAddVersionKey /LANG=${LANG_ENGLISH} "InternalName" "1.2.3"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "Test Application is a trademark of Fake company"
VIAddVersionKey /LANG=${LANG_ENGLISH} "OriginalFilename" "1.2.3"
VIAddVersionKey /LANG=${LANG_ENGLISH} "PrivateBuild" "1.2.3"
VIAddVersionKey /LANG=${LANG_ENGLISH} "SpecialBuild" "1.2.3"

InstallDir "${INSTALL_ROOT_DIR}" #Set installation path
OutFile "${INSTALL_EXE_NAME}" #Set the name of the installer (MakeNSIS should write)

Section "Install"
	CreateDirectory "$INSTDIR" #Creates the specified directory
	SetOutPath "$INSTDIR" #Set the current output directory ($OUTDIR value)
	File *.exe #Add exe files to be extracted into $OUTDIR
	File *.dll #Add dll files to be extracted into $OUTDIR
	WriteUninstaller "${UNINSTALL_EXE_NAME}"
SectionEnd

Section "Uninstall"
	Delete "$INSTDIR\${UNINSTALL_EXE_NAME}" #Delete uninstaller exe
	Delete "$INSTDIR\*.*" #Delete installation folder contents
	RMDir "$INSTDIR" #Delete installation directory
SectionEnd