"""
Plotting the performance of the GPU

Python script takes 2 arguments: -i inputfilename -n Plot name

inputfile name format
0 1 -> log scale iterations & log scale array size (1 for log, 0 for linear)
10 100 10 10 -> iterations start, end, step, and size
1 16 2 5 -> array size start, end, step, and size
123 -> data
452 -> data
456 -> data
456 -> data
... -> data
"""

import sys, getopt

def main(argv):
  from mpl_toolkits.mplot3d import Axes3D
  import matplotlib.pyplot as plt
  import numpy as np
  
  inputfile = ''
  outputfile = ''
  try:
     opts, args = getopt.getopt(argv,"hi:n:",["ifile=","nfile="])
  except getopt.GetoptError:
     #print 'test.py -i <inputfile> -o <outputfile>'
     print 'test.py -i <inputfile> -n <name>'
     sys.exit(2)
  for opt, arg in opts:
     if opt == '-h':
        #print 'test.py -i <inputfile> -o <outputfile>'
        print 'test.py -i <inputfile> -n <name>'
        sys.exit()
     elif opt in ("-i", "--ifile"):
        inputfile = arg
     elif opt in ("-n", "--nfile"):
        name = arg
     #elif opt in ("-o", "--ofile"):
        #outputfile = arg
  print 'Input file is :', inputfile
  #print 'Output file is :', outputfile
  
  # X -> number of iterations; Y -> array size
  with open(inputfile) as f:
    Xlog, Ylog = [int(x) for x in f.readline().split()] # first line
    Xbeg, Xend, Xstep, Xlen = [int(x) for x in f.readline().split()] # second line
    Ybeg, Yend, Ystep, Ylen = [float(x) for x in f.readline().split()] # third line
    flops = []
    Ybeg = int(Ybeg)
    Yend = int(Yend)
    Ylen = int(Ylen)
    for line in f: # read rest of lines
      flops.append([float(x) for x in line.split()])
  
  #for i in range(0,len(flops)):
    #flops[i][0] = flops[i][0]*0.001
  if (Xlog):
    X = np.zeros(Xlen)
    X[0] = Xbeg
    for i in range(1,Xlen):
      X[i] = X[i-1]*Xstep
  else:
    X = np.arange(Xbeg, Xend+1, Xstep)
  if (Ylog):
    Y = np.zeros(Ylen)
    Y[0] = Ybeg
    for i in range(1,Ylen):
      Y[i] = Y[i-1]*Ystep
    print Y
  else:
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
  
  fig = plt.figure()
  ax = fig.gca(projection='3d')
  surf = ax.plot_surface(X, Y, Z, rstride=1, cstride=1,linewidth=0, antialiased=False)
  plt.title(name)
  ax.set_xlabel('Number of Iterations')
  ax.set_ylabel('Array Size')
  ax.set_zlabel('MegaFLOPS')
  ax.xaxis.get_major_formatter().set_powerlimits((0, 1))
  ax.yaxis.get_major_formatter().set_powerlimits((0, 1))
  ax.zaxis.get_major_formatter().set_powerlimits((0, 1))
  #plt.savefig(outputfile)
  plt.show()

if __name__ == "__main__":
   main(sys.argv[1:])
