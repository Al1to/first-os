#include "./memory.h"

/*/
 * на аппаратном уровне у нас есть физическая память,
 * но для процессов и прочей хери нам нужно переводить это все в виртуальную память (т.к. это удобнее)
 * в физ памяти все делится на фреймы, а в виртуальной на страницы. размерами они по 4кб
 * 
 * PAGES_DIR-ы - это по сути таблицы, которые сопоставляют виртуальные адреса с физ адресами
 * PAGES_TABLE - это таблица, которая сопоставляет отдельные виртуальные страницы с физическими фреймами
 * 
 * 
/*/

// | тут будут храниться значения мин и макс номера для страниц и фреймов
// | TODO: скорее всего, имеются в виду мин и макс адреса в памяти для будующих страниц и фреймов 
// ↓ ...
static uint32_t page_frame_min;
static uint32_t page_frame_max;

// ↓ кол-во выделенных страниц; при ините pmm выставляется в 0
static uint32_t total_alloc;

// ↓ кол-во PAGES_DIR
#define NUM_PAGES_DIRS 256

// | это кол-во фреймов в физ памяти и равносильно страниц в виртуальной памяти
// | 0x100000000 - это 4гб в байтах, т.е. вся доступная физ память (ну я так понял)
// | 0x1000 - это 4кб в байтах, по сути фулл память делим на размер страницы/фрейма, чтобы получить их кол-во
// | TODO: делим еще на 8, т.к. ... та хрен его знает
// ↓ P.S. далее страница/фрейм буду сокращать с/ф
#define NUM_PAGES_FRAMES (0x100000000 / 0x1000 / 8) // 0x20_000

// | битмап физ памяти; каждому биту соответствует один фрейм
// | кол-во с/ф делим на 8, т.к. нам надо, чтобы было по одному биту на одну с/ф,
// | т.е. нам нужен массив по размеру равный формуле (1бит * кол-во с/ф)
// | ну и поскольку 1 байт это минималочка которую мы можем выделять на ячейку массива в Си, 
// | мы просто уменьшаем кол-во ячеек в 8 раз 
// ↓ 0 - фрейм свободен, 1 - фрейм занят
uint8_t phys_memory_bitmap[NUM_PAGES_FRAMES / 8]; // 0x4_000

// | массив PAGES_DIR-ов, в каждом таком по 1024 32бит записей
// | TODO: выровнен по 4кб, т.к. ...
// | TODO: каждая запись представляет из себя ...
// ↓ ...
static uint32_t page_dirs[NUM_PAGES_DIRS][1024] __attribute__((aligned(4096)));

// | это массив флагов, каждый из которых показывает, использован ли такой-то то PAGES_DIR
// | TODO: пока что хз зачем ... и поч uint32_t
// ↓ ...
static uint32_t page_dir_used[NUM_PAGES_DIRS];

// | кол-во виртуальных страниц 
// | TODO: НУ НАХЕРААААААА ОНА НУЖНАА ...
// ↓ ...
int mem_num_vpages;

// | инит менеджера физической памяти (pmm - physical memory manager)
// | устанавливает мин и макс номера(адреса?) с/ф, 
// ↓ а также обнуляет битмап физ памяти
void pmm_init(uint32_t mem_low,uint32_t mem_high) {

    // ↓ 
    page_frame_min = CEIL_DIV(mem_low, 0x1000);

    // ↓
    page_frame_max = mem_high / 0x1000;

    // ↓ обнуление счетчика выделенных страниц
    total_alloc = 0;

    // ↓ обнуление битмапы физ памяти
    memset(phys_memory_bitmap, 0, sizeof(phys_memory_bitmap));
}

// ↓ изменяет c текущего PAGES_DIR на другой, а что это вообще значит, я сам еще несовсем понимаю
void mem_change_page_dir(uint32_t* pd) {

    // | вычисление адреса PAGES_DIR
    // | далее мы его положим в cr3, а туда нужно класть смещение от KERNEL_START
    // ↓ P.S. чекай boot.asm
    pd = (uint32_t*)(((uint32_t)pd) - KERNEL_START);

    // ↓ загружает в cr3 адрес нового PAGES_DIR
    asm volatile("mov %0, %%eax \n mov %%eax, %%cr3 \n" :: "m"(pd));
}

