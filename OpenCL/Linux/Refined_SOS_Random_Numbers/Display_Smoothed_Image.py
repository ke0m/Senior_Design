import numpy as np
import matplotlib.cm as cm
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
import csv
from string import *


def format_coord(x, y):
    col = int(x+0.5)
    row = int(y+0.5)
    if col>=0 and col<numcols and row>=0 and row<numrows:
        z = diffimage[row,col]
        return 'x=%1.4f, y=%1.4f, z=%1.4f'%(x, y, z)
    else:
        return 'x=%1.4f, y=%1.4f'%(x, y)


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
numrows, numcols = diffimage.shape
f3 = plt.figure()
ax = f3.add_subplot(111);
plt.imshow(diffimage,cmap=plt.gray())
plt.colorbar()
plt.suptitle("Difference Between Images")

ax.format_coord = format_coord

plt.show()
