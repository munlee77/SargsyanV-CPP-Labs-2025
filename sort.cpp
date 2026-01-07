// Подключение заголовочного файла с объявлениями ф-ций сортировки
#include "sort.hpp"

using namespace std;

// для скрытия вспомогательных функций от внешнего доступа
namespace {

// размер статического массива
const size_t kStaticArraySize = 5;
// флаг для сортировки по возрастанию (мин значения в начале)
const bool kSwitchMin = false;
// флаг для сортировки по убыванию (макс значения в начале)
const bool kSwitchMax = true;
// нач. индекс для всех циклов
const size_t kLoopStart = 0;
const size_t kMultiplierTen = 10;
const size_t kMultiplierOne = 1;
const int kcounterInitialValue = 0;

// генератор рандом чисел для массива
void CreateRandomArray(int* sourceArray, size_t arraySize) {
    // минимальное значение для генерации случайных чисел
    const int kminDistribution = 0;
    // максимальное значение для генерации случайных чисел
    const int kmaxDistribution = 99;

    // источник энтропии для случайных чисел
    random_device r{};
    // генератор случ.ч. с сидом от random_device
    default_random_engine randomEngine(r());
    // генератор случ.ч. с сидом от random_device
    uniform_int_distribution distribution(kminDistribution, kmaxDistribution);
    // цикл по всем элементам массива
    for (size_t i = kLoopStart; i < arraySize; ++i) {
        // заполнение элемента случайным числом
        sourceArray[i] = distribution(randomEngine);
    }
}

// ф-ция копирования массива в новый массив
void CreateCloneArray(int* arrNew, int* arrOld, size_t arraySize) {
    for (size_t i = kLoopStart; i < arraySize; ++i) {
        arrNew[i] = arrOld[i];
    }
}

// ф-ция проверки корректности кол-ва элементов динамического массива
void CheckDynamicArrayElementsNumber(size_t elementsNumber) {
    // мин кол-во элементов
    const size_t arrayElementsMinNumber = 2;
    // макс кол-во элементов
    const size_t arrayElementsMaxNumber = 1e5;
    // проверка диапозона
    if (elementsNumber < arrayElementsMinNumber || elementsNumber > arrayElementsMaxNumber) {
        cout << "Введены неверные данные" << endl;
        exit(0);
    }
}

// ввод номера задания пользователем
[[nodiscard]] int EnterTaskNumber() {
    // переменная для номера задания
    int task{};
    cout << "Выберите (1/2/3)" << endl;
    cin >> task;
    return task;
}

// ввод кол-ва элементов динамического массива
[[nodiscard]] size_t EnterDynamicArrayElementsNumber() {
    // переменная для кол-ва элементов
    size_t dynamicArrayElementsNumber{};
    cout << "Введите количество элементов динамического массива: (2 <= n <= 10000)" << endl;
    cin >> dynamicArrayElementsNumber;
    CheckDynamicArrayElementsNumber(dynamicArrayElementsNumber);
    return dynamicArrayElementsNumber;
}

// выбор задания (меню)
void SelectTask() {
    cout << "Выберите задание:" << endl;
    cout << "1. Сортировка статического массива" << endl;
    cout << "2. Сортировка динамического массива" << endl;
    cout << "3. Выход" << endl;
}

// обмен значений двух int переменных
void FuncSwap(int& left, int& right) {
    int replacement = left;
    left = right;
    right = replacement;
}

// вывод элементов массива
void ArrayOutput(int* array, size_t arraySize) {
    for (size_t i = kLoopStart; i < arraySize; ++i) {
        cout << array[i] << " ";
    }
}

// форматированный вывод результатов сортировки в виде таблицы
void TableOutput(int* array, size_t arraySize, int permutationsNumber, int comparisonsNumber) {
    // ширина колонки для количества перестановок
    const int kWidthPermutationsNumber = 32;
    // ширина колонки для количества сравнений
    const int kWidthComparisonsNumber = 32;

    cout << '\t' << "Количество перестановок" << '\t' << '\t' << "Количество сравнений\n";
    ArrayOutput(array, arraySize);
    cout << setw(kWidthPermutationsNumber) << permutationsNumber << setw(kWidthComparisonsNumber) << comparisonsNumber << '\n';
}
}  // namespace
// namespace

// для ф-ций интерфейса
namespace InterfaceFunctions {
// запуск приложения
void LaunchApp() {
    char continueExecution = 'y';
    while (continueExecution == 'y') {
        Menu();

        cout << "Продолжить (y/n)?" << endl;
        cin >> continueExecution;

        if (continueExecution != 'y' && continueExecution != 'n') {
            cout << "Введены неверные данные" << endl;
            exit(0);
        }
    }
}

// главное меню
void Menu() {
    SelectTask();
    // ввод номера задания
    int task = EnterTaskNumber();

    // преобразование номера в <ArrayType> и выбор варианта
    switch (static_cast<ArrayType>(task)) {
        case ArrayType::staticArr: {
            StaticArrayOutput();
            break;
        }
        case ArrayType::dynamicArr: {
            DynamicArrayOutput();
            break;
        }
        case ArrayType::exit: {
            cout << "Работа программы завершена" << endl;
            exit(0);
            break;
        }
        default: {
            cout << "error" << endl;
            break;
        }
    }
}

// статический массив
void StaticArrayOutput() {
    // счетчик перестановок
    int permutationsNumber = 0;
    // счетчик сравнений
    int comparisonsNumber = 0;
    // источник статического массива
    int sourceArray[kStaticArraySize]{};
    // копия статического массива
    int cloneArray[kStaticArraySize]{};

    CreateRandomArray(sourceArray, kStaticArraySize);
    CreateCloneArray(cloneArray, sourceArray, kStaticArraySize);

    cout << "Исходный массив:\n";
    ArrayOutput(sourceArray, kStaticArraySize);

    SelectionSort(sourceArray, kStaticArraySize, kSwitchMin, permutationsNumber, comparisonsNumber);
    cout << "\nСортировка выбором по возрастанию 1";
    TableOutput(sourceArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
    SelectionSort(sourceArray, kStaticArraySize, kSwitchMin, permutationsNumber, comparisonsNumber);
    cout << "Сортировка выбором по возрастанию 2";
    TableOutput(sourceArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
    SelectionSort(sourceArray, kStaticArraySize, kSwitchMax, permutationsNumber, comparisonsNumber);
    cout << "Сортировка выбором по убыванию" << '\t';
    TableOutput(sourceArray, kStaticArraySize, permutationsNumber, comparisonsNumber);

    BubbleSort(cloneArray, kStaticArraySize, kSwitchMin, permutationsNumber, comparisonsNumber);
    cout << "Сортировка пузырьком по возрастанию 1";
    TableOutput(cloneArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
    BubbleSort(cloneArray, kStaticArraySize, kSwitchMin, permutationsNumber, comparisonsNumber);
    cout << "Сортировка пузырьком по возрастанию 2";
    TableOutput(cloneArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
    BubbleSort(cloneArray, kStaticArraySize, kSwitchMax, permutationsNumber, comparisonsNumber);
    cout << "Сортировка пузырьком по убыванию";
    TableOutput(cloneArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
}

// динамический массив
void DynamicArrayOutput() {
    // счет перестановок
    int permutationsNumber{};
    // счет сравнений
    int comparisonsNumber{};
    size_t dynamicArrayElementsNumber = EnterDynamicArrayElementsNumber();

    // Множители для создания массивов разного размера
    const size_t multipliers[] = {1, 10, 100};

    for (size_t multiplier : multipliers) {
        size_t currentSize = dynamicArrayElementsNumber * multiplier;

        cout << "\nРабота с массивом размером " << currentSize;
        if (multiplier == kMultiplierOne) {
            cout << " (исходный размер)" << endl;
        } else if (multiplier == kMultiplierTen) {
            cout << " (исходный * 10)" << endl;
        } else {
            cout << " (исходный * 100)" << endl;
        }

        // выделение памяти
        int* sourceArray = new int[currentSize];
        int* cloneArray = new int[currentSize];

        // заполнение массивов
        CreateRandomArray(sourceArray, currentSize);
        CreateCloneArray(cloneArray, sourceArray, currentSize);

        // сортировка выбором
        SelectionSort(sourceArray, currentSize, kSwitchMin, permutationsNumber, comparisonsNumber);
        cout << "Сортировка выбором: перестановок = " << permutationsNumber << ", сравнений = " << comparisonsNumber << endl;

        // сортировка пузырьком
        BubbleSort(cloneArray, currentSize, kSwitchMin, permutationsNumber, comparisonsNumber);
        cout << "Сортировка пузырьком: перестановок = " << permutationsNumber << ", сравнений = " << comparisonsNumber << endl;

        // освобождение памяти
        delete[] sourceArray;
        delete[] cloneArray;
    }

    cout << endl;
}

// алгоритм сортировки выбором
void SelectionSort(int* arrSelection, size_t arraySize, bool switchMinMax, int& permutationsNumber, int& comparisionsNumber) {
    permutationsNumber = 0;
    comparisionsNumber = 0;
    // переменная для запоминания индекса мин/макс элемента
    size_t rememberedIndex{};

    for (size_t i = kLoopStart; i < arraySize; i++) {
        rememberedIndex = i;

        for (size_t t = i + 1; t < arraySize; t++) {
            if (switchMinMax ? arrSelection[t] > arrSelection[rememberedIndex] : arrSelection[t] < arrSelection[rememberedIndex]) {
                rememberedIndex = t;
            }
            ++comparisionsNumber;
        }

        if (arrSelection[i] != arrSelection[rememberedIndex]) {
            FuncSwap(arrSelection[i], arrSelection[rememberedIndex]);
            ++permutationsNumber;
        }
    }
}

// алгоритм сортировки пузырьком
void BubbleSort(int* arrBubble, size_t arraySize, bool switchMinMax, int& permutationsNumber, int& comparisionsNumber) {
    int counter = 1;
    permutationsNumber = 0;
    comparisionsNumber = 0;
    int occupiedCellsNumber = 0;

    while (counter != kcounterInitialValue) {
        counter = kcounterInitialValue;
        for (size_t i = kLoopStart; i < arraySize - 1 - occupiedCellsNumber; ++i) {
            if (switchMinMax ? arrBubble[i] < arrBubble[i + 1] : arrBubble[i] > arrBubble[i + 1]) {
                FuncSwap(arrBubble[i], arrBubble[i + 1]);
                ++counter;
                ++permutationsNumber;
            }
            ++comparisionsNumber;
        }
        ++occupiedCellsNumber;
    }
}

}  // namespace InterfaceFunctions
// namespace InterfaceFunctions
