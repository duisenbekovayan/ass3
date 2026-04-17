#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    const size_t region_size = 64UL * 1024UL * 1024UL;
    long page_size = sysconf(_SC_PAGESIZE);
    unsigned char *region;
    size_t touched_pages = 0;

    if (page_size <= 0) {
        fprintf(stderr, "sysconf(_SC_PAGESIZE) failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    region = mmap(NULL, region_size, PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (region == MAP_FAILED) {
        fprintf(stderr, "mmap failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("PID: %ld\n", (long)getpid());
    printf("Allocated %zu bytes (%zu MiB) at %p\n",
           region_size, region_size / (1024UL * 1024UL), (void *)region);
    printf("Page size: %ld bytes\n", page_size);
    printf("\nPress Enter before touching memory...\n");
    fflush(stdout);

    if (getchar() == EOF && ferror(stdin)) {
        perror("getchar");
        munmap(region, region_size);
        return EXIT_FAILURE;
    }

    for (size_t offset = 0; offset < region_size; offset += (size_t)page_size) {
        region[offset] = 1;
        touched_pages++;
    }

    printf("Touched %zu pages.\n", touched_pages);
    printf("Press Enter after touching memory...\n");
    fflush(stdout);

    if (getchar() == EOF && ferror(stdin)) {
        perror("getchar");
        munmap(region, region_size);
        return EXIT_FAILURE;
    }

    if (munmap(region, region_size) != 0) {
        fprintf(stderr, "munmap failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
