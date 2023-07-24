#include <omp.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cstring>

// Переименуем тип данных: "временная точка" из стандартной библиотеки chrono в короткое название: time_point
typedef std::chrono::time_point<std::chrono::system_clock> time_point;
// Переименуем тип данных: "секунды" из стандартной библиотеки chrono в короткое название: seconds
typedef std::chrono::seconds seconds;
// Переименуем тип данных: "милисекунды" из стандартной библиотеки chrono в короткое название: milliseconds
typedef std::chrono::milliseconds milliseconds;

// Переименуем тип данных: вектор целочисленных значений из стандартной бибилиотеки в короткое название: Vector
typedef std::vector<int> Vector;
// Переименуем тип данных: вектор векторов целочисленных значений - Matrix
typedef std::vector<Vector> Matrix;

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

// шаблонная функция, которая генерирует квадратную матрицу случайных значений от left_border до right_border
template <typename T>
std::vector<std::vector<T>> generateMatrix(size_t matrix_size, T left_border, T right_border, int generator_arg = 0);

// шаблонная функция, которая генерирует вектор случайных значений от left_border до right_border
template <typename T>
std::vector<T> generateVector(size_t vector_size, T left_border, T right_border, int generator_arg = 0);

// Объявление функции перемножения двух матриц
Matrix multiplyMatrices(const Matrix& matrix_1, const Matrix& matrix_2, size_t thread_num=1);

// Объявление функции печати матрицы на экран
void printMatrix(const Matrix& matrix);

int assignment2_OMP(int argc, char* argv[])
{
    // Чтобы проверить правилность умножения матриц, можно в программу подавать аргумент "print"
    // Тогда программа не будет высчитывать время выполнения по потокам, а просто перемножим две матрицы между собой и выведет третью
    bool check_flag = false;
    if ((argc > 2) && !(std::strcmp(argv[2], "print")))
    {
        check_flag = true;
    }

    // Создадим таймер в милисекундах
    MyTimer timer(MyTimer::FORMAT::MILLISECONDS);

    // Размер матриц. Размер не может быть отрицательным, поэтому используется тип size_t
    size_t size;

    // Если при запуске не было указано размера матриц,
    if (argc == 1)
    {
        // тогда его размер по умолчанию 1'000
        size = 500;
    }
    else
    {
        // А если задан, тогда мы записываем этот размер в переменную
        // Функция atoi() преобразует строку в целочисленный тип
        size = atoi(argv[1]);
    }

    // Левая граница значений
    int left_border = 1;
    // Правая граница значений
    int right_border = 9;

    // Выводим на экран заголовок столбиков
    // Thread - количество потоков для поиска максимума
    // Time - время, затраченное на поиск максимума
    // Efficiency - эффективность умножения при заданном количестве потоков
    if (!check_flag)
        std::cout << "Thread \t Time, ms \t Efficiency" <<  std::endl;

    // Время выполнения умножения матриц при одном потоке
    int t1 = 0;
    // Эффективность вычисления для тестов с количеством потоков от 2 и больше
    float efficiency = 0.0f;

    // сделам тест для каждого количества потоко от 1 до 10
    for (size_t i = 1; i <= 10; ++i)
    {
        // Количество потоков каждую итерацию увеличивается
        size_t threads = i;

        // Генерируем две матрицы случайных значений от 1 до 10
        auto matrix_1 = generateMatrix(size, left_border, right_border, i);
        auto matrix_2 = generateMatrix(size, left_border, right_border, i+10);

        if (check_flag)
        {
            // Выводим матрицы на экран
            std::cout << "Matrix 1\n"; printMatrix(matrix_1);
            std::cout << "Matrix 2\n"; printMatrix(matrix_2);
        }

        // Старт таймера
        timer.start();

        // Находим максимум
        auto new_matrix = multiplyMatrices(matrix_1, matrix_2, threads);

        if (check_flag)
        {
            // Печатаем третью матрицу
            std::cout << "New Matrix\n"; printMatrix(new_matrix);
            return 0;
        }

        // Останавливаем таймер
        int time = timer.getTime();

        // для первого теста с одним потоком вычисляем время его выполнения
        if (i == 1)
            t1 = time;
        // для остальных тестов вычисляем эффективность в процентах для наглядности
        else
        {
            // чтобы на нуль не делить, поставим проверку знаменателя
            if (time == 0)
                efficiency = 0;
            else
                efficiency = static_cast<float>(t1) / time;
        }

        // Выводим на экран информацию о поиске максимума
        // %zu - для вывода типа size_t - количество потоко
        // %d - для вывода типа int - время умножения
        // %d - для вывода типа int - эффективность
        // \t - табуляция
        printf("%zu\t %d\t\t %f\n", threads, time, efficiency);
    }

    return 0;
}

// Описание функции перемножения двух матриц
Matrix multiplyMatrices(const Matrix& m1, const Matrix& m2, size_t thread_num)
{
    // Объявим размер - он для всех матриц, их столбцов и строк одинаковый
    size_t size = m1.size();

    // Если размер нулевой, тогда вернем пустую матрицу
    if (size == 0)
        return {{}};

    // Если размер равен единице, тогда вернем перемноженные два элемента двух матриц
    if (size == 1)
        return {{m1[0][0] * m2[0][0]}};

    // по условию задачи потоков может быть от 1 до 10
    if (thread_num < 0)
        thread_num = 1;
    else if (thread_num > 10)
        thread_num = 10;

    // Потоков не должно быть больше, чем размер матрицы. То есть на каждый поток должна найтись своя задача
    if (thread_num > size)
        thread_num = size;

    // Чтобы найти количество итераций на каждый поток, нужно общее количество элементов (итераций) поделить на заданное количество потоков
    size_t iter_number = size / thread_num;

    // создаем новую матрицу
    Matrix m;

    // резервируем у нее количество строк размером size
    m.resize(size);

    // теперь для каждой строки нужно зарезервировать количество элементов размером size
    for (size_t i = 0; i < size; ++i)
    {
        m[i].resize(size);
    }

    #pragma omp parallel for schedule(static, iter_number)
    for (size_t y = 0; y < size; ++y)
    {
        //// Добавим пустую строку в новую матрицу
        //m.push_back(Vector());

        for (size_t x = 0; x < size; ++x)
        {
            // Заполним элементами первую строчку в новой матрице
            int new_element = 0;

            // Вычислим элемент, перемножив элементы матриц m1 и m2
            for (int i = 0; i < size; ++i)
            {
                new_element += m1[y][i] * m2[i][x];
            }

            //// Добавим новый элемент в новую матрицу
            //m[y].push_back(new_element);
            #pragma omp critical
            m[y][x] = new_element;
        }
    }

    return m;
}

// Описание функции печати матрицы на экран
void printMatrix(const Matrix& m)
{
    for (size_t y = 0; y < m.size(); ++y)
    {
        for (size_t x = 0; x < m[y].size(); ++x)
        {
            std::cout << m[y][x] << "\t";
        }
        std::cout << std::endl;
    }
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
std::vector<std::vector<T>> generateMatrix(size_t size, T left_border, T right_border, int generator_arg)
{
    // Если размер задан нулевой, тогда вернем пустую матрицу
    if (size == 0)
        return {{}};

    std::vector<std::vector<T>> matrix;

    // Заполним матрицу векторами случайных значений
    for (size_t i = 0; i < size; ++i)
    {
        matrix.push_back(generateVector(size, left_border, right_border, i + (size * generator_arg)));
    }

    return matrix;
}











