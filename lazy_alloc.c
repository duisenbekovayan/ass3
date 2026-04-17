/*
 * lazy_alloc.c
 *
 * CSCI 332 - Assignment 3, Task 2: Demonstrate demand paging.
 *
 * 1. Allocate 64 MB with mmap (MAP_PRIVATE | MAP_ANONYMOUS).
 * 2. Print the PID.
 * 3. Pause (getchar) BEFORE touching memory so the process can be inspected.
 * 4. Touch one byte per page, stepping with sysconf(_SC_PAGESIZE).
 * 5. Pause (getchar) AFTER touching memory for a second inspection.
 *
 * Build:  gcc -Wall -O0 -o lazy_alloc lazy_alloc.c
 * Run:    ./lazy_alloc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#define ALLOC_BYTES (64UL * 1024UL * 1024UL)

int main(void)
{
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) {
        perror("sysconf(_SC_PAGESIZE)");
        return 1;
    }

    unsigned char *buf = mmap(NULL, ALLOC_BYTES,
                              PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS,
                              -1, 0);
    if (buf == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    size_t num_pages = ALLOC_BYTES / (size_t)page_size;

    printf("PID           = %d\n", (int)getpid());
    printf("page size     = %ld bytes\n", page_size);
    printf("allocated     = %lu bytes (%.0f MB)\n",
           ALLOC_BYTES, (double)ALLOC_BYTES / (1024.0 * 1024.0));
    printf("virtual base  = %p\n", (void *)buf);
    printf("pages in buf  = %zu\n", num_pages);
    fflush(stdout);

    printf("\n[Phase 1] Memory is mapped but NOT touched yet.\n");
    printf("Inspect this process in another terminal, then press ENTER...\n");
    fflush(stdout);
    getchar();

    for (size_t i = 0; i < num_pages; ++i) {
        buf[i * (size_t)page_size] = (unsigned char)(i & 0xFF);
    }

    printf("\n[Phase 2] Every page has been touched once (one byte per page).\n");
    printf("Inspect this process again, then press ENTER to exit...\n");
    fflush(stdout);
    getchar();

    if (munmap(buf, ALLOC_BYTES) != 0) {
        perror("munmap");
        return 1;
    }

    return 0;
}
