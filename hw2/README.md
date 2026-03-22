# Домашнее задание для лекции по теме "Процессы и потоки"

Реализована функция `ApplyFunction` для применения преобразования ко всем элементам `std::vector<T>` с поддержкой заданного числа потоков.

Добавлены:
- unit-тесты на `GoogleTest`
- benchmark на `Google Benchmark`

В benchmark показаны два случая:
- однопоточная версия быстрее на маленьком векторе и лёгкой функции
- многопоточная версия быстрее на большом векторе и тяжёлой функции

## Сборка

```bash
cd hw2
cmake -S . -B build
cmake --build build
```

## Тесты

```bash
cd hw2/build
ctest --output-on-failure
```

## Benchmark

```bash
cd hw2/build
./apply_function_benchmark
```

## Полученные результаты

### Тесты

```text
    Start 1: ApplyFunctionTest.WorksInSingleThread
1/7 Test #1: ApplyFunctionTest.WorksInSingleThread ..............   Passed    0.00 sec
    Start 2: ApplyFunctionTest.WorksInMultipleThreads
2/7 Test #2: ApplyFunctionTest.WorksInMultipleThreads ...........   Passed    0.00 sec
    Start 3: ApplyFunctionTest.ThreadCountGreaterThanDataSize
3/7 Test #3: ApplyFunctionTest.ThreadCountGreaterThanDataSize ...   Passed    0.00 sec
    Start 4: ApplyFunctionTest.EmptyVector
4/7 Test #4: ApplyFunctionTest.EmptyVector ......................   Passed    0.00 sec
    Start 5: ApplyFunctionTest.OneElement
5/7 Test #5: ApplyFunctionTest.OneElement .......................   Passed    0.00 sec
    Start 6: ApplyFunctionTest.SupportsOtherTypes
6/7 Test #6: ApplyFunctionTest.SupportsOtherTypes ...............   Passed    0.00 sec
    Start 7: ApplyFunctionTest.InvalidThreadCountThrows
7/7 Test #7: ApplyFunctionTest.InvalidThreadCountThrows .........   Passed    0.00 sec

100% tests passed, 0 tests failed out of 7
```

### Benchmark

```text
Running ./apply_function_benchmark
Run on (4 X 3393.67 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 512 KiB (x4)
  L3 Unified 65536 KiB (x1)
Load Average: 1.40, 0.71, 0.79
---------------------------------------------------------------------
Benchmark                           Time             CPU   Iterations
---------------------------------------------------------------------
BM_SingleThread_LightSmall       1602 ns         1602 ns       420678
BM_MultiThread_LightSmall      461143 ns       298864 ns         2245
BM_SingleThread_HeavyLarge 3902385112 ns   3898376353 ns            1
BM_MultiThread_HeavyLarge  1083025895 ns       615388 ns           10
```