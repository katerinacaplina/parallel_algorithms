'''
8. Develop a program that demonstrates the asynchronous communication mechanism:
    1 host sends a message in non-blocking mode
    2 host must spend 25 seconds in sleep mode before it receives the message
    3 after sending the message, the host should send a "WAITING" message to the console every 5 seconds 
    until it receives confirmation of receiving the message.
'''

'''
8. Разработайте программу, демонстрирующую механизм асинхронной связи:
    1 хост отправляет сообщение в неблокирующем режиме
    2 хост должен провести 25 секунд в спящем режиме, прежде чем он получит сообщение
    3 после отправки сообщения хост должен отправлять сообщение "ОЖИДАНИЕ" на консоль каждые 5 секунд,
    пока не получит подтверждение о получении сообщения.
'''

# Use log.error() instead of print() function because print() fuction blocks the console window
# and prints all sended messages to console after programm ending!

# def ibsend(self, obj: Any, dest: int, tag: int = 0) -> Request: ...
# def isend(self, obj: Any, dest: int, tag: int = 0) -> Request: ...

import time
import logging as log
from mpi4py import MPI as mpi

def workForHost(workersCount: int):

    message = "Hello from Host"

    requests = []

    # 1 host sends a message in non-blocking mode
    # the host sends message to each worker
    for worker in range(1, workersCount+1):
        req = mpi.COMM_WORLD.isend(message, worker)
        requests.append(req)

    # waiting time after message sending
    wait_time = 25 # seconds
    # time to print on the screen message "WAITING"
    print_time = 5 # seconds
    
    # 2 host must spend 25 seconds in sleep mode before it receives the message
    for i in range(wait_time//print_time):
        time.sleep(print_time)
        # 3 after sending the message, the host should send a "WAITING" message to the console every 5 seconds 
        log.error("WAITING")
        #print("WAITING")

    # until it receives confirmation of receiving the message.
    # wait confirmation from each worker
    requests[0].waitall(requests)

def work():
    host = 0
    message = mpi.COMM_WORLD.recv(source=host)
    log.error("Rank {}, message {}".format(mpi.COMM_WORLD.Get_rank(), message))
    #print("Rank {}, message {}".format(mpi.COMM_WORLD.Get_rank(), message))

def main():
    size = mpi.COMM_WORLD.Get_size()
    rank = mpi.COMM_WORLD.Get_rank()

    if (size < 2):
        if (rank == 0):
            print("Must be host and one worker at least!")
        return

    if (rank == 0):
        workForHost(size-1) # for host
    else:
        work() # for workers

if __name__ == "__main__":
    main()