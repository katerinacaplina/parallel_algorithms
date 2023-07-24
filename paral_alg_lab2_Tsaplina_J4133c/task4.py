"""
4. Using the sleep function write a program in which worker sends a message to host and continues to run (outputs some message to console) 
	while host is idle and gets the message only when sleep is over then outputs it to console. 
"""

"""
P.S.: используя функцию print(), почему-то блокируется консоль, и все print() из цикла выводятся за раз, когда процесс заканчивает работу.
И так с каждый процессом. Вместо функции print() используется логирование logging
"""

from mpi4py import MPI as mpi
from time import sleep
import logging as log

# Коммуникатор
comm = mpi.COMM_WORLD
# Процесс
rank = comm.Get_rank()

if (comm.Get_size() < 2):
    if (rank == 0):
        print("Number of processes must be 4!")
        print("Run program on several processes: mpiexec -n <processes number> python <program name>")
    exit()

# Первый процесс отправляет сообщения со своей скоростью (после каждого отправления ждет свои 0.1 секунду) и не ждет никого больше
if rank == 0:
    # that kind of message you should send in order to calculate
    # time on worker?
    message = "Hello, world "
    for i in range(0, 10):
        # this program will work only with single worker
        comm.send(message + f"{i}", dest=1, tag=i)
        #print(f"Sender: {i+1} message was sent")
        log.error(f"Sender: {i} message was sent")
        sleep(0.1) # 0.1 second
    print("Sender finished!")

# А второй процесс тоже работает со своей скоростью: получает сообщение, выводит на экран, ждет некторое время (пол секунды) и продолжает заново
if rank == 1:    
    for i in range(0, 10):
        # Получаем сообщение
        message = comm.recv(source=0, tag=i)
        # perform additional calculations in order to determine 
        # the time to transfer the message
        #print(f"Reciever: {i} message was recieved: {message}")
        log.error(f"Reciever: {i} message was recieved: {message}")
        # идет ожидание (выполнение некоторой работы)
        sleep(0.5) # 0.5 second
    print("Reciever finished!")


