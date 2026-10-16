# 9. C++ Cookbook with Index to the C++ Core Guidelines and cppreference

Sources:

- **C++ Core Guidelines** (Bjarne Stroustrup, Herb Sutter, editors): <https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines>
- **cppreference.com**: <https://en.cppreference.com/w/cpp>
- ISO C++ FAQ: <https://isocpp.org/faq>

Section links point to the Guidelines' section anchors. For a rule, open the page and search for its ID (for example `R.20`).
Recipes show a "UFMNext example" so juniors can find the idea in real code.

## Index

| Topic | Core Guidelines section | cppreference | Recipe |
|-------|------------------------|--------------|--------|
| Philosophy | [P](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-philosophy) | - | - |
| Interfaces | [I](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-interfaces) | - | [R1](#r1-pass-parameters) |
| Functions | [F](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-functions) | [functions](https://en.cppreference.com/w/cpp/language/functions) | [R1](#r1-pass-parameters), [R2](#r2-return-values) |
| Classes | [C](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-class) | [class](https://en.cppreference.com/w/cpp/language/classes) | [R3](#r3-rule-of-zero), [R4](#r4-polymorphic-base-class) |
| Enumerations | [Enum](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-enum) | [enum](https://en.cppreference.com/w/cpp/language/enum) | [R5](#r5-scoped-enumerations) |
| Resource management | [R](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-resource) | [memory](https://en.cppreference.com/w/cpp/memory) | [R6](#r6-ownership-with-smart-pointers), [R7](#r7-raii) |
| Expressions and statements | [ES](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-expr) | [expressions](https://en.cppreference.com/w/cpp/language/expressions) | [R8](#r8-casts) |
| Performance | [Per](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-performance) | - | [R9](#r9-containers-and-reserve) |
| Concurrency | [CP](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-concurrency) | [thread](https://en.cppreference.com/w/cpp/thread) | - |
| Error handling | [E](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-errors) | [exceptions](https://en.cppreference.com/w/cpp/language/exceptions) | [R10](#r10-error-handling) |
| Constants and immutability | [Con](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-const) | [cv](https://en.cppreference.com/w/cpp/language/cv) | [R11](#r11-const-everything) |
| Templates and generic programming | [T](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-templates) | [templates](https://en.cppreference.com/w/cpp/language/templates) | [R12](#r12-lambdas-and-stdfunction) |
| C-style programming | [CPL](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-cpl) | - | [R13](#r13-macros) |
| Source files | [SF](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-source) | [include](https://en.cppreference.com/w/cpp/preprocessor/include) | [R14](#r14-headers) |
| Standard library | [SL](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-stdlib) | [library](https://en.cppreference.com/w/cpp/header) | [R9](#r9-containers-and-reserve), [R15](#r15-algorithms), [R16](#r16-strings-and-streams) |
| Naming and layout | [NL](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-naming) | - | - |

### C++17 feature index (cppreference)

| Feature | Where to read | In UFMNext |
|---------|---------------|-----------|
| Nested namespaces `a::b` | [namespace](https://en.cppreference.com/w/cpp/language/namespace) | every header |
| `inline` variables / `static inline` members | [inline](https://en.cppreference.com/w/cpp/language/inline) | `Tracker::DefaultHandleSize` |
| `std::clamp` | [clamp](https://en.cppreference.com/w/cpp/algorithm/clamp) | `LineShape::HitTest`, `SetZoom` |
| `std::optional` | [optional](https://en.cppreference.com/w/cpp/utility/optional) | exercise in lesson 17 |
| `std::string_view` | [string_view](https://en.cppreference.com/w/cpp/string/basic_string_view) | exercise in lesson 16 |
| `std::from_chars` | [from_chars](https://en.cppreference.com/w/cpp/utility/from_chars) | exercise in lesson 16 |
| Structured bindings | [structured binding](https://en.cppreference.com/w/cpp/language/structured_binding) | `for (const auto& [name, creator] : m_creators)` |
| `if` with initializer | [if](https://en.cppreference.com/w/cpp/language/if) | `if (auto it = m_creators.find(n); it != end)` |
| `if constexpr` | [if](https://en.cppreference.com/w/cpp/language/if#Constexpr_if) | templates |
| `[[nodiscard]]`, `[[maybe_unused]]`, `[[fallthrough]]` | [attributes](https://en.cppreference.com/w/cpp/language/attributes) | candidates: `Create`, `IsRegistered` |
| `std::filesystem` | [filesystem](https://en.cppreference.com/w/cpp/filesystem) | candidate to replace `QFileInfo::exists` in core |
| Class template argument deduction | [CTAD](https://en.cppreference.com/w/cpp/language/class_template_argument_deduction) | `std::lock_guard lock(m);` |
| Fold expressions | [fold](https://en.cppreference.com/w/cpp/language/fold) | variadic helpers |
| Guaranteed copy elision | [copy elision](https://en.cppreference.com/w/cpp/language/copy_elision) | returning `Palette`, `std::vector` by value |

---

## R1 Pass parameters

Guidelines: F.15, F.16, F.17, F.20 ([Functions](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-functions)).

| Parameter kind | Pass as | Example |
|----------------|---------|---------|
| cheap to copy (`int`, `double`, `bool`, enums) | by value | `SetStrokeWidth(double sw)` |
| larger, read only | `const T&` | `SetFillColor(const Color& c)`, `Draw(...)` text `const std::string&` |
| in/out | `T&` | `Palette& outPalette`, `ConnectionPoint& outPoint` |
| sink (will be stored) | by value + `std::move` | `Register(const std::string&, Creator creator)` then `std::move(creator)` |
| optional non-owning | `T*` | `QWidget* parent = nullptr` |

```cpp
bool PaletteLoader::LoadFromString(const std::string& json, Palette& outPalette);  // in: const&, out: &
```

## R2 Return values

F.20 (prefer return values to out-parameters), F.21 (return a struct/tuple for multiple outputs), F.43 (never return a pointer or reference to a local).

```cpp
struct SnapResult { bool snapped; uint64_t targetShapeId; int anchorIndex; Point anchorPosition; };
static SnapResult FindSnapAnchor(...);    // a struct instead of four out-parameters
```
Note: `GetConnectionPointByIndex(int, ConnectionPoint&)` is an out-parameter + `bool` API; with C++17 a
`std::optional<ConnectionPoint>` return would be more modern.

## R3 Rule of Zero

C.20: "If you can avoid defining default operations, do". Classes holding `std::string`, `std::vector`, `Color`... get correct copy/move/destructor generated.
`Shape` only declares `virtual ~Shape() = default;` (C.35 + C.21) because it is a polymorphic base.
`UFMNEXT_DECLARE_*_SHAPE` clones with the compiler-generated copy constructor.

```cpp
std::unique_ptr<Shape> Clone() const override { return std::make_unique<ClassName>(*this); }
```

## R4 Polymorphic base class

C.35 virtual destructor, C.121 abstract base is a pure interface, C.128 `override`, C.67 polymorphic classes should suppress copying (public copy in `Shape` is possible; provide `Clone` instead).

```cpp
class IDrawingContext { public: virtual ~IDrawingContext() = default; virtual void DrawRect(...) = 0; ... };
```
Checklist: virtual destructor, no data members for an interface, `override` in derived classes, never call virtuals from constructors.

## R5 Scoped enumerations

Enum.3: prefer `enum class`. Enum.5: don't use ALL_CAPS for enumerators.
```cpp
enum class DragMode { None, SelectBox, Move, Resize, Create, ConnectLine };
```

## R6 Ownership with smart pointers

R.3 raw pointer is non-owning, R.11 avoid `new`/`delete`, R.20 use smart pointers to represent ownership, R.21 prefer `unique_ptr` unless sharing is needed, R.22 `make_shared`, R.23 `make_unique`, R.32-R.36 passing smart pointers.

```cpp
auto s = std::make_shared<core::StarShape>(x, y, 10, 10);   // R.22
std::unique_ptr<Shape> copy = shape.Clone();                // R.21: factory-like function returns unique_ptr
void Draw(IDrawingContext& ctx);                            // R.33: a function that does not retain uses a reference
```
cppreference: [unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr), [shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr), [make_shared](https://en.cppreference.com/w/cpp/memory/shared_ptr/make_shared).

## R7 RAII

R.1. A resource (file, lock, painter state) is owned by an object whose destructor releases it.
```cpp
std::ifstream in(path);          // closes itself
QPainter painter(this);          // ends painting itself
const QSignalBlocker blocker(w); // restores signals itself
```
cppreference: [RAII](https://en.cppreference.com/w/cpp/language/raii).

## R8 Casts

ES.48 avoid casts, ES.49 if you must, use a named cast, ES.50 don't cast away `const`.

| Cast | Use |
|------|-----|
| `static_cast<int>(x)` | well-defined conversion (`static_cast<int>(paper.width * m_zoom)`) |
| `dynamic_cast<Derived*>(basePtr)` | checked downcast |
| `reinterpret_cast` | low level only (`lib->resolve(...)` to function pointer in the plugin loader) |
| `const_cast` | avoid |

cppreference: [explicit cast](https://en.cppreference.com/w/cpp/language/explicit_cast).

## R9 Containers and `reserve`

SL.con.1 prefer `std::vector`; Per.7 design to enable optimization.
```cpp
std::vector<Point> vertices; vertices.reserve(totalVertices);
```
`std::vector`: [reference](https://en.cppreference.com/w/cpp/container/vector); `std::map`: [reference](https://en.cppreference.com/w/cpp/container/map).
Rule of thumb: start with `vector`; use `map`/`unordered_map` only for lookup by key.

## R10 Error handling

E.2 throw to signal failure, E.3 exceptions for errors only, E.6 use RAII to prevent leaks, E.12 `noexcept` when you cannot/will not throw.
```cpp
try { const auto doc = nlohmann::json::parse(json); ... } catch (const nlohmann::json::exception&) { return false; }
```
Catch by `const&` (E.15). Convert exceptions at module boundaries.

## R11 `const` everything

Con.1 by default make objects immutable, Con.2 member functions `const` by default, Con.4 constants for values that never change.
```cpp
const core::Point docPt = WidgetToDoc(event->position());
virtual Rect GetBoundingBox() const;
```

## R12 Lambdas and `std::function`

F.50 use a lambda when a function won't do, F.52 capture by reference for local use, by value when it escapes.
```cpp
auto it = std::find_if(b, e, [id](const auto& s) { return s && s->GetId() == id; });
connect(btn, &QPushButton::clicked, this, [this]() { ... });  // [this]: safe while 'this' outlives the connection
```
cppreference: [lambda](https://en.cppreference.com/w/cpp/language/lambda), [std::function](https://en.cppreference.com/w/cpp/utility/functional/function).

## R13 Macros

ES.30 don't use macros for program text manipulation, ES.31 don't use macros for constants or "functions", ES.32 ALL_CAPS for all macro names.
UFMNext keeps macros local and documented (`UFMNEXT_REGISTER`, `UFMNEXT_DECLARE_UML_SHAPE`, `UFMNEXT_PLUGIN_EXPORT`).

## R14 Headers

SF.7 no `using namespace` at global scope in a header, SF.8 include guards (`#pragma once`), SF.9 avoid cyclic dependencies, SF.10 avoid dependencies on implicitly included names (include what you use), SF.11 headers self-contained.
Prefer forward declarations (`class QComboBox;` in [ShapeToolboxWidget.h](../include/ufmnext/gui/ShapeToolboxWidget.h)) to reduce compile time.

## R15 Algorithms

P.3/ES.1: prefer the standard library to hand-written loops where a named algorithm exists.

| Need | Algorithm |
|------|-----------|
| find the first match | `std::find_if` |
| remove while iterating | erase-remove idiom or `std::erase_if` (C++20) |
| copy a subset | `std::copy_if` |
| bound a value | `std::clamp` (C++17) |
| min / max | `std::min`, `std::max`, `std::minmax` |

cppreference: [algorithms](https://en.cppreference.com/w/cpp/algorithm).

## R16 Strings and streams

SL.str.1 use `std::string` to own character sequences, SL.str.2 use `std::string_view` to refer to them, SL.io.* streams.
```cpp
std::ostringstream ss; ss << "#" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << int(r);
```
cppreference: [string](https://en.cppreference.com/w/cpp/string/basic_string), [iomanip](https://en.cppreference.com/w/cpp/io/manip), [sstream](https://en.cppreference.com/w/cpp/io/basic_stringstream).

## R17 Class design checklist

1. Data members `private`/`protected` with accessors (C.9, C.131 avoid trivial getters/setters when a struct is enough: `Point`, `Rect` are public structs on purpose).
2. Initialize members in-class (`double m_width{100.0};`) (C.45, C.48).
3. Constructors `explicit` when they take one argument (C.46). *Candidate fix:* `Document(const PaperSize&)` is already `explicit`.
4. Member order: declare in the order you initialize (C.47).
5. Prefer composition to inheritance (C.129 when to use inheritance: for interface hierarchies, not code reuse).

## R18 Naming and style used by this project

| Element | Style | Example |
|---------|-------|---------|
| Types | PascalCase | `DrawingCanvas` |
| Public functions | PascalCase (Qt overrides keep camelCase) | `AddShape`, `paintEvent` |
| Private helpers | camelCase | `drawGrid` |
| Data members | `m_` prefix | `m_zoom` |
| Constants | PascalCase `static inline` | `DefaultHandleSize` |
| Macros | `UFMNEXT_` + ALL_CAPS | `UFMNEXT_REGISTER` |

Guideline NL.* leaves the exact style to the team: be consistent.

## R19 Concurrency note

The code is single-threaded (Qt GUI thread). If you load large files on a worker thread, protect `ShapeFactory::m_creators`
(registration mutates a `std::map`) with a `std::mutex` ([CP.20](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-concurrency): use RAII locks, never plain lock/unlock).
cppreference: [mutex](https://en.cppreference.com/w/cpp/thread/mutex), [lock_guard](https://en.cppreference.com/w/cpp/thread/lock_guard).

## R20 Short rule glossary used in this course

| ID | Rule (short) |
|----|--------------|
| P.1 | Express ideas directly in code |
| I.4 | Make interfaces precisely and strongly typed |
| F.16 | Pass cheap types by value, others by `const&` |
| C.20 | Rule of zero |
| C.35 | Base class destructor: public virtual or protected non-virtual |
| C.128 | Use `override` |
| R.1 | RAII |
| R.11 | Avoid calling `new` and `delete` explicitly |
| R.20 | Use `unique_ptr`/`shared_ptr` to represent ownership |
| ES.20 | Always initialize an object |
| ES.48 | Avoid casts |
| E.12 | Use `noexcept` when throwing is impossible |
| Con.1 | By default make objects immutable |
| T.1 | Use templates to raise the level of abstraction |
| SF.7 | Don't write `using namespace` at global scope in a header |
| SL.con.1 | Prefer `std::vector` |

## Further reading

- *A Tour of C++* (3rd ed.), Bjarne Stroustrup - concise overview of modern C++.
- *The C++ Programming Language* (4th ed.), Bjarne Stroustrup - reference.
- *Effective Modern C++*, Scott Meyers - items on smart pointers, lambdas, move.
- cppreference [C++17 compiler support](https://en.cppreference.com/w/cpp/17) and [language index](https://en.cppreference.com/w/cpp/language).
- Qt documentation: <https://doc.qt.io/qt-6/>; nlohmann json: <https://json.nlohmann.me>.
