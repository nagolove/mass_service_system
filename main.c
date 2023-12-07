#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

enum TaskType {
    TASK_TYPE_1,
    TASK_TYPE_2,
};

struct Task {
    //float           service_time;
    unsigned int    service_time;
    enum TaskType   type;
};

static struct {
    struct Task  *t;
    size_t       cap, num;
    int          i, j;
} tasks;

static bool         verbose = false;
// Задача которая обрабатывается в данный момент
static struct Task  current_task;
static unsigned int tick = 0;
static unsigned int last_try_add_time = 0, 
                    wait_for_add = 0,
                    wait_for_task = 0;
// Задача которая будет добавлена в очередь
static struct Task  new_task;

static void init() {
    size_t cap = 1010;
    size_t sz = cap * sizeof(struct Task);
    tasks.t = malloc(sz);
    tasks.num = 0;
    tasks.cap = cap;
}

static void shutdown() {
    free(tasks.t);
}

// Помещается задачу в хвост очереди
static void add2tail(struct Task task) {
    if (tasks.num < tasks.cap) {
        tasks.t[tasks.i] = task;
        tasks.i = (tasks.i + 1) % tasks.cap;
        tasks.num++;
    } else {
        printf("Buffer overflow\n");
        abort();
    }
}

// Снимает задачу из головы очереди
static struct Task *pop_from_head() {
    struct Task *ret = NULL; 
    if (tasks.num > 0) {
        ret = &tasks.t[tasks.j];
        tasks.j = (tasks.j + 1) % tasks.cap;
        tasks.num--;
    }

    return ret;
}

// Может добавить задачу если наступило время добавления
static void try_add() {
    //if (last_add_time
    if (wait_for_add == 0) {
        // Добавить ожидающую задачу
        add2tail(new_task);

        // Создать новую
        new_task = (struct Task) {
            .service_time = rand() % 4,
            .type = TASK_TYPE_1,
        };
        // Случайное время ожидания
        wait_for_add = rand() % 5;
        last_try_add_time = tick;
    }

    // Избежать декремента нуля
    // Декремент делать только после одного тика времени
    wait_for_add--;
}

// Обрабатывает одну еденицу в Обслуживающем Аппарате.
// Увеличивает счетчик времени на 1.
static void process_tick() {
    // Начать обработку следующей задачи
    if (wait_for_task == 0) {
        wait_for_task = current_task.service_time;
    }

    wait_for_task--;

    tick++;
}

int main(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "--verbose")) {
            verbose = true;
        }
    }

    srand(time(NULL));

    init();

    add((struct Task) {
        .service_time = rand() % 4,
        .type = TASK_TYPE_2,
    });
    process_tick();

    for (int i = 0; i < 1000; i++) {
        try_add();
        process_tick();
    }

    shutdown();
}
