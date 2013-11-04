"""
Plotting the performance of the GPU
"""

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np


# X -> number of iterations; Y -> array size
with open('test.txt') as f:
  Xbeg, Xend, Xstep = [int(x) for x in f.readline().split()] # read first line
  Ybeg, Yend, Ystep = [int(x) for x in f.readline().split()] # read second line
  flops = []
  for line in f: # read rest of lines
    flops.append([int(x) for x in line.split()])

X = np.arange(Xbeg, Xend+1, Xstep)
Y = np.arange(Ybeg, Yend+1, Ystep)
X, Y = np.meshgrid(X, Y)

# Z -> number of flops
Z = []
k = 0
for i in xrange(len(X)):
  Z.append([])
  for j in xrange(len(X[0])):
    #Z[i].append(1)
    Z[i] = Z[i] + flops[k]
    k += 1
print Z

fig = plt.figure()
ax = fig.gca(projection='3d')
surf = ax.plot_surface(X, Y, Z, rstride=1, cstride=1,linewidth=0, antialiased=False)
plt.title('GPU Flops')
ax.set_xlabel('Number of Iterations')
ax.set_ylabel('Array Size')
ax.set_zlabel('Flops')
plt.show()
