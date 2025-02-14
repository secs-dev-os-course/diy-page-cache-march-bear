# ОС 2024. Лабораторная работа №2

```
Выполнил: Марухленко Иван Сергеевич
Группа: P3333
Вариант: Second Chance Cache
```

## Использование

1. Выполнить скрипты:
```shell
ci/prepare.bash
ci/format.bash check
```

2. Директория build/app будет содержать файлы:

`io-lat-write` - неизмененный нагрузчик из lab-1 <br />
`io-lat-write-2` - нагрузчик, использующий file-api <br />
`libfile-api.so` - разделяемая библиотека file-api

3. Запуск нагрузчиков:
```shell
./io-lat-write <number-of-iterations> <filename>
./io-lat-write-2 <number-of-iterations> <filename>
```

