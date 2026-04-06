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

## Appendix: Test Strategy and Audit Findings

### Test Suite Priority Order

| # | Suite | What It Catches |
|---|-------|----------------|
| 1 | `tst_lvglobject` | Serialization copy-paste bugs, codeName sanitization, child lifecycle |
| 2 | `tst_codeexport` | C code generation correctness, style code bugs |
| 3 | `tst_widgets` | Wrong enum casts, defaultStyle gaps, smoke test all widget types |
| 4 | `tst_lvglitem` | Resize edge cases, small widget handling |
| 5 | `tst_assetmanagers` | Dangling default pointers, removal lifecycle |
| 6 | `tst_properties_ext` | Val2 format bug, range validation, color round-trip, null safety |

### Sources of Truth (most authoritative first)

1. **LVGL 6.1 API contracts** — vendored headers in `lvgl/` are the spec
2. **C language rules** — generated code must be valid C identifiers
3. **Round-trip invariants** — save/load/save produces identical JSON
4. **The code's own intent** — naming reveals spec (e.g., `shadow.width` reading from `border.width` is wrong)
5. **Defensive invariants** — no crash on null, no use-after-free, cleanup on delete

