# 1. Overview and Build

## 1.1 What UFMNext does

- Draws vector shapes (square, rounded rectangle, circle, oval, line with caps, star, block arrow, UML shapes, pictures) on a paper sheet.
- Supports select, move, resize (tracker handles), rubber-band multi-selection, cut/copy/paste/delete, z-order, zoom, grid.
- Connects lines to shape anchors (snapping); connected lines follow their shapes.
- Saves/loads a text format (`.ufm`), exports PNG, prints.
- Shape **palettes** are described in JSON files and can be extended by plugins (shared libraries).
- Three UI themes (Dark, Light, Modern Blue).

## 1.2 Source tree

```
include/ufmnext/
  core/   Point Rect Color ShapeType ConnectionPoint IDrawingContext
          Shape SquareShape RoundRectangleShape CircleShape OvalShape LineShape StarShape ArrowShape
          UmlShapes PictureShapes
          Tracker Document Serializer ShapeFactory Palette PluginApi
  gui/    QtDrawingContext ShapeToolboxWidget DrawingCanvas MdiChild ThemeManager MainWindow
src/
  core/   *.cpp implementing the headers above (static library ufmnext_core)
  gui/    *.cpp + main.cpp (executable UFMNext)
resources/palettes/  shapes.json  uml.json  pictures.json
tests/test_main.cpp  unit tests of the core library (no Qt)
docs/                this documentation
```

## 1.3 CMake targets

| Target | Type | Content | Dependencies |
|--------|------|---------|--------------|
| `ufmnext_core` | static library | all of `core` | nlohmann_json (private) |
| `UFMNext` | WIN32 executable | all of `gui` + `main.cpp` | `ufmnext_core`, Qt6 Core/Gui/Widgets/PrintSupport |
| `ufmnext_tests` | executable | `tests/test_main.cpp` | `ufmnext_core` |

Important points of [CMakeLists.txt](../CMakeLists.txt):

- `CMAKE_CXX_STANDARD 17`, `CMAKE_CXX_EXTENSIONS OFF` (strict ISO C++, no compiler extensions).
- Warnings: `/W4 /permissive-` (MSVC) or `-Wall -Wextra -pedantic`.
- `CMAKE_AUTOMOC ON`: Qt's meta-object compiler runs automatically on headers containing `Q_OBJECT`.
- nlohmann_json is searched with `find_package`; if missing, `FetchContent` downloads v3.11.3.
- A post-build step copies `resources/palettes` to `<exe dir>/palettes`; the macro `UFMNEXT_PALETTES_SOURCE_DIR`
  points to the source folder as a fallback (see [05-palette-system.md](05-palette-system.md)).

## 1.4 Build and run

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:/Qt/6.x/msvc2022_64"
cmake --build build --config Debug
ctest --test-dir build -C Debug
build\Debug\UFMNext.exe
```

## 1.5 Why two modules?

`core` contains the *domain* (shapes, document, serialization) and compiles without Qt, so it is testable with a
plain console executable and could be reused with another GUI. `gui` contains everything that needs Qt. The only
bridge is the abstract interface `core::IDrawingContext`, implemented by `gui::QtDrawingContext`.
See [02-architecture.md](02-architecture.md).
