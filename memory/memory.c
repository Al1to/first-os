#include "./memory.h"

page_dir_entry kernel_page_dir[1024] __attribute__((aligned(0x1000)));
page_table_entry new_base_page_table[1024] __attribute__((aligned(0x1000)));
page_table_entry full_kernel_table_storage[1024] __attribute__((aligned(0x1000)));

page_table_entry *kernel_tables = (page_table_entry *)0xC0400000;

uint8_t pmem_used[131072] __attribute__((aligned(0x1000)));
const char * mem_types[] = {"ERROR","Available","Reserved","ACPI Reclaimable","NVS","Bad RAM"};

#define pagedir_addr 768

multiboot_memory_map_t *mmap;

bool check_phys_page(void *addr) {
	uint32_t page = (uint32_t)addr;
	page/=0x1000;
	bool r = (bool)(pmem_used[page/8] & (1 << (page%8)));
	return r;
}

bool warn_claimed = true;

void claim_phys_page(void *addr) {
	// FOR DEBUGGING
	// if (check_phys_page(addr)&&warn_claimed) {
	//     dprintf("[WARN] Attempting to claim already claimed page: %p\n", addr);
	// }
	uint32_t page = (uint32_t)addr;
	page/=0x1000;
	pmem_used[page/8]|=1<<(page%8);
}

void release_phys_page(void *addr) {
	uint32_t page = (uint32_t)addr;
	page/=0x1000;
	pmem_used[page/8]&= ~(1 << (page%8));
}

uint8_t check_page_cluster(void *addr) {
	uint32_t page = (uint32_t)addr;
	page/=0x1000;
	return pmem_used[page/8];
}

inline page_table_entry get_table_vaddr(void *vaddr) {
	return kernel_tables[(uint32_t)vaddr/0x1000];
}

