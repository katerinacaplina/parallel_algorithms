"""
2. Complete the code fragment to solve the problem:
    1 create objects: list, class object, numpy array
    2 the host should send messages to three Workers with the 1 - list, 2 - class object and 3 - numpy array.
    3 the workers have to display the obtained objects in the console.
"""

"""
3. Define the time between sending a message from a worker and receiving it at the host. This assumes that multiple workers can be run. 
	The result of the program should be printed to the console in milliseconds.
"""


from mpi4py import MPI as mpi
from sys import exit
from time import perf_counter as getTime
#from time import time as getTime

# подключим объект 'array' из библиотеки numpy
from numpy import array

# Создадим класс для передачи его данных между процессами
class Human:

    # Члены класса по умолчанию пустые и задаются в конструкторе
    firstName = ""
    secondName = ""
    age = 0

    # Конструктор класса принимает фамилию, имя и возраст человека
    def __init__(self, firstName: str, secondName: str, age: int):
        self.firstName = firstName
        self.secondName = secondName
        self.age = age

    # При выводе на экран объекта этого класса будет выводится следующее
    def __str__(self):
        obj_presentation = self.firstName + " " + self.secondName + ", "
        obj_presentation += str(self.age) + " years old"
        return obj_presentation

# Берем коммуникатор
comm = mpi.COMM_WORLD
# Получаем идентификатор процесса, запустившего эту программу
rank = comm.Get_rank()
n_rank = comm.Get_size()

if (n_rank != 4):
    if (rank == 0):
        print("Number of processes must be 4!")
        print("Run program on several processes: mpiexec -n <processes number> python <program name>")
    exit()

'''1 create objects: list, class object, numpy array'''
# Создаем список стран
object1 = ["Russia", "USA", "Denmark", "Latvia"]
# Создадим объект ранее созданного класса
object2 = Human("Ivanov", "Ivan", 50)
# Создадим массив библиотеки numpy
object3 = array([1, 2, 6, 10, -5], int)

# Помещаем все созданные объекты (список, объект, массив) в один список
list_of_objects = [object1, object2, object3]

# Первый процесс будет посылать данные
if rank == 0:
    '''2 the host should send messages to three Workers with the 1 - list, 2 - class object and 3 - numpy array.'''
    # Отправим три сообщения от первого процесса каждому из трих других процессам (1,2,3) 
    # Сообщения передадим в цикле (три сообщения - три объекта - три итерации - трем процессам)

    # Для первого процесса создадим список из трех элементов - время кждого отправления сообщения
    #sendTime = [0,0,0]

    for i in range(0, len(list_of_objects)):
        # Зададим передаваемый объект
        obj = list_of_objects[i]
        # Зададим номер процесса
        n_process = rank + 1 + i
        # Тег у каждого сообщения можно оставить одинаковым, т.к. каждому процессу посылается всего одно сообщение
        tag = 0
        # Засекаем и отправляем
        #sendTime[i] = getTime()
        # В сообщение запакуем передаваемый объект и время передачи
        comm.send([obj, getTime()], n_process, tag)

# Остальные процессы, кроме первого, будут принимать сообщения
if rank > 0:
    # Принимать сообщения будут все остальные процессы (кроме первого) 

    # Номер отправителя
    n_sender = 0
    # Процесс получает сообщение (объект, время передачи)
    message = comm.recv(source=n_sender, tag=0)
    # зафиксируем время приема сообщения
    recvTime = getTime()

    """3 the workers have to display the obtained objects in the console."""
    """3. Define the time between sending a message from a worker and receiving it at the host."""
    # Выведем полученную информацию от первого процесса-отправителя и время, затраченное на передачу сообщения
    print(f"Trasnfer time of message for {rank} process is {recvTime-message[1]:0.4f} sec. Message: {message[0]}")