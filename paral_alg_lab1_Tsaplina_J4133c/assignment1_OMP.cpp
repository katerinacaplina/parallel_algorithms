#include <omp.h>

#include <iostream>
#include <thread>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <cstring>

// Переименуем тип данных: "временная точка" из стандартной библиотеки chrono в короткое название: time_point
typedef std::chrono::time_point<std::chrono::system_clock> time_point;
// Переименуем тип данных: "секунды" из стандартной библиотеки chrono в короткое название: seconds
typedef std::chrono::seconds seconds;
// Переименуем тип данных: "милисекунды" из стандартной библиотеки chrono в короткое название: milliseconds
typedef std::chrono::milliseconds milliseconds;

// класс - таймер
// Позволяет от стартовой точки до конечной получить интервал в секундах или милисекундах
class MyTimer
{
public:

    enum class FORMAT {MILLISECONDS, SECONDS};

    MyTimer(FORMAT format=FORMAT::SECONDS)
        : format_(format)
	{
		start();
	}

    void changeFormat(FORMAT format)
    {
        format_ = format;
    }
	
	void start()
	{
		start_ = std::chrono::system_clock::now();
	}
	
	float getTime()
	{
		end_ = std::chrono::system_clock::now();

        if (format_ == FORMAT::MILLISECONDS)
            timer_ = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start_).count();
        else if (format_ == FORMAT::SECONDS)
            timer_ = std::chrono::duration_cast<std::chrono::seconds>(end_ - start_).count();

		return timer_;
	}
	
private:
	float timer_;
	time_point start_;
	time_point end_;
    FORMAT format_;
};

// шаблонная функция, которая генерирует вектор случайных значений от left_border до right_border
template <typename T>
std::vector<T> generateVector(size_t vector_size, int generator_arg = 0);

template <typename T>
T findMax(const std::vector<T>& vector, size_t thread_number=1);

template <typename T>
void printVector(const std::vector<T>& vector);


int assignment1_OMP(int argc, char* argv[])
{
    // Чтобы проверить правилность поиска максимального элемента вектора, можно в программу подавать третий аргумент "print"
    // Тогда программа не будет высчитывать время выполнения по потокам, а просто найдет максимум и выведет его на экран вместе с вектором
    bool check_flag = false;
    if ((argc > 2) && !(std::strcmp(argv[2], "print")))
    {
        check_flag = true;
    }

    // Таймер в милисекундах
    MyTimer my_timer(MyTimer::FORMAT::MILLISECONDS);

    // Размер вектора значений
    size_t vector_size;

    // Если при запуске не было указано размера вектора,
    if (argc == 1)
    {
        // тогда его размер по умолчанию 1'000'000
        vector_size = 1000000;
    }
    else
    {
        // А если задан, тогда мы записываем этот размер в переменную
        // функция atoi() преобразует строку в целочисленный тип
        vector_size = atoi(argv[1]);
    }

    // Выводим на экран заголовок столбиков
    // Thread - количество потоков для поиска максимума
    // Max - найденный максимум
    // Time - время, затраченное на поиск максимума
    if (!check_flag)
        std::cout << "Thread \t Max \t    Time, ms" <<  std::endl;

    // сделам тест для каждого количества потоко от 1 до 10
    for (size_t i = 1; i <= 10; ++i)
    {
        // Количество потоков каждую итерацию увеличивается
        size_t threads = i;

        // Генерируем вектор случайных значений от 0.0 до 1.0
        auto vector = generateVector<double>(vector_size, i);

        // Выводим значения вектора на экран
        if (check_flag)
            printVector(vector);

        // Старт таймера
        my_timer.start();

        // Находим максимум
        auto max = findMax<double>(vector, threads);

        // Выводим максимум вектора
        if (check_flag)
        {
            std::cout << "Max of vector: " << max << std::endl;
            return 0;
        }


        // Останавливаем таймер
        int time = my_timer.getTime();

        // Выводим на экран информацию о поиске максимума
        // %zu - для вывода типа size_t
        // %f - для вывода типа double/float
        // %d - для вывода типа int
        // \t - табуляция
        printf("%zu\t%f    %d\n", threads, max, time);
    }
	return 0;
}


template <typename T>
T findMax(const std::vector<T>& vector, size_t thread_num)
{
	// если вектор пустой, вернем нуль
	if (vector.empty())
        return static_cast<T>(0);
	
    // по условию задачи потоков может быть от 1 до 10
    if (thread_num < 0)
		thread_num = 1;
    else if (thread_num > 10)
        thread_num = 10;
	
    // Чтобы найти количество итераций на каждый поток, нужно общее количество элементов (итераций) поделить на заданное количество потоков
    size_t iter_number = vector.size() / thread_num;

    // определяем первый элемент вектора как максимальный элемент (глобальный относительно потоков)
    T glob_max = vector[0];

    // Начинаем распараллеливать программу поиска максимального элемента
    #pragma omp parallel
    {
        // Для каждого потока будет свой максимальный элемент - локальныйм максимум - берется первый элемент вектора
        T loc_max = glob_max;

        // А теперь каждый поток обработает свою часть вектора - распределяется автоматически в зависимости от заданного количества потоков
        // В данном случае директива принимает высчитанный параметр - количество итераций на один поток (iter_number)
        #pragma omp for schedule(static, iter_number)
        for (size_t i = 0; i < vector.size(); ++i)
        {
            // если поток в своей части вектора нашел элемент больше, чем предыдущий локальный максимум,
            // тогда этот элемент становится локальным максимумом
            if (vector[i] > loc_max)
                loc_max = vector[i];
        }

        // После того как поток заканчивает поиск максимума в своей части вектора, он сравнивает свой локальный максимум
        // с глобальным максимумом. И если глобальный оказался меньше локального, то глобальный принимает значение локального.
        // Пока какой-либо поток проверяет сравнивает свой локальный максимум с глобальным,
        // остальные потоки не могут начать это сравнение - это связано с тем, что каждый поток обращается к глобальной переменной glob_max и возможно меняет ее значение.
        #pragma omp critical
        if (loc_max > glob_max)
        {
            glob_max = loc_max;
        }
    }
	
    return glob_max;
}

template <typename T>
std::vector<T> generateVector(size_t vector_size, T left_border, T right_border, int generator_arg)
{
    // Если размер вектора задан нулевой, тогда возвращается пустой вектор
    if (vector_size == 0)
        return {};

    srand(generator_arg * time(nullptr));

    // Вектор элементов типа T
    std::vector<T> vector;

    // Теперь заполним вектор элементами в количестве vector_size
    for (size_t i = 0; i < vector_size; ++i)
    {
        // Генерируется случайное число в диапазоне от left_border до right_border
        T element = left_border + rand() % (right_border - left_border + 1);
        // Добавляется элемент в вектор
        vector.push_back(element);
    }

    return vector;
}

template <typename T>
std::vector<T> generateVector(size_t vector_size, int generator_arg)
{
    if (vector_size == 0)
        return {};

    srand(generator_arg * time(nullptr));

    // Вектор элементов типа T
    std::vector<T> vector;

    // Теперь заполним вектор элементами в количестве vector_size
    for (size_t i = 0; i < vector_size; ++i)
    {
        // Генерируется случайное число
        T element = static_cast<T>(std::rand()) / RAND_MAX;
        // Добавляется элемент в вектор
        vector.push_back(element);
    }

    return vector;
}

template <typename T>
void printVector(const std::vector<T>& vector)
{
    for (const auto& element: vector)
    {
        std::cout << element << " ";
    }

    std::cout << std::endl;
}















