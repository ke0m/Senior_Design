'''
Created on Mar 26, 2014

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

sw = Stopwatch()

############ Creating the SOS Objects###########
kernel = LocalDiffusionKernel(LocalDiffusionKernel.Stencil.D22CL)


#####Getting the Tensors#####
iimg, s1, s2 = readPnzImage()
simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));
simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
diff = zerofloat(len(s1.getValues()), len(s2.getValues()));

lof = LocalOrientFilter(4.0)
s = lof.applyForTensors(iimg)
d00 = EigenTensors2(s); d00.invertStructure(0.0,0.0)
d01 = EigenTensors2(s); d01.invertStructure(0.0,1.0)
d02 = EigenTensors2(s); d02.invertStructure(0.0,2.0)
d04 = EigenTensors2(s); d04.invertStructure(0.0,4.0)
d11 = EigenTensors2(s); d11.invertStructure(1.0,1.0)
d12 = EigenTensors2(s); d12.invertStructure(1.0,2.0)
d14 = EigenTensors2(s); d14.invertStructure(1.0,4.0)


####Run Benchmark######

n1 = len(iimg);
n2 = len(iimg[0]);
maxTime = 2
cpucount = 0
gpucount = 0
endnum = 22
step = 2
startnum = 4
cpudata = zerofloat((endnum-startnum)/step)
gpudata = zerofloat((endnum-startnum)/step)
alphas = zerofloat((endnum-startnum)/step)
speedup = zerofloat((endnum-startnum)/step)
j = 0

for i in xrange(startnum,endnum,step):
    smooth_gpu = LocalSmoothingFilter(0, 5*i, kernel)
    smooth_cpu = LocalSmoothingFilter(0, 5*i)
    c = (i*i)/2
#     sw.restart()
#     while (sw.time() < maxTime):
#         smooth_cpu.apply(d04, c, iimg, simgc)
#         smooth_cpu.applySmoothS(simgc, simgc);
#         cpucount = cpucount + 1
#     
#     sw.stop()
#     cputime = sw.time()
#     
#     sw.restart()
# 
#     while(sw.time() < maxTime):       
#         smooth_gpu.applyGPU(d04, c, iimg, simgg)
#         smooth_gpu.applySmoothS(simgg, simgg)
#         gpucount = gpucount + 1
#     sw.stop()
#     gputime = sw.time()


    sw.restart()
    for k in xrange(100):
        smooth_cpu.apply(d04, c, iimg, simgc)
        smooth_cpu.applySmoothS(simgc, simgc);
    sw.stop();
    cputime = sw.time()


    sw.restart()
    for l in xrange(100):
        smooth_gpu.applyGPU(d04, c, iimg, simgg)
        smooth_gpu.applySmoothS(simgg, simgg)
    sw.stop()
    gputime = sw.time()
    
    totalnumflops = (35.0*n1*n2 - 1)
    iterflops = totalnumflops*5*i + totalnumflops
    alphas[j] = c
    cpudata[j] = cputime#(cpucount*iterflops*1.0e-6)/cputime
    gpudata[j] = gputime#(gpucount*iterflops*1.0e-6)/gputime
    speedup[j] = cputime/gputime
    j = j + 1
    print("alpha: %f" % c)
    print("Java: %f Filters per second" % cputime)#((cpucount*iterflops*1.0e-6)/cputime))
    print("JOCL: %f Filters per second" % gputime)#((gpucount*iterflops*1.0e-6)/gputime))# how many FLOPS
    print("Speedup: %f\n" % (cputime/gputime))
    cpucount = gpucount = 0 
    
    
fgpudata = open('gpucopydata.txt', 'w')
for y in xrange(len(gpudata)):
    fgpudata.write(toString(gpudata[y])+"\n")

fgpudata.close()


sp = SimplePlot()
poiv1 = sp.addPoints(alphas, cpudata)
poiv1.setStyle("r-o")
poiv2 = sp.addPoints(alphas, gpudata)
poiv2.setStyle("b-o")
#sp.setTitle("Structured Oriented Smoothing Performance")
sp.setHLabel("Smoothing parameter")
sp.setVLabel("Overhead time (s)")
#sp.setVLimits(0, 50)
#sp.setHLimits(4, 200)
sp.paintToPng(360,3.3,"../../png_bench/smooth_report_copy.png")


sp1 = SimplePlot()
poiv3 = sp1.addPoints(alphas, speedup)
poiv3.setStyle("g-o")
sp1.setHLabel("Smoothing Parameter")
sp1.setVLabel("Speedup")
sp1.paintToPng(360,3.3,"../../png_bench/speedup_test.png")