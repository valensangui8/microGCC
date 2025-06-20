set -euo pipefail

if [ -z "$1" ]; then
    echo "Error: Please provide the test case name (e.g., 1)"
    exit 1
fi

TEST_NAME="$1"
ASM_FILE="$TEST_NAME.asm"

# Get absolute base path of project
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(realpath "$SCRIPT_DIR/../../..")"

ACCEPT_PATH="$PROJECT_ROOT/src/test/c/accept/$TEST_NAME"
ASM_OUT_DIR="$PROJECT_ROOT/src/test/asm/outputs"
ASM_OUTPUT_FILE="$ASM_OUT_DIR/$ASM_FILE"

mkdir -p "$ASM_OUT_DIR"

# Run start.sh inside the container and save output as $ASM_FILE on the host
docker run \
    --rm \
    --user root \
    --volume "$PROJECT_ROOT":/home/ubuntu/Flex-Bison-Compiler \
    --workdir /home/ubuntu/Flex-Bison-Compiler \
    flex-bison-compiler:latest \
    bash -c "./script/ubuntu/start.sh ./src/test/c/accept/$TEST_NAME" > "$ASM_OUTPUT_FILE"

echo "Assembly generated: $ASM_OUTPUT_FILE"

# Assemble with NASM (on host)
if nasm -f elf64 -g -F dwarf "$ASM_OUTPUT_FILE" -o "$ASM_OUT_DIR/output.o"; then
    echo "NASM compilation succeeded."
else
    echo "NASM compilation failed."
    exit 1
fi

# Link with LD (on host)
if ld -no-pie -o "$ASM_OUT_DIR/output" "$ASM_OUT_DIR/output.o"; then
    echo "Linking succeeded."
else
    echo "Linking with LD failed."
    exit 1
fi
