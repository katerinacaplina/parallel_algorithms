#include<string>    // для использования строкового типа данных
#include<vector>    // для использования структуры "вектор" - последовательность однотипных данных
#include<iostream>  // для использования вывода информации на экран

// разделители
const char space = ' ';
const char newline = '\n';
const char tab = '\t';

// список разделителей
const std::vector<char> separators {space, newline, tab};

// обявление некоторых функций
int countWords(std::string line);   // подсчет слов в строке
bool isSeparator(char symbol);      // определить, является ли символ разделителем или не является

// главная функция программы - первый параметр - количество аргументов командной строки, второй - список аргументов командной строки
int assignment0(int argc, char *argv[])
{
    // набор тестовых данных - список пар, где каждая пара это "строка" и "количество слов в строке"
    std::vector<std::pair<std::string, int>> samples
    {
        std::make_pair("hello",                     1),
        std::make_pair("hello bro",                 2),
        std::make_pair("hello bro   and no bro",    5),
        std::make_pair("      ",                    0),
        std::make_pair("",                          0),
        std::make_pair(" f d",                      2),
        std::make_pair("\n  Ay Arnold!  Hello .",   4),
    };

    // пройдемся по набору тестовых данных и подсчитаем количество слов в каждой строке с помощью функции countWords()
    // и выведем результат каждого теста: PASSED (пройден) или FAILED (провален)
    for (const auto& sample: samples)
    {
        // запомним фактическое количество слов в рассматриваемом тесте
        int wordNumberInFact = sample.second; // second - это второй член пары (т.е. количество слов)
        // запомним подсчитанное количество слов в рассматриваемом тесте
        int wordNumberCalculated = countWords(sample.first); // first - это первый член пары (т.е. строка)

        // теперь если подсчитанное количество слов соответствует фактическому рассматриваемой строки
        if (wordNumberInFact == wordNumberCalculated)
        {
            // тогда выведем сообщение, что тест пройден - функция правильно подсчитала количество слов строке
            std::cout << "PASSED: " << sample.first << std::endl;
        }
        else
        {
            // иначе тест провален: выводим информацию о фактическом и подсчитанном количестве слов
            std::cout << "FAILED: " << sample.first << ". Fact: " << wordNumberInFact << "; calculated: " << wordNumberCalculated << std::endl;
        }
    }

    return 0;
}

// функция, которая проверяет, является ли символ разделителем
bool isSeparator(char symbol)
{
    for (char separator: separators)
    {
        if (separator == symbol)
        {
            return true;
        }
    }

    return false;
}

// функция, которая подсчитывает количество слов в строке
int countWords(std::string line)
{
    // по умолчанию в строке нуль слов
    int wordNumber = 0;

    // флаг, который поможет определить, есть ли буква перед разделителем
    bool letterRead = false;

    // а теперь пройдемся по строке и проверим каждый ее символ
    for (char symbol: line)
    {
        // проверяем, является ли этот символ разделителем
        if (isSeparator(symbol))
        {
            // а если перед разделителем была буква, тогда засчитаем одно слово в копилку
            if (letterRead)
            {
                // увеличивает количество слов в строке
                wordNumber += 1;
                // и показываем, что только что был разделитель, а не буква
                letterRead = false;
            }
        }
        // если попался НЕ разделитель, а буква
        else {
            // и перед этим был разделитель
            if (!letterRead)
            {
                // тогда нужно указать, что только что была буква
                letterRead = true;
            }
        }
    }

    // после того, как прошлись по каждому символу в строке, скорей всего не записали последнее слово, т.к. в конце строки нет разделителя:
    // табуляции (\t), пробела ( ) или символа новой строки (\n). Поэтому проверяем, не было ли только что буквы
    if (letterRead)
    {
        // и если всё-таки была буква, тогда нужно добавить еще одно слово
        wordNumber += 1;
    }

    return wordNumber;
}
