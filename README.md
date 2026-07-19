# Sigma Archiver

Sigma Archiver is a high-performance C++ library for data compression and archiving, designed around a chunk-based, multithreaded processing model.

The project focuses on efficient handling of large data streams, extensible pipeline architecture, and robust integrity validation.

---

## Overview

Sigma implements a custom binary archive format and a modular compression pipeline.
Data is processed incrementally in independent chunks, allowing parallel execution and controlled memory usage.

### Compression pipeline

```
RAW → RLE → Huffman → CRC → Archive
```

### Decompression pipeline

```
Archive → Chunk → Huffman → RLE → CRC → RAW
```

---

## Key Features

* **Chunk-based processing**

  * Files are split into independent chunks
  * Enables parallel compression and decompression
  * Scales to large files with predictable memory usage

* **Multithreaded execution**

  * Thread pool with bounded in-flight tasks
  * Deterministic output order preservation

* **Pipeline architecture**

  * Modular processing stages via `CompressControl`
  * Easily extendable with new algorithms

* **Combined compression**

  * Run-Length Encoding (RLE)
  * Huffman Coding

* **Data integrity**

  * CRC32 validation at:

    * chunk level
    * file level

* **Custom archive format**

  * Structured layout: Header → Data → Index
  * Stores directory hierarchy
  * Supports fast sequential writes

* **Atomic extraction**

  * Temporary directory + swap mechanism
  * Crash-safe restore process

---

## Architecture

The system is built around a streaming pipeline abstraction:

```
IDataSource → CompressControl → ... → Output
```

### Core Components

* `IDataSource` — abstract data source
* `CompressControl` — pipeline stage interface
* `StreamPipeline` — pipeline execution engine
* `FileProcessor` — compression/decompression logic
* `ThreadPool` — parallel task execution
* `FormatWriter / FormatReader` — archive format handling

---

## Archive Format

```
[HEADER]
[FILE DATA (chunked)]
[INDEX]
```

### Chunk Structure

* `offset`
* `compressed size`
* `original size`
* `CRC32`

The index contains metadata required for reconstruction of files and directories.

---

## Parallel Processing Model

* Input is split into chunks
* Chunks are processed independently in parallel
* Results are buffered and written in-order
* Memory usage is controlled via `MAX_IN_FLIGHT`

---

## Usage (Library)

```cpp
ArchivatorControl archiver;

// Create archive
archiver.Archive("archive.sigma", "folder/");

// Extract archive
archiver.Extract("archive.sigma");
```

---

## Design Goals

* High throughput on large datasets
* Predictable memory usage
* Extensible compression pipeline
* Deterministic output
* Fault-tolerant extraction

---

## Current Limitations

* No command-line interface (library-only usage)
* Archive format versioning is not finalized
* Streaming Huffman decoding is buffered
* Limited metadata support (permissions, timestamps)

---

## Roadmap

* Archive format versioning
* Streaming Huffman implementation
* Additional compression algorithms
* Extended metadata support
* CLI interface

---

## Author

Developed as a systems programming project focused on performance, architecture, and data processing pipelines.
