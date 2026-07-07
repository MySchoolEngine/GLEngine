---
name: clang-format
description: Use when editing C/C++ files, before claiming edits are complete, or when tempted to manually adjust indentation, spacing, brace placement, or include ordering
---

# clang-format

## Overview

Never manually reformat C/C++ code. Always delegate formatting to `clang-format`. Each module has its own `.clang-format` — clang-format picks up the nearest one automatically.

## Rule

After editing any C/C++ file, run:

```bash
clang-format -i <file>
```

For multiple files:

```bash
clang-format -i File1.cpp File1.h
```

## When This Applies

- After any edit to `.cpp`, `.h`, or `.inl` files
- Before stating that an edit is complete
- When you notice misaligned code in a file you just touched

## What NOT to Do

- Do not manually add/remove spaces to align assignments or declarations
- Do not manually reorder `#include`s
- Do not manually adjust brace placement or indentation
- Do not skip clang-format because "the change is small"

## Common Mistake

Running clang-format on the whole directory when you only edited one file — this risks reformatting vendor or generated code. Format only the files you touched.
