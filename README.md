# Enhanced Indexed File System

## Overview

This project implements an enhanced indexed file system using C++. It provides various functionalities to manage files and directories, with support for file encryption and decryption, indexing, and basic file operations. The system uses a simple encryption method to secure file contents and maintains an index table to track file block locations.

## Features

- **File Encryption/Decryption**: Encrypts and decrypts file contents using a simple XOR-based method.
- **File Operations**: Create, read, write, truncate, and delete files.
- **Directory Operations**: Create, delete, and change directories.
- **Indexed File Management**: Maintains an index table for efficient file block management.
- **Basic Command Interface**: Supports commands to interact with the file system through a simple command-line interface.

## Constants

- **DIRECTORY_SIZE**: Maximum number of entries in a directory (128).
- **MAX_FILE_NAME_SIZE**: Maximum length of file or directory names (64 bytes).
- **BLOCK_SIZE**: Size of each block in the file system (1 KB).
- **TOTAL_DISK_SIZE**: Total disk size of the file system (64 MB).
- **ENCRYPTION_KEY**: Key used for encrypting and decrypting file contents (0x5A).

## Usage
Welcome to the enhanced indexed file system!
Commands: create_file, read_file, write_file, truncate_file, delete_file, create_dir, delete_dir, change_dir, list, go_back, exit

> create_file example.txt
File "example.txt" created successfully.
> write_file example.txt
Enter content (end with ';'): Hello, this is an example file content;
File "example.txt" updated successfully.
> read_file example.txt
Content of "example.txt":
Hello, this is an example file content
> list
Contents of "root":
- example.txt
> create_dir documents
Directory "documents" created successfully.
> change_dir documents
Changed to directory "documents".
> go_back
Moved back to directory: "root".
> delete_file example.txt
File "example.txt" deleted successfully.
> list
Contents of "root":
- documents [DIR]
> delete_dir documents
Directory "documents" deleted successfully.
> exit
Exiting file system. Goodbye!

### Compilation

To compile the program, use the following command:

```sh
g++ -o file_system file_system.cpp -std=c++17