// TODO: вот эту херь нужно как-то реализовать и в нашем коде
// void *get_phys_addr(void *vaddr) {
//     return (void *)((kernel_tables[(uint32_t)vaddr/4096].address*4096)+((uint32_t)vaddr&0xFFF));
// }









// TODO:
// Синхронизация каталогов страниц.
// Сбрасывает флаг принадлежности для страниц, которые используются 
// для ядра, в каталогах, которые не являются текущим.
// Это необходимо для предотвращения ошибок при переключении 
// контекстов.
void sync_page_dirs() {
    // Проход по всем каталогам страниц.
    for (int i = 0; i < NUM_PAGES_DIRS; ++i) {
        // Если каталог страниц используется.
        if (page_dir_used[i]) {
            // Получение адреса каталога страниц.
            uint32_t* page_dir = page_dirs[i];

            // Сброс флага принадлежности для страниц 768-1023.
            // Эти страницы используются ядром, и флаг 
            // принадлежности должен быть сброшен для 
            // предотвращения ошибок при переключении контекстов.
            for (int i = 768; i < 1023; ++i) {
                page_dir[i] = page_dir_init[i] & ~PAGE_FLAG_OWNER;
            }
        }
    }
}










// ↓ возвращает физ адрес текущего PAGES_DIR
uint32_t* mem_get_cur_page_dir() {

    // ↓ получаем смещение текущего PAGES_DIR от KERNEL_START 
    uint32_t pd;
    asm volatile("mov %%cr3, %0" : "=r"(pd));

    // ↓ переводим в физ адрес
    pd += KERNEL_START;

    return (uint32_t*)pd;
}












// ↓ сопоставляет (мапит) виртуальные и физические адреса
void mem_map_page(uint32_t vaddr, uint32_t paddr, uint32_t flags) {
    
    // | сохранение адреса предыдущего PAGES_DIR
    // ↓ это необходимо для восстановления предыдущего контекста после завершения мапинга
    uint32_t *prev_page_dir = 0;

    // ↓ если виртуальный адрес принадлежит ядру
    if (vaddr >= KERNEL_START) {

        // ↓ получение адреса текущего PAGES_DIR
        paddr = mem_get_cur_page_dir(); 

        // | page_dir_init - это самый первый PAGES_DIR, он объявлен в boot.asm
        // ↓ если текущий PAGES_DIR не является начальным
        if (prev_page_dir != page_dir_init) {

            // ↓ изменение текущего PAGES_DIR на начальный
            mem_change_page_dir(page_dir_init);
        }
    }
    






    // TODO:
    // Вычисление индексов каталога и таблицы страниц.
    // Эти индексы используются для поиска соответствующих записей
    // в каталоге и таблице страниц.
    uint32_t pd_index = vaddr >> 22;
    uint32_t pt_index = vaddr >> 12 & 0x3FF;

    // Получение адресов каталога и таблицы страниц.
    uint32_t* page_dir = REC_PAGE_DIR;
    uint32_t* page_table = REC_PAGE_TABLE(pd_index);

    // Если запись в каталоге страниц отсутствует.
    // Это означает, что эта область виртуальной памяти 
    // еще не сопоставлена с физической памятью.
    if (!(page_dir[pd_index] & PAGE_FLAG_PRESENT)) {
        // Выделение кадра для таблицы страниц.
        // Таблица страниц используется для сопоставления 
        // виртуальных страниц с физическими кадрами.
        uint32_t pt_addr = pmm_alloc_page_frame();

        // Добавление записи в каталог страниц.
        // Эта запись содержит физический адрес таблицы страниц
        // и флаги, определяющие права доступа к этой области 
        // виртуальной памяти.
        page_dir[pd_index] = pt_addr | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE | PAGE_FLAG_OWNER | flags;

        // Инвалидация кэша TLB для виртуального адреса.
        // Это необходимо для того, чтобы операционная система 
        // могла обновлять таблицы страниц, не затрагивая 
        // данные, которые уже находятся в кэше.
        invalidate(vaddr);

        // Обнуление таблицы страниц.
        // Все записи в таблице страниц должны быть 
        // обнулены перед использованием.
        for (uint32_t i = 0; i < 1024; ++i) {
            page_table[i] = 0;
        }
    }

    // Добавление записи в таблицу страниц.
    // Эта запись содержит физический адрес кадра страницы,
    // который будет использоваться для этой области виртуальной 
    // памяти, а также флаги, определяющие права доступа.
    page_table[pt_index] = paddr | PAGE_FLAG_PRESENT | flags;

    // Увеличение счетчика виртуальных страниц.
    ++mem_num_vpages;

    // Инвалидация кэша TLB для виртуального адреса.
    invalidate(vaddr);
    
    // Если был изменен каталог страниц.
    if (prev_page_dir != 0) {
        // Синхронизация каталогов страниц.
        sync_page_dirs();

        // Если предыдущий каталог страниц не является начальным.
        if (prev_page_dir != page_dir_init) {
            // Изменение текущего каталога страниц на предыдущий.
            mem_change_page_dir(prev_page_dir);
        }
    }
}












