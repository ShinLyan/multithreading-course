# Домашнее задание для лекции по теме "Примитивы синхронизации"

Реализован потокобезопасный буферизированный канал `BufferedChannel<T>`.

Поддерживаются операции:
- `Send` – отправка значения (блокируется при полном буфере)
- `Recv` – получение значения (блокируется при пустом буфере)
- `Close` – закрытие канала

## Сборка

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Тесты

```bash
./build/buffered_channel_test
```

## Benchmark

```bash
./build/buffered_channel_benchmark
```

## Полученные результаты

### Тесты

```text
[==========] Running 6 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 6 tests from Correctness
[ RUN      ] Correctness.Simple
[       OK ] Correctness.Simple (300 ms)
[ RUN      ] Correctness.Senders
[       OK ] Correctness.Senders (302 ms)
[ RUN      ] Correctness.Receivers
[       OK ] Correctness.Receivers (301 ms)
[ RUN      ] Correctness.SmallBuf
[       OK ] Correctness.SmallBuf (361 ms)
[ RUN      ] Correctness.BigBuf
[       OK ] Correctness.BigBuf (312 ms)
[ RUN      ] Correctness.Random
[       OK ] Correctness.Random (303 ms)
[----------] 6 tests from Correctness (1881 ms total)

[----------] Global test environment tear-down
[==========] 6 tests from 1 test suite ran. (1881 ms total)
[  PASSED  ] 6 tests.
```

### Benchmark

```text
Running ./build/buffered_channel_benchmark
Run on (4 X 3393.67 MHz CPU s)
Load Average: 2.50, 1.00, 0.62
-----------------------------------------------------------------------------------------------
Benchmark                                                     Time             CPU   Iterations
-----------------------------------------------------------------------------------------------
Run/2/2/2/min_time:0.100/process_time/real_time           13670 ms        12352 ms            1
Run/10/2/2/min_time:0.100/process_time/real_time           2691 ms         2315 ms            1
Run/100000/2/2/min_time:0.100/process_time/real_time       24.9 ms         38.9 ms            5
```