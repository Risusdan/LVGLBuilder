# How to Build LVGLBuilder

## Prerequisites

- **Qt 6** with modules: Core, Gui, Widgets, Test
- **CMake** 3.16 or later
- A **C++17** compiler (GCC, Clang, or MSVC)

LVGL and FreeType are vendored in the repo — no extra downloads needed.

### Installing Qt 6

- **macOS:** `brew install qt@6`
- **Ubuntu/Debian:** `sudo apt install qt6-base-dev qt6-base-dev-tools`
- **Windows:** Download from https://www.qt.io/download-open-source and
  install with Qt Creator

Verify Qt is available:

```bash
qmake6 --version
```

## Build Steps

```bash
# Configure
cmake -B build

# Build
cmake --build build

# Run the application
./build/LVGLBuilder
```

If CMake can't find Qt6, set the path explicitly:

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/qt6
```

### Alternative Generators

```bash
# Ninja (faster than Make)
cmake -B build -G Ninja
cmake --build build

# Xcode (macOS)
cmake -B build -G Xcode
cmake --build build --config Release
```

## Running Tests

```bash
# All tests
ctest --test-dir build

# Verbose output (see individual pass/fail)
ctest --test-dir build --verbose

# Run one test by name
ctest --test-dir build -R tst_lvglcore

# List all available tests
ctest --test-dir build -N
```

See [03-How-to-Write-Tests.md](03-How-to-Write-Tests.md) for writing new
tests.

## IDE Setup

**Qt Creator:** Open `CMakeLists.txt` as a project. Everything is
auto-detected.

**VS Code:** Install the CMake Tools extension. Open the repo folder,
select your kit, and build via the CMake sidebar.

**CLion:** Open `CMakeLists.txt` as a project. Set `CMAKE_PREFIX_PATH` in
Settings > Build > CMake > CMake options if Qt6 is not auto-detected.

## Troubleshooting

**"Could not find a package configuration file provided by Qt6"**
Set `CMAKE_PREFIX_PATH` as shown above.

**Linker errors about missing Qt symbols**
Make sure all required Qt6 modules are installed (Core, Gui, Widgets,
Test).

**"QStandardPaths: wrong ownership" warnings (Linux CI)**
This is a Qt warning about directory permissions, not a test failure. Check
the actual exit code.
