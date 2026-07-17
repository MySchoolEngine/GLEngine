---
name: natvis
description: Use ONLY when the user explicitly requests a Natvis visualizer for a specific type - never generate unprompted
---

# Natvis Visualizer Generation

## Overview

Add Visual Studio debugger visualizers for C++ types to the project's existing `.natvis` files. Only generate when explicitly requested.

**Official documentation:** https://learn.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects

## File Location Convention (GLEngine)

Each module has one `.natvis` at its root. Find the right file by the type's namespace:

| Namespace | File |
|-----------|------|
| `GLEngine::Physics::*` | `Physics/Physics.natvis` |
| `Utils::*` | `Utils/utils.natvis` |
| `GLEngine::Renderer::*` | `Renderer/renderer.natvis` |
| `GLEngine::Core::*` | `Core/Core.natvis` |
| `GLEngine::Entity::*` | `Entity/Entity.natvis` |
| `GLEngine::GUI::*` | `GUI/GUI.natvis` |

Add the new `<Type>` entry inside the existing `<AutoVisualizer>` block.

## Common Patterns

### Basic display
```xml
<Type Name="GLEngine::MyNS::MyType">
  <DisplayString>{m_field1} {m_field2}</DisplayString>
  <Expand>
    <Item Name="[Field1]">m_field1</Item>
    <Item Name="[Field2]">m_field2</Item>
  </Expand>
</Type>
```

### Conditional display
```xml
<DisplayString Condition="m_valid">{m_value}</DisplayString>
<DisplayString Condition="!m_valid">invalid</DisplayString>
```

### Intrinsic helper (avoids repeating expressions)
```xml
<Intrinsic Name="IsValid" Expression="!isinf(m_Min.x)"/>
<DisplayString Condition="IsValid()">{m_Min}-{m_Max}</DisplayString>
<DisplayString Condition="!IsValid()">uninitialized</DisplayString>
```

### SSE/`__m128` field access
```xml
<DisplayString>{data.m128_f32[0]},{data.m128_f32[1]},{data.m128_f32[2]}</DisplayString>
```

### Template type (use `&lt;`/`&gt;` and `*` wildcards, `$T1`/`$T2` for params)
```xml
<Type Name="MyNS::MyTemplate&lt;*, *&gt;">
  <DisplayString>size={$T2} ptr={($T1*)m_ptr}</DisplayString>
  <Expand>
    <ArrayItems>
      <Size>$T2</Size>
      <ValuePointer>($T1*)m_ptr</ValuePointer>
    </ArrayItems>
  </Expand>
</Type>
```

### Synthetic (computed/label-only item in Expand)
```xml
<Synthetic Name="sides">
  <DisplayString Condition="twoSided">two-sided</DisplayString>
  <DisplayString Condition="!twoSided">one-sided</DisplayString>
</Synthetic>
```

### Literal braces in DisplayString — use `{{` and `}}`
```xml
<DisplayString>{{x={m_x}, y={m_y}}}</DisplayString>
<!-- renders as: {x=1.0, y=2.0} -->
```

## Steps

1. Read the type definition to understand fields and invariants.
2. Identify the correct `.natvis` file from the table above.
3. Read the existing `.natvis` to check formatting and avoid duplicates.
4. Add the new `<Type>` block inside `<AutoVisualizer>`, before `</AutoVisualizer>`.
5. For `__m128` members, use `.m128_f32[i]` to access components.
