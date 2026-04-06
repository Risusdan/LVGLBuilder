# How to Write Tests

This project uses **QTest** (Qt's built-in test framework). Tests live in
`tests/`, one subdirectory per test module.

## Running Existing Tests

```bash
cmake -B build && cmake --build build

# All tests
ctest --test-dir build

# Verbose (see pass/fail per test method)
ctest --test-dir build --verbose

# One specific test
ctest --test-dir build -R tst_lvglcore

# List all tests
ctest --test-dir build -N
```

## Anatomy of a Test File

Here is a minimal working test (`tests/tst_example/tst_example.cpp`):

```cpp
#include <QtTest>
#include "LVGLCore.h"

class TestExample : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {       // runs ONCE before all tests
    Q_INIT_RESOURCE(res);     // load Qt resources (icons, etc.)
    lvgl.init(320, 480);      // initialize LVGL singleton
  }

  void cleanup() {            // runs after EACH test method
    lvgl.removeAllObjects();  // prevent state leaking between tests
  }

  void testSomethingWorks() {
    // Arrange
    const LVGLWidget *btn = lvgl.widget("lv_btn");
    QVERIFY(btn != nullptr);

    // Act
    LVGLObject *obj = new LVGLObject(btn, "my_btn", lv_scr_act());
    lvgl.addObject(obj);

    // Assert
    QCOMPARE(lvgl.allObjects().size(), 1);
    QCOMPARE(obj->name(), QString("my_btn"));
  }
};

QTEST_MAIN(TestExample)
#include "tst_example.moc"
```

### Key Points

- **`initTestCase()`** runs once. Always call `Q_INIT_RESOURCE(res)` and
  `lvgl.init(width, height)` here — most classes depend on the LVGL
  singleton.

- **`cleanup()`** runs after every test method. Call
  `lvgl.removeAllObjects()` to isolate tests from each other.

- **`#include "tst_example.moc"`** at the bottom must match your `.cpp`
  filename. Required by Qt's meta-object compiler.

- **`QTEST_MAIN(TestExample)`** generates `main()`. Each test file is a
  standalone executable.

## Adding a New Test

### 1. Create the test file

```bash
mkdir tests/tst_myfeature
```

Then create `tests/tst_myfeature/tst_myfeature.cpp` with the pattern above.

### 2. Register it in CMake

Add one line to `tests/CMakeLists.txt`:

```cmake
add_lvgl_test(tst_myfeature tst_myfeature/tst_myfeature.cpp)
```

The `add_lvgl_test` helper creates the executable, links it against
`lvglbuilder_lib` (all app code minus `main.cpp`) and `Qt6::Test`, and
registers it with CTest.

### 3. Build and run

```bash
cmake --build build
ctest --test-dir build -R tst_myfeature --verbose
```

## QTest Assertion Reference

| Macro | What it checks |
|-------|---------------|
| `QVERIFY(cond)` | `cond` is true |
| `QCOMPARE(actual, expected)` | `actual == expected` (prints both values on failure) |
| `QVERIFY_EXCEPTION_THROWN(expr, ExType)` | `expr` throws `ExType` |
| `QSKIP("reason")` | Skip this test (shows as "skipped", not "failed") |

Prefer `QCOMPARE` over `QVERIFY(a == b)` — it prints both values on
failure.

## Tips

- **One test method, one concern.** Small focused tests are easier to debug.
- **Test names describe the expectation.** `testCodeNameReplacesSpaces` is
  better than `testCodeName3`.
- **Data-driven tests.** QTest supports table-driven tests — add a method
  named `testFoo_data()` alongside `testFoo()` to run the same logic with
  multiple inputs. See the
  [QTest docs](https://doc.qt.io/qt-6/qtest-overview.html).

---

## Appendix: Test Strategy

### Test Suite Priority Order

The suites are ordered by the severity of bugs they catch. Higher-priority
suites protect against data corruption and incorrect code generation — bugs
that silently produce wrong output. Lower-priority suites cover crashes and
edge cases that are serious but more visible when they happen.

| # | Suite | What It Catches | Why This Priority |
|---|-------|----------------|-------------------|
| 1 | `tst_lvglobject` | Serialization copy-paste bugs, codeName sanitization, child lifecycle | LVGLObject is the central data model — a serialization bug silently corrupts every saved project |
| 2 | `tst_codeexport` | C code generation correctness, style code bugs | Exported C code runs on embedded hardware — a bug here means the device behaves differently from the builder preview |
| 3 | `tst_widgets` | Wrong enum casts, defaultStyle gaps, smoke test all widget types | 28 widget types with repetitive code — copy-paste errors are common and affect style rendering |
| 4 | `tst_lvglitem` | Resize edge cases, small widget handling | Resize bugs are user-visible but don't corrupt data |
| 5 | `tst_assetmanagers` | Dangling default pointers, removal lifecycle | Asset cleanup bugs cause crashes, but only on specific user actions (removing assets) |
| 6 | `tst_properties_ext` | Val2 format bug, range validation, color round-trip, null safety | Property edge cases are narrow — they affect specific property types, not the whole system |

### Sources of Truth

When writing a test, you need to know what "correct" means. This project
has no external spec document, so we derive correctness from these sources,
listed from most authoritative to least:

**1. LVGL 6.1 API contracts** — The vendored headers in `lvgl/` are the
ground truth for field names, enum values, and function signatures. If the
builder uses `lv_btn_set_state()` with the wrong enum type, that's a bug
regardless of whether it happens to work at runtime. Example: when testing
that `setStyle()` casts to the right enum, look at the LVGL header for that
widget to find the correct `lv_xxx_style_t` enum.

**2. C language rules** — The builder exports C code that must compile on
embedded toolchains. A valid C identifier matches `[a-zA-Z_][a-zA-Z0-9_]*`.
If `codeName()` produces `"my widget"` (with a space), the exported code
won't compile. This is an objective, language-level rule — no design
decision needed. Example: test that `codeName()` sanitizes special
characters, not just spaces.

**3. Round-trip invariants** — These are mathematical properties that must
hold regardless of any spec: if you save a project and load it back, the
result should be identical. No information should be lost or transformed.
This applies at every level:
- `save() -> load() -> save()` produces identical JSON
- `property.set(x) -> property.get()` returns `x`
- `toJson() -> fromJson() -> toJson()` is stable

Example: create a widget, set properties, serialize to JSON, deserialize,
re-serialize — the two JSON outputs should match byte-for-byte.

**4. The code's own intent** — When the code names something `shadow.width`
but reads from `border.width`, the name itself tells you it's wrong. No
spec needed — the developer who wrote it clearly intended to read the shadow
width. Similarly, a format string `"%3, %3"` where two different getters
exist is self-evidently a copy-paste bug (should be `"%3, %4"`). Example:
when testing serialization, verify that each field name in the JSON matches
the getter being called.

**5. Defensive invariants** — Universal correctness rules that apply to any
program:
- No crash on null input
- No use-after-free (deleting an object cleans up all references to it)
- No assertion-only validation (`Q_ASSERT` disappears in Release builds,
  so code that relies on it for safety is broken in production)

Example: test that deleting a widget also removes it from its parent's
child list, and that the parent doesn't hold a dangling pointer.

