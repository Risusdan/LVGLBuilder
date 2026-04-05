# Testing & Bug-Hunting Strategy

Established 2026-04-05 after a comprehensive code audit.

## Approach: Test-Driven Bug Hunting

Instead of fixing bugs from the audit list then writing tests afterward:

1. **Write a test for expected correct behavior** (based on sources of truth below)
2. **Test fails** → confirmed real bug
3. **Fix the bug** to make the test pass
4. **Test passes** → bug fixed with regression protection

Fix as we go — each test suite is a self-contained red-green cycle. We always keep a green test suite and can stop at any natural boundary.

## Sources of Truth (most authoritative first)

### Layer 1: LVGL 6.1 API Contracts
The vendored headers in `lvgl/` are the spec. Field names, enum types, and function signatures are unambiguous.
- Covers: enum cast bugs, style field copy-paste bugs, code generation correctness

### Layer 2: C Language Rules
A valid identifier is `[a-zA-Z_][a-zA-Z0-9_]*`. Generated code must compile.
- Covers: `codeName()` sanitization, header guards, variable names

### Layer 3: Round-Trip Invariants
No spec needed — mathematical property:
- `save → load → save` produces identical JSON
- `property.set(x) → property.get()` returns `x`
- `toJson() → fromJson() → toJson()` is stable

### Layer 4: The Code's Own Intent
Naming reveals spec:
- `shadow.width` reading from `border.width` is self-evidently wrong
- Format string `%3, %3` where two different getters exist is self-evidently wrong

### Layer 5: Defensive Invariants
Universal correctness:
- No crash on null input
- No use-after-free
- Deleting X cleans up all references to X
- No assertion-only validation (Q_ASSERT disappears in Release)

## Test Suite Plan (priority order)

| # | Suite | What It Catches | Source of Truth |
|---|-------|----------------|-----------------|
| 1 | `tst_lvglobject` | Serialization copy-paste bugs, codeName sanitization, child lifecycle | Layers 1, 2, 3, 4 |
| 2 | `tst_codeexport` | C code generation correctness, style code bugs | Layers 1, 2 |
| 3 | `tst_widgets` | Wrong enum casts, defaultStyle gaps, smoke test all 35+ types | Layer 1 |
| 4 | `tst_lvglitem` | Resize edge cases, small widget handling | Layer 5 |
| 5 | `tst_assetmanagers` | Dangling default pointers, removal lifecycle | Layer 5 |
| 6 | `tst_properties_ext` | Val2 format bug, range validation, color round-trip, null safety | Layers 1, 3, 5 |

## Out of Scope (needs design decisions first)

Some behaviors have no clear "correct" answer without a spec decision. These are tracked separately and will not be tested until discussed:

- Minimum widget size for resize
- Drag-drop on incompatible parents
- Atomic save / backup strategy
- Partial load failure handling
- Project name sanitization rules
- Thread safety model
- Z-order synchronization
- Others

## Key Audit Findings by Category

### Critical — Data Corruption
1. Shadow width serialized as border width (`LVGLObject.cpp:269`)
2. Line opacity serialized as image opacity (`LVGLObject.cpp:332`)
3. Wrong field comparison in codeStyle (`LVGLObject.cpp:423`)
4. LVGLPropertyVal2 outputs `%3, %3` instead of `%3, %4` (`LVGLPropertyVal2.cpp:61`)
5. 9 widgets cast style type to wrong enum (Calendar, Line, DropDownList, Chart, Canvas, ButtonMatrix, Container, Page, LED)
6. codeName() only replaces spaces — invalid C identifiers (`LVGLObject.cpp:72`)
7. Tab name not escaped in C export (`LVGLProject.cpp:277`)

### High — Crashes & Dangling Pointers
8. Constructor dereferences null parent (`LVGLObject.cpp:9`)
9. Canvas destructor dereferences ext/data without null check (`LVGLObject.cpp:46`)
10. Q_ASSERT for style type index — gone in Release (`LVGLObject.cpp:458`)
11. findChildByIndex result not null-checked (`LVGLObject.cpp:725`)
12. Delete key handler doesn't check null selection (`LVGLSimulator.cpp:360`)
13. Image manager m_default not cleared on removal (`LVGLImageManager.cpp:85-90`)
14. Font manager m_defaultFont not cleared on removal (`LVGLFontManager.cpp:61-63`)
15. SelectionManager holds raw pointers to deleted objects (`SelectionManager.cpp:10-26`)
16. Early return in Left resize breaks corner resize (`LVGLItem.cpp:188,209`)
17. Series property uses wrong index removing points (`LVGLPropertySeries.cpp:175-181`)
18. DateList crashes on remove from empty list (`LVGLPropertyDateList.cpp:80`)

### Medium — Silent Failures
19. File save truncates before writing — crash = data loss (`LVGLProject.cpp:98-142`)
20. No JSON doc validation on load (`LVGLProject.cpp:44-48`)
21. Null font data added to font list (`LVGLProject.cpp:80`)
22. Child not removed from parent list on destruction (`LVGLObject.cpp`)
23. removeChild doesn't null child's parent pointer (`LVGLObject.cpp:619`)
24. Color property missing alpha on 16-bit depth (`LVGLPropertyColor.cpp:62-74`)
25. No min<=max validation in range properties (`LVGLPropertyRange.cpp:57-63`)
26. Resize handles break on widgets <12px (`LVGLItem.cpp:39-41`)
27. Variable maximumWidth used for height (`LVGLItem.cpp:201-203`)
28. Incomplete defaultStyle in TextArea — 3/5 missing (`LVGLTextArea.cpp:91-98`)
