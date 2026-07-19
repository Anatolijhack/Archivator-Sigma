# Sigma Archiver

**Sigma Archiver** — высокопроизводительный архиватор на C++, реализующий потоковую обработку, многопоточность и собственный бинарный формат.

> Проект демонстрирует навыки системного программирования, архитектурного проектирования и работы с большими объёмами данных.

---
📦 Архиватор (Compression Pipeline)
Сжатие
RAW DATA
   ↓
[RLE Compression]
   ↓
[Huffman Encoding]
   ↓
[CRC Calculation]
   ↓
[Write to Archive]
Распаковка
ARCHIVE DATA
   ↓
[Read Chunk]
   ↓
[Huffman Decoding]
   ↓
[RLE Decompression]
   ↓
[CRC Validation]
   ↓
[Write File]
Особенности
Обработка идёт чанками (chunks)
Каждый chunk:
сжимается независимо
проверяется через CRC
Поддерживается параллельная обработка чанков
Pipeline реализован через CompressControl
##  Ключевые особенности

*  **Многопоточность** (ThreadPool, контроль нагрузки)
* **Chunk-based обработка** (поддержка больших файлов)
* **Pipeline-архитектура** (расширяемая система обработки)
*  **Комбинированное сжатие**

  * RLE
  * Huffman Coding
*  **Контроль целостности**

  * CRC32 (на уровне чанков и файлов)
*  **Собственный формат архива**

  * Header + Data + Index
*  Сохранение структуры директорий

---

##  Архитектура

Проект построен на потоковой модели обработки данных:

```cpp
IDataSource → CompressControl → ... → Output
```

### Основные компоненты

* `IDataSource` — абстракция источника данных
* `CompressControl` — этап обработки (pipeline)
* `StreamPipeline` — выполнение цепочки
* `FileProcessor` — логика сжатия/распаковки
* `ThreadPool` — параллельная обработка
* `FormatWriter` — работа с бинарным форматом

---

##  Pipeline сжатия

```text
RAW → RLE → Huffman → Output
```

Распаковка:

```text
Input → Huffman → RLE → RAW
```

---

##  Формат архива

```text
[HEADER]
[FILE DATA (chunks)]
[INDEX]
```

### Chunk структура

* offset
* compressed size
* original size
* CRC32

---

##  Параллелизм

* Обработка файлов разбивается на чанки
* Чанки сжимаются параллельно
* Сохраняется порядок при записи
* Ограничение нагрузки (`MAX_IN_FLIGHT`)

---

##  Использование

```cpp
ArchivatorControl archiver;

// Архивация
archiver.Archive("archive.sigma", "folder/");

// Извлечение
archiver.Extract("archive.sigma");
```

---

## Что демонстрирует проект

* Проектирование систем (pipeline, формат данных)
* Работа с потоками и синхронизацией
* Оптимизация под большие данные
* Реализация алгоритмов сжатия
* Разработка собственного бинарного формата

---

## Roadmap

* [ ] Поддержка нескольких алгоритмов сжатия
* [ ] Потоковый Huffman (без буферизации)
* [ ] CLI интерфейс
* [ ] Улучшение формата (endianness, versioning)
* [ ] RAII и безопасное управление памятью

---

## Автор

Разработано как pet-проект с упором на **system design и performance**.