void paging_init(multiboot_info_t *mbi) {
	// Этап 1:
	// Во-первых, нам нужно сопоставить новую часть оперативной памяти, чтобы у нас было достаточно места для новой таблицы подкачки
	kernel_page_dir[pagedir_addr].present = 1;
	kernel_page_dir[pagedir_addr].readwrite = 1;
	uint32_t nbpt_addr = (uint32_t)&new_base_page_table[0];
	nbpt_addr -= 0xC0000000;
	nbpt_addr >>=12;
	kernel_page_dir[pagedir_addr].address = nbpt_addr;
	for (uint16_t i = 0; i < 1023; i++) {
		if ((i*0x1000)>=0x100000) {
			new_base_page_table[i].present = 1;
			new_base_page_table[i].readwrite = 1;
			new_base_page_table[i].address = i;
		}
	}
	new_base_page_table[1023].present = 1;
	new_base_page_table[1023].readwrite = 1;
	new_base_page_table[1023].address = 0x000B8;

	uint32_t new_addr = (uint32_t)&kernel_page_dir[0];
	new_addr-=0xC0000000;
	asm volatile("mov %0, %%cr3":: "r"(new_addr));

	// Этап 2:
	// Далее мы выделяем 4 МБ пространства для нашей новой таблицы страниц FULL.

	kernel_page_dir[pagedir_addr+1].present = 1;
	kernel_page_dir[pagedir_addr+1].readwrite =1;
	kernel_page_dir[pagedir_addr+1].address = (((uint32_t)&full_kernel_table_storage)-0xC0000000)>>12;

	for (uint16_t i = 0; i < 1024; i++) {
		full_kernel_table_storage[i].present = 1;
		full_kernel_table_storage[i].readwrite = 1;
		full_kernel_table_storage[i].address = (0x400000>>12)+i;
	}

    // reset cr3, чтобы он очистил кэш TLB?
	asm volatile("movl %cr3, %ecx; movl %ecx, %cr3");

	// Этап 3:
	// Создаем новую полностраничную таблицу, чтобы мы могли выделять элементы в любое время

	memset((void*)0xC0400000,0,0x400000);

	for (uint16_t i = 0; i < 1023; i++) {
		if ((i*0x1000)>=0x100000) {
			kernel_tables[(768*1024)+i].present = 1;
			kernel_tables[(768*1024)+i].readwrite = 1;
			kernel_tables[(768*1024)+i].address = i;
		}
	}

	kernel_tables[(768*1024)+1023].present = 1;
	kernel_tables[(768*1024)+1023].readwrite = 1;
	kernel_tables[(768*1024)+1023].address = 0x000B8;

	for (uint16_t i = 0; i < 1024; i++) {
		kernel_tables[(769*1024)+i].present = 1;
		kernel_tables[(769*1024)+i].readwrite = 1;
		kernel_tables[(769*1024)+i].address = (0x400000>>12)+i;
	}

	// Теперь мы связываем таблицы с каталогом страниц, расположение которого не изменится
	for (uint16_t i = 0; i < 1024; i++) {
		kernel_page_dir[i].address = 0x400+(i);
		kernel_page_dir[i].readwrite = 1;
		kernel_page_dir[i].present = 1;
        
        // Все записи каталога страниц принадлежат пользователю, потому что мы можем установить супервизора только внутри записи таблицы страниц.
		kernel_page_dir[i].user = 1; 
	}

    // reset cr3, чтобы он очистил кэш TLB?
	asm volatile("movl %cr3, %ecx; movl %ecx, %cr3");

	// Потрясающий! Теперь таблицы страниц по умолчанию находятся по адресам vaddr:0xC0400000 и Paddr:0x400000.
	// Это не помешает программам (которые загружаются по адресу vaddr:0x100000), но у них по-прежнему мало памяти (сейчас нам нужно всего 8 МБ памяти)

	// Разрешим нам прочитать MBI, чтобы найти карту памяти.
	identity_map(mbi);
	mmap = (multiboot_memory_map_t *)mbi->mmap_addr;

	// Очищаем текущую карту, чтобы все записи были "затребованы"
	memset((void *)&pmem_used[0],255,0x20000);

	// Сканируем карту памяти на предмет областей, которые мы можем использовать для общих целей
	for (uint8_t i = 0; i < 15; i++) {
		uint32_t type = mmap[i].type;
		if (mmap[i].type>5)
			type = 2;
		if (i>0&&mmap[i].addr==0)
			break;
		if (mmap[i].type==1){
			for (uint64_t physptr = mmap[i].addr; physptr<mmap[i].addr+mmap[i].len;physptr+=0x1000) {
				release_phys_page((void *)(uint32_t)physptr);
			}
		}
		// vga_printf("%u: %p+%p %s\n",i,(uintptr_t)mmap[i].addr,(uintptr_t)mmap[i].len,mem_types[type]);
	}

	// Освобождаем до 8МБ (для ядра и таблиц страниц)
	memset((void *)&pmem_used[0],255,256);
}

void identity_map(void *addr) {
	uint32_t page = (uint32_t)addr;
	page/=0x1000;
	// Don't do anything if already mapped.
	if (kernel_tables[page].present&&kernel_tables[page].readwrite&&kernel_tables[page].address==(uint32_t)addr>>12)
		return;
	warn_claimed = false;
	claim_phys_page(addr);
	warn_claimed = true;
	/* volatile uint32_t directory_entry = page/1024;
	volatile uint32_t table_entry = page % 1024; */
	kernel_tables[page/* (directory_entry*1024)+table_entry */].address = page;
	kernel_tables[page/* (directory_entry*1024)+table_entry */].readwrite = 1;
	kernel_tables[page/* (directory_entry*1024)+table_entry */].present = 1;
	asm volatile("movl %cr3, %ecx; movl %ecx, %cr3");
}

void map_addr(void *vaddr, void *paddr) {
	claim_phys_page(paddr);
	uint32_t vaddr_page = (uint32_t)vaddr;
	vaddr_page/=0x1000;
	uint32_t paddr_page = (uint32_t)paddr;
	paddr_page/=0x1000;
	kernel_tables[vaddr_page].address = paddr_page;
	kernel_tables[vaddr_page].readwrite = 1;
	kernel_tables[vaddr_page].present = 1;
	asm volatile("movl %cr3, %ecx; movl %ecx, %cr3");
}

