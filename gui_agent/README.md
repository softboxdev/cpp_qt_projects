# Финальный обзор: Программирование на Qt5

## 1. Основные концепции Qt

| Концепция | Описание | Пример |
|-----------|----------|--------|
| **Сигналы и слоты** | Механизм асинхронного вызова методов. Сигнал "издаёт" событие, слот "ловит" его | `connect(button, &QPushButton::clicked, this, &MyClass::onClick)` |
| **QObject** | Базовый класс для всех объектов Qt. Даёт сигналы/слоты, родительскую иерархию | `class MyClass : public QObject` |
| **Родитель-потомок** | Автоматическое управление памятью. При удалении родителя удаляются все потомки | `QPushButton* btn = new QPushButton(parent)` |
| **Цикл событий** | Бесконечный цикл, обрабатывающий события (таймеры, сокеты, нажатия) | `QApplication::exec()` |
| **MOC (Meta-Object Compiler)** | Компилятор Qt, генерирующий код для сигналов/слотов | Автоматически вызывается CMake при `AUTOMOC ON` |

## 2. Создание объектов в Qt (все способы)

| Тип создания | Синтаксис | Когда использовать | Управление памятью |
|--------------|-----------|-------------------|-------------------|
| **На стеке (авто)** | `QPushButton btn("OK");` | Временные объекты, локальные переменные | Автоматическое (при выходе из блока) |
| **Через new с parent** | `new QPushButton("OK", parent)` | Виджеты, объекты, живущие долго | Удаляется родителем |
| **Через new без parent** | `new QPushButton("OK")` | Объекты, которые нужно удалить вручную | Ручной вызов `delete` |
| **Умные указатели** | `std::unique_ptr<QObject>` | Когда нужно явное владение | Автоматическое через RAII |
| **QScopedPointer** | `QScopedPointer<QObject> ptr(new QObject)` | Qt-специфичный умный указатель | Автоматическое |

## 3. Жизненный цикл Qt-объекта

```
┌─────────────────────────────────────────────────────────────────────┐
│                    ЖИЗНЕННЫЙ ЦИКЛ ОБЪЕКТА QT                         │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  new QObject(parent)                                                 │
│         │                                                           │
│         ▼                                                           │
│  ┌─────────────┐                                                    │
│  │ Конструктор │  ← Выделение памяти, инициализация                  │
│  └─────────────┘                                                    │
│         │                                                           │
│         ▼                                                           │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │              Рабочее состояние                               │    │
│  │  • Соединения сигнал-слот                                     │    │
│  │  • Свойства (Q_PROPERTY)                                      │    │
│  │  • Динамические параметры                                     │    │
│  └─────────────────────────────────────────────────────────────┘    │
│         │                                                           │
│         ▼                                                           │
│  delete obj  или  удаление parent                                    │
│         │                                                           │
│         ▼                                                           │
│  ┌─────────────┐                                                    │
│  │ Деструктор  │  ← Освобождение ресурсов, уведомление о удалении    │
│  └─────────────┘                                                    │
│                                                                      │
│  ВАЖНО: Деструктор Qt-объекта автоматически:                         │
│  1. Отключает все сигналы                                            │
│  2. Удаляет всех детей                                               │
│  3. Отписывается от таймеров                                         │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

## 4. Макросы Qt (самые важные)

| Макрос | Назначение | Где писать |
|--------|-----------|-----------|
| `Q_OBJECT` | Обязателен для сигналов/слотов | В `private` или `public` секции класса |
| `Q_PROPERTY` | Создаёт свойство с getter/setter | В `class` после `Q_OBJECT` |
| `Q_ENUM` | Регистрирует перечисление | В `class` после `Q_OBJECT` |
| `Q_SIGNALS` | Объявляет секцию сигналов | Вместо `signals:` |
| `Q_SLOTS` | Объявляет секцию слотов | Вместо `slots:` |
| `Q_DECLARE_METATYPE` | Регистрирует пользовательский тип | После определения класса |
| `Q_INVOKABLE` | Делает метод доступным из QML | Перед методом |

## 5. Типы данных Qt и STL (соответствие)

| Qt тип | STL эквивалент | Когда использовать |
|--------|---------------|-------------------|
| `QString` | `std::string` | Везде в Qt, UTF-8, удобный |
| `QList<T>` | `std::vector<T>` | Большинство случаев |
| `QVector<T>` | `std::vector<T>` | Массивы фиксированного размера |
| `QMap<K,V>` | `std::map<K,V>` | Сортированные ключи |
| `QHash<K,V>` | `std::unordered_map<K,V>` | Быстрый доступ |
| `QByteArray` | `std::vector<char>` | Бинарные данные |
| `QSharedPointer<T>` | `std::shared_ptr<T>` | Общее владение |
| `QScopedPointer<T>` | `std::unique_ptr<T>` | Эксклюзивное владение |

## 6. Шаблон создания класса с сигналами и слотами

```cpp
// myclass.h
#ifndef MYCLASS_H
#define MYCLASS_H

