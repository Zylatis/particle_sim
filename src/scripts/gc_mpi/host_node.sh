gcloud compute instances create mpi-node-0 --metadata-from-file startup-script=startup.sh \
 --image projects/debian-cloud/global/images/family/debian-9  --custom-cpu 2 --custom-memory 4 --preemptible --scopes=compute-rw,storage-full

gcloud compute scp /home/graeme/coding_local/particle_sim/cpp/src/scripts/gc_mpi/mpi_test.cpp mpi-node-0:~/
gcloud compute scp /home/graeme/coding_local/particle_sim/cpp/src/scripts/gc_mpi/makefile mpi-node-0:~/
gcloud compute scp /home/graeme/coding_local/particle_sim/cpp/src/scripts/gc_mpi/mpi_ssh.sh mpi-node-0:~/

