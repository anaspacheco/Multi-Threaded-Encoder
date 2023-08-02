# Multi-Threaded Encoder (C)

## Introduction

This program offers both sequential and parallel encoding options for input files. 

![image](https://github.com/anaspacheco/Multi-Threaded-Encoder/assets/121977567/d4fa3b43-f571-48e6-8431-2f56d7eb4bbc)

Src: https://www.baeldung.com/cs/async-vs-multi-threading 

## Getting Started

Before using the Multi-Threaded Encoder, ensure you have the following prerequisites:

1. A C compiler installed on your system.
2. This cloned repository on your local machine.

## Usage

### Sequential Encoding

To perform sequential encoding, simply run the following command:

```bash
./encoder (input file) > (output file)
```

Replace `(input file)` with the path to the file you want to encode and `(output file)` with the desired name for the encoded output file.

### Parallel Encoding

To use parallel encoding, use the following command:

```bash
./encoder -j (# of threads) (input file) > (output file)
```

Replace `(# of threads)` with the number of threads you want to utilize for encoding. This option will distribute the workload across multiple threads, significantly reducing the encoding time.

## Example

For instance, if you want to encode `data.txt` using 4 threads and save the output to `encoded_data.txt`, run the command:

```bash
./encoder -j 4 data.txt > encoded_data.txt
```

## Troubleshooting

If you encounter any issues while using the Multi-Threaded Encoder, submit them to Issues.

