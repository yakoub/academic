cluster:cluster.o monitor.o scatter.o stalker.o mpi_inbox.o move_list.o 
	mpicc cluster.o monitor.o scatter.o stalker.o\
	      mpi_inbox.o move_list.o -lm -o cluster
	tar -uvf lab.tar *.c *.h     
cluster.o:cluster.h cluster.c
	mpicc -c cluster.c
monitor.o:monitor.c cluster.h move_list.h
	mpicc -c monitor.c
mpi_inbox.o:mpi_inbox.c cluster.h move_list.h
	mpicc -c mpi_inbox.c
scatter.o:scatter.c cluster.h
	mpicc -c scatter.c
stalker.o:stalker.c cluster.h move_list.h
	mpicc -c stalker.c
move_list.o:move_list.c move_list.h
	mpicc -c move_list.c
clean:
	rm -f *o cluster
	rm -f lab.tar
debug:
	mpicc -g *c -o cluster -lm
