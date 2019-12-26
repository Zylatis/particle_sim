echo "StrictHostKeyChecking no" | sudo tee --append /etc/ssh/ssh_config
echo "HashKnownHosts No" | sudo tee --append ~/.ssh/config

ssh-keygen -t rsa -f ~/.ssh/id_rsa -N '' -C "MPI Keys"
cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys
make
sudo cp mpi_test /usr/bin
