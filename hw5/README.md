# Домашнее задание для лекции по теме "lock-free, средства межпроцессного взаимодействия"

Реализована lock-free MPSC очередь сообщений, размещённая в разделяемой памяти (`shared memory`).

Очередь построена на основе кольцевого буфера и поддерживает передачу произвольных сообщений между процессами.

## Поддерживаемые сущности

- `ProducerNode` - запись сообщений в очередь, инициализация shared memory
- `ConsumerNode` - чтение сообщений с фильтрацией по типу

## Особенности реализации

- lock-free алгоритм (без мьютексов)
- несколько producer процессов
- один логический consumer (несколько процессов могут читать, но конкурируют за данные)
- фильтрация сообщений по типу
- автоматическое завершение consumer при отсутствии producer
- автоматическое удаление shared memory последним producer

## Сборка

``` bash
cmake -S . -B build
cmake --build build
```

## Запуск

### Терминал 1

``` bash
./build/producer 1
```

### Терминал 2

``` bash
./build/consumer 1
```

### Терминал 3

``` bash
./build/producer 2
```

### Терминал 4

``` bash
./build/consumer 2
```

## Проверка

- producer 1 отправляет текстовые сообщения
- producer 2 отправляет числовые сообщения
- consumer читает только сообщения заданного типа

После завершения всех producer:

- consumer автоматически завершает работу
- shared memory удаляется

## Полученные результаты

``` text
[producer 1] sent TEXT: hello from producer 1 #56
[producer 2] sent NUMBER: 47

[consumer] message
  type: 1
  size: 26
  data (text): hello from producer 1 #56

[consumer] message
  type: 2
  size: 4
  data (int): 47

[consumer] no producers left, exiting

[producer] shutting down...
[producer] skip cleanup (others still running)

[producer] shutting down...
[producer] shared memory removed
```