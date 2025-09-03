#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sarthak");
MODULE_DESCRIPTION("Kernel Module: Tree, Linked List, Array with VMA Limit + Birthday Info");
MODULE_VERSION("3.0");

struct birthday {
    int day;
    int month;
    int year;
    struct list_head list;
};

static LIST_HEAD(birthday_list);

static int *pid_array = NULL;
static int pid_count = 0;
static int pid_capacity = 0;

static int max_vmas = 3;
module_param(max_vmas, int, 0644);
MODULE_PARM_DESC(max_vmas, "Maximum number of memory maps (VMAs) to print per process");

void print_memory_map(struct task_struct *task, int level);
void traverse_process_tree(struct task_struct *task, int level);
void store_pid(int pid);
const char* get_relation_string(int level);
struct birthday* get_birthday_by_level(int level);

void store_pid(int pid) {
    if (pid_count >= pid_capacity) {
        int new_capacity = pid_capacity ? pid_capacity * 2 : 16;
        int *new_array = krealloc(pid_array, new_capacity * sizeof(int), GFP_KERNEL);
        if (!new_array) {
            pr_err("Failed to allocate memory for pid_array\n");
            return;
        }
        pid_array = new_array;
        pid_capacity = new_capacity;
    }
    pid_array[pid_count++] = pid;
}

const char* get_relation_string(int level) {
    switch(level) {
        case 0: return "Parent";
        case 1: return "Child";
        case 2: return "Grandchild";
        case 3: return "Great-Grandchild";
        default: return "Descendant";
    }
}

struct birthday* get_birthday_by_level(int level) {
    int idx = level;
    struct birthday *bday;
    int count = 0;

    list_for_each_entry(bday, &birthday_list, list) {
        if (count == idx) return bday;
        count++;
    }

    count = 0;
    list_for_each_entry(bday, &birthday_list, list) {
        if (count == 4) return bday;
        count++;
    }
    return NULL;
}

void print_memory_map(struct task_struct *task, int level) {
    struct mm_struct *mm;
    struct vm_area_struct *vma;
    const char* relation = get_relation_string(level);
    struct birthday* bday = get_birthday_by_level(level);

    mm = get_task_mm(task);
    if (!mm) return;

    pr_info("%*s[%s] PID: %d | Birthday: %d-%d-%d\n", level * 4, "", relation, task->pid,
            bday->day, bday->month, bday->year);

    int count = 0;
    for (vma = find_vma(mm, 0); vma && count < max_vmas; vma = find_vma(mm, vma->vm_end), count++) {
        pr_info("%*sStart: 0x%lx - End: 0x%lx\n", (level + 1) * 4, "", vma->vm_start, vma->vm_end);
    }
    if (vma) {
        pr_info("%*s... (More VMAs not shown)\n", (level + 1) * 4, "");
    }

    mmput(mm);
}

void traverse_process_tree(struct task_struct *task, int level) {
    struct task_struct *child;
    struct list_head *list;

    store_pid(task->pid);
    print_memory_map(task, level);

    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        traverse_process_tree(child, level + 1);
    }
}

static int __init process_tree_init(void) {
    int i;
    struct birthday *person;

    pr_info("Kernel Module Loaded: Tree + Linked List + Array + VMA limit=%d\n", max_vmas);

    for (i = 0; i < 5; i++) {
        person = kmalloc(sizeof(*person), GFP_KERNEL);
        if (!person) {
            pr_err("Failed to allocate memory for birthday\n");
            return -ENOMEM;
        }
        person->day = i + 1;
        person->month = 1;
        person->year = 2000 + i;
        INIT_LIST_HEAD(&person->list);
        list_add_tail(&person->list, &birthday_list);
        pr_info("Added Birthday: %d-%d-%d\n", person->day, person->month, person->year);
    }

    traverse_process_tree(&init_task, 0);

    pr_info("Stored %d PIDs in Array:\n", pid_count);
    for (i = 0; i < pid_count; i++) {
        pr_info("pid_array[%d] = %d\n", i, pid_array[i]);
    }

    return 0;
}

static void __exit process_tree_exit(void) {
    struct birthday *ptr, *next;

    pr_info("Unloading Kernel Module...\n");

    list_for_each_entry_safe(ptr, next, &birthday_list, list) {
        pr_info("Removing Birthday: %d-%d-%d\n", ptr->day, ptr->month, ptr->year);
        list_del(&ptr->list);
        kfree(ptr);
    }

    if (pid_array) {
        kfree(pid_array);
        pr_info("Freed pid_array memory\n");
    }

    pr_info("Kernel Module Unloaded: Cleaned up\n");
}

module_init(process_tree_init);
module_exit(process_tree_exit);
