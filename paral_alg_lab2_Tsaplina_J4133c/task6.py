'''
6. Determine the bandwidth when forwarding MPI messages depending on the size of the data.      
    1 Create a list object of one int type element
    2 Measure the size L of the object using the sys library
    3 host should send a message to the worker, then the worker should send it back
    4 repeat point (3) N=10 times
    5 measure the time T from sending the first message to receiving the last one at the host
    6 bandwidth is measured by the formula: R=(2*N*L) / T
    7 output the result to the console in the format "object_size (bytes): R (MB/s)
    8 increase the number of sheet elements by 1000
    9 repeat from point (2) until the number of elements is equal to 50 000 elements
'''

'''
6. Определите пропускную способность при пересылке MPI-сообщений в зависимости от размера данных.
    1 Создайте объект списка из одного элемента типа int
    2 Измерьте размер L объекта с помощью библиотеки sys
    3 хост должен отправить сообщение работнику, затем работник должен отправить его обратно
    4 повторите пункт (3) N=10 раз
    5 измерьте время T от отправки первого сообщения до получения последнего на хосте
    6 пропускная способность измеряется по формуле: R=(2*N*L) / T
    7 выведите результат на консоль в формате "object_size (байты): R (МБ/с)
    8 увеличьте количество элементов листа на 1000
    9 повторяйте с точки (2) до тех пор, пока количество элементов не станет равным 50 000 элементов
'''

import time
import sys
from mpi4py import MPI as mpi

def sendRecvMessage(N: int, listSize: int):
    comm = mpi.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    # 1 Create a list object of one int type element
    # 8 increase the number of sheet elements by 1000
    intList = [1] * listSize

    # 2 Measure the size L of the object using the sys library
    L = sys.getsizeof(intList) # sizeof list in bytes
    
    message = None

    # 3 host should send a message to the worker, then the worker should send it back
    # 4 repeat point (3) N=10 times
    for i in range(N):

        # start timer before first message sending
        if (i == 0 and rank == 0):
            t_start = time.perf_counter()


        if (rank == 1):
            # first of all to recieve message from root
            message = comm.recv(tag=i, source=0)

            # and then send this message to root
            comm.send(message, dest=0, tag=i+1)
        elif (rank == 0):
            # send message to all workers from host and each worker get this message
            comm.send(intList, dest=1, tag=i)

            # root shoult get message
            message = comm.recv(tag=i+1, source=1)

            # stop timer after last message receiving
            if (i == N - 1):
                t_stop = time.perf_counter()
                T = float(t_stop - t_start)

                # 5 measure the time T from sending the first message to receiving the last one at the host
                print("Time from first message sending to last message receiving: {:0.5f} sec ({:0.5f} mls)".format(T, T*1000))

                # 6 bandwidth is measured by the formula: R=(2*N*L) / T
                # 7 output the result to the console in the format "object_size (bytes): R (MB/s)
                R = (2 * N * L) / T
                print("object_size {} (bytes): R {:0.2f} (MB/s)".format(L, R))


def main():
    if (mpi.COMM_WORLD.Get_size() != 2):
        if (mpi.COMM_WORLD.rank == 0):
            print("Wrong number of threads!")
        return
    
    # 4 repeat point (3) N=10 times
    N = 10
    # 8 increase the number of sheet elements by 1000
    elementsCount = 1000
    # 9 repeat from point (2) until the number of elements is equal to 50 000 elements
    totalElementsCount = 50000

    iterCount = totalElementsCount // elementsCount + 1

    # for elements list with one element

    if (mpi.COMM_WORLD.rank == 0):
            print("With one element")
    sendRecvMessage(N, 1)
    if (mpi.COMM_WORLD.rank == 0):
            print()

    for i in range(1, iterCount):
        if (mpi.COMM_WORLD.rank == 0):
            print("Iter #{}".format(i))

        sendRecvMessage(N, elementsCount*i)

        if (mpi.COMM_WORLD.rank == 0):
            print()


if __name__ == "__main__":
    main()