#include <QObject>

class MyClass : public QObject
{
    Q_OBJECT  // МАКРОС ОБЯЗАТЕЛЬНО!

public:
    // Конструктор с родителем (стандарт Qt)
    explicit MyClass(QObject *parent = nullptr);
    
    // Обычный метод
    void doWork();
    
    // Метод, доступный из QML
    Q_INVOKABLE void processData(const QString& data);
    
    // Свойство (геттер/сеттер автоматически)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

public slots:
    // Слот - может быть вызван по сигналу
    void onTimerTimeout();
    
    // Слот с параметрами
    void onDataReceived(const QByteArray& data);

signals:
    // Сигналы - только объявление, реализацию генерирует MOC
    void workFinished();
    void valueChanged(int newValue);
    void errorOccurred(const QString& message);

private:
    int m_value = 0;
};

#endif
```

```cpp
// myclass.cpp
#include "myclass.h"
#include <QDebug>
#include <QTimer>

MyClass::MyClass(QObject *parent) 
    : QObject(parent)
{
    // Создаём дочерний объект (автоматически удалится)
    QTimer* timer = new QTimer(this);
    
    // Сигнал к слоту
    connect(timer, &QTimer::timeout, this, &MyClass::onTimerTimeout);
    
    // Лямбда вместо слота
    connect(timer, &QTimer::timeout, [this]() {
        qDebug() << "Lambda called";
    });
    
    timer->start(1000);
}

void MyClass::doWork()
{
    qDebug() << "Working...";
    emit workFinished();  // Генерация сигнала
}

void MyClass::processData(const QString& data)
{
    qDebug() << "Processing:" << data;
}

void MyClass::onTimerTimeout()
{
    static int counter = 0;
    setValue(counter++);  // Вызовет valueChanged сигнал
}

void MyClass::onDataReceived(const QByteArray& data)
{
    qDebug() << "Received:" << data;
}

int MyClass::value() const { return m_value; }

