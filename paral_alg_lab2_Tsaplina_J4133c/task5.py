"""
5. A program to calculate dot-product in distributed mode: two vectors of size 100 000 must be initialized on host with values 1 (first vector) 
	and 2 (second vector), each worker must receive their fragment of both vectors, perform element-by-element multiplication and sum the result, 
	host then must collect the results from all vectors and calculate the final result by displaying it on the console
"""

"""
5. Программа для вычисления точечного произведения в распределенном режиме: 
два вектора размером 100 000 должны быть инициализированы на хосте со значениями 1 (первый вектор)
и 2 (второй вектор), каждый работник должен получить свой фрагмент обоих векторов, 
выполнить поэлементное умножение и суммировать результат,
затем хост должен собрать результаты из всех векторов и вычислить конечный результат, отобразив его на консоли
"""

from mpi4py import MPI as mpi
import numpy
import logging as log
from sys import exit

   
def workForHost():

    comm = mpi.COMM_WORLD
    size = comm.Get_size()
    rank = comm.Get_rank()

    # Размер генерируемых векторов
    arraySize = 100000

    # Сгенерируем два вектора размером 100'000 
    v1 = numpy.ones(arraySize, dtype=int)
    
    v2 = v1.copy()
    for i in range(len(v2)):
        v2[i] += 1

    # Каждому работнику достанется равное количество элементов вектора (и второго)
    equalPart = arraySize // (size-1)
    
    # Последнему работнику придется взять на себя остатки элементов (если такие будут)
    remainder = arraySize % (size-1)

    elementsCount = equalPart

    requests1 = []
    requests2 = []

    # для каждого работника отправим кусок от каждого вектора
    for i in range(1, size):

        end = elementsCount*i

        # если процессу достался последний кусок вектора, тогда он заберет остатки
        if (i == size-1):
            end += remainder

        # для каждого работника будем брать новое начало вектора и отмерять нужное количество элементов
        start = (i-1) * elementsCount

        # отправляем каждому работник соответствующий кусок каждого вектора
        req = comm.isend(v1[start:end], dest=i, tag=1)
        requests1.append(req)
        req = comm.isend(v2[start:end], dest=i, tag=2)
        requests2.append(req)

    # дожидаемся передачи данных каждому работнику
    for i in range(0, size-1):
            requests1[i].wait()
            requests2[i].wait()

    # В консоль сообщим, что хост отослал всем задания - посчитать сумму данных им элементов
    log.error(f"Host gave tasks to all workers.")

    sum = 0
    for i in range(1, size):
        sum += comm.recv()

    log.error("Host sum = {}".format(sum))

def work():

    comm = mpi.COMM_WORLD
    size = comm.Get_size()
    rank = comm.Get_rank()

    # каждый процесс принимает свой кусок вектора
    array1 = comm.recv(source=0, tag=1)
    array2 = comm.recv(source=0, tag=2)

    #sum = array(int32(0))
    sum = 0
    for i in range(0, len(array1)):
        sum += (array1[i] * array2[i])

    # показываем, какой процесс какую сумму подсчитал
    log.error(f"{rank} rank calculated sum = {sum}")

    comm.send(sum, dest=0)    

def main():
    N = mpi.COMM_WORLD.Get_size()
    if (N < 2):
        if (mpi.COMM_WORLD.rank == 0):
            print("Must be 1 host and 1 worker at least!")
        return

    rank = mpi.COMM_WORLD.Get_rank()

    if (rank == 0):
        workForHost()
    else:
        work()


if __name__ == "__main__":
    main()