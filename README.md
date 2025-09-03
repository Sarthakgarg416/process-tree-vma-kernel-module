# Linux Kernel Module: Process Tree + Linked List + Dynamic Array + VMA Limit

## 📌 Overview
This project is a custom Linux kernel module that demonstrates:
- **Process Tree Traversal**: Recursively traverses processes starting from the `init_task`.
- **Virtual Memory Maps (VMAs)**: Prints a limited number of memory maps for each process.
- **Dynamic Array (pid_array)**: Stores all traversed process IDs dynamically.
- **Linked List (birthdays)**: Maintains a kernel-linked list of birthday records.
- **Tree Representation**: Outputs processes in a hierarchical (parent → child → grandchild) tree-like format.

This combines **trees, linked lists, and arrays** within the Linux kernel space.

---

## ⚙️ Features
- Traverse the process tree and print:
  - Process ID (PID)
  - Relationship level (`Parent`, `Child`, `Grandchild`, etc.)
  - Associated birthday info
  - Limited memory mappings (VMAs)
- Store all PIDs dynamically in an expandable array.
- Linked list to store and clean up birthday entries.
- Adjustable **VMA print limit** via module parameter.

---

## 📂 Project Structure
```
├── process_tree.c   # Kernel module source code
├── Makefile         # Build script for kernel module
└── README.md        # Project documentation
```

---

## 🛠️ Build & Run Instructions

### 1. Clone Repository
```bash
git clone https://github.com/<your-username>/process-tree-module.git
cd process-tree-module
```

### 2. Build Kernel Module
```bash
make
```

### 3. Insert Module
```bash
sudo insmod process_tree.ko max_vmas=5
```
- Default: `max_vmas = 3`
- You can set `max_vmas` (number of memory maps to display per process).

### 4. Check Kernel Logs
```bash
dmesg | tail -n 50
```

### 5. Remove Module
```bash
sudo rmmod process_tree
```

### 6. Clean Build Files
```bash
make clean
```

---

## 📖 Example Output (from `dmesg`)
```
[   45.123456] Kernel Module Loaded: Tree + Linked List + Array + VMA limit=3
[   45.123457] Added Birthday: 1-1-2000
[   45.123458] Added Birthday: 2-1-2001
...
[   45.123460] [Parent] PID: 1 | Birthday: 1-1-2000
[   45.123461]     Start: 0x55aa0000 - End: 0x55ab0000
[   45.123462]     ... (More VMAs not shown)
[   45.123463] [Child] PID: 42 | Birthday: 2-1-2001
...
[   45.123480] Stored 128 PIDs in Array:
[   45.123481] pid_array[0] = 1
[   45.123482] pid_array[1] = 42
...
```

---

## 🧹 Cleanup on Exit
When the module is removed:
- Frees all dynamically allocated **birthday list nodes**.
- Frees the **pid_array**.
- Prints cleanup messages in kernel logs.

---

## 📝 Requirements
- Linux kernel headers installed (for your current kernel).
- GCC and `make`.

Example (Ubuntu/Debian):
```bash
sudo apt install build-essential linux-headers-$(uname -r)
```

---

=

## 👨‍💻 Author
- **Sarthak Garg**
