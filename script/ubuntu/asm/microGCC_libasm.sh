#!/bin/bash

set -euo pipefail

if [ -z "${1:-}" ]; then
    echo "Error: Please provide the test case name (e.g., 1)"
    exit 1
fi

TEST_NAME="$1"
ASM_FILE="$TEST_NAME.asm"

# Get absolute path to this script and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(realpath "$SCRIPT_DIR/../../..")"

ACCEPT_DIR="$PROJECT_ROOT/src/test/c/accept"
ASM_OUT_DIR="$PROJECT_ROOT/src/test/asm/outputs"
ASM_LIB="$PROJECT_ROOT/src/test/asm/lib/libasm.asm"

# Ensure outputs directory exists
mkdir -p "$ASM_OUT_DIR"

# Run the container, generate .asm, and save it
docker run \
    --rm \
    --user root \
    --volume "$PROJECT_ROOT":/home/ubuntu/Flex-Bison-Compiler \
    --workdir /home/ubuntu/Flex-Bison-Compiler \
    flex-bison-compiler:latest \
    bash -c "./script/ubuntu/start.sh ./src/test/c/accept/$TEST_NAME" > "$ASM_OUT_DIR/$ASM_FILE"

echo "Assembly generated: $ASM_OUT_DIR/$ASM_FILE"

# Assemble the test file
if nasm -f elf64 -g -F dwarf "$ASM_OUT_DIR/$ASM_FILE" -o "$ASM_OUT_DIR/output.o"; then
    echo "NASM compilation succeeded: test"
else
    echo "NASM compilation failed: test"
    exit 1
fi

# Assemble the libasm
if nasm -f elf64 -g -F dwarf "$ASM_LIB" -o "$ASM_OUT_DIR/libasm.o"; then
    echo "NASM compilation succeeded: libasm"
else
    echo "NASM compilation failed: libasm"
    exit 1
fi

# Link both together
if ld -no-pie -o "$ASM_OUT_DIR/output" "$ASM_OUT_DIR/output.o" "$ASM_OUT_DIR/libasm.o"; then
    echo "Linking succeeded."
else
    echo "Linking failed."
    exit 1
fi

