#############################################################################
# Tests 2D tensor-guided gridder

import sys
from math import *
from java.awt import *
from java.lang import *
from java.util import *
from java.nio import *
from javax.swing import *

from edu.mines.jtk.awt import *
from edu.mines.jtk.dsp import *
from edu.mines.jtk.io import *
from edu.mines.jtk.mosaic import *
from edu.mines.jtk.util import *
from edu.mines.jtk.util.ArrayMath import *

from fmm import *

#############################################################################
# global parameters

dataDir = "/data"
#pngDir = "./png"
pngDir = None

gray = ColorMap.GRAY
jet = ColorMap.JET
prism = ColorMap.PRISM

#############################################################################
# test functions

def main(args):
  #testSimple()
  #testConstant()
  testSine()
  #testTsai()

def testSimple():
  n1,n2 = 201,203
  i1,i2 = randomSites(10,n1,n2)
  tensors = ConstantTensors2(n1,n2,0.0,1.000,1.000)
  testGridder(n1,n2,i1,i2,tensors)
 
def testConstant():
  n1,n2 = 501,501
  #i1 = [3*n1/8,6*n1/8,4*n1/8]
  #i2 = [2*n2/8,4*n2/8,6*n2/8]
  i1 = [9*n1/16,7*n1/16]
  i2 = [7*n2/16,9*n2/16]
  i1,i2 = randomSites(100,n1,n2)
  tensors = ConstantTensors2(n1,n2,0.0,1.000,1.000)
  #tensors = ConstantTensors2(n1,n2,-30.0,0.050,1.000)
  #tensors = ConstantTensors2(n1,n2,-30.0,0.0001,1.000)
  #tensors = ConstantTensors2(n1,n2,0.0,0.050,1.000)
  testGridder(n1,n2,i1,i2,tensors)
 
def testSine():
  n1,n2 = 501,501
  #i1 = [7*n1/8,3*n1/8,5*n1/8,2*n1/8]
  #i2 = [1*n2/8,4*n2/8,4*n2/8,6*n2/8]
  i1,i2 = randomSites(10,n1,n2)
  tensors = SineTensors2(n1,n2,400.0,30.0)
  #tensors = SineTensors2(n1,n2,0.0,30.0)
  testGridder(n1,n2,i1,i2,tensors)
 
def testTsai():
  n1,n2 = 101,101
  #i1 = [3*n1/8,3*n1/8,5*n1/8,1*n1/8]
  #i2 = [2*n2/8,5*n2/8,6*n2/8,6*n2/8]
  i1,i2 = randomSites(10,n1,n2)
  tensors = TsaiTensors2(n1,n2)
  testGridder(n1,n2,i1,i2,tensors)

def testGridder(n1,n2,i1,i2,tensors):
  t = fillfloat(-1.0,n1,n2)
  p = zerofloat(n1,n2)
  q = zerofloat(n1,n2)
  for k in range(len(i1)):
    k1,k2 = i1[k],i2[k]
    t[k2][k1] = 0.0
    p[k2][k1] = 1.0+k
  tgg = TensorGuidedGridder2(tensors)
  tgg.gridNearest(t,p)
  print "t: min =",min(t),"max =",max(t)
  print "p: min =",min(p),"max =",max(p)
  t = clip(0.0,200.0,t)
  tgg.gridBlended(t,p,q)
  print "q: min =",min(q),"max =",max(q)
  plot(t,0,0,prism)
  plot(p,0,0,jet)
  plot(q,0,0,jet)

def randomSites(n,n1,n2):
  random = Random()
  seed = random.nextInt()
  #seed = 1329742170 # curvy boundary in constant anisotropic test
  #seed = 124868546 # island in sine test
  #seed = 1527419053 # island in sine test
  #seed = -652323882 # island in sine test
  print "seed =",seed
  random.setSeed(seed)
  i1,i2 = [],[]
  for i in range(n):
    i1.append(int(n1*random.nextFloat()))
    i2.append(int(n2*random.nextFloat()))
  return i1,i2

#############################################################################
# tensors

