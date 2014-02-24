import numpy as np
import numpy.random
import matplotlib.pyplot as plt

a=256*np.random.rand(64,64)

f0=plt.figure()
plt.imshow(a,cmap=plt.gray())
plt.suptitle("imshow")

f1=plt.figure()
plt.figimage(a,cmap=plt.gray())
plt.suptitle("figimage")

plt.show()