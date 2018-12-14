ffmpeg -r 20  -start_number 0 -f image2 -i ../../output/plots/%d_plot.png -vcodec libx264 -crf 0  -pix_fmt yuv420p ../../output/movies/sim.MP4
