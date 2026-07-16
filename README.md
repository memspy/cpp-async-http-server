# Asynchronous Multi-threaded C++20 HTTP Server

A high-performance, asynchronous HTTP/1.1 web server built from scratch using modern **C++20 Coroutines** and **Boost.Asio**. This project demonstrates low-level networking, production-grade multithreading architectures, and a deep focus on application security (Cybersecurity mitigating common web vulnerabilities).

## 🚀 Key Features

* **C++20 Stackless Coroutines**: Uses `co_await` and `boost::asio::awaitable` for highly scalable, non-blocking I/O without the visual clutter of callback hell.
* **Thread Pool Architecture**: Features a custom-built, thread-safe `ThreadPool` task queue utilizing condition variables to safely distribute events across CPU cores.
* **Production-Grade Multithreading**: Runs a multi-threaded `boost::asio::io_context` event loop protected by explicit **`asio::strand`** synchronization to eliminate data races.
* **Zero-Copy Performance Concept**: The HTTP parser leverages `std::string_view` to slice and navigate raw network buffers without unnecessary heap allocations.
* **Express-Style Router**: Supports dynamic path parameters (e.g., `/api/users/:id`) compiled via optimized regular expressions (`std::regex`).
* **Static File Server**: Serves frontend assets (`html`, `css`, `js`, images) out-of-the-box from a public directory.

---

## 🔒 Cybersecurity & Hardening (Mitigated Vulnerabilities)

This server was intentionally designed and refactored to withstand common network attacks that target naive HTTP daemons:

1. **Path Traversal Protection (`CWE-22`)**: Static file resolution utilizes `std::filesystem::canonical` and path-prefix mismatch validation. This completely prevents attackers from using nested `../` manipulation to break out of the root directory and read sensitive system files like `/etc/passwd`.
2. **Data Race Elimination (`CWE-362`)**: All connection payloads are bound to isolated strands. This ensures sequential processing of socket read/write events across the multi-threaded pool, preventing memory corruption and `Segmentation Fault` (Crash DoS).
3. **ReDoS (Regular Expression DoS) Mitigation**: URL path inputs are strictly length-capped (`MAX_URL_PATH_LENGTH = 2048`) before regex validation, eliminating the risk of catastrophic backtracking attacks.
4. **Memory Exhaustion (OOM DoS) Prevention**: Safe numeric parsing via `std::from_chars` without throwing unhandled runtime exceptions. `Content-Length` fields are bounded by `MAX_BODY_SIZE` restrictions to stop integer overflow attacks from crashing the service.
5. **TCP Fragmentation Resiliency**: Replaced unstable raw buffer reads with `async_read_until` delimiters, ensuring safe multi-packet reassembly before execution.

---

## 📁 Project Structure

```text
├── include/                 # Header files (.hpp)
│   ├── http_parser.hpp      # High-performance zero-copy HTTP parser
│   ├── router.hpp           # Path-param regex router with ReDoS protection
│   └── thread_pool.hpp      # Safe multithreaded task dispatcher
├── src/                     # Source code (.cpp)
│   ├── async_server.cpp     # Network coroutine engine & event loop
│   └── main.cpp             # API endpoints setup & filesystem service
├── examples/                # Educational baselines (unuse/ archived)
│   ├── raw_socket_linux.cpp # Bloking single-threaded POSIX socket baseline
│   └── raw_socket_windows.cpp# Low-level Windows Winsock baseline
└── public/                  # Frontend directory served by the backend
```

---

## 💻 Technical Prerequisites

* **Compiler**: GCC / Clang with full **C++20** support
* **Build System**: CMake 3.20+
* **Dependencies**: 
  * `Boost` (specifically `Boost.Asio` and `Boost.System`)
  * `nlohmann/json` (automatically fetched via FetchContent during CMake build)

---

## ⚡ Quick Start & Compilation

1. Clone the repository and navigate to the root directory.
2. Create a build directory and execute CMake:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
3. Run the compiled executable:
```bash
./server
```
4. Open your web browser and navigate to `http://localhost:8080` to access the interactive diagnostic interface.

# Асинхронный многопоточный HTTP-сервер на C++20

Высокопроизводительный асинхронный веб-сервер HTTP/1.1, написанный с нуля с использованием современных **бесстековых корутин C++20** и библиотеки **Boost.Asio**. Проект демонстрирует принципы низкоуровневого сетевого программирования, архитектуру коммерческих многопоточных систем и глубокий упор на информационную безопасность (разработку защищенного ПО).

