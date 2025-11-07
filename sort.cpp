#include "sort.hpp"

namespace {

const size_t kStaticArraySize = 5;
const bool kSwitchMin = false;
const bool kSwitchMax = true;
const size_t kLoopStart = 0;

void CreateRandomArray(int* sourceArray, size_t arraySize) {
    const int kminDistribution = 0;
    const int kmaxDistribution = 99;

    std::random_device r{};
    std::default_random_engine randomEngine(r());
    std::uniform_int_distribution distribution(kminDistribution, kmaxDistribution);
    for (size_t i = kLoopStart; i < arraySize; ++i) {
        sourceArray[i] = distribution(randomEngine);
    }
}

void CreateCloneArray(int* arrNew, int* arrOld, size_t arraySize) {
    for (size_t i = kLoopStart; i < arraySize; ++i) {
        arrNew[i] = arrOld[i];
    }
}

void CheckDynamicArrayElementsNumber(size_t elementsNumber) {
    const size_t arrayElementsMinNumber = 2;
    const size_t arrayElementsMaxNumber = 1e5;
    if (elementsNumber < arrayElementsMinNumber || elementsNumber > arrayElementsMaxNumber) {
        std::cout << "Введены неверные данные" << std::endl;
        exit(0);
    }
}

[[nodiscard]] int EnterTaskNumber() {
    int task{};
    std::cout << "Выберите (1/2/3)" << std::endl;
    std::cin >> task;
    return task;
}

[[nodiscard]] size_t EnterDynamicArrayElementsNumber() {
    size_t dynamicArrayElementsNumber{};
    std::cout << "Введите количество элементов динамического массива: (2 <= n <= 10000)" << std::endl;
    std::cin >> dynamicArrayElementsNumber;
    CheckDynamicArrayElementsNumber(dynamicArrayElementsNumber);
    return dynamicArrayElementsNumber;
}

void SelectTask() {
    std::cout << "Выберите задание:" << std::endl;
    std::cout << "1. Сортировка статичного массива" << std::endl;
    std::cout << "2. Сортировка динамического массива" << std::endl;
    std::cout << "3. Выход" << std::endl;
}

void FuncSwap(int& left, int& right) {
    int replacement = left;
    left = right;
    right = replacement;
}

void ArrayOutput(int* array, size_t arraySize) {
    for (size_t i = kLoopStart; i < arraySize; ++i) {
        std::cout << array[i] << " ";
    }
}

void TableOutput(int* array, size_t arraySize, int permutationsNumber, int comparisonsNumber) {
    const int kWidthPermutationsNumber = 32;
    const int kWidthComparisonsNumber = 32;

    std::cout << '\t' << "Количество перестановок" << '\t' << '\t' << "Количество сравнений\n";
    ArrayOutput(array, arraySize);
    std::cout << std::setw(kWidthPermutationsNumber) << permutationsNumber << std::setw(kWidthComparisonsNumber) << comparisonsNumber << '\n';
}
}  // namespace

namespace InterfaceFunctions {

void LaunchApp() {
    char continueExecution = 'y';
    while (continueExecution == 'y') {
        Menu();

        std::cout << "Продолжить (y/n)?" << std::endl;
        std::cin >> continueExecution;

        if (continueExecution != 'y' && continueExecution != 'n') {
            std::cout << "Введены неверные данные" << std::endl;
            exit(0);
        }
    }
}

void Menu() {
    SelectTask();
    int task = EnterTaskNumber();

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
            std::cout << "Работа программы завершена" << std::endl;
            exit(0);
            break;
        }
        default: {
            std::cout << "error" << std::endl;
            break;
        }
    }
}

void StaticArrayOutput() {
    int permutationsNumber = 0;
    int comparisonsNumber = 0;
    int sourceArray[kStaticArraySize]{};
    int cloneArray[kStaticArraySize]{};

    CreateRandomArray(sourceArray, kStaticArraySize);
    CreateCloneArray(cloneArray, sourceArray, kStaticArraySize);

    std::cout << "Исходный массив:\n";
    ArrayOutput(sourceArray, kStaticArraySize);

    SelectionSort(sourceArray, kStaticArraySize, kSwitchMin, permutationsNumber, comparisonsNumber);
    std::cout << "\nСортировка выбором по возрастанию 1";
    TableOutput(sourceArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
    SelectionSort(sourceArray, kStaticArraySize, kSwitchMin, permutationsNumber, comparisonsNumber);
    std::cout << "Сортировка выбором по возрастанию 2";
    TableOutput(sourceArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
    SelectionSort(sourceArray, kStaticArraySize, kSwitchMax, permutationsNumber, comparisonsNumber);
    std::cout << "Сортировка выбором по убыванию" << '\t';
    TableOutput(sourceArray, kStaticArraySize, permutationsNumber, comparisonsNumber);

    BubbleSort(cloneArray, kStaticArraySize, kSwitchMin, permutationsNumber, comparisonsNumber);
    std::cout << "Сортировка пузырьком по возрастанию 1";
    TableOutput(cloneArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
    BubbleSort(cloneArray, kStaticArraySize, kSwitchMin, permutationsNumber, comparisonsNumber);
    std::cout << "Сортировка пузырьком по возрастанию 2";
    TableOutput(cloneArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
    BubbleSort(cloneArray, kStaticArraySize, kSwitchMax, permutationsNumber, comparisonsNumber);
    std::cout << "Сортировка пузырьком по убыванию";
    TableOutput(cloneArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
}

void DynamicArrayOutput() {
    int permutationsNumber{};
    int comparisonsNumber{};
    size_t dynamicArrayElementsNumber = EnterDynamicArrayElementsNumber();

    int* sourceArray = new int[dynamicArrayElementsNumber];
    int* cloneArray = new int[dynamicArrayElementsNumber];

    CreateRandomArray(sourceArray, dynamicArrayElementsNumber);
    CreateCloneArray(cloneArray, sourceArray, dynamicArrayElementsNumber);

    SelectionSort(sourceArray, dynamicArrayElementsNumber, kSwitchMin, permutationsNumber, comparisonsNumber);
    std::cout << "Количество перестановок и сравнений:" << std::endl;
    std::cout << permutationsNumber << ' ' << comparisonsNumber << std::endl;

    BubbleSort(cloneArray, dynamicArrayElementsNumber, kSwitchMin, permutationsNumber, comparisonsNumber);
    std::cout << "Количество перестановок и сравнений: " << std::endl;
    std::cout << permutationsNumber << ' ' << comparisonsNumber << std::endl;

    delete[] sourceArray;
    delete[] cloneArray;
}

void SelectionSort(int* arrSelection, size_t arraySize, bool switchMinMax, int& permutationsNumber, int& comparisionsNumber) {
    permutationsNumber = 0;
    comparisionsNumber = 0;
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