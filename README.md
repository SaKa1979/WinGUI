WinGUI
======

A test to compile with gcc and winapi and windres.exe for compiling resource file

This is a working version with an windows icon. 

To precompile a resource file (windows GUI) follow the next steps: 
One way is to create a Pre-Build Step. 
Under menue Project | Properties | C/C++-Build | Settings | Build Steps | Pre-Build Steps fill in the command-line: 

windres --use-temp-file -i..\MyProject.rc -o..\MyProject_rc\MyProject_rc.o 

Make the object known to the linker. Under menue Project | Properties | C/C++-Build | Settings Tool Settings | MinGW C++ Linker | Miscellaneous | Other Objects click the icon 'Add', fill in the line:

"C:\MyWorkspace\MyProject\MyProject_rc\MyProject_rc.o" 

'MyWorkspace' and 'MyProject' replace with whatever is fitting for your purpose.
