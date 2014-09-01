import sys; print('%s %s' % (sys.executable or sys.platform, sys.version))
def readPnzImage():
    s1 = Sampling(501,0.0125,0.0)
    s2 = Sampling(501,0.0125,0.0)
    g = readImage("C:\Users\Dave\workspace\ggs256",s1,s2)
    return g,s1,s2
reads = [readPnzImage]
reads
print reads
enumerate(reads)
print enumerate(reads)
cls
exit
end
import sys; print('%s %s' % (sys.executable or sys.platform, sys.version))
test_smooth.py
ls
[wd
pwd
e
exit
qqq
l
import sys; print('%s %s' % (sys.executable or sys.platform, sys.version))
import sys
from java.awt import *
from java.lang import *
from java.nio import *
from javax.swing import *
from edu.mines.jtk.awt import *
from edu.mines.jtk.dsp import *
from edu.mines.jtk.io import *
from edu.mines.jtk.mosaic import *
#from edu.mines.jtk.util import *
from edu.mines.jtk.util.ArrayMath import *
def readImage(fileName,s1,s2):
    n1,n2 = s1.count,s2.count
    ais = ArrayInputStream(fileName+".dat")
    x = zerofloat(n1,n2)
    ais.readFloats(x)
    ais.close()
    return x
def readPnzImage():
    s1 = Sampling(501,0.0125,0.0)
    s2 = Sampling(501,0.0125,0.0)
    g = readImage("C:\Users\Dave\workspace\ggs256",s1,s2)
    return g,s1,s2
 iimg, s1, s2 = readPnzImage()
    simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));
    
    lof = LocalOrientFilter(4.0)
    s = lof.applyForTensors(iimg)
    d00 = EigenTensors2(s); d00.invertStructure(0.0,0.0)
    d01 = EigenTensors2(s); d01.invertStructure(0.0,1.0)
    d02 = EigenTensors2(s); d02.invertStructure(0.0,2.0)
    d04 = EigenTensors2(s); d04.invertStructure(0.0,4.0)
    d11 = EigenTensors2(s); d11.invertStructure(1.0,1.0)
    d12 = EigenTensors2(s); d12.invertStructure(1.0,2.0)
d14 = EigenTensors2(s); d14.invertStructure(1.0,4.0)
iimg, s1, s2 = readPnzImage()
simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));
 lof = LocalOrientFilter(4.0)
lof = LocalOrientFilter(4.0)
s = lof.applyForTensors(iimg)
d00 = EigenTensors2(s); d00.invertStructure(0.0,0.0)
d00.getTensor(1, 1)
c;ear
clear
import sys; print('%s %s' % (sys.executable or sys.platform, sys.version))
sp = SimplePlot()
from java.awt import *
from java.lang import *
from java.nio import *
from javax.swing import *
from java.util import *
from edu.mines.jtk.awt import *
from edu.mines.jtk.dsp import *
from edu.mines.jtk.io import *
from edu.mines.jtk.mosaic import *
from edu.mines.jtk.util.ArrayMath import *
from edu.mines.jtk.util import *
sp = SimplePlot()
hello = toFloat('20')