## 🚀 Ключевые особенности

* **Корутины C++20**: Использование оператора `co_await` и контекста `boost::asio::awaitable` для создания масштабируемого неблокирующего ввода-вывода без ухода в «callback hell» (ад обратных вызовов).
* **Архитектура Thread Pool**: Собственный потокобезопасный пул потоков (`ThreadPool`) с очередью задач на базе условных переменных (`condition_variable`) для эффективного распределения нагрузки по ядрам процессора.
* **Многопоточность уровня Production**: Параллельный запуск цикла обработки событий `boost::asio::io_context`, защищенный механизмами **`asio::strand`** для полного исключения состояний гонки (Data Races).
* **Концепция Zero-Copy**: Сетевой парсер HTTP-пакетов активно использует `std::string_view`, что позволяет разбирать заголовки «на лету» прямо в сетевом буфере без лишних аллокаций памяти в куче (heap).
* **Роутер в стиле Express.js**: Поддержка динамических параметров пути (например, `/api/users/:id`), компилируемых в оптимизированные регулярные выражения (`std::regex`).
* **Раздача статики**: Сервер «из коробки» умеет отдавать фронтенд-ресурсы (`html`, `css`, `js`, картинки) из выделенной директории.

---

## 🔒 Информационная безопасность и защита от уязвимостей

Архитектура сервера была полностью переработана для противодействия классическим атакам на веб-службы:

1. **Защита от обхода папок (Path Traversal / `CWE-22`)**: Логика раздачи файлов использует системные вызовы `std::filesystem::canonical` и строгую валидацию префиксов путей. Это полностью блокирует попытки хакера использовать конструкции вида `../`, чтобы выйти из папки `./public` и прочитать системные файлы (например, секретные ключи или логи Windows/Linux).
2. **Исключение состояний гонки (Data Race / `CWE-362`)**: Все операции чтения и записи конкретного сокета привязаны к изолированным объектам `strand`. Это гарантирует последовательную обработку сетевых пакетов в пуле потоков, защищая сервер от порчи памяти и случайных падений (`Segmentation Fault` как метод Crash DoS).
3. **Защита от ReDoS (Удаленный отказ в обслуживании через регулярные выражения)**: Длина входящих URL-путей жестко ограничена константой (`MAX_URL_PATH_LENGTH = 2048`). Это не позволяет злоумышленнику отправить аномально длинный URL для вызова катастрофического бэктрекинга (зависания) в движке `std::regex`.
4. **Предотвращение переполнения памяти (OOM DoS)**: Перевод строк в числа (например, ID пользователя) реализован через безопасный метод `std::from_chars` без выбрасывания пагубных исключений. Заголовок `Content-Length` контролируется константой `MAX_BODY_SIZE`, что не позволяет хакеру обрушить сервер фейковым требованием выделить терабайт памяти под тело запроса.
5. **Устойчивость к фрагментации TCP**: Вместо нестабильного чтения случайных кусков данных через `async_read_some`, сервер использует сборку пакетов через `async_read_until` до разделителя `\r\n\r\n`. Это гарантирует корректную сборку HTTP-заголовков, даже если провайдер дробит пакеты.

---

## 📁 Структура проекта

```text
├── include/                 # Заголовочные файлы (.hpp)
│   ├── http_parser.hpp      # Высокопроизводительный парсер запросов без лишних копирований
│   ├── router.hpp           # Роутер параметров пути с защитой от ReDoS
│   └── thread_pool.hpp      # Стабильный многопоточный диспетчер задач
├── src/                     # Исходный код (.cpp)
│   ├── async_server.cpp     # Сетевой движок на корутинах Boost.Asio
│   └── main.cpp             # Точка входа, конфигурация API и раздача файлов
├── examples/                # Демонстрационная база (низкоуровневые примеры)
│   ├── raw_socket_linux.cpp # Синхронный однопоточный сервер на системных вызовах POSIX
│   └── raw_socket_windows.cpp# Версия на чистом Winsock под Windows
└── public/                  # Папка фронтенда, обслуживаемая сервером
```

---

## ⚡ Быстрый запуск и компиляция

1. Клонируйте репозиторий и перейдите в его корень.
2. Создайте папку сборки и запустите CMake:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
3. Запустите скомпилированный сервер:
```bash
./server
```
4. Откройте браузер и перейдите по адресу `http://localhost:8080` для работы с интерактивной тестовой панелью.
