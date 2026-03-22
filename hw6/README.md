# Домашнее задание для лекции по теме "Structured concurrency"

Реализован пул потоков `ThreadPool` с поддержкой асинхронного выполнения задач и собственного аналога `Future`.

## Поддерживаемые сущности

- `ThreadPool` – выполнение задач в пуле потоков
- `Future<T>` – получение результата выполнения задачи

## Сборка

```bash
cmake -S . -B build
cmake --build build
```

## Запуск тестов

```bash
./build/thread_pool_tests
```

## Проверка

Тестами проверяется:

- выполнение большого количества задач
- корректность результатов
- обработка исключений
- поддержка `void` задач
- работа `Future` (`Get`, `Wait`, `IsReady`, `Valid`)
- вложенные задачи (`nested submit`)
- завершение всех задач в деструкторе
- передача аргументов в `Submit`

## Полученные результаты

```text
Running main() from ./googletest/src/gtest_main.cc
[==========] Running 11 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 10 tests from ThreadPool
[ RUN      ] ThreadPool.SimpleExecution
[       OK ] ThreadPool.SimpleExecution (1 ms)
[ RUN      ] ThreadPool.FutureResult
[       OK ] ThreadPool.FutureResult (0 ms)
[ RUN      ] ThreadPool.FutureException
[       OK ] ThreadPool.FutureException (0 ms)
[ RUN      ] ThreadPool.VoidTask
[       OK ] ThreadPool.VoidTask (0 ms)
[ RUN      ] ThreadPool.ManyTasks
[       OK ] ThreadPool.ManyTasks (0 ms)
[ RUN      ] ThreadPool.WaitAndIsReady
[       OK ] ThreadPool.WaitAndIsReady (50 ms)
[ RUN      ] ThreadPool.NestedSubmit
[       OK ] ThreadPool.NestedSubmit (0 ms)
[ RUN      ] ThreadPool.DestructorWaitsTasks
[       OK ] ThreadPool.DestructorWaitsTasks (21 ms)
[ RUN      ] ThreadPool.SubmitWithArguments
[       OK ] ThreadPool.SubmitWithArguments (4 ms)
[ RUN      ] ThreadPool.SubmitWithDifferentTypes
[       OK ] ThreadPool.SubmitWithDifferentTypes (0 ms)
[----------] 10 tests from ThreadPool (81 ms total)

[----------] 1 test from Future
[ RUN      ] Future.ValidAndDoubleGet
[       OK ] Future.ValidAndDoubleGet (0 ms)
[----------] 1 test from Future (0 ms total)

[----------] Global test environment tear-down
[==========] 11 tests from 2 test suites ran. (81 ms total)
[  PASSED  ] 11 tests.
```