void MyClass::setValue(int newValue)
{
    if (m_value == newValue) return;
    m_value = newValue;
    emit valueChanged(m_value);  // Сигнал об изменении
}
```

## 7. Правила подключений сигналов и слотов

| Способ | Синтаксис | Безопасность | Qt версия |
|--------|-----------|--------------|-----------|
| **Старый (SIGNAL/SLOT)** | `connect(obj, SIGNAL(mySignal(int)), this, SLOT(mySlot(int)))` | Проверка в рантайме | Qt 1-5 |
| **Новый (&Class::method)** | `connect(obj, &MyClass::mySignal, this, &MyClass::mySlot)` | Проверка компилятором | Qt 5+ |
| **Лямбда** | `connect(obj, &Class::signal, [this](){ ... })` | Типобезопасно | Qt 5+ |
| **С указателем на метод** | `connect(obj, &Class::signal, this, &Class::slot)` | Типобезопасно | Qt 5+ |

**Правильный порядок аргументов:**
```cpp
connect(отправитель, &Отправитель::сигнал, получатель, &Получатель::слот);
//        ↑                  ↑                ↑            ↑
//    sender object      signal           receiver     slot
```

## 8. Управление памятью в Qt (в таблице)

| Сценарий | Что делать | Что НЕ делать |
|----------|-----------|----------------|
| **Виджеты** | `new QWidget(parent)` | Удалять вручную |
| **QTimer** | `new QTimer(this)` | Создавать на стеке без parent |
| **QLocalSocket** | `new QLocalSocket(this)` | Забыть про `deleteLater()` |
| **Свои объекты с QObject** | Передавать `parent` в конструктор | Вызывать `delete` напрямую |
| **Объекты без parent** | `QScopedPointer` или ручной `delete` | Терять указатель (утечка) |
| **Временные объекты** | Стек (`QString s = "text"`) | `new QString("text")` |

## 9. Цикл событий Qt (Event Loop)

```
┌─────────────────────────────────────────────────────────────────────┐
│                       ЦИКЛ СОБЫТИЙ QT                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   ┌─────────────┐                                                    │
│   │   START     │                                                    │
│   └──────┬──────┘                                                    │
│          │                                                          │
│          ▼                                                          │
│   ┌─────────────┐     ┌─────────────┐     ┌─────────────┐          │
│   │ Получить    │────►│ Обработать  │────►│ Вызвать     │          │
│   │ событие     │     │ событие     │     │ слот        │          │
│   └─────────────┘     └─────────────┘     └─────────────┘          │
│          │                                              │           │
│          │                                              │           │
│          ▼                                              │           │
│   ┌─────────────┐                                      │           │
│   │ Есть ещё    │◄─────────────────────────────────────┘           │
│   │ события?    │                                                   │
│   └──────┬──────┘                                                   │
│          │                                                          │
│     НЕТ  │  ДА ──────────────────────────────────────┐              │
│          │                                            │              │
│          ▼                                            │              │
│   ┌─────────────┐                                    │              │
│   │   БЛОК      │                                    │              │
│   │ (ожидание)  │                                    │              │
│   └─────────────┘                                    │              │
│          │                                            │              │
│          └────────────────────────────────────────────┘              │
│                                                                      │
│  Как прервать:                                                       │
│  • QCoreApplication::quit()  - завершить цикл                       │
│  • QEventLoop::exit()        - выйти из вложенного цикла             │
│  • QTimer::singleShot(0, ...) - отложенный вызов                     │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

## 10. Основные классы Qt, которые нужно знать

| Категория | Классы | Назначение |
|-----------|--------|------------|
| **Ядро** | `QObject`, `QCoreApplication`, `QTimer`, `QEvent` | Базовые объекты, цикл событий |
| **Контейнеры** | `QList`, `QVector`, `QMap`, `QHash`, `QString` | Хранение данных |
| **Ввод-вывод** | `QFile`, `QTextStream`, `QDataStream`, `QBuffer` | Файлы, потоки |
| **Сети** | `QTcpServer`, `QTcpSocket`, `QLocalServer`, `QUdpSocket` | Сетевое взаимодействие |
| **GUI** | `QWidget`, `QMainWindow`, `QLabel`, `QPushButton` | Визуальные компоненты |
| **Модели** | `QAbstractItemModel`, `QStandardItemModel` | Данные для View |
| **Графика** | `QPainter`, `QImage`, `QPixmap` | Рисование |
| **Многопоточность** | `QThread`, `QMutex`, `QWaitCondition` | Параллельное выполнение |

## 11. Паттерны проектирования в Qt

| Паттерн | Как реализован | Пример |
|---------|---------------|--------|
| **Наблюдатель** | Сигналы и слоты | `connect(source, &Class::signal, observer, &Class::slot)` |
| **Одиночка** | `qobject_cast`, глобальные указатели | `QCoreApplication::instance()` |
| **Фабрика** | `QObject::create()` с известным метатипом | `QMetaType::create()` |
| **Компоновщик** | Родитель-потомок в QObject | `widget->setParent(parent)` |
| **Посредник** | QEvent, фильтры событий | `installEventFilter()` |
| **Стратегия** | QStyle, QProxyStyle | Кастомизация отрисовки |

## 12. Отладка Qt-приложений

