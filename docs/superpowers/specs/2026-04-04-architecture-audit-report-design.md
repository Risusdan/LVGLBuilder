# Architecture Audit Report Design

**Date:** 2026-04-04
**Status:** Draft
**Goal:** Produce a standalone HTML report analyzing LVGLBuilder's architectural problems, to convince management that refactoring is necessary.

## Context

LVGLBuilder has accumulated significant architectural debt. Before executing the refactoring strategy (see `2026-04-04-refactoring-strategy-design.md`), we need a clear, evidence-based report that communicates the problems and their business impact to a manager who is proficient in C/C++ but unfamiliar with Qt or LVGL.

## Output

- **File:** `docs/architecture-audit-report.html`
- **Format:** Self-contained static HTML with inline CSS. No external dependencies, no JavaScript libraries. Opens directly in any browser. Printable.
- **Language:** Chinese (Traditional)

## Report Structure

### 1. Title

Report title only. No date or author line.

### 2. Executive Summary

One paragraph summarizing: the project's scale, the number of architectural problems found, the overall risk level, and the conclusion that refactoring is necessary.

### 3. Project Overview

Key metrics about the codebase:
- 130 source files (excluding vendored dependencies)
- ~14,000 lines of application code
- 0 tests
- Top file sizes (LVGLCore.cpp: 818 lines, LVGLObject.cpp: 766 lines, MainWindow.cpp: 508 lines, LVGLSimulator.cpp: 443 lines)

Includes a **horizontal bar chart** (pure CSS) showing the line counts of the largest files, to visually highlight the bloated core files.

### 4. Architecture Problems (7 items)

Each problem follows this template:

```
Problem Title + Severity Badge (高/中/低)
├── Problem Description (2-3 sentences, using C/C++ universal concepts)
│   - Qt/LVGL-specific terms get parenthetical C/C++ analogies
├── Code Evidence (1-2 concise snippets with file name and line numbers)
├── Impact (concrete effects on development speed, bug risk, onboarding)
└── Suggested Solutions (1-2 specific approaches: what to do + expected outcome)
```

Severity color coding: Red = 高, Yellow = 中, Green = 低.

#### Problem List

| # | Severity | Problem | Core Concept (C/C++ terms) |
|---|----------|---------|---------------------------|
| 1 | 高 | God Class (LVGLCore) | Single Responsibility Principle violation — one class with 8 responsibilities (818 lines) |
| 2 | 高 | Global Singleton Coupling | Global variable `lvgl` accessed by 24+ files — equivalent to a massive `extern` global struct |
| 3 | 高 | Zero Test Coverage, Untestable | No tests exist, and tight coupling makes unit testing impossible without full system init |
| 4 | 中 | UI and Business Logic Mixed | LVGLSimulator and MainWindow interleave rendering/event handling with data manipulation in single functions |
| 5 | 中 | Manual Memory Management Risk | Raw `new/delete` chains, multi-level pointer ownership without RAII or smart pointers |
| 6 | 中 | Duplicated Widget Boilerplate | 20+ widget files repeat identical property definition patterns — change one, must change all |
| 7 | 低 | Circular Dependencies | LVGLObject ↔ LVGLCore mutual dependency, unclear object lifetime ownership |

#### Diagrams

- **Problem 1 (God Class) — Suggested Solution:** Before/after decomposition diagram. Left: one large box with 8 responsibilities listed. Right: multiple focused boxes (LVGLDisplay, LVGLImageManager, LVGLFontManager, LVGLWidgetRegistry, LVGLObjectManager) with LVGLCore as thin facade. Pure CSS box layout.

- **Problem 2 (Global Singleton):** Dependency spider-web diagram. Center node = `lvgl` global, surrounding nodes = the 24+ files that depend on it, with arrows pointing inward. Pure CSS/HTML layout.

### 5. Refactoring Benefits

A summary section mapping each problem to its expected improvement after refactoring:
- Testability: from 0% to measurable coverage
- Modifiability: changes localized to single module instead of rippling across 24+ files
- Onboarding: new developers can understand one focused class instead of an 818-line god class
- Bug risk: RAII and clear ownership eliminate memory leak categories
- Code reuse: shared property definitions instead of copy-paste across 20+ files

### 6. Risks of Not Refactoring

What happens as the project grows (OSD Tool feature integration) without addressing these problems:
- God class grows further as features are added
- Global coupling makes every change a potential regression
- No tests means regressions are caught only by manual testing
- Memory issues compound with more widget types
- Onboarding time increases, making it harder to scale the team

## Visual Design

- Clean white background, suitable for screen and print
- Sans-serif body font, monospace for code
- Severity badges: inline colored labels (red/yellow/green with text)
- Code blocks: light gray background, rounded corners, file path + line number header
- Charts: pure CSS (flexbox/grid), no canvas or SVG library needed
- Diagrams: CSS box model with borders and connecting lines/arrows
- Responsive: readable on both wide screens and printed A4

## Translation Strategy for Manager

The target reader is proficient in C/C++ but does not know Qt or LVGL. Every Qt/LVGL concept must be grounded in C/C++ equivalents:

| Qt/LVGL Term | Report Explanation |
|---|---|
| `QGraphicsView` | A rendering viewport class (similar to a display buffer manager) |
| `lv_obj_t` | LVGL's base widget struct (like a UI element handle) |
| Singleton pattern | A global variable with methods — essentially `extern MyClass instance` |
| Signal/slot | Callback mechanism (like function pointers registered for events) |
| `QSettings` | Persistent config storage (like writing to an INI file) |
| RAII | Resource Acquisition Is Initialization — the manager likely knows this already |

Avoid LVGL API details in explanatory text. API names appear only in code snippets where they serve as evidence.
