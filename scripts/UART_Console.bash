#!/usr/bin/env bash

#THIS SCRIPT IS AI GENERATED

DEV="/dev/ttyS0"
BAUD=9600

# What gets appended when you press Enter.
TX_EOL=$'\n'

PROMPT="> "

# ---------------------------------------------------------------------------
# Setup
# ---------------------------------------------------------------------------

if [[ ! -e "$DEV" ]]; then
    echo "Serial device does not exist: $DEV" >&2
    exit 1
fi

if ! command -v tput >/dev/null 2>&1; then
    echo "This script requires 'tput'." >&2
    exit 1
fi

# FD 3 = serial port
# FD 4 = user's terminal
exec 3<> "$DEV" || exit 1
exec 4<> /dev/tty || exit 1

# Save current settings so we can restore them later.
TTY_STATE=$(stty -g <&4)
DEV_STATE=$(stty -g -F "$DEV")

# Configure serial port: 8N1, raw data.
stty -F "$DEV" raw -echo "$BAUD" cs8 -cstopb -parenb

# Put the user's terminal into character-at-a-time mode.
#
# IMPORTANT: we do NOT use "raw" here.
# ISIG remains enabled, so Ctrl+C still produces SIGINT.
stty -echo -icanon isig min 1 time 0 <&4

INPUT=""
SERIAL_PARTIAL=""
RESIZED=1
ALT_SCREEN=0

# ---------------------------------------------------------------------------
# Cleanup
# ---------------------------------------------------------------------------

cleanup() {
    trap - EXIT INT TERM HUP QUIT WINCH

    # Restore the serial port.
    if [[ -n "${DEV_STATE:-}" ]]; then
        stty -F "$DEV" "$DEV_STATE" 2>/dev/null
    fi

    # Restore full-screen scrolling before leaving.
    if [[ -n "${ROWS:-}" && "$ROWS" -gt 0 ]]; then
        tput csr 0 $((ROWS - 1)) >&4 2>/dev/null
    fi

    # Restore the original terminal screen.
    if (( ALT_SCREEN )); then
        tput rmcup >&4 2>/dev/null
    fi

    # Most importantly, restore keyboard settings.
    if [[ -n "${TTY_STATE:-}" ]]; then
        stty "$TTY_STATE" <&4 2>/dev/null
    fi
}

trap cleanup EXIT

# Ctrl+C -> exit status 130.
trap 'exit 130' INT
trap 'exit 143' TERM
trap 'exit 129' HUP
trap 'exit 131' QUIT

# Don't allow Ctrl+Z to suspend us while the terminal is in this mode.
trap '' TSTP

# Window resize just sets a flag. The main loop will redraw safely.
trap 'RESIZED=1' WINCH

# Use the terminal's alternate screen, like less/vim/top.
if tput smcup >&4 2>/dev/null; then
    ALT_SCREEN=1
fi

# ---------------------------------------------------------------------------
# UI functions
# ---------------------------------------------------------------------------

draw_input() {
    local available visible

    available=$((COLS - ${#PROMPT} - 1))

    if (( available < 1 )); then
        available=1
    fi

    # If the command is wider than the terminal, show its end.
    if (( ${#INPUT} > available )); then
        visible="${INPUT: -available}"
    else
        visible="$INPUT"
    fi

    # Bottom row = input field.
    tput cup $((ROWS - 1)) 0 >&4
    tput el >&4

    printf '%s%s' "$PROMPT" "$visible" >&4
}

draw_separator() {
    local separator width

    width=$((COLS - 1))

    if (( width < 1 )); then
        width=1
    fi

    printf -v separator '%*s' "$width" ''
    separator=${separator// /-}

    # Second-to-last row = separator.
    tput cup $((ROWS - 2)) 0 >&4
    tput el >&4

    printf '%s' "$separator" >&4
}

setup_layout() {
    ROWS=$(tput lines)
    COLS=$(tput cols)

    # Need:
    #   output area
    #   separator
    #   input line
    if (( ROWS < 4 )); then
        ROWS=4
    fi

    OUTPUT_BOTTOM=$((ROWS - 3))

    # Temporarily restore full-screen scrolling.
    tput csr 0 $((ROWS - 1)) >&4

    tput clear >&4

    # Only rows 0 .. OUTPUT_BOTTOM are allowed to scroll.
    #
    # The final two rows therefore remain fixed.
    tput csr 0 "$OUTPUT_BOTTOM" >&4

    draw_separator
    draw_input

    RESIZED=0
}

print_serial_line() {
    local line="$1"

    # Remove CR when the device sends CRLF.
    line="${line%$'\r'}"

    # Move to the final row of the scrolling area.
    tput cup "$OUTPUT_BOTTOM" 0 >&4
    tput el >&4

    printf '%s\n' "$line" >&4

    # Serial output cannot scroll these rows because they're outside
    # the terminal scroll region.
    draw_separator
    draw_input
}

send_input() {
    printf '%s%s' "$INPUT" "$TX_EOL" >&3

    INPUT=""
    draw_input
}

handle_key() {
    local key="$1"
    local dummy

    case "$key" in

        # Enter
        $'\n'|$'\r')
            send_input
            ;;

        # Backspace / Delete
        $'\x7f'|$'\b')
            if [[ -n "$INPUT" ]]; then
                INPUT="${INPUT%?}"
                draw_input
            fi
            ;;

        # Ctrl+U: clear current input line
        $'\x15')
            INPUT=""
            draw_input
            ;;

        # Escape sequence, e.g. arrow keys.
        #
        # We don't implement cursor movement/history yet, so consume
        # the rest of the common three-byte escape sequence.
        $'\e')
            IFS= read -r -s -N1 -t 0.01 -u 4 dummy 2>/dev/null || true

            if [[ "$dummy" == "[" ]]; then
                IFS= read -r -s -N1 -t 0.01 -u 4 dummy 2>/dev/null || true
            fi
            ;;

        *)
            # Only put printable characters in the input field.
            if [[ "$key" == [[:print:]] ]]; then
                INPUT+="$key"
                draw_input
            fi
            ;;
    esac
}

# ---------------------------------------------------------------------------
# Main loop
# ---------------------------------------------------------------------------

setup_layout

while true; do

    if (( RESIZED )); then
        setup_layout
    fi

    # -----------------------------------------------------------------------
    # Check serial input.
    #
    # "read" waits for LF, but has a short timeout so keyboard input remains
    # responsive. Any incomplete serial data is retained until LF arrives.
    # -----------------------------------------------------------------------

    chunk=""

    IFS= read -r -t 0.005 -u 3 chunk
    status=$?

    if (( status == 0 )); then

        # Complete line received.
        line="${SERIAL_PARTIAL}${chunk}"
        SERIAL_PARTIAL=""

        print_serial_line "$line"

    elif (( status > 128 )); then

        # Timeout. Bash preserves any partial input it received.
        if [[ -n "$chunk" ]]; then
            SERIAL_PARTIAL+="$chunk"
        fi
    fi

    # -----------------------------------------------------------------------
    # Check keyboard input.
    # -----------------------------------------------------------------------

    key=""

    if IFS= read -r -s -N1 -t 0.005 -u 4 key; then
        handle_key "$key"
    fi
done