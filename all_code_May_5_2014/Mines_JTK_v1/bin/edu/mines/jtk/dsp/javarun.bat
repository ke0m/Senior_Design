@echo on
setlocal

rem Where is the Mines JTK? (Where is your build.xml?)
set MINES_JTK_HOME=c:\Users\Dave\workspace\jtk

rem Where will Java look for classes? 
rem Add other jars to this list as necessary.
set CLASSPATH=^
%MINES_JTK_HOME%\build\libs\edu_mines_jtk.jar;^
%MINES_JTK_HOME%\libs\arpack-java.jar;^
%MINES_JTK_HOME%\libs\netlib-java.jar;^
%MINES_JTK_HOME%\libs\gluegen-rt.jar;^
%MINES_JTK_HOME%\libs\jogl-all.jar;^
%MINES_JTK_HOME%\libs\junit.jar;^
C:\Users\Dave\Documents\Senior_Design\JOCL-0.1.9-bin\JOCL-0.1.9.jar;^
.

rem Run a server VM with assertions enabled and a 1GB max Java heap.
rem Modify these flags and properties as necessary for your system.
java -server -ea -Xmx1g ^
-Djava.util.logging.config.file=c:\Users\Dave\workspace\logs\config ^
%*

endlocal
