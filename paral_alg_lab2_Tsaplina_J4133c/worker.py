'''
9. Write a program which implements master-worker mode:
    1 host must create N worker processes by itself
    2 when worker is created, he writes his rank in the console 
    3 each worker sends his rank to host
    4 host shows the messages received in the console
    For this program you need to write two scripts - host.py and worker.py, each will implement its own logic.
'''

'''
9. Напишите программу, которая реализует режим master-worker:
    1 хост должен сам создать N рабочих процессов
    2 когда работник создан, он записывает свой ранг в консоль
    3 каждый работник отправляет свой ранг хозяину
    4 хост показывает сообщения, полученные в консоли
    Для этой программы вам нужно написать два скрипта - host.py и worker.py , каждый будет реализовывать свою собственную логику.
'''

import logging as log
from mpi4py import MPI as mpi

comm = mpi.Comm.Get_parent()
rank = comm.Get_rank()
host = 0

# 2 when worker is created, he writes his rank in the console 
log.error("Host created {} worker!".format(rank))
#print("Host created {} worker!".format(rank))

# 3 each worker sends his rank to host
comm.send(rank, dest=host)

comm.Disconnect()