void unmap_vaddr(void *vaddr) {
	uint32_t vaddr_page = (uint32_t)vaddr;
	vaddr_page/=0x1000;
	release_phys_page((void *)(kernel_tables[vaddr_page].address*0x1000));
	kernel_tables[vaddr_page].address = 0;
	kernel_tables[vaddr_page].readwrite = 0;
	kernel_tables[vaddr_page].present = 0;
	asm volatile("movl %cr3, %ecx; movl %ecx, %cr3");
}

void trade_vaddr(void *vaddr) {
	uint32_t vaddr_page = (uint32_t)vaddr;
	vaddr_page/=0x1000;
	// Не освобождаем страницу, так как программа отдала ее в другое адресное пространство
	kernel_tables[vaddr_page].address = 0;
	kernel_tables[vaddr_page].readwrite = 0;
	kernel_tables[vaddr_page].present = 0;
	asm volatile("movl %cr3, %ecx; movl %ecx, %cr3");
}

void mark_user(void *vaddr, bool user) {
	kernel_tables[(uint32_t)vaddr/0x1000].user = user?1:0;
	asm volatile("movl %cr3, %ecx; movl %ecx, %cr3");
}

void mark_write(void *vaddr, bool write) {
	kernel_tables[(uint32_t)vaddr/0x1000].readwrite = write?1:0;
	asm volatile("movl %cr3, %ecx; movl %ecx, %cr3");
}

void *get_phys_addr(void *vaddr) {
	return (void *)((kernel_tables[(uint32_t)vaddr/0x1000].address*0x1000)+((uint32_t)vaddr&0xFFF));
}

void *find_free_phys_page() {
	for (uint32_t k=0x800; k<1048576; k++) {
		if (check_page_cluster((void *)(k*0x1000))==255)
			k+=7;
		else if (!check_phys_page((void *)(k*0x1000))) {
			return (void *)(uintptr_t)(k*0x1000);
		}
	}
	return 0;
}

void* map_page_to(void *vaddr) {
	if (kernel_tables[(uint32_t)vaddr/0x1000].present) return 0;

	void *paddr = find_free_phys_page();
	if (!paddr) return 0;

    map_addr(vaddr, paddr);
    return vaddr;
}

void map_page_secretly(void *vaddr, void *paddr) {
	uint32_t vaddr_page = (uint32_t)vaddr;
	vaddr_page/=0x1000;
	uint32_t paddr_page = (uint32_t)paddr;
	paddr_page/=0x1000;
	kernel_tables[vaddr_page].address = paddr_page;
	kernel_tables[vaddr_page].readwrite = 1;
	kernel_tables[vaddr_page].present = 1;
	asm volatile("movl %cr3, %ecx; movl %ecx, %cr3");
}

void unmap_secret_page(void *vaddr, size_t pages) {
	uint32_t vaddr_page = (uint32_t)vaddr;
	vaddr_page/=0x1000;
	for (size_t i = 0; i < pages; i++) {
		kernel_tables[vaddr_page+i].address = 0;
		kernel_tables[vaddr_page+i].readwrite = 0;
		kernel_tables[vaddr_page+i].present = 0;
	}
	asm volatile("movl %cr3, %ecx; movl %ecx, %cr3");
}

void *map_paddr(void *paddr, size_t pages) {
	if (!pages)
		return 0;
	uint32_t paddr_page = (uint32_t)paddr;
	paddr_page/=0x1000;
	for(uint32_t i = 0xC0000; i < 0x100000; i++) {
		if (!kernel_tables[i].present) {
			 size_t successful_pages = 1;
			for (uint32_t j = i+1; j-i<pages+1; j++) {
				if (successful_pages==pages)
						break;
				if (!kernel_tables[j].present) {
					successful_pages++;
				}
				else
					break;
			}
			if (successful_pages==pages) {
				for (uint32_t step=0; step<pages;step++) {
					map_page_secretly((void *)(uintptr_t)((i+step)*0x1000),(void *)(uintptr_t)((paddr_page+step)*0x1000));
				}
				return (void *)(uintptr_t)(i*0x1000)+((uintptr_t)paddr&0xFFF);
			}
		}
	}
	return 0;
}

