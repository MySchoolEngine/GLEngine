---
name: verify-build
description: Use when about to claim a fix compiles, a bug is resolved, or a change is complete - requires actually building before asserting success
---

# Verify Build

## Overview

A change that looks correct can silently break a different compilation unit. Do not claim a fix is done until you have read zero-error build output.

## Process

1. **Identify all affected projects** — headers are included by many `.cpp` files across different projects. A change to `SSEUtils.h` can break `PhysicsRTTR.cpp`, `BVH.cpp`, and benchmarks simultaneously.
2. **Build all affected projects**, not just the one you were editing.
3. **Read the full error output** — errors in unexpected compilation units mean the file is included there too.
4. Only claim the fix is complete after confirming zero errors.

## Build Commands (GLEngine)

See memory `reference_build.md` for full commands.

```bash
# Generate solution
premake5 vs2022

# Build a specific project (Debug)
MSBuild GLEngine.sln /t:<ProjectName> /p:Configuration=Debug /p:Platform=x64

# Build everything
MSBuild GLEngine.sln /p:Configuration=Debug /p:Platform=x64
```

## Common Mistake

Editing a header and only mentally verifying the one `.cpp` you were looking at. The compiler sees every translation unit that includes the header — they all must compile.

**Red flag:** "The change looks correct" — looking is not building.
