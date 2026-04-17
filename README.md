# ass3

Starter materials for the paging/page-fault assignment.

## Files in this repo

- `lazy_alloc.c`: a minimal program for demonstrating demand paging with a 64 MiB anonymous `mmap()`

## Build

```bash
gcc -Wall -Wextra -O2 -o lazy_alloc lazy_alloc.c
```

## Run

```bash
./lazy_alloc
```

The program:

1. allocates 64 MiB with `mmap()`
2. prints its PID
3. pauses before touching the mapping
4. writes one byte per page using the system page size
5. pauses again so you can inspect the process after first touch

## Helpful inspection commands

In a second terminal, replace `<pid>` with the printed PID:

```bash
sed -n '1,25p' /proc/<pid>/maps
cat /proc/<pid>/stat
./pagemap2 <pid>
```

If you want to watch fault counters more clearly:

```bash
awk '{print "minflt=" $10, "majflt=" $12}' /proc/<pid>/stat
```

`minflt` should increase much more than `majflt` for this assignment because anonymous demand-zero pages usually cause minor faults, not disk-backed major faults.

## Suggested workflow for the assignment

### Task 1

- choose a running process
- capture `/proc/<pid>/maps`
- run `./pagemap2 <pid>`
- identify code, heap, and stack regions
- look for a mapped-but-not-present page if your environment exposes that information

### Task 2

1. start `./lazy_alloc`
2. stop at the first `Press Enter...`
3. inspect the process and take Screenshot A
4. press Enter in the program terminal so it touches one byte per page
5. inspect the process again and take Screenshot B

### Task 3

Record a small table with:

- phase
- `minflt`
- `majflt`
- short observation

You can capture values before touching memory and after touching memory with:

```bash
awk '{print "minflt=" $10, "majflt=" $12}' /proc/<pid>/stat
```

## Notes for the report

- A mapping in `/proc/<pid>/maps` describes virtual address space, not guaranteed RAM residency.
- A page can be mapped but not present until the first access triggers a page fault.
- First touch of anonymous pages typically results in a minor fault because the kernel allocates a zero-filled physical page without loading it from disk.
- Touching one byte is enough because presence is tracked at page granularity, not byte granularity.

## External tool used by the assignment

The PDF asks you to build `pagemap2` from:

```bash
git clone https://github.com/dwks/pagemap.git
cd pagemap
make
```

Then run it against the PID you want to inspect:

```bash
./pagemap2 <pid>
```