void* alloc_page(size_t pages) {
	if (!pages)
		return 0;
	// Сначала найдите последовательные виртуальные страницы в памяти ядра.
	for(uint32_t i = 0xC0000; i < 0x100000; i++) {
		if (!kernel_tables[i].present) {
			 size_t successful_pages = 1;
			for (uint32_t j = i+1; j-i<pages+1; j++) {
				if (successful_pages==pages)
						break;
				if (!kernel_tables[j].present) {
					successful_pages++;
				}
				else
					break;
			}
			if (successful_pages==pages) {
				for (uint32_t step=0; step<pages;step++) {
					void *paddr = find_free_phys_page();
					if (!paddr) {
						return 0;
					}
					map_addr((void *)((i+step)*0x1000), paddr);
				}
				return (void *)(i*0x1000);
			}
		}
	}
	return 0;
}

void free_page(void *start, size_t pages) {
	for (uint32_t i = 0; i < pages; i++) {
		unmap_vaddr((void *)((uint32_t)start+(i*0x1000)));
	}
}

void *calloc_page(size_t pages) {
	void *out = alloc_page(pages);
	memset(out, 0, 0x1000*pages);
	return out;
}

// Аналогично alloc_page, но требует, чтобы физические страницы были последовательными.
void *alloc_sequential(size_t pages) {
	if (!pages)
		return 0;
	for(uint32_t i = 0xC0000; i < 0x100000; i++) {
		if (!kernel_tables[i].present) {
			size_t successful_pages = 1;
			for (uint32_t j = i+1; j-i<pages+1; j++) {
				if (successful_pages==pages)
						break;
				if (!kernel_tables[j].present) {
					successful_pages++;
				}
				else
					break;
			}
			if (successful_pages==pages) {
				for (uint32_t j = 0x1000; j < 0x100000; j++) {
					successful_pages = 1;
					if (check_page_cluster((void *)(j*0x1000))==255)
						j+=7;
					else if (!check_phys_page((void *)(j*0x1000))) {
						for (uint32_t k = j+1; k-j<pages+1; k++) {
							if (successful_pages==pages)
									break;
							if (!check_phys_page((void *)(k*0x1000))) {
								successful_pages++;
							}
							else
								break;
						}
						if (successful_pages==pages) {
							for (uint32_t k = 0; k < pages; k++) {
								map_addr((void *)(uintptr_t)((i+k)*0x1000),(void *)(uintptr_t)((j+k)*0x1000));
							}
							return (void *)(i*0x1000);
						}
					}
				}
			}
		}
	}
	return 0;
}

uint32_t *get_current_tables() {
	return (uint32_t *)kernel_tables;
}

void switch_tables(void *new) {
	kernel_tables = new;
}

void use_kernel_map() {
	kernel_tables = (page_table_entry *)0xC0400000;
	asm volatile("mov %0, %%cr3"::"r"(get_phys_addr(&kernel_page_dir[0])));
}

void *clone_tables() {
	void * new = alloc_sequential(1025);
	memset(new,0,0x1000*1025);
	page_table_entry * new_page_tables = new+0x1000;
	page_dir_entry * new_page_dir = new;
	memcpy(new_page_tables,kernel_tables,1024*0x1000);
	memset(new_page_dir,0,0x1000);
	for (uint32_t i = 0; i < 1024; i++) {
		page_dir_entry *pde = &new_page_dir[i];
		pde->user = 1;
		pde->present = 1;
		pde->readwrite = 1;
		pde->address = ((uint32_t)get_phys_addr(new+0x1000)/0x1000)+i;
	}

	uint32_t new_addr = (uint32_t)get_phys_addr(new);
	switch_tables(new+0x1000);
	asm volatile("mov %0, %%cr3":: "r"(new_addr));

	return (void *)new_addr;
}

