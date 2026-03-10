# LLVM Pass

## Цель задачи

Цель задачи заключалась в том, чтобы написать **пасс-плагин** в **LLVM**, который бы строил **Control Flow Graph** и **Data Flow Graph** для инструкций, а так же переходов между базовыми блоками и вызовов функций.

## Реализованный граф

### Описание графа

В реализованном графе строятся **CFG** и **DFG** для инструкций внутри одного модуля. Инструкции поделены подграфами на базовые блоки, эти блоки поделены подграфами на функции, а их объединяет один общий подграф.

Синими нодами представлены инструкции, а зелёными - параметры инструкций. На графе **красными стрелками** показан **CFG** внутри одной функции, а **вызовы функций** отображены **синими стрелками**. Функции, которые **не имеют тела внутри данного модуля** представлены в виде **жёлтых нод**. **Чёрными** же стрелками показан **DFG**.

### Примеры графа

#### Граф для программы из файла [fact.c](examples/fact.c)

Во-первых я проверил свой граф на примере из репозитория [llvm_course](https://github.com/lisitsynSA/llvm_course/tree/main). Граф можно посмотреть в файле [fact.svg](data/fact.svg).

<details>
<summary> Содержимое программы fact.c из папки examples </summary>

``` C++
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t fact(uint64_t arg) {
  uint64_t res = 0;
  if (arg < 2) {
    res = 1;
  } else {
    uint64_t next = fact(arg - 1);
    res = arg * next;
  }
  return res;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: 1 argument - factorial len\n");
    return 1;
  }
  uint64_t arg = atoi(argv[1]);
  if (errno == 0) {
    printf("Fact(%lu) = %lu\n", arg, fact(arg));
  } else {
    printf("Usage: 1 argument - factorial len\n");
    return 1;
  }
  return 0;
}
```

</details>

<div style="width: 100%; height: 800px; overflow: auto; border: 3px solid #707070;">
  <img src="data/fact.svg" alt="fact.c" style="max-height: none; max-width: 100%;">
</div>

#### Граф для программы из файла [stack.cpp](examples/Stack/stack.cpp)

Так же в качестве примера графа относительно большого модуля можно привести граф, построенный для файла из моего репозитория [Stack](https://github.com/Dinichthys/Stack). Содержимое файла можно посмотреть в файле [stack.cpp](examples/Stack/stack.cpp). Граф же можно посмотреть в файле [stack.svg](data/stack.svg).

<div style="width: 100%; height: 800px; overflow: auto; border: 3px solid #707070;">
  <img src="data/stack.svg" alt="stack.cpp" style="max-height: none; max-width: 1000%;">
</div>

## Build

Для сборки проекта запустите следующий скрипт

``` bash
cmake -S . -B build
cmake --build build
```

## Usage

Для использования плагина запустить следующий скрипт

``` bash
sh run.sh ваш_файл_для_анализа.cpp
```
