#!/bin/sh
set -eu

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 /path/to/zpaqfranz [file-size-MiB] [ram-budget-MiB] [timeout-seconds]" >&2
    exit 2
fi

executable=$1
file_size_mib=${2:-5}
ram_budget_mib=${3:-2}
timeout_seconds=${4:-60}

if [ ! -x "$executable" ]; then
    echo "Executable does not exist or is not executable: $executable" >&2
    exit 2
fi
if [ "$ram_budget_mib" -ge "$file_size_mib" ]; then
    echo "ram-budget-MiB must be smaller than file-size-MiB" >&2
    exit 2
fi

case "$executable" in
    /*) ;;
    *) executable=$(cd "$(dirname "$executable")" && pwd)/$(basename "$executable") ;;
esac

if command -v timeout >/dev/null 2>&1; then
    timeout_command=timeout
elif command -v gtimeout >/dev/null 2>&1; then
    timeout_command=gtimeout
else
    echo "GNU timeout or gtimeout is required" >&2
    exit 2
fi

if command -v sha256sum >/dev/null 2>&1; then
    hash_file() { sha256sum "$1" | awk '{print $1}'; }
elif command -v shasum >/dev/null 2>&1; then
    hash_file() { shasum -a 256 "$1" | awk '{print $1}'; }
else
    echo "sha256sum or shasum is required" >&2
    exit 2
fi

test_root=$(mktemp -d "${TMPDIR:-/tmp}/zpaqfranz-w-ramdisk.XXXXXXXX")
trap 'rm -rf "$test_root"' EXIT HUP INT TERM

source_directory=$test_root/source
restore_directory=$test_root/restore
archive_path=$test_root/streaming.zpaq
mkdir -p "$source_directory" "$restore_directory"

# A deterministic, highly compressible fixture keeps archive setup fast. Size
# and SHA-256 verification still detect truncation, gaps, and offset mistakes.
dd if=/dev/zero of="$source_directory/payload.bin" bs=1048576 count="$file_size_mib" 2>/dev/null
dd if=/dev/zero of="$source_directory/small.bin" bs=524288 count=1 2>/dev/null
source_hash=$(hash_file "$source_directory/payload.bin")
small_source_hash=$(hash_file "$source_directory/small.bin")

(
    cd "$source_directory"
    "$timeout_command" 120 "$executable" a "$archive_path" payload.bin small.bin -m0 -noeta -nocolor
)

extract_log=$test_root/extract.log
if ! (
    cd "$test_root"
    "$timeout_command" "$timeout_seconds" "$executable" w "$archive_path" \
        -to "$restore_directory" \
        -ramdisk \
        -maxsize "${ram_budget_mib}MB" \
        -checksum \
        -verify \
        -noeta \
        -nocolor >"$extract_log" 2>&1
); then
    cat "$extract_log" >&2
    exit 1
fi
cat "$extract_log"

file_bytes=$((file_size_mib * 1048576))
window_bytes=$((ram_budget_mib * 1048576))
window_count=$(((file_bytes + window_bytes - 1) / window_bytes))
window_index=0
while [ "$window_index" -lt "$window_count" ]; do
    window_offset=$((window_index * window_bytes))
    current_window_bytes=$((file_bytes - window_offset))
    if [ "$current_window_bytes" -gt "$window_bytes" ]; then
        current_window_bytes=$window_bytes
    fi
    expected_window_log="RAM window $((window_index + 1))/$window_count offset=$window_offset size=$current_window_bytes"
    if ! grep -F "$expected_window_log" "$extract_log" >/dev/null; then
        echo "Missing window extraction evidence: $expected_window_log" >&2
        exit 1
    fi
    window_index=$((window_index + 1))
done

restored_path=$(find "$restore_directory" -type f -name payload.bin -print)
restored_count=$(printf '%s\n' "$restored_path" | awk 'NF { count++ } END { print count + 0 }')
if [ "$restored_count" -ne 1 ]; then
    echo "Expected exactly one restored payload.bin, found $restored_count" >&2
    exit 1
fi

restored_hash=$(hash_file "$restored_path")
if [ "$restored_hash" != "$source_hash" ]; then
    echo "Restored SHA-256 mismatch: expected $source_hash, got $restored_hash" >&2
    exit 1
fi

restored_small_path=$(find "$restore_directory" -type f -name small.bin -print)
restored_small_count=$(printf '%s\n' "$restored_small_path" | awk 'NF { count++ } END { print count + 0 }')
if [ "$restored_small_count" -ne 1 ]; then
    echo "Expected exactly one restored small.bin, found $restored_small_count" >&2
    exit 1
fi
small_restored_hash=$(hash_file "$restored_small_path")
if [ "$small_restored_hash" != "$small_source_hash" ]; then
    echo "Small-file SHA-256 mismatch: expected $small_source_hash, got $small_restored_hash" >&2
    exit 1
fi

echo "PASS: w -ramdisk restored a ${file_size_mib} MiB windowed file and a normal RAM-batch file with a ${ram_budget_mib} MiB budget."
echo "SHA-256: $source_hash"
