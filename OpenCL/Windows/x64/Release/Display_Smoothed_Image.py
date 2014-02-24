import numpy as np
import matplotlib.cm as cm
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
import csv
from string import *


#Read in a text file
inputimage = np.loadtxt("inputData.txt");
cpuimage = np.loadtxt("cpuoutput.txt");
gpuimage = np.loadtxt("gpuoutput.txt");


test = np.random.random((100,101))

#Display the arrays
#fig, ax = plt.subplots();
#ax.imshow(npcpuimage)
#plt.show()

f0 = plt.figure()
plt.imshow(inputimage,cmap=plt.gray())
plt.colorbar()
plt.suptitle("Input Image")


f1 = plt.figure()
plt.imshow(cpuimage,cmap=plt.gray())
plt.colorbar()
plt.suptitle("CPU Output")


f2 = plt.figure()
plt.imshow(gpuimage,cmap=plt.gray())
plt.colorbar()
plt.suptitle("GPU Output")

diffimage = cpuimage - gpuimage

f3 = plt.figure()
plt.imshow(diffimage,cmap=plt.gray())
plt.colorbar()
plt.suptitle("Difference Between Images")

plt.show()