class ConstantTensors2(EigenTensors2):
  """
  2D constant tensors
  dip is angle clockwise between eigenvector v and x2 axis (in degrees)
  du is eigenvalue corresponding to eigenvector u
  dv is eigenvalue corresponding to eigenvector v
  """
  def __init__(self,n1,n2,dip,du,dv):
    EigenTensors2.__init__(self,n1,n2)
    a = dip*pi/180.0
    u1 =  cos(a)
    u2 = -sin(a)
    v1 = -u2
    v2 =  u1
    for i2 in range(n2):
      for i1 in range(n1):
        self.setEigenvalues(i1,i2,du,dv)
        self.setEigenvectorU(i1,i2,u1,u2)

class SineTensors2(EigenTensors2):
  """
  2D seismic-like sine waves
  ampmax is max amplitude of sine waves
  dipmax is maximum dip (in degrees) of sine waves
  """
  def __init__(self,n1,n2,ampmax,dipmax):
    EigenTensors2.__init__(self,n1,n2)
    b1 = 9.0*2.0*pi/(n1-1) # 9 cycles vertically
    b2 = 3.0*2.0*pi/(n2-1) # 3 cycles horizontally
    a1 = ampmax
    a2 = atan(dipmax*pi/180.0)/b2
    f = zerofloat(n1,n2)
    for i2 in range(n2):
      for i1 in range(n1):
        s2 = a2*sin(b2*i2)
        f[i2][i1] = a1*cos(b1*(i1+s2))
        e1 = -a1*b1*sin(b1*(i1+s2))
        e2 = e1*a2*b2*cos(b2*i2)
        den = 1.0+e1*e1+e2*e2
        d11 = (1.0+e2*e2)/den
        d22 = (1.0+e1*e1)/den
        d12 = -e1*e2/den
        self.setTensor(i1,i2,(d11,d12,d22))
    #plot(f)

class TsaiTensors2(EigenTensors2):
  """
  2D geodetic tensors for Tsai et al.'s test function.
  """
  def __init__(self,n1,n2):
    EigenTensors2.__init__(self,n1,n2)
    a1 = 2.0*pi
    a2 = 2.0*pi
    d1 = 2.0/(n1-1)
    d2 = 2.0/(n2-1)
    f1 = -1.0
    f2 = -1.0
    f = zerofloat(n1,n2)
    for i2 in range(n2):
      x2 = f2+i2*d2
      for i1 in range(n1):
        x1 = f1+i1*d1
        f[i2][i1] = cos(a1*x1)*sin(a2*x2)
        e1 = -a1*sin(a1*x1)*sin(a2*x2)
        e2 =  a2*cos(a1*x1)*cos(a2*x2)
        den =  1.0+e1*e1+e2*e2
        d11 =  1.0-e1*e1/den
        d22 =  1.0-e2*e2/den
        d12 = -e1*e2/den
        self.setTensor(i1,i2,(d11,d12,d22))
    #plot(f)

#############################################################################
# plotting

fontSize = 24
width = 920
height = 900

def plot(f,cmin=0.0,cmax=0.0,cmap=jet,png=None):
  n1 = len(f[0])
  n2 = len(f)
  p = panel()
  s1 = Sampling(n1,1.0,0.0)
  s2 = Sampling(n2,1.0,0.0)
  pv = p.addPixels(s1,s2,f)
  if cmin<cmax:
    pv.setClips(cmin,cmax)
  else:
    pv.setPercentiles(0.0,100.0)
  pv.setInterpolation(PixelsView.Interpolation.LINEAR)
  pv.setColorModel(cmap)
  frame(p,png)

def frame(panel,png=None):
  frame = PlotFrame(panel)
  frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE)
  frame.setFontSize(fontSize)
  frame.setSize(width,height)
  frame.setVisible(True)
  if png and pngDir:
    frame.paintToPng(100,6,pngDir+"/"+png+".png")
  return frame

def panel():
  p = PlotPanel(1,1,
    PlotPanel.Orientation.X1DOWN_X2RIGHT,
    PlotPanel.AxesPlacement.LEFT_TOP)
  return p

#############################################################################
# Do everything on Swing thread.

class RunMain(Runnable):
  def run(self):
    main(sys.argv)
SwingUtilities.invokeLater(RunMain())
