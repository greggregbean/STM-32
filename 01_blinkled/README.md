# Лабораторная работа №01: Мигание Диодом

## Описание лабораторной работы

В этой работе будут представлены основы работы с микроконтроллером STM32F051:
- Процесс программирования и прошивки микроконтроллера;
- Документация на микроконтроллер;
- Простейший пример программы;
- Симуляция работы программы в среде моделирования QEMU;

В работе будет очень много ссылок на документацию (даташит) STM32F051, делается это намеренно. Дело в том, что именно документация (а не туториалы в интернете) дает полное представление о микроконтроллере. В профессиональной работе вы обязательно встретитесь с проблемами, которые не получится решить без внимательного прочтения даташита. Полезно уже в самом начале приучать себя к чтению документации на английском языке.

Также в этой работе есть ссылки на документы и видео. **Обязательно просматривайте документы и видео!** Невозможно описать все аспекты STM32 в методичке к лабораторной работе.

## Подготовка

Для программирования микроконтроллера необходимо установить минимальный набор программ - toolchain. Инструкция для установки toolchain-а под Linux находится [здесь](../../Toolchain-installation-guide.md).

Перед началом работы с микроконтроллером необходимо просмотреть документацию по нему. В этой лабораторной работе мы будем работать только с двумя документами:
- [stm32f0xx_rm](docs/stm32f0xx_rm.pdf) - полное описание памяти и периферии микроконтроллера STM32.
- [stm32f0discovery](docs/stm32f0discovery.pdf) - описание отладочной платы.

Документы снабжены комментариями, чтобы вам удобнее было ориентироваться по ним. Список всех комментарии можно посмотреть, например, в программе Adobe Acrobat Reader. Подойдет бесплатная десктопная версия или онлайн-версия. В онлайн-версии список всех комментариев выглядит вот так:

![imgs/comments.png](imgs/comments.png)

### Запуск кода на QEMU

В данном репозитории находится код, который мигает светодиодом. Перед погружением в файлы репозитория и их изучения, убедимся в том, что код работает.

Вероятно, что сначала придётся удалить версию программы, скомпилированную под отладочную плату:
```
make clean
```

Скомпилируем версию программы для эмулятора QEMU и запустим её.
```
make QEMU=1
make qemu
```

При этом откроется новое окно с изображением отладочной платы, в котором мигание светодиода символизируется появлением и исчезновением синего прямоугольника около надписи LD4:

![imgs/qemu_led.png](imgs/qemu_led.png)

Эмулятор QEMU является очень сильным инструментом для тестирования системного программного обеспечения и прошивок, однако убедиться в этом получится только после освоения отладчика `gdb`.

## Описание файлов в репозитории

Приступим к описанию файлов в репозитории:
- [Makefile](Makefile) - файл для сборки проекта;
- [blinkled.c](blinkled.c) - тот самый код на Си, который управляет светодиодами;
- [entry.S](entry.S) - код на языке ассемблер, который выполняется до функции main, мы рассмотрим его подробно в следующих лабораторных;
- [entry.lds](entry.lds) - линкер-скрипт - определяет как в конечной программе должны располагаться секции, мы рассмотрим его подробно в следующих лабораторных.

После сборки также появятся файлы:
- build/blinkled.o, build/entry.o - объектные файлы, получаемые из скомпилированных [blinkled.c](blinkled.c) и [entry.S](entry.S);
- build/blinkled.elf - elf-файл, сдержащий бинарный исходный код, информацию об адресах загрузки секций исходного кода, отладочные символы и многое другое.
- build/blinkled.bin - бинарный файл, содержащий в себе весь исполняемый бинарный код и только его. Всё уже расположено по нужным сдвигам внутри файла;

## Описание процесса сборки

