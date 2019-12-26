gcloud compute disks snapshot "mpi-node-0" --snapshot-names "mpi-node"
gcloud compute disks create mpi-disk-{1..2}  --source-snapshot "mpi-node"
for i in `seq 1 2`; do gcloud compute instances create mpi-node-$i --disk name=mpi-disk-$i,boot=yes,mode=rw; done;
