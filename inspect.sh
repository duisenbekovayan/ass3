#!/usr/bin/env bash
#
# inspect.sh -- collect the /proc numbers Task 2 and Task 3 ask for.
#
# Usage: ./inspect.sh <pid> [label]
#
# Run it in a SECOND terminal once while lazy_alloc is paused BEFORE
# the touch loop, and once while it is paused AFTER the touch loop.

set -u

PID="${1:-}"
LABEL="${2:-phase}"

if [ -z "$PID" ] || [ ! -d "/proc/$PID" ]; then
    echo "usage: $0 <pid> [label]" >&2
    exit 1
fi

echo "===================== $LABEL (pid=$PID) ====================="
echo

echo "--- VmSize / VmRSS / VmData from /proc/$PID/status ---"
grep -E '^(VmPeak|VmSize|VmRSS|VmData|VmStk|VmExe|VmLib):' "/proc/$PID/status"
echo

echo "--- minor/major page faults from ps ---"
ps -o pid,comm,min_flt,maj_flt,vsz,rss -p "$PID"
echo

echo "--- the 64 MB mmap region in /proc/$PID/maps ---"
while read -r line; do
    range=$(awk '{print $1}' <<<"$line")
    perms=$(awk '{print $2}' <<<"$line")
    path=$(awk '{print $6}' <<<"$line")
    [ "$perms" = "rw-p" ] || continue
    [ -z "$path" ] || continue
    start_hex=${range%-*}
    end_hex=${range#*-}
    size=$(( 0x$end_hex - 0x$start_hex ))
    if [ "$size" -ge $((64*1024*1024)) ]; then
        printf '  %s   <-- size = %d bytes (%d MB)\n' "$line" "$size" "$((size/1024/1024))"
    fi
done < "/proc/$PID/maps"
echo

if [ -x ./pagemap2 ]; then
    echo "--- present vs not-present page counts from pagemap2 ---"
    ./pagemap2 "$PID" | awk '
        /present 1/ { present++ }
        /present 0/ { absent++  }
        END { printf "  present pages = %d\n  absent  pages = %d\n", present, absent }'
fi

echo
echo "============================================================="
