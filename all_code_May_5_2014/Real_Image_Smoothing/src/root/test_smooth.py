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
    
    ############ Creating the SOS Objects###########
    kernel = LocalDiffusionKernel(LocalDiffusionKernel.Stencil.D22CL)
    smooth_gpu = LocalSmoothingFilter(0, 100, kernel)
    smooth_cpu = LocalSmoothingFilter(0, 100)
    
    #####Getting the Tensors#####
    iimg, s1, s2 = readPnzImage()
    simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));
    simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
    diff = zerofloat(len(s1.getValues()), len(s2.getValues()));
    
    
    iimg = mul(0.0001,iimg)
    
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
  
######CPU/GPU Image Smoothing############

    #####D00#####
#     smooth_cpu.apply(d00, 80, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     plotPnz("CPU Output: D00", simgc, s1, s2, dscale=1, png="cpud00")
#     plotPnz("CPU Output: D00", simgc, s1, s2, d00, dscale=1, png="cpud00f")
#   
#     smooth_gpu.applyGPU(d00, 80, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     plotPnz("GPU Output: D00", simgg, s1, s2, dscale=1, png="gpud00")
#     plotPnz("GPU Output: D00", simgg, s1, s2, d00, dscale=1, png="gpud00f")
#       
#     diff = sub(simgg, simgc)
#     plotPnz("Difference between Images: D00", diff, s1, s2, dscale=1, png="diffd00")
#        
#     simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));     
#     simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
#       
#     #######D01#######
#     smooth_cpu.apply(d01, 80, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     plotPnz("CPU Output: D01", simgc, s1, s2, dscale=1, png="cpud01")
#     plotPnz("CPU Output: D01", simgc, s1, s2, d01, dscale=1, png="cpud01f")
#            
#           
#     smooth_gpu.applyGPU(d01, 80, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     plotPnz("GPU Output: D01", simgg, s1, s2, dscale=1, png="gpud01")
#     plotPnz("GPU Output: D01", simgg, s1, s2, d01, dscale=1, png="gpud01f")
#        
#     diff = sub(simgg, simgc)
#     plotPnz("Difference between Images: D01", diff, s1, s2, dscale=1, png="diffd01")
#        
#     simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));   
#     simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
#        
#        
#     ########D02##########
#     smooth_cpu.apply(d02, 80, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     plotPnz("CPU Output: D02", simgc, s1, s2, dscale=1, png="cpud02")
#     plotPnz("CPU Output: D02", simgc, s1, s2, d02, dscale=1, png="cpud02f")
#            
#     smooth_gpu.applyGPU(d02, 80, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     plotPnz("GPU Output: D02", simgg, s1, s2, dscale=1, png="gpud02")
#     plotPnz("GPU Output: D02", simgg, s1, s2, d02, dscale=1, png="gpud02f")
#        
#     diff = sub(simgg, simgc)
#     plotPnz("Difference between Images: D02", diff, s1, s2, dscale=1, png="diffd02")
#         
#     simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));     
#     simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
#        
#     ###########D04###########
    smooth_cpu.apply(d04, 160, iimg, simgc)
    smooth_cpu.applySmoothS(simgc, simgc);
    plotPnz("C", simgc, s1, s2, dscale=1, png="cpud04_square_cb_alpha0")
    #plotPnz("", simgc, s1, s2, d04, dscale=1, png="cpud04f_square")
               
    smooth_gpu.applyGPU(d04, 160, iimg, simgg)
    smooth_gpu.applySmoothS(simgg, simgg);
    plotPnz("G", simgg, s1, s2, dscale=1, png="gpud04_square_cb_alpha160")    
    #plotPnz("", simgg, s1, s2, d04, dscale=1, png="gpud04f_square")
         
    #diff = sub(simgg, simgc)
    #plotPnz("", diff, s1, s2, dscale=1, png="diffd04")
         
    #simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));      
    #simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
         
    #plotPnz("", iimg, s1, s2, dscale=1, png="input_square")
        