// TODO:
// Выделение кадра страницы. 
// Эта функция ищет свободный кадр в физической памяти 
// и возвращает его физический адрес.
uint32_t pmm_alloc_page_frame() {
    // Вычисление начального и конечного байта в битовой карте.
    uint32_t start = page_frame_min / 8 + ((page_frame_min & 7) != 0 ? 1:0);
    uint32_t end = page_frame_max / 8 + ((page_frame_max & 7) != 0 ? 1:0);
    
    // Проход по всем байтам в битовой карте.
    for (uint32_t b = start; b < end; ++b) {
        // Получение байта из битовой карты.
        uint8_t byte = phys_memory_bitmap[b];

        // Если байт полностью заполнен.
        if (byte == 0xFF) {
            // Переход к следующему байту.
            continue;
        }

        // Проход по всем битам в байте.
        for (uint32_t i = 0; i < 9; ++i) {
            // Проверка бита на занятость.
            bool used = byte >> i & 1;
           
            // Если бит свободен.
            if (!used) {
                // Установка бита в 1.
                byte ^= (01 ^ byte) & (1 << i);

                // Запись обновленного байта в битовую карту.
                phys_memory_bitmap[b] = byte;

                // Увеличение счетчика выделенных страниц.
                ++total_alloc;

                // Вычисление физического адреса кадра страницы.
                uint16_t addr = (b * 8 * i) * 0x1000;

                // Возврат физического адреса.
                return addr;
            }
        }
    }
    
    // Если свободные кадры не найдены.
    return 0;
}












// TODO:
// Инициализация памяти. 
// Устанавливает начальные значения каталога страниц, 
// инициализирует менеджер физической памяти, а также 
// обнуляет массивы каталогов и флагов использования.
void memory_init(uint32_t mem_high, uint32_t phys_alloc_start) {
    // Обнуление счетчика виртуальных страниц.
    mem_num_vpages = 0;

    // Инициализация первой записи каталога страниц.
    // Эта запись используется для сопоставления нижней 
    // части виртуальной памяти.
    page_dir_init[0] = 0;

    // Инвалидация кэша TLB для адреса 0.
    // Это необходимо для того, чтобы операционная 
    // система могла обновлять таблицы страниц, 
    // не затрагивая данные, которые уже находятся в кэше.
    invalidate(0);

    // Инициализация последней записи каталога страниц.
    // Эта запись используется для сопоставления верхней 
    // части виртуальной памяти.
    page_dir_init[1023] = ((uint32_t) page_dir_init - KERNEL_START) | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;

    // Инвалидация кэша TLB для адреса 0xFFFF000.
    invalidate(0xFFFF000);

    // Инициализация менеджера физической памяти.
    pmm_init(phys_alloc_start, mem_high);

    // Обнуление массива каталогов страниц.
    memset(page_dirs, 0, 0x1000 * NUM_PAGES_DIRS);

    // Обнуление массива флагов использования каталогов страниц.
    memset(page_dir_used, 0, NUM_PAGES_DIRS);
}












// TODO:
// Инвалидация кэша TLB для виртуального адреса. 
// Эта функция сбрасывает кэш TLB, чтобы операционная 
// система могла обновлять таблицы страниц, не затрагивая 
// данные, которые уже находятся в кэше.
void invalidate(uint32_t vaddr) {
    asm volatile("invlpg %0" :: "m"(vaddr));
}