```cpp
#include <QDebug>

// Разные уровни вывода
qDebug() << "Debug message";      // Отладка
qWarning() << "Warning message";   // Предупреждение
qCritical() << "Critical error";   // Критическая ошибка
qFatal("Fatal error");            // Завершение программы

// Форматированный вывод
qDebug() << "Value:" << 42 << "String:" << "Hello";

// Проверка соединений
bool ok = connect(obj, SIGNAL(mySignal()), this, SLOT(mySlot()));
qDebug() << "Connection:" << (ok ? "OK" : "FAILED");

// Вывод типа объекта
qDebug() << object->metaObject()->className();

// Трассировка вызовов
qDebug() << Q_FUNC_INFO;  // Выведет имя функции
```

## 13. Частые ошибки и их решения

| Ошибка | Причина | Решение |
|--------|---------|---------|
| `undefined reference to vtable` | Нет `Q_OBJECT` макроса или не запущен MOC | Добавить `Q_OBJECT`, почистить сборку |
| `No such signal/slot` | Ошибка в сигнатуре сигнала/слота | Проверить типы параметров |
| `QObject::connect: No such slot` | Слот не найден | Проверить, что слот объявлен в `slots:` |
| **Утечка памяти** | new без parent, нет delete | Добавить parent или `deleteLater()` |
| **Программа висит** | Бесконечный цикл в главном потоке | Использовать таймеры или потоки |
| **Краш при удалении** | Двойное удаление | Использовать `deleteLater()` для сокетов |

## 14. Шпаргалка: как создать правильно

```cpp
// 1. Класс с сигналами и слотами
class Worker : public QObject {
    Q_OBJECT
public:
    explicit Worker(QObject* parent = nullptr) : QObject(parent) {}
signals:
    void done();
public slots:
    void process() { emit done(); }
};

// 2. Правильный connect
connect(worker, &Worker::done, this, &MyClass::handleDone);

// 3. Правильное создание объектов
QPushButton* btn = new QPushButton("Click", this);  // parent = this
QLocalSocket* socket = new QLocalSocket(this);       // будет удалён

// 4. Правильное удаление
socket->disconnectFromServer();
socket->deleteLater();  // Безопасно! Не delete socket

// 5. Правильный таймер
QTimer* timer = new QTimer(this);
QTimer::singleShot(1000, this, &MyClass::delayedCall);  // Однократный

// 6. Правильная работа с потоками
QThread* thread = new QThread(this);
worker->moveToThread(thread);
connect(thread, &QThread::started, worker, &Worker::process);
thread->start();
```

## 15. Главные принципы программирования на Qt (запомнить)

| № | Принцип | Почему важно |
|---|---------|--------------|
| 1 | **Всегда наследуйте от QObject** для сигналов/слотов | Даёт метаинформацию и parent-иерархию |
| 2 | **Добавляйте Q_OBJECT макрос** в классы с сигналами | MOC генерирует нужный код |
| 3 | **Передавайте parent в конструктор** для объектов на куче | Автоматическое управление памятью |
| 4 | **Используйте deleteLater() вместо delete** для сокетов | Безопасно в цикле событий |
| 5 | **Не создавайте QObject на стеке** без parent | Можно, но опасно при копировании |
| 6 | **Используйте новый синтаксис connect** (&Class::method) | Проверка на этапе компиляции |
| 7 | **Не блокируйте главный поток** | GUI замрёт, таймеры не сработают |
| 8 | **Для долгих операций используйте QThread** | GUI остаётся отзывчивым |
| 9 | **Используйте qDebug() для отладки** | Вывод в консоль, можно отключить |
| 10 | **Изучите документацию Qt** | Лучшая документация среди фреймворков |

## Итог: минимальный работающий Qt-проект

```cpp
// main.cpp
#include <QApplication>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QPushButton button("Hello Qt!");
    button.show();
    
    return app.exec();
}
```

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(HelloQt)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(hello main.cpp)
target_link_libraries(hello Qt5::Widgets)
```

**Золотое правило Qt:** Если класс наследует `QObject` и использует сигналы/слоты, он должен иметь макрос `Q_OBJECT`, а CMake должен иметь `set(CMAKE_AUTOMOC ON)`.