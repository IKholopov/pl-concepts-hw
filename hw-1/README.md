## VNVM-ассемблер:
### Регистры:
 - r0-r9 - общее назначение
 - sp - StackPointer
 - lr - LinkRegister (адресс возврата)
 - pc - ProcessCounter (текущая операция)

### "Загловки"
 - .stack (n) - размер стэка, который довыделится машиной при первой загрузке образа
 - .entry (label) - точка входа
 - .ascii "(some str)" - статическая строка
 - .number #some_n - статическое число
 - (label): - лэйбл
### Инструкции
 - mov (reg), (reg/#some_n) - мув
 - push { (reg/reg1, reg2/reg1 - reg2) } - пуш регистра/пары регистров/всех регистров отрезка на стэк
 - pop { (reg/reg1, reg2/reg1 - reg2) } - поп регистра/пары регистров/всех регистров отрезка из стэка
 - add (reg), (reg/$some_n) - сложение (+=)
 - sub (reg), (reg/$some_n) - вычетание (-=)
 - inc (reg) - инкремент
 - dec (reg) - декремент
 - cmp (regA), (regB/#some_n) - сравнение, запись в r0: 0 - regA==regB, >0 - regA > regB, <0 - regA < regB
 - jeq (label) -  прыжок на label, если r0 == 0
 - bl (label) - вызов лэйбла (прыжок с записью адресса возврата)
 - printf - Печать нуль-терминированной строки по адресу [r0]
 - printn - Печать числа r0
 - scann - Ввод чила в r0

### Комментарии
В случае, если pc == 0, программа находится в терминальном состоянии, она закончила работу

## Комманды VNVM/VNAsm/VNDasm

### VNVM

#### Запуск
```
./VNVM #Запуск
./VNVM <бинарный файл> #Запуск с предварительной загрузкой образа
```
#### Комманды
 - run - запуск исполнения загрузанного образа до достижения терминального состояния
 - load <путь> - загрузить образ в машину
 - step - сделать один шаг итерации образа
 - regs - показать содержимое регистров
 - stack <n> - показать n последних элементов стэка
 - dump <путь> - сохранение текущего образа
 - exit - выйти

### VNAsm

```
./VNAsm <путь к ассемблеру> #Транслировать в образ с именем <путь к ассемблеру>.bin
./VNAsm <путь к ассемблеру> <путь к бинарному образу> #Транслировать в образ
```

### VNDasm

```
./VNDasm <путь к образу> #Транслировать в ассемблер с именем <путь к образу>.vn
./VNDasm <путь к бинарному образу> <путь к ассемблеру> #Транслировать в ассемблер
```