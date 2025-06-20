SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(realpath "$SCRIPT_DIR/../../..")"

ASM_OUT_DIR="$PROJECT_ROOT/src/test/asm/outputs"


"$ASM_OUT_DIR/output"
echo $?

