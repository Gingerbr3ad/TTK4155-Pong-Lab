#!/usr/bin/env bash

#THIS SCRIPT WAS AI GENERATED

DEV="/dev/ttyS0"
BAUD=9600

# Configure serial port: 115200 8N1, raw mode
stty -F "$DEV" "$BAUD" cs8 -cstopb -parenb raw -echo

cleanup() {
    [[ -n "${READER_PID:-}" ]] && kill "$READER_PID" 2>/dev/null
}
trap cleanup EXIT INT TERM

# Read serial output in the background.
# Print only once a newline is received.
while IFS= read -r line; do
    # Remove CR if device sends CRLF
    line="${line%$'\r'}"
    printf '%s\n' "$line"
done < "$DEV" &

READER_PID=$!

# Send keyboard input to serial port
while IFS= read -r input; do
    # Change \r\n to \n if your device expects LF only.
    printf '%s\r\n' "$input" > "$DEV"
done