# Задача: Реализация скальпер-бота на C++ с использованием FAST/FIX

## Описание
Реализовать торгового скальпера по архитектуре, приведённой в пользовательском запросе. Использовать C++17, структуру из файлов SimpleScalper.h, SimpleScalper.cpp, main.cpp, CMakeLists.txt. В качестве FIX-движка выбран QuickFIX (https://github.com/quickfix/quickfix) — полностью бесплатная и open-source библиотека.

## Логика и reasoning
- Сначала создал структуру файлов и заготовки.
- Далее поэтапно реализую методы, начиная с базовой логики и мест для интеграции с FIX.
- Все TODO и места для адаптации под конкретную биржу/FIX отмечаю в коде.
- После уточнения, что лицензии нет, выбран QuickFIX вместо OnixS.
- Везде, где требуется FIX API, использую интерфейсы QuickFIX (наследование от Application, MessageCracker и т.д.).
- Реализован парсинг аргументов командной строки в main.cpp.
- В CMakeLists.txt добавлен find_package(QuickFIX REQUIRED) и линковка с QuickFIX.

## Проверенные пути
- [x] SimpleScalper.h создан и адаптирован под QuickFIX
- [x] SimpleScalper.cpp реализует Application/MessageCracker и логику торговли
- [x] main.cpp реализует парсинг аргументов и запуск
- [x] CMakeLists.txt поддерживает QuickFIX
- [x] tasks/implement_scalper.md ведёт reasoning

## Следующие шаги
- Подготовить пример конфигурационного файла config.cfg для QuickFIX.
- Реализовать отправку MarketDataRequest и ордеров через QuickFIX.
- Доработать обработку ExecutionReport и MarketData. 