# 8. Teaching Guide: Learning ISO C++17 with UFMNext

Audience: junior developers who know basic programming. Each lesson names a technique, shows where it appears in
the code, explains why, and ends with an exercise. Follow the lessons in order; read the referenced files.

Companion: [09-cpp-cookbook.md](09-cpp-cookbook.md) (index of C++ Core Guidelines and cppreference pages).

Suggested path: lessons 1-5 (week 1: language basics through the code), 6-10 (week 2: OO and ownership),
11-16 (week 3: library and modern idioms), 17-20 (week 4: architecture and tooling).

---

## Lesson 1 - Headers, include guards, namespaces

```cpp
#pragma once
namespace ufmnext::core { struct Point { double x{0.0}; double y{0.0}; }; }
```

- `#pragma once` prevents double inclusion (the portable alternative is `#ifndef` guards).
- `namespace ufmnext::core { }` is the **C++17 nested namespace** syntax (before: `namespace ufmnext { namespace core {`).
- Declarations go in `include/`, definitions in `src/`; small functions may be `inline` in the header ([ShapeType.h](../include/ufmnext/core/ShapeType.h)).

Exercise: why are `ShapeTypeToString` and `StringToShapeType` marked `inline`? (Answer: One Definition Rule, they are defined in a header included by many .cpp files.)

## Lesson 2 - Structs, default member initializers, constructors

```cpp
struct Rect {
    double x{0.0}; double y{0.0}; double width{0.0}; double height{0.0};
    Rect() noexcept = default;
    Rect(double inX, double inY, double inW, double inH) noexcept : x(inX), y(inY), width(inW), height(inH) {}
};
```

- `{0.0}` default member initializers guarantee no uninitialized fields.
- `= default` asks the compiler to generate the default constructor.
- The member initializer list (after `:`) initializes members before the constructor body runs.
- Value types (`Point`, `Rect`, `Color`) are cheap to copy and are passed by `const&`.

Exercise: add `Rect::Area()` as a `const noexcept` member.

## Lesson 3 - `const` correctness and `noexcept`

```cpp
Point GetPosition() const noexcept { return m_position; }
virtual Rect GetBoundingBox() const;
```

- A `const` member function promises not to modify the object; it can be called on `const Shape&`.
- `noexcept` documents that a function cannot throw (trivial getters/setters).
- Parameters: small types by value, big types by `const T&`, e.g. `SetText(const std::string& txt)`.

Exercise: find three functions in `Shape` that return `const std::string&` and explain the lifetime risk.

## Lesson 4 - Enumerations (`enum class`)

```cpp
enum class LineCapStyle { None = 0, Arrow, Dot };
```

Scoped enums do not leak names or convert implicitly to `int`. See conversion functions with `switch` in
[ShapeType.h](../include/ufmnext/core/ShapeType.h).

Exercise: add `LineCapStyle::Diamond` and follow the compiler to every place to update (use warnings `/W4`).

## Lesson 5 - Operator overloading and floating-point comparison

```cpp
bool operator==(const Point& o) const noexcept { return std::abs(x - o.x) < 1e-6 && std::abs(y - o.y) < 1e-6; }
Point operator-(const Point& o) const noexcept { return Point(x - o.x, y - o.y); }
```

Never compare floating point numbers with exact `==`; use a tolerance. `docPt - m_lastDocPt` reads like math.

Exercise: implement `Point operator*(double, const Point&)` as a free function.

## Lesson 6 - Inheritance, virtual functions, abstract classes

```cpp
class Shape {
public:
    virtual ~Shape() = default;
    virtual void Draw(IDrawingContext& ctx) const = 0;     // pure virtual: Shape is abstract
    virtual ShapeType GetType() const = 0;
    virtual std::unique_ptr<Shape> Clone() const = 0;
};
class StarShape : public Shape { void Draw(IDrawingContext&) const override; ... };
```

- `= 0` makes a class abstract; it cannot be instantiated.
- **Virtual destructor** is mandatory when deleting derived objects through a base pointer (Core Guidelines C.35).
- `override` makes the compiler verify that you really override (C.128). Always write it.
- Polymorphism in action: `shape->Draw(ctx)` in `paintEvent` calls the right derived function.

Exercise: remove `override` from one `Draw` and change its signature (drop `const`): what does the compiler say, with and without `override`?

## Lesson 7 - Virtual `Clone()` (Prototype) and covariant ownership

`Clone()` returns `std::unique_ptr<Shape>`: exclusive ownership transferred to the caller. In the canvas:

```cpp
g_clipboard.push_back(std::shared_ptr<core::Shape>(sh->Clone()));
```
A `unique_ptr` converts to `shared_ptr` implicitly or explicitly. Why clone? Copying a `Shape` by value through a base
reference would **slice** it (lose derived data). Compare the hand-written `StarShape::Clone` with the macro version
`std::make_unique<ClassName>(*this)`.

Exercise: add a `std::string m_tag` to `Shape`. Which `Clone()` implementations silently lose it?

## Lesson 8 - Smart pointers and ownership

| Type | Use in the project |
|------|--------------------|
| `std::shared_ptr<Shape>` | `Document::m_shapes`, clipboard, `m_activeShape` (shared ownership) |
| `std::unique_ptr<Shape>` | return value of `Clone()` |
| raw `T*` / `T&` | non-owning observation (`QPainter*`, `Shape&` parameters) |
| Qt parent/child | widgets (`new QPushButton(..., this)`) are deleted by their parent |

```cpp
auto line = std::make_shared<core::LineShape>(docPt, docPt);   // prefer make_shared / make_unique over new
```
Guideline R.20-R.23, R.30: use smart pointers to express ownership; raw pointers/references do not own.

Exercise: why does `DrawingCanvas` keep `m_activeShape` as a `shared_ptr` and not a raw pointer?

## Lesson 9 - Downcasting with `dynamic_cast`

```cpp
auto* line = dynamic_cast<core::LineShape*>(m_activeShape.get());
if (line) { line->SetEndPoint(docPt); }
```
`dynamic_cast` to a pointer returns `nullptr` on failure (checked at run time, needs RTTI and a polymorphic type).
It is used where type-specific code is unavoidable (`Document::UpdateConnectedLines`, `Serializer`, `Tracker`).
Design discussion: a virtual function would remove casts; the cost is a wider base interface. Discuss trade-offs.

Exercise: replace the `dynamic_cast<LineShape*>` in `Tracker::GetHandles` by a virtual `Shape::GetHandles()`.

## Lesson 10 - RAII and scope-based resources

- `QPainter painter(this);` begins painting in the constructor and ends in the destructor.
- `QSignalBlocker blocker(list);` blocks signals for its scope ([ShapeToolboxWidget.cpp](../src/gui/ShapeToolboxWidget.cpp)).
- `std::ifstream in(path);` closes the file automatically.
- `painter.save()` / `restore()` is the *non*-RAII counterpart: an early `return` between them would leak state. Discuss.

Rule: R.1 "Manage resources automatically using resource handles and RAII".

## Lesson 11 - Standard containers and range-based for

```cpp
for (const auto& shape : m_shapes) { if (shape && shape->IsSelected()) selected.push_back(shape); }
std::vector<std::string> names; names.reserve(m_creators.size());
```
`std::vector` is the default container (contiguous, cache friendly). `reserve` avoids reallocations.
`const auto&` avoids copying the `shared_ptr` (and its atomic reference count). `std::map` gives sorted names.

Exercise: why does `Document::FindShapeById` use a linear search? When would you add `std::unordered_map<uint64_t, ...>`?

## Lesson 12 - Algorithms and lambdas

```cpp
auto it = std::find_if(m_shapes.begin(), m_shapes.end(), [id](const auto& s) { return s && s->GetId() == id; });
if (it != m_shapes.end()) m_shapes.erase(it);
```

- `[id]` captures by value; `[this]` captures the object pointer (Qt connect lambdas).
- `const auto&` in a lambda parameter is a **generic lambda** (C++14).
- Reverse iteration `rbegin()/rend()` finds the top-most shape in `FindShapeAt`.

Exercise: rewrite `GetSelectedShapes` with `std::copy_if` and `std::back_inserter`.

## Lesson 13 - `std::function` and the registry

```cpp
using Creator = std::function<std::shared_ptr<Shape>()>;
std::map<std::string, Creator> m_creators;
Register("StarShape", [] { return std::make_shared<StarShape>(); });
```
`std::function` stores any callable (lambda, function pointer, functor). A capture-less lambda is a tiny factory.
`emplace(...).second` tells whether the insertion happened (false on duplicate key).

Exercise: register a creator that makes a `StarShape` with 8 points under the name `"Star8Shape"`.

## Lesson 14 - Singleton the modern way

```cpp
ShapeFactory& ShapeFactory::Instance() { static ShapeFactory factory; return factory; }
```
Function-local statics are initialized once and **thread-safely** since C++11 (Meyers singleton). The constructor is private.
Discuss why global mutable singletons hurt testing (tests here share the registry).

## Lesson 15 - Move semantics

```cpp
result.items.push_back(std::move(pi));
outPalette = std::move(result);
```
`std::move` casts to an rvalue so the vector steals the buffers instead of copying. Moved-from objects are valid but
unspecified. Return local variables by value; do not write `return std::move(x);`.

Exercise: find all `std::move` calls in `src/core` and say what is moved.

## Lesson 16 - Strings, streams and parsing

[Serializer.cpp](../src/core/Serializer.cpp) uses `std::ostringstream` to format and `std::istringstream` to parse (`lineStream >> key`).
`Color::ToHex` uses `std::setw`, `std::setfill`, `std::hex`. Discuss C++17 alternatives: `std::from_chars`,
`std::string_view` for non-owning parsing, `std::optional` for "no value" results.

Exercise: write `std::optional<double> ParseDouble(std::string_view)` using `std::from_chars` (MSVC and recent GCC support doubles).

## Lesson 17 - Error handling strategies

| Boundary | Strategy |
|----------|----------|
| Palette parsing | exceptions from nlohmann caught, converted to `bool` |
| Serializer | `bool` return; tolerant parser |
| Factory | `nullptr` for unknown name; callers check |
| Qt GUI | message boxes |

Guideline E.* and I.10: decide one error strategy per layer and keep it. Never let an exception cross the Qt event handler
boundary (Qt does not support exceptions thrown from slots).

Exercise: change `PaletteLoader` to return `std::optional<Palette>` and fix callers (C++17).

## Lesson 18 - Preprocessor macros: when and why

```cpp
#define UFMNEXT_REGISTER(T) Register(#T, [] { return std::make_shared<T>(); })
...
#undef UFMNEXT_REGISTER
```
`#T` stringizes. The macro is local (defined then `#undef`'d) to limit damage. Guideline ES.30-ES.31: avoid macros;
this is one of the rare justified uses (generate repetitive class declarations and keep name and string in sync).
Exercise: rewrite `UFMNEXT_REGISTER` as a function template `template<class T> void RegisterShape(const char*)` and note what you lose (the automatic string).

## Lesson 19 - Interfaces and dependency inversion

`IDrawingContext` is an **interface class**: only pure virtuals and a virtual destructor. `core` depends on the abstraction,
`gui` provides the implementation. Test idea: write a `RecordingContext : IDrawingContext` that appends call names to a
`std::vector<std::string>` and verify what `StarShape::Draw` emits - no Qt, no window needed.

Exercise: implement an `SvgDrawingContext` that writes SVG text, then export documents to SVG.

## Lesson 20 - Build system, warnings, tests

- CMake targets, `target_link_libraries` with `PUBLIC/PRIVATE`, `target_include_directories`.
- Warnings as design feedback: `/W4` or `-Wall -Wextra -pedantic`.
- `CMAKE_CXX_EXTENSIONS OFF` keeps the code ISO C++.
- A test executable registered with `add_test`, run by `ctest`.
- Qt specifics: `Q_OBJECT`, signals/slots, Automoc. Qt idioms (raw `new` with parent) differ from the Core Guidelines, and that is fine at the framework boundary.

---

## Qt for C++ beginners (mini primer)

| Concept | Example in code |
|---------|-----------------|
| Event loop | `app.exec()` in [main.cpp](../src/gui/main.cpp) |
| Event handler override | `void DrawingCanvas::mousePressEvent(QMouseEvent*) override` |
| Signal / slot | `connect(canvas, &DrawingCanvas::selectionChanged, this, &MainWindow::updateMenusAndToolbars);` |
| Lambda slot | `connect(m_actZoomIn, &QAction::triggered, this, [this]{ ... });` |
| `emit` | `emit documentModified();` (documentation keyword, expands to nothing) |
| Parent ownership | `new QVBoxLayout(this)` is deleted with `this` |
| `tr("...")` | translatable text |
| `update()` vs `repaint()` | `update()` schedules a paint; handlers use it |

## Guided code reading exercises

1. Trace what happens from clicking "Star" in the toolbox to a star appearing (files: ShapeToolboxWidget.cpp, MainWindow.cpp, DrawingCanvas.cpp, StarShape.cpp). Draw your own sequence diagram and compare with [06-gui-event-loop.md](06-gui-event-loop.md).
2. Add a new shape `DiamondShape`: header, source, factory registration, palette entry, serializer support (`type Custom` is enough). List every file you touched.
3. Add a "Duplicate" command to the Edit menu (Ctrl+D) using `CopySelection` + `Paste`.
4. Write 5 unit tests for `Rect::Intersects` in [tests/test_main.cpp](../tests/test_main.cpp).
5. Implement undo for "move": store the delta on release, and revert with `MoveBy(-delta)`.
6. Replace the linear `FindShapeById` by an index map and measure with 10,000 shapes.

## Review checklist for code in this project

- [ ] Virtual destructor present in every polymorphic base; `override` on all overrides.
- [ ] No owning raw pointers in `core`; ownership expressed by `shared_ptr` / `unique_ptr`.
- [ ] `const` on every non-mutating member; parameters passed by `const&` for non-trivial types.
- [ ] No Qt include in `include/ufmnext/core`.
- [ ] New shape: registered in `ShapeFactory`, `Clone()` copies every field, serializer knows it.
- [ ] Event handlers change the model and call `update()`, never paint.
