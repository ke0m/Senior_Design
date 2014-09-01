'''
Created on Apr 27, 2014

@author: Joseph Jennings
'''

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

from tensors import *

sp = SimplePlot()

copydata = zerofloat(9)
realdata = zerofloat(9)
spart    = zerofloat(9)
alphas   = zerofloat(9)

endnum = 22
step = 2
startnum = 4

file1 = open("gpucopydata.txt", 'r')
file2 = open("gpudata.txt", "r")

i = 0
for line in file1:
    line = line.strip()
    for number in line.split():
        copydata[i] = float(number)
        i = i + 1

x = 0
for line1 in file2:
    line1 = line1.strip()
    for num in line1.split():
        realdata[x] = float(num)
        x = x + 1
        
k = 0
for j in xrange(startnum, endnum, step):
    alphas[k] = (j*j)/2

    k = k + 1
    
for z in xrange(9):
    spart[z] = copydata[z]/realdata[z]
    
print("Average: %f", (sum(spart))/len(spart))

poiv1 = sp.addPoints(alphas, realdata)
poiv1.setStyle("b-o")
poiv2 = sp.addPoints(alphas, copydata)
poiv2.setStyle("k-o")
#sp.setTitle("Structured Oriented Smoothing Performance")
sp.setHLabel("Smoothing parameter")
sp.setVLabel("Time (s)")
#sp.setVLimits(0, 50)
#sp.setHLimits(4, 200)
sp.paintToPng(360,3.3,"../../png_bench/amdplot_other.png")

 
 

