#!/bin/bash
# show_deps.sh - Visualize C Makefile deps as tree
# Usage: ./show_deps.sh [Makefile]

MAKEFILE="${1:-Makefile}"
if [[ ! -f "$MAKEFILE" ]]; then
    echo "Error: $MAKEFILE not found" >&2
    exit 1
fi

# Extract target, sources, objects (common vars; adapt if needed)
TARGET=$(sed -n '/^[[:space:]]*[^#]*=/ { s/.*=\(.*\)/\1/; s/[[:space:]$]//g; p }' "$MAKEFILE" | grep -i '^all\|^program\|^exe\|^bin\|^app\|^elf\|^out\|^[^[:space:]]* *:$' | head -1 | tr -d ' ')
SRCS=$(sed -n 's/.*SRCS *=\(.*\)/\1/p; s/.*SOURCES *=\(.*\)/\1/p' "$MAKEFILE" | tr -d ' \\n' | tr ' ' '\n' | grep '\.c$')
OBJS=$(sed -n 's/.*OBJS *=\(.*\)/\1/p; s/.*OBJECTS *=\(.*\)/\1/p' "$MAKEFILE" | tr -d ' \\n' | tr ' ' '\n' | grep '\.o$')

echo "Project Dependency Tree (C project)"
echo "=================================="
echo "Target: $TARGET"
echo ""

# Show objects under target
echo "$TARGET"
for obj in $OBJS; do
    src="${obj%.o}.c"
    echo "├── $obj (from $src)"
    
    # Headers for this source (gcc -MM)
    if [[ -f "$src" ]]; then
        echo "│   └── Headers:"
        gcc -MM -I. "$src" 2>/dev/null | sed 's/^/│       ├── /' | sed 's/ \\$//'
    else
        echo "│   └── (source $src missing)"
    fi
done
echo ""
