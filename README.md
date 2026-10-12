# UFMNext

A modern 2D Drawing MDI (Multiple Document Interface) application skeleton written in **ISO C++ 17** with **Qt 6**, **CMake** support, and **Visual Studio 2022 Solution (.sln)** editing.

---

## Key Features & Architecture

### 1. Strict Architectural Separation
- **`ufmnext::core` (ISO C++ 17 Library)**: Pure domain model, geometry primitives, shapes hierarchy, tracker manipulation, viewport culling, and serialization. **Zero GUI/Qt dependencies** — compiles independently on any ISO C++17 conforming compiler.
- **`ufmnext::gui` (Qt 6 Plumbing)**: MDI mainframe, drawing canvas, shape toolbox panel dock, themes, print preview, PNG export, and Qt drawing context implementing the abstract `IDrawingContext` interface.

### 2. Polymorphic Shape Hierarchy (`ufmnext::core::Shape`)
All shapes derive from the abstract base class `Shape` and override the virtual `Draw(IDrawingContext& ctx)` method:
- **`SquareShape`**: Rectangle and square shapes.
- **`RoundRectangleShape`**: Rounded rectangle with configurable corner radii.
- **`CircleShape`**: Perfect circle with radius and radial hit testing.
- **`OvalShape`**: Ellipse with horizontal/vertical semi-axes and elliptical hit testing.
- **`LineShape`**: Connector lines with customizable caps (None, Arrow, Dot) and automatic endpoint binding to shape anchors.
- **`StarShape`**: Multi-point star polygon (default 5-pointed).
- **`ArrowShape`**: 7-point directional block arrow.

### 3. Common Shape Properties
Every shape contains:
- Unique identifier (`id`)
- Position (`x`, `y`)
- Dimensions (`width`, `length`/`height`)
- Primary color (`color`)
- Outline stroke color (`outlineColor`)
- Fill color (`fillColor`)
- Fill enabled flag (`fillEnabled`: `true`/`false`)
- Visibility flag (`visible`: `true`/`false`)
- Text annotation (`text`)
- Font size (`fontSize`)
- Font color (`fontColor`)
- Stroke width (`strokeWidth`)

### 4. Interactive Tracker & Connector Snapping
- **Selection & Resize Handles**: 8 tracker handles (Top-Left, Top, Top-Right, Right, Bottom-Right, Bottom, Bottom-Left, Left) with aspect-ratio preservation support.
- **Moving Shapes**: Dragging shape body updates position and automatically updates any connected line endpoints.
- **Line Connections & Anchors**: Shapes expose anchor points (`Top`, `Right`, `Bottom`, `Left`, `Center`). Moving a line endpoint near an anchor snaps to it and establishes a persistent connection. Moving connected shapes automatically recalculates the line endpoints.

### 5. Office-Style Left Tool Palette
- Dockable shape panel (`ShapeToolboxWidget`) displaying shapes in a clean grid list with vector preview icons (Select, Square, Rounded Rect, Circle, Oval, Line, Star, Arrow).

### 6. MDI Mainframe & Polished UI
- Standard menu bar: **File**, **Edit**, **View**, **Format**, **Window**, **Help**.
- Comprehensive toolbars:
  - File operations (New, Open Raw Data, Save Raw Data, Export PNG, Print Preview).
  - Zoom controls (Zoom In, Zoom Out, Reset 100%, Zoom Combobox).
  - Quick shape creation buttons.
  - Property toolbar (Shape outline color, fill color, text input, fill toggle, paper size width/height).
- Multi-theme support: **Dark Theme**, **Light Theme**, and **Modern Blue Theme**.

### 7. Fast Rendering & Parametrized Paper Size
- **Viewport Culling**: During `paintEvent`, only shapes intersecting the current visible document viewport are rendered, maintaining high performance on complex diagrams.
- **Parametrized Paper Size**: Configurable document resolution (default `1200 x 1400` or custom dimensions like `1920 x 1080`).

### 8. Export, Print & Serialization
- **Print Preview**: Qt `QPrintPreviewDialog` integration for multi-page/custom paper sizes.
- **Export as PNG**: High-resolution PNG rasterization of the drawing canvas.
- **Save As Raw Data (.ufm)**: Human-readable structured raw data serialization and deserialization format storing document dimensions, shape attributes, and connector bindings.

---

## Building and Running

### Prerequisites
- CMake 3.16+
- C++17 compatible compiler (GCC 9+, Clang 10+, or MSVC 2022)
- Qt 6 (Core, Gui, Widgets, PrintSupport)

### Building with CMake (Linux / macOS / Windows)
```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run unit tests
ctest --test-dir build --output-on-failure

# Launch application
./build/UFMNext
```

### Visual Studio 2022 (Windows)
You can open and build UFMNext in Visual Studio 2022 in two ways:
1. **Solution File**: Open `UFMNext.sln` directly in Visual Studio 2022.
2. **CMake Open Folder**: Open the repository folder in Visual Studio 2022 (`File > Open > Folder...`). Visual Studio will automatically detect `CMakePresets.json` and configure the project.

---

## Running Unit Tests
A comprehensive test suite in `tests/test_main.cpp` validates:
- Shape properties and cloning
- Polymorphic `Draw()` invocation via `IDrawingContext`
- Tracker hit testing, resizing, and moving
- Line connection snapping and dynamic updates
- Fast viewport culling
- Paper size parametrization
- Raw Data serialization and deserialization round-trip
