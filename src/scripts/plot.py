from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D
import random
import pandas as pd
from random import randrange, uniform
n = len(pd.read_csv("../../output/data/"+str(0)+"_pos.dat", sep='\t'))
sizes = [ uniform(1, 2) for i in range(n) ]

for i in range(1000):
	print i
	fig = pyplot.figure(figsize=pyplot.figaspect(1)*3)
	ax = Axes3D(fig)
	ax.set_xlim3d(-20,20)
	ax.set_ylim3d(-20,20)
	ax.set_zlim3d(-20,20)

	ax.grid(False) 
	#~ ax.w_xaxis.pane.fill = False
	#~ ax.w_yaxis.pane.fill = False
	#~ ax.w_zaxis.pane.fill = False
	ax.w_xaxis.set_pane_color((0.0, 0.0, 0.0, 0.0))
	ax.w_yaxis.set_pane_color((0.0, 0.0, 0.0, 0.0))
	ax.w_zaxis.set_pane_color((0.0, 0.0, 0.0, 0.0))
	test = pd.read_csv("../../output/data/"+str(i)+"_pos.dat", sep='\t')
	test.columns = ['x','y','z']
	ax.scatter(test['x'], test['y'], test['z'], color = 'white', s = sizes)
	fig.set_facecolor('black')
	ax.set_facecolor('black')
	pyplot.savefig('../../output/plots/'+str(i)+'_plot.png', bbox_inches='tight')
	pyplot.close()