#     #########D11########
#     smooth_cpu.apply(d11, 80, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     plotPnz("CPU Output: D11", simgc, s1, s2, dscale=2, png="cpud11")
#     plotPnz("CPU Output: D11", simgc, s1, s2, d11, dscale=2, png="cpud11f")
#         
#     smooth_gpu.applyGPU(d11, 80, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     plotPnz("GPU Output: D11", simgg, s1, s2, dscale=2, png="gpud11")
#     plotPnz("GPU Output: D11", simgg, s1, s2, d11, dscale=2, png="gpud11f")
#         
#     diff = sub(simgg, simgc)
#     plotPnz("Difference between Images: D11", diff, s1, s2, dscale=2, png="diffd11")
#            
#     simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));    
#     simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
#      
#      ##########D12###########         
#     smooth_cpu.apply(d12, 80, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     plotPnz("CPU Output: D12", simgc, s1, s2, dscale=2, png="cpud12")
#     plotPnz("CPU Output: D12", simgc, s1, s2, d12, dscale=2, png="cpud12f")
#       
#     smooth_gpu.applyGPU(d12, 80, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     plotPnz("GPU Output: D12", simgg, s1, s2, dscale=2, png="gpud12")
#     plotPnz("GPU Output: D12", simgg, s1, s2, d12, dscale=2, png="gpud12f")
#       
#     diff = sub(simgg, simgc)
#     plotPnz("Difference between Images: D12", diff, s1, s2, dscale=2, png="diffd12")
#        
#     simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));     
#     simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
#       
#       
#      #########D14###########    
#     smooth_cpu.apply(d14, 80, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     plotPnz("CPU Output: D14", simgc, s1, s2, dscale=2, png="cpud14")
#     plotPnz("CPU Output: D14", simgc, s1, s2, d14, dscale=2, png="cpud14f")
#           
#     smooth_gpu.applyGPU(d14, 80, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     plotPnz("GPU Output: D14", simgg, s1, s2, dscale=2, png="gpud14")
#     plotPnz("GPU Output: D14", simgg, s1, s2, d14, dscale=2, png="gpud14f")
#      
#     diff = sub(simgg, simgc)
#     plotPnz("Difference between Images: D14", diff, s1, s2, dscale=2, png="diffd14")
#            
#     ########Image difference and Input Data#################
#     
    plotPnz("", iimg, s1, s2, dscale=1, png="input")


# endnum = 20
# step = 2
# cpudata = zerofloat(endnum/step)
# gpudata = zerofloat(endnum/step)
# alphas = zerofloat(endnum/step)
# j = 0
#      
# for i in xrange(0, endnum, step):
#     kernel = LocalDiffusionKernel(LocalDiffusionKernel.Stencil.D22CL)
#     smooth_gpu = LocalSmoothingFilter(0, 5*i, kernel)
#     smooth_cpu = LocalSmoothingFilter(0, 5*i)
#     c = (i*i)/2
#     iimg, s1, s2 = readPnzImage()
#     iimg = mul(0.0001,iimg)
#     simgc = zerofloat(len(s1.getValues()), len(s2.getValues()));
#     simgg = zerofloat(len(s1.getValues()), len(s2.getValues()));
#     lof = LocalOrientFilter(4.0)
#     s = lof.applyForTensors(iimg)
#     d04 = EigenTensors2(s); d04.invertStructure(0.0,4.0)
#     smooth_cpu.apply(d04, c, iimg, simgc)
#     smooth_cpu.applySmoothS(simgc, simgc);
#     smooth_gpu.applyGPU(d04, c, iimg, simgg)
#     smooth_gpu.applySmoothS(simgg, simgg);
#     param = toString(c)
#     namegpu = "gpualpha" + param
#     namecpu = "cpualpha" + param
#     plotPnz(namegpu, simgg, s1, s2, dscale=2, png=namegpu)
#     plotPnz(namecpu, simgc, s1, s2, dscale=2, png=namecpu)

class RunMain(Runnable):
    def run(self):
        main(sys.argv)
SwingUtilities.invokeLater(RunMain()) 
    