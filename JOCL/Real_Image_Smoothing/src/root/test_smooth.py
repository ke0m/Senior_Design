'''
Created on Mar 24, 2014

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

from tensors import *


def main(args):

    n1 = 501;
    n2 = n1;
    
    r = zerofloat(n1,n2)
    sc = zerofloat(n1,n2)
    sg = zerofloat(n1,n2)
    
    
    random = Random()
    
    for i in range(n1):
        for j in range(n2):
            r[i][j] = random.nextFloat();
    
    
    ############ Creating the SOS Objects###########
    kernel = LocalDiffusionKernel(LocalDiffusionKernel.Stencil.D22CL)
    smooth_gpu = LocalSmoothingFilter(0.01, 100, kernel)
    smooth_cpu = LocalSmoothingFilter()
    
    #####Now attepting to Smooth the proveid seismic image#####
    
    
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
    
    #Testing for the unpacking of the tensors
     
#     n1img = len(s1.getValues())
#     n2img = len(s2.getValues())
#      
#     di = zerofloat(3)
#     dib = zerofloat(3*n1img*n2img)
#      
#     x = 0
#     y = 0
#     i = 1
#     j = 1
#     for i in range(n1img):
#         for j in range(n2img):
#             d01.getTensor(j,i,di)
#             for k in range(len(di)):
#                 dib[x] = di[k]
#                 x = x + 1
#             print("%f, %f, %f" % (di[0], dib[y], y))
#             print("%f, %f, %f" % (di[1], dib[y+1], y+1))
#             print("%f, %f, %f" % (di[2], dib[y+2], y+2))
#             y = y + 3
            
########CPU Image Smoothing################
     
#     smooth_cpu.apply(d00, 20, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     plotPnz(simgc, s1, s2, dscale=1)
#     plotPnz(simgc, s1, s2, d00, dscale=1)
#         
#     simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));
#        
    smooth_cpu.apply(d01, 20, iimg, simgc)
    smooth_cpu.applySmoothS(simgc, simgc);
    plotPnz(simgc, s1, s2, dscale=1)
    plotPnz(simgc, s1, s2, d01, dscale=1)
#        
#     simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));
#        
#     smooth_cpu.apply(d02, 20, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     plotPnz(simgc, s1, s2, dscale=1)
#     plotPnz(simgc, s1, s2, d02, dscale=1)
#         
#     simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));
#          
#     smooth_cpu.apply(d04, 20, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     plotPnz(simgc, s1, s2, dscale=1)
#     plotPnz(simgc, s1, s2, d04, dscale=1)
#          
#     simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));
#          
#     smooth_cpu.apply(d11, 20, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     plotPnz(simgc, s1, s2, dscale=1)
#     plotPnz(simgc, s1, s2, d11, dscale=1)
#          
#     simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));
#          
#     smooth_cpu.apply(d12, 20, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     plotPnz(simgc, s1, s2, dscale=1)
#     plotPnz(simgc, s1, s2, d12, dscale=1)
#          
#     simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));
#         
#     smooth_cpu.apply(d14, 20, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     plotPnz(simgc, s1, s2, dscale=1)
#     plotPnz(simgc, s1, s2, d14, dscale=1)
#       
      
      
    
######GPU Image Smoothing############

#     smooth_gpu.apply(d00, 20, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     plotPnz(simgg, s1, s2, dscale=1)
#     plotPnz(simgg, s1, s2, d00, dscale=1)
#         
#     simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
       
    smooth_gpu.apply(d01, 20, iimg, simgg)
    smooth_gpu.applySmoothS(simgg, simgg);
    plotPnz(simgg, s1, s2, dscale=1)
    plotPnz(simgg, s1, s2, d01, dscale=1)
       
#     simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
#        
#     smooth_gpu.apply(d02, 20, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     plotPnz(simgg, s1, s2, dscale=1)
#     plotPnz(simgg, s1, s2, d02, dscale=1)
#         
#     simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
#          
#     smooth_gpu.apply(d04, 20, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     plotPnz(simgg, s1, s2, dscale=1)
#     plotPnz(simgg, s1, s2, d04, dscale=1)
#          
#     simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
#          
#     smooth_gpu.apply(d11, 20, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     plotPnz(simgg, s1, s2, dscale=1)
#     plotPnz(simgg, s1, s2, d11, dscale=1)
#          
#     simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
#          
#     smooth_gpu.apply(d12, 20, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     plotPnz(simgg, s1, s2, dscale=1)
#     plotPnz(simgg, s1, s2, d12, dscale=1)
#          
#     simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
#         
#     smooth_gpu.apply(d14, 20, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     plotPnz(simgg, s1, s2, dscale=1)
#     plotPnz(simgg, s1, s2, d14, dscale=1)
      



######Plotting the smoothed images#######
       
    diff = sub(simgg, simgc)
    
    plotPnz(diff, s1, s2, dscale=1)
    plotPnz(iimg, s1, s2, dscale=1)
    

class RunMain(Runnable):
    def run(self):
        main(sys.argv)
SwingUtilities.invokeLater(RunMain()) 
    