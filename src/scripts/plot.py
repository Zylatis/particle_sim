from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D
import random
import multiprocessing
import pandas as pd
from random import randrange, uniform
import os 
n = len(pd.read_csv("../../output/data/"+str(0)+"_pos.dat", sep='\t'))
sizes = [ uniform(0.1, 1) for i in range(n) ]


def empty_folder(folder):

	print(f"Emptying folder {folder}")
	for the_file in os.listdir(folder):
	    file_path = os.path.join(folder, the_file)
	    try:
	        if os.path.isfile(file_path):
	            os.unlink(file_path)
	        #elif os.path.isdir(file_path): shutil.rmtree(file_path)
	    except Exception as e:
	        print(e)

# empty_folder("../../output/plots/")
def do_plot(i):
	print(i)
	fig = pyplot.figure(figsize=(6,6))
	ax = Axes3D(fig)
	ax.set_xlim3d(-20,20)
	ax.set_ylim3d(-20,20)
	ax.set_zlim3d(-20,20)

	ax.grid(False) 

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

p = multiprocessing.Pool( multiprocessing.cpu_count() )
p.map(do_plot, [i for i in range(1000)])