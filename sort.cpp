// Подключение заголовочного файла с объявлениями ф-ций сортировки
#include "sort.hpp"

using namespace std;

// для скрытия вспомогательных функций от внешнего доступа
namespace {

const size_t kStaticArraySize = 5; // размер статического массива
const bool kSwitchMin = false; // флаг для сортировки по возрастанию (мин значения в начале)
const bool kSwitchMax = true; // флаг для сортировки по убыванию (макс значения в начале)
const size_t kLoopStart = 0; // нач. индекс для всех циклов

//генератор рандом чисел для массива
void CreateRandomArray(int* sourceArray, size_t arraySize) {
    const int kminDistribution = 0; // минимальное значение для генерации случайных чисел
    const int kmaxDistribution = 99; // максимальное значение для генерации случайных чисел

    random_device r{}; // источник энтропии для случайных чисел
    default_random_engine randomEngine(r()); // генератор случ.ч. с сидом от random_device
    uniform_int_distribution distribution(kminDistribution, kmaxDistribution); // равномерное распределение
    // цикл по всем элементам массива
    for (size_t i = kLoopStart; i < arraySize; ++i) {
        sourceArray[i] = distribution(randomEngine); //заполнение элемента случайным числом
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
    const size_t arrayElementsMinNumber = 2; //мин кол-во элементов
    const size_t arrayElementsMaxNumber = 1e5; //макс кол-во элементов
    //проверка диапозона
    if (elementsNumber < arrayElementsMinNumber || elementsNumber > arrayElementsMaxNumber) {
        cout << "Введены неверные данные" << endl;
        exit(0);
    }
}

// ввод номера задания пользователем
[[nodiscard]] int EnterTaskNumber() {
    int task{}; // переменная для номера задания
    cout << "Выберите (1/2/3)" << endl;
    cin >> task;
    return task;
}

// ввод кол-ва элементов динамического массива
[[nodiscard]] size_t EnterDynamicArrayElementsNumber() {
    size_t dynamicArrayElementsNumber{}; // переменная для кол-ва элементов
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
    const int kWidthPermutationsNumber = 32; // ширина колонки для количества перестановок
    const int kWidthComparisonsNumber = 32; // ширина колонки для количества сравнений

    cout << '\t' << "Количество перестановок" << '\t' << '\t' << "Количество сравнений\n";
    ArrayOutput(array, arraySize);
    cout << setw(kWidthPermutationsNumber) << permutationsNumber << setw(kWidthComparisonsNumber) << comparisonsNumber << '\n';
}
}  // namespace

// для ф-ций интерфейса
namespace InterfaceFunctions {
// запуск приложения
void LaunchApp() {
    char continueExecution = 'y'; // продолжение работы приложения
    while (continueExecution == 'y') {
        Menu();

        cout << "Продолжить (y/n)?" << endl;
        cin >> continueExecution;

        // проверка корректности ввода
        if (continueExecution != 'y' && continueExecution != 'n') {
            cout << "Введены неверные данные" << endl;
            exit(0);
        }
    }
}

void Menu() { // главное меню
    SelectTask();
    int task = EnterTaskNumber(); // ввод номера задания

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
    int permutationsNumber = 0; // счетчик перестановок
    int comparisonsNumber = 0; // счетчик сравнений
    int sourceArray[kStaticArraySize]{}; // источник статического массива
    int cloneArray[kStaticArraySize]{}; // копия статического массива

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
    int permutationsNumber{}; // счет перестановок
    int comparisonsNumber{}; // счет сравнений
    size_t dynamicArrayElementsNumber = EnterDynamicArrayElementsNumber();

    int* sourceArray = new int[dynamicArrayElementsNumber]; // выделение памяти для исходного массива
    int* cloneArray = new int[dynamicArrayElementsNumber]; // выделение памяти для массива-копии

    CreateRandomArray(sourceArray, dynamicArrayElementsNumber);
    CreateCloneArray(cloneArray, sourceArray, dynamicArrayElementsNumber);

    SelectionSort(sourceArray, dynamicArrayElementsNumber, kSwitchMin, permutationsNumber, comparisonsNumber); // сортировка выбором
    cout << "Количество перестановок и сравнений (сортировка выбором):" << endl;
    cout << permutationsNumber << ' ' << comparisonsNumber << endl;

    BubbleSort(cloneArray, dynamicArrayElementsNumber, kSwitchMin, permutationsNumber, comparisonsNumber); // сортировка пузырьком
    cout << "Количество перестановок и сравнений (сортировка пузырьком): " << endl;
    cout << permutationsNumber << ' ' << comparisonsNumber << endl;

    //освобождение памяти перестановок и сравнений
    delete[] sourceArray;
    delete[] cloneArray;
}

// алгоритм сортировки выбором
void SelectionSort(int* arrSelection, size_t arraySize, bool switchMinMax, int& permutationsNumber, int& comparisionsNumber) {
    permutationsNumber = 0;
    comparisionsNumber = 0;
    size_t rememberedIndex{}; // переменная для запоминания индекса мин/макс элемента

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

//алгоритм сортировки пузырьком
void BubbleSort(int* arrBubble, size_t arraySize, bool switchMinMax, int& permutationsNumber, int& comparisionsNumber) {
    const int kcounterInitialValue = 0;
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