Описание синтаксиса Makefile-ов выходит за рамки данного курса. Если вы не знакомы с синтаксисом Makefile, то рекомендуем [данный туториал](https://makefiletutorial.com/) и ![документацию на Make](https://www.gnu.org/software/make/manual/make.html).

Давайте соберем наш проект и создадим исполняемый файл. Для начала создадим папку build:
```
mkdir build
```
Теперь сгенерируем объектный файл из ассемблерного [entry.S](entry.S):
```
arm-none-eabi-gcc -march=armv6-m -mcpu=cortex-m0 -o build/entry.o -c entry.S
```
`arm-none-eabi-gcc` - кросс-компилятор, а `-march=armv6-m -mcpu=cortex-m0` - флаги, которые указывают на то, что мы собираем код для ARM процессора с ядром cortex-M0.
Теперь создадим объектный файл из [blinkled.c](blinkled.c):
```
arm-none-eabi-gcc -march=armv6-m -mcpu=cortex-m0 -o build/blinkled.o -c blinkled.c
```
И осталось создать исполняемый .elf файл:
```
arm-none-eabi-gcc -nostdlib -march=armv6-m -mcpu=cortex-m0 -Wl,-T,entry.lds build/entry.o build/blinkled.o -o build/blinkled.elf
```
Смысл флага `-nostdlib` можно посмотреть [в документации gnu](https://gcc.gnu.org/onlinedocs/gcc-4.4.7/gcc/Link-Options.html#:~:text=option%20is%20specified.-,%2Dnostdlib,used%20to%20ensure%20C%2B%2B%20constructors%20will%20be%20called%3B%20see%20collect2.). Флаг `-Wl,-T,entry.lds` передает линкеру линкер-скрипт, согласно которому линкер должен будет разместить секции.

Последний шаг. Создадим файл, который можно будет загрузить в микроконтроллер:
```
arm-none-eabi-objcopy -O binary build/blinkled.elf build/blinkled.bin
```
Готово! Чтобы не писать всё это при каждом изменении проекты как раз и нужен Makefile. Вместо всех этих строчек можно написать `make`. Чтобы удалить все исполняемые файлы, достаточно исполнить `make clean`.

## Описание blinkled.c

### Мигание диодом

Теперь мы можем приступить непосредственно к программированию микроконтроллеров. **Самая главная концепция программирования STM32**, которую нужно держать в голове - это то, что управление переферией контроллера происходит двумя способами:
- Специальный ассемблерные инструкции. В данной лабораторной работе мы не будем их касаться;
- Запись информации в специальные участки памяти, которые называются регистрами.

Именно изменяя информацию в регистрах можно управлять микроконтроллером. Рассмотрим для примера следующую команду, которая находится внизу файла [blinked.c](blinked.c):
```C
*(volatile uint32_t*)(uintptr_t)0x48000814U |=  0x100U;
```
Данная команда записывает единицу в восьмой бит по адресу `0x48000814`. Выставление единицы по этому адресу включает синий светодиод на отладочной плате!

А вот следующая команда выставляет в этом же бите 0 и выключает синий светодиод.
```C
*(volatile uint32_t*)(uintptr_t)0x48000814U &= ~0x100U;
```
Любой, владеющий булевой логикой и синтаксисом языка Си может убедиться сам, что данные команды действительно выставляют единицу и ноль в восьмом бите по адресу `0x48000814` и, что важно, не изменяют значение других битов.

Каким же образом мы определили, что восьмой бит по адресу `0x48000814` управляет состоянием синего светодиода на отладочной плате? Давайте заглянем в документацию!

Во-первых, скачайте Adobe Acrobat Reader, если вы этого еще не сделали, или зарегистрируйтесь в онлайн версии. Далее, откройте документы из папки [docs](docs/) в Adobe Acrobat. Теперь заглянем в [stm32f0discovery](docs/stm32f0discovery.pdf). Мы выделили те строчки документации, которые понадобятся нам в этой лабораторной работе. Итак, в документации на плату написано:

> User LD4: Blue user LED connected to the I/O PC8 of the STM32F051R8T6.

Значит, синий светодиод подключен к ножке микроконтроллера под названием PC8. В микроконтроллере много выходных ножек, для удобства их группируют. PC8 означает, что выходная ножка принадлежит к группе ножек С, а её номер равен 8. Группу ножек С будем обозначать GPIOC (General Purpose Input Output C).
Теперь заглянем в [stm32f0xx_rm](docs/stm32f0xx_rm.pdf).

В списке всех комментариев вы можете увидеть вот такую табличку:

![imgs/GPIOC.png](imgs/GPIOC.png)

Значит, регистры, управляющие GPIOC находятся в диапозоне адресов `0x48000800-0x48000BFF`.

Правее ссылка на описание всех управляющих регистров. Если перейти по ней, то можно увидеть огромную таблицу, в которой перечислены все биты, контролируюшие GPIOC. Нас интересуют биты, которые выставляют 0 или 1 на выходе ножки, они находятся в регистре GPIOx_ODR, где x - порт GPIO. Из таблицы мы видим, что `offset` у этого регистра равен `0x14`. Под `offset` подразумевается смещение относительно первого регистра, управляющего данной группой. В нашем случае это `0x48000800`.

Итого, получается `0x48000800 + 0x14 = 0x48000814`, а это как раз тот самый регистр, в который мы записываем данные в нашей программе!

Теперь подробнее посмотрим на GPIOx_ODR. Перемотайте чуть выше до описание этого регистра, вы должны видеть это:
![imgs/GPIOx_ODR.png](imgs/GPIOx_ODR.png)

Здесь все просто, i-ый бит соответсвует i-ой ножке в группе ножек С. А значит мы должны менять `0x100` бит в `0x48000814`, что мы и делаем в коде. Попробуйте самостоятельно изменить код так, чтобы мигал не синий, а зеленый светодиод.

### Настройка тактирования микроконтроллера

Теперь, когда мы посмотрели на примере как происходит управление микроконтроллером, мы можем понять что происходит в [blinked.c](blinked.c). Начнем с первой функцией, которая вызвается в main - функция включения тактирования.

Чтобы понять что происходит рекомендуется посмотреть видео про [тактирование в STM32](https://www.youtube.com/watch?v=PP94Q0OYLkY&list=PLhtMaaf_npBzsEQ94eGn5RnuE-VdGVObR&index=5).

```C
void board_clocking_init()
{
    // (1) Clock HSE and wait for oscillations to setup.
    *REG_RCC_CR = 0x00010000U;
    while ((*REG_RCC_CR & 0x00020000U) != 0x00020000U);

    // (2) Configure PLL:
    // PREDIV output: HSE/2 = 4 MHz
    *REG_RCC_CFGR2 |= 1U;

    // (3) Select PREDIV output as PLL input (4 MHz):
    *REG_RCC_CFGR |= 0x00010000U;

    // (4) Set PLLMUL to 12:
    // SYSCLK frequency = 48 MHz
    *REG_RCC_CFGR |= (12U-1U) << 18U;

    // (5) Enable PLL:
    *REG_RCC_CR |= 0x01000000U;
    while ((*REG_RCC_CR & 0x02000000U) != 0x02000000U);

    // (6) Configure AHB frequency to 48 MHz:
    *REG_RCC_CFGR |= 0b000U << 4U;

    // (7) Select PLL as SYSCLK source:
    *REG_RCC_CFGR |= 0b10U;
    while ((*REG_RCC_CFGR & 0xCU) != 0x8U);

    // (8) Set APB frequency to 24 MHz
    *REG_RCC_CFGR |= 0b001U << 8U;
}
```
Здесь мы последовательно выставляем биты в нужных регистрах. (i) указывает на комментарий в документе [stm32f0xx_rm](docs/stm32f0xx_rm.pdf). Согласно этому документу и видео выше запускается тактирование на микроконтроллере.

Остановимся на функции board_gpio_init() и внимательно ее рассмотрим.

```C
void board_gpio_init()
{
    // (1) Enable GPIOC clocking:
    *REG_RCC_AHBENR |= 0x80000U;

    // (2) Configure PC8 mode:
    *GPIOC_MODER |= 0b01U << (2*8U);

    // (3) Configure PC8 type:
    *GPIOC_TYPER |= 0b0U << 8U;
}
```
Упоминание о REG_RCC_AHBENR в документе находится здесь:
![imgs/RCC.png](imgs/RCC.png)

Значит, регистры, которые управляют тактирование GPIOC находятся по адресу `0x40021000` - `0x400213FF`. Нас интересует регистр под названием RCC_AHBENR, рассмотрим его:

![imgs/RCC_more.png](imgs/RCC_AHBENR.png)

Из документации понятно, что бит IOPCEN отвечает за включение тактирования на шине GPIOC. В Регистре RCC_AHBENR он находится на 19 позицией, `2^19 = 0x80000`.

Рассмотрим GPIOC_MODER. На самом деле он равен уже знакомому нам регистру `0x48000800`. Это связано с тем, что offset у этого регистра равен 0.

![imgs/RCC_more.png](imgs/GPIOC_MODER.png)

Для того, чтобы управлять светодиодом мы должны правильно выставить режим работы ножки, к которой подключен светодиод. Делается это выставление 01 по адресу MODER8[1:0], MODERi отвечает за i-ый пин. Именно это и делает вторая команда функции board_gpio_init().

Итак, осталась последния команда. Самостоятельно найдите определение GPIOC_TYPER в даташите и определите что делает последняя команда. Будьте внимательны, GPIOC_TYPER - это просто define адреса регистра в файле blinked.c, в даташите регистр по этому адресу называется иначе.

## Задачи к лабораторной №01
- [ ] Запустить код в эмуляторе.
- [ ] Проделать руками процесс компиляции минимального примера хотя бы один раз. Оценить ускорение процесса компиляции программы при использовании Makefile-ов в сравнении с ручной сборкой.
- [ ] Отрефакторить код:
    - [ ] Использовать регистры только по их именам.

        В реальных проектах никогда не обращаются к памяти напрямую, как мы это делали здесь: `*(volatile uint32_t*)(uintptr_t)0x48000814U |= 0x100U;` Потому что это небезопасно, несложно сделать ошибку при написании `0x48000418` и невозможно держать в голове таблицу всех регистров микроконтроллера и значения всех бит всех регистров, для этих целей пишутся define-ы. В blinked.c уже определены define для некоторых адресов. Ваша задача состоит в том, чтобы определить define для всех адресов файла. И название define должно совпадать с названием регистра в даташите!
    - [ ] Использовать биты регистров только по их именам
        Из названий макросов должно быть понятно, какой смысл у той или иной записи бита в регистр.
- [ ] Написать удобный макрос для работы с регистрами и битами
```C
// Все невыставленные биты MODIFYMASK регистра REG останутся неизменными
// Все выставленные биты MODIFYMASK
#define MODIFY_REG(REG, MODIFYMASK, VALUE)

```
- [ ] Изменить программу так, чтобы плата попеременно мигала синим и зелёным диодами.