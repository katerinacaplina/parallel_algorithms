'''
7. Write a program that implements a circular message exchange for the number of workers N=10:
    1 the host sends a message to the worker with rank 1
    2 the worker receives the message, displays his rank and the message in the console, and forwards it to the next rank
    3 the process stops when host receives the original message and outputs the "DONE" message
'''

'''
7. Напишите программу, реализующую циклический обмен сообщениями для числа работников N=10:
    1 хост отправляет сообщение работнику с рангом 1
    2 работник получает сообщение, отображает свой ранг и сообщение в консоли и пересылает его следующему рангу
    3 процесс останавливается, когда хост получает исходное сообщение и выводит сообщение "ГОТОВО"
'''

from mpi4py import MPI as mpi

def workForHost():
    # host generates some message for the first worker
    message = "Hello from host!"
    
    # 1 the host sends the message to the worker with rank 1
    mpi.COMM_WORLD.send(message, dest=1)

    # host must recieve sended message from last worker (rank 10)
    message = mpi.COMM_WORLD.recv(source=10)

    # displays his rank and the message in the console,
    print("Rank {}, message: {}".format(mpi.COMM_WORLD.Get_rank(), message))

def workForLastWorker():
    # last worker must recieve sended message from previous worker (rank 9)

    # 2 the worker receives the message,
    message = mpi.COMM_WORLD.recv(source=9)

    # displays his rank and the message in the console,
    print("Rank {}, message: {}".format(mpi.COMM_WORLD.Get_rank(), message))

    # and forwards it to the next rank (host in this case)
    message = "Hello to host too!"
    mpi.COMM_WORLD.send(message, dest=0)

def work():
    # each worker must recieve sended message from previous worker

    # current worker
    worker = mpi.COMM_WORLD.Get_rank()
    # next worker
    next = worker + 1
    # previous worker
    prev = worker - 1

    # 2 the worker receives the message,
    message = mpi.COMM_WORLD.recv(source=prev)

    # displays his rank and the message in the console,
    print("Rank {}, message: {}".format(worker, message))

    # and forwards it to the next rank (host in this case)
    message += " And worker_{}!".format(worker)
    mpi.COMM_WORLD.send(message, dest=next)


def main():
    # Write a program that implements a circular message exchange for the number of workers N=10:
    N = mpi.COMM_WORLD.Get_size()
    # checking
    if (N != 11):
        if (mpi.COMM_WORLD.rank == 0):
            print("Must be 1 host and 10 workers!")
        return

    rank = mpi.COMM_WORLD.Get_rank()
    if (rank == 0):
        workForHost()
    elif (rank == 10):
        workForLastWorker()
    else:
        work()


if __name__ == "__main__":
    main()