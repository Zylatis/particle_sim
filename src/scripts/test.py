
from mayavi.mlab import *
import numpy as np

x = [0,1,2,-20]
y = [0,1,2,-20]
z = [1,2,3,4]
points3d(x, y, z, colormap="copper", scale_factor=.25)
savefig(filename='test.png',size=(400,400))
