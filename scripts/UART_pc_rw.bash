#!/usr/bin/env bash

#THIS SCRIPT IS AI GENERATED

DEV="/dev/ttyS0"
BAUD=9600

# Configure serial port: 115200 baud, 8N1, raw serial data
stty -F "$DEV" "$BAUD" cs8 -cstopb -parenb raw -echo

cleanup() {
    if [[ -n "${READER_PID:-}" ]]; then
        kill "$READER_PID" 2>/dev/null
        wait "$READER_PID" 2>/dev/null
    fi
}

trap cleanup EXIT

# Read from serial port.
# read waits until the device sends '\n'.
while IFS= read -r line; do
    # Strip '\r' if the device sends CRLF.
    line="${line%$'\r'}"
    printf '%s\n' "$line"
done < "$DEV" &

READER_PID=$!

# Read keyboard input one complete line at a time.
while IFS= read -r input; do
    printf '%s\n' "$input" > "$DEV"
done