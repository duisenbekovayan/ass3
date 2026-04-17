#!/usr/bin/env bash
# drive_test.sh -- smoke-test lazy_alloc end-to-end:
#   start it, capture the PID it prints, inspect BEFORE touch,
#   send ENTER, inspect AFTER touch, send ENTER, wait.
#
# This script is only for local verification; for the real screenshots
# you should run ./lazy_alloc in one terminal and ./inspect.sh <pid> in
# a second terminal so the inspection output is visible on screen.
set -u
cd "$(dirname "$0")"

coproc LA { stdbuf -oL ./lazy_alloc; }

# Read startup banner and capture the PID the program itself prints.
LA_REAL_PID=""
while IFS= read -r -t 2 -u "${LA[0]}" line; do
    echo "  [la] $line"
    case "$line" in
        "PID           = "*)
            LA_REAL_PID="${line##* = }"
            ;;
        *"press ENTER"*) break ;;
    esac
done

if [ -z "$LA_REAL_PID" ]; then
    echo "Could not capture lazy_alloc PID" >&2
    kill "$LA_PID" 2>/dev/null
    exit 1
fi
echo "lazy_alloc real PID = $LA_REAL_PID"

echo
echo "=============================================================="
echo "                PHASE 1 -- BEFORE touch loop"
echo "=============================================================="
./inspect.sh "$LA_REAL_PID" "BEFORE TOUCH"

printf '\n' >&"${LA[1]}"

while IFS= read -r -t 5 -u "${LA[0]}" line; do
    echo "  [la] $line"
    case "$line" in
        *"press ENTER to exit"*) break ;;
    esac
done

echo
echo "=============================================================="
echo "                PHASE 2 -- AFTER touch loop"
echo "=============================================================="
./inspect.sh "$LA_REAL_PID" "AFTER TOUCH"

printf '\n' >&"${LA[1]}"
wait "$LA_PID" 2>/dev/null
echo "lazy_alloc exited"
