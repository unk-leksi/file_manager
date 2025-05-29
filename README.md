# 📁 File System Manager in C

## 📌 Project Description
This project implements a simplified hierarchical file system in **C**, designed for educational and practical use. The system supports creation and deletion of directories and files, path navigation, and counting files — all operating within a virtual disk environment.

## 🔧 Core Features

✔️ **Hierarchical Directory Structure** – Support for nested folders  
✔️ **File & Directory Creation** – Basic creation APIs  
✔️ **Recursive Deletion** – Clean removal of entire directory trees  
✔️ **Path Navigation** – Change and retrieve current working directory  
✔️ **File Counting** – Counts all files in the current or specified directory  
✔️ **Memory Management** – Manual control over allocation and cleanup

---

## 📜 Function Overview

| Function | Description |
|---------|-------------|
| `create(int disk_size)` | Initializes the file system with the specified disk size |
| `destroy()` | Frees all allocated memory and resets the file system |
| `create_dir(const char* path)` | Creates a new directory at the given path |
| `create_file(const char* path, int file_size)` | Creates a file with a specific size |
| `remove(const char* path, int recursive)` | Removes a file or directory (recursively if needed) |
| `change_dir(const char* path)` | Changes the current working directory |
| `get_cur_dir(char* dst)` | Retrieves the full path of the current working directory |
| `files_count(const char* path)` | Returns the number of files under the specified directory (recursively) |

---

## 🚀 How It Works

### 1️⃣ Initialization – `create`
- Sets up the root directory (`/`) and allocates virtual space for the file system.

### 2️⃣ Creation – `create_file`, `create_dir`
- Creates uniquely named files and directories.
- Ensures proper placement within the file system hierarchy.

### 3️⃣ Navigation – `change_dir`, `get_cur_dir`
- Allows moving between directories using absolute (`/dir/subdir`) and relative (`../`) paths.
- Tracks and returns the current working directory.

### 4️⃣ Deletion – `remove`
- Deletes files directly.
- Deletes entire directories if the `recursive` flag is enabled.
- Updates virtual memory and frees resources.

### 5️⃣ File Counting – `files_count`
- Traverses the file system starting from the given path.
- Returns the count of all regular files within that subtree.

---

## 📎 Example Use

```c
create(1024); // Create a file system with 1KB space
create_dir("/docs");
create_file("/docs/report.txt", 100);
change_dir("/docs");
get_cur_dir(buf); // "/docs"
files_count("/"); // returns 1
remove("/docs", 1); // Recursive delete
destroy();
