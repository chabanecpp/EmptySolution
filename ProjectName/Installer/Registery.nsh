 Function .onInit
   MessageBox MB_YESNO "This will install. Continue?" IDYES NoAbort
     Abort ; causes installer to quit.
   NoAbort:
   ReadINIStr $INSTDIR 
 FunctionEnd
 
#WriteINIStr $TEMP\something.ini section1 something 123
#DeleteINISec $TEMP\something.ini section1