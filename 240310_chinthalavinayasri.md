# CS253 Assignment 1

## Word Indexing System Using Buffered Processing

### Author

Name: Chinthala Vinayasri
Roll Number: 240310

---

# Overview

This program builds a **word-level index** from large text files using a **fixed-size memory buffer**.
The system processes the input file incrementally without loading the entire file into memory.

Words are defined as **contiguous sequences of alphanumeric characters**, and all word matching is **case-insensitive**.

The program supports the following queries:

1. **Word Count Query** – returns the frequency of a given word in a file.
2. **Top-K Query** – returns the top K most frequent words in a file.
3. **Difference Query** – returns the difference in frequency of a word between two versions of a file.

---

# Compilation

Use the following command to compile the program:

```
g++ analyzer.cpp -o analyzer
```

---

# Running the Program

The program accepts input from the command line.

General syntax:

```
./analyzer [options]
```

---

# Command Line Arguments

| Argument            | Description                                 |
| ------------------- | ------------------------------------------- |
| `--file <path>`     | Path to input file (single-version queries) |
| `--file1 <path>`    | First input file (difference query)         |
| `--file2 <path>`    | Second input file (difference query)        |
| `--version <name>`  | Version identifier for single file queries  |
| `--version1 <name>` | Version identifier for first file           |
| `--version2 <name>` | Version identifier for second file          |
| `--buffer <kb>`     | Buffer size in KB (256 – 1024)              |
| `--query <type>`    | Query type: `word`, `top`, or `diff`        |
| `--word <token>`    | Word used in word and diff queries          |
| `--top <k>`         | Number of top words to display              |

---

# Example Commands

### Word Frequency Query

```
./analyzer --file test_logs.txt --version v1 --buffer 512 --query word --word error
```

Output:

* Version name
* Frequency of the word
* Buffer size
* Execution time

---

### Top-K Query

```
./analyzer --file test_logs.txt --version v1 --buffer 512 --query top --top 10
```

Output:

* Version name
* Top 10 most frequent words
* Buffer size
* Execution time

---

### Difference Query

```
./analyzer --file1 test_logs.txt --version1 v1 --file2 verbose_logs.txt --version2 v2 --buffer 512 --query diff --word error
```

Output:

* Word frequency in version1
* Word frequency in version2
* Difference between them
* Buffer size
* Execution time

---

# Program Design

The system follows an **object-oriented design** using multiple classes.

### BufferedReader

Reads the input file using a **fixed-size buffer** and prevents loading the entire file into memory.

### Tokenizer

Splits text into individual words using **alphanumeric character detection**.

### WordIndex

Stores the **frequency of each word** using a hash map.

### Query (Abstract Base Class)

Defines the interface for executing queries.

### WordQuery

Derived class that implements the **word count query**.

### TopQuery

Derived class that implements the **top-K query**.

### DiffQuery

Derived class that implements the **difference query between two versions**.

---

# Memory Efficiency

The program processes the file in **fixed-size chunks** and does not load the entire file into memory.
Memory usage grows only with the **number of unique words**, not with the size of the input file.

---

# C++ Features Demonstrated

The program demonstrates several C++ programming concepts:

* Object-oriented design
* Abstract base classes
* Inheritance
* Runtime polymorphism
* Function overloading
* Exception handling
* Template functions
* STL containers and algorithms

---

# Output Format

The program outputs:

* Version name(s)
* Query result
* Allocated buffer size
* Total execution time

Example:

```
Version: v1
Word: error
Count: 1452
Buffer size: 512 KB
Execution time: 0.02 sec
```

---

# Conclusion

This program demonstrates how large files can be processed efficiently using **incremental buffered reading** and **word indexing techniques** while maintaining strict memory constraints.