uint32_t free_pages() {
	uint32_t retval = 0;
	for (uint32_t i = 0; i < 0x20000; i++) {
		if (!pmem_used[i])
			retval+=8;
		else if (pmem_used[i]!=(uint8_t)255) {
			for (uint8_t j = 0; j < 8; j++) {
				if (!check_phys_page((void *)(i*8*0x1000)+(j*0x1000)))
					retval++;
			}
		}
	}
	return retval;
}

void *realloc_page(void *ptr, uint32_t old_pages, uint32_t new_pages) {
	int64_t amt_pages = (int64_t)new_pages - (int64_t)old_pages;
	uint32_t ptr_page = (uint32_t)ptr/0x1000;
	if (old_pages==new_pages)
		return ptr;
	else if (old_pages<new_pages) {
		bool failed = false;
		for (uint32_t i = 0; i < (uint32_t)amt_pages; i++) {
			if (kernel_tables[ptr_page+old_pages+i].present) {
				failed = true;
				break;
			}
		}
		if (failed) {
			void * ret = alloc_page(new_pages);
			memcpy(ret,ptr,old_pages*0x1000);
			free_page(ptr,old_pages);
			return ret;
		} else {
			for (uint32_t i = 0; i < (uint32_t)amt_pages; i++) {
				map_page_to(ptr+(old_pages*0x1000)+(i*0x1000));
			}
			return ptr;
		}
	} else {
		for (int64_t i = 0; i < -amt_pages; i++) {
			unmap_vaddr(ptr+(new_pages*0x1000)+(i*0x1000));
		}
		return ptr;
	}
}

void free_all_user_pages() {
	for (uint32_t i = 0; i < 1024*1024; i++) {
		if (kernel_tables[i].user&&kernel_tables[i].present) {
			free_page((void *)(uintptr_t)(i*0x1000),1);
		}
	}
}

uint8_t clone_data[0x1000];

bool clone_user_pages() {
	for (uint32_t i = 0; i < 1024*1024; i++) {
		if (kernel_tables[i].user&&kernel_tables[i].present) {
			// Find free physical page
			void *paddr = find_free_phys_page();
			if (!paddr) {
				return false;
			}

			// Copy the data from the original page
			memcpy(clone_data, (void *)(i*0x1000), 0x1000);

			// Swap the virtual address' physical page for the new physical page
			kernel_tables[i].address = (uint32_t)paddr>>12;
			claim_phys_page(paddr);

			// Set page to writeable when we are copying it
			uint8_t rw = kernel_tables[i].readwrite;
			kernel_tables[i].readwrite = true;

			// Flush TLB to update the map
			asm volatile("movl %%cr3, %%ecx; movl %%ecx, %%cr3":::"ecx","memory");

			// Copy the data into the new page
			memcpy((void *)(i*0x1000), clone_data, 0x1000);

			// Set page's write permission back to what it was before.
			// This will be updated next time we flush the TLB.
			kernel_tables[i].readwrite = rw;
		}
	}
	// Fix last changed page's write permission
	asm volatile("movl %%cr3, %%ecx; movl %%ecx, %%cr3":::"ecx","memory");
	return true;
}

bool check_user(void *vaddr) {
	uint32_t vaddr_page = (uint32_t)vaddr;
	vaddr_page/=0x1000;
	return kernel_tables[vaddr_page].user;
}

uint8_t get_page_permissions(void *vaddr) {
	uint32_t vaddr_page = (uint32_t)vaddr;
	vaddr_page/=0x1000;
	return kernel_tables[vaddr_page].present |
			(kernel_tables[vaddr_page].readwrite<<1) |
			(kernel_tables[vaddr_page].user<<2);
}