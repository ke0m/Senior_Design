import numpy as np
import matplotlib.pyplot as plt

# evenly sampled time at 200ms intervals
#t = np.arange(0., 5., 0.2)


perfdata = np.loadtxt("perfplot1.txt")

cpudata = perfdata[:,0]
gpudata = perfdata[:,1]

sizedata = np.arange(len(perfdata))

#for i in range(len(cpudata)):
#    sizedata[i] = 2**i
 
sizedata = [1, 2, 4, 8, 16, 32, 128, 512, 1024]   
    
ax = plt.subplot(111)
#leg = plt.plot(sizedata, cpudata,'ro', sizedata, cpudata, 'r', sizedata, gpudata,'bs', sizedata, gpudata, 'b')
leg = plt.plot(sizedata, cpudata, 'k', label="Java")
leg = plt.plot(sizedata, cpudata, 'ko')
leg = plt.plot(sizedata, gpudata, 'g', label="JOCL")
leg = plt.plot(sizedata, gpudata, 'gs')
plt.xlabel('Array Size')
plt.ylabel('MFLOPS')
plt.title("Performance of Smoothing Kernel with Copy")


# Shink current axis by 20%
box = ax.get_position()
ax.set_position([box.x0, box.y0, box.width * 0.8, box.height])

# Put a legend to the right of the current axis
plt.legend(loc='center left', bbox_to_anchor=(1, 0.5))

# red dashes, blue squares and green triangles
#f1 = plt.figure
#plt.plot(t, t, 'r--', t, t**2, 'bs', t, t**3, 'g^')
plt.show()



