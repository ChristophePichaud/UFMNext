# 3. Core API Reference (`include/ufmnext/core`, `src/core`)

All types are in `namespace ufmnext::core`. The library is built as `ufmnext_core` and has no Qt dependency.

## 3.1 Value types

### `Point` ([Point.h](../include/ufmnext/core/Point.h))
2D point/vector with `double x, y`. Header-only.

| Member | Description |
|--------|-------------|
| `Point()`, `Point(x, y)` | `noexcept` constructors |
| `operator+ - *`, `+=`, `-=` | vector arithmetic (`delta = docPt - m_lastDocPt`) |
| `operator==` | tolerance comparison (`1e-6`), never compare doubles with exact `==` |
| `DistanceTo(other)` | Euclidean distance (used by snapping and hit tests) |

### `Rect` ([Rect.h](../include/ufmnext/core/Rect.h))
`x, y, width, height`; width/height may be **negative** (a rectangle dragged right-to-left), hence `Normalized()`.

| Member | Description |
|--------|-------------|
| `Left/Right/Top/Bottom()` | edges, correct even for negative sizes |
| `Center()`, `TopLeft()` ... | helper points |
| `Normalized()` | rectangle with positive width/height |
| `Contains(pt)` | point-in-rectangle on the normalized rectangle |
| `Intersects(other)` | overlap test; used for viewport culling and rubber-band selection |
| `Inflated(dx, dy)` | grows the rectangle on every side (hit-test tolerance) |

### `Color` ([Color.h](../include/ufmnext/core/Color.h))
`uint8_t r, g, b, a`.

| Member | Description |
|--------|-------------|
| `FromHex("#RRGGBB" / "#AARRGGBB")` | parse; returns black for other lengths. 8-digit form is **alpha first** |
| `ToHex(includeAlpha)` | `#AARRGGBB` when `includeAlpha` is true |
| `Black()`, `White()`, `SoftBlue()`, ... | named constants as `static` functions (avoids static-init-order issues) |

### Enumerations ([ShapeType.h](../include/ufmnext/core/ShapeType.h))
- `ShapeType { None, Square, RoundRectangle, Circle, Oval, Line, Star, Arrow, Custom }`.
  `Custom` means "created by the factory from a class name" (UML, pictures, plugins).
- `LineCapStyle { None, Arrow, Dot }`, `TextAlignment { Center, Left, Right }`.
- Conversion helpers `ShapeTypeToString`, `StringToShapeType`, `LineCapStyleToString`, `StringToLineCapStyle`.
  They are `inline` so the header can be included from many translation units without ODR violations.

### `ConnectionPoint` and `AnchorLocation` ([ConnectionPoint.h](../include/ufmnext/core/ConnectionPoint.h))
An anchor on a shape: `index`, `ownerShapeId`, `position`, outward `normal`, `location` (`Top, Right, Bottom, Left, Center, Custom`).
Default shapes expose 5 anchors in this order: **0 Top, 1 Right, 2 Bottom, 3 Left, 4 Center**. Lines expose 2 (start, end).

## 3.2 `IDrawingContext` ([IDrawingContext.h](../include/ufmnext/core/IDrawingContext.h))

Abstract rendering interface (pure virtual, virtual destructor). One method per primitive:

`DrawRect`, `DrawRoundedRect`, `DrawCircle`, `DrawEllipse`, `DrawLine(start, end, color, width, startCap, endCap)`,
`DrawPolygon(points, ...)`, `DrawText(bounds, text, color, fontSize, align)`.

Every call receives the full style (stroke color/width, fill flag, fill color): the context is **stateless** from the
shape's point of view.

## 3.3 `Shape` ([Shape.h](../include/ufmnext/core/Shape.h), [Shape.cpp](../src/core/Shape.cpp))

Abstract base class of every drawable object.

### Pure virtual functions (must be overridden)

| Function | Role |
|----------|------|
| `void Draw(IDrawingContext&) const` | render the shape in document coordinates |
| `ShapeType GetType() const` | type identification |
| `std::unique_ptr<Shape> Clone() const` | polymorphic deep copy (Prototype) |

### Virtual functions with a default implementation

| Function | Default behavior |
|----------|------------------|
| `GetClassName()` | `ShapeTypeToString(GetType()) + "Shape"` -> `"StarShape"`; key used by `ShapeFactory`, palettes and files. Custom shapes override it |
| `GetBoundingBox()` | `Rect(position, width, height).Normalized()` |
| `HitTest(pt, tolerance)` | bounding box inflated by tolerance contains `pt`; `false` if invisible |
| `GetConnectionPoints()` | 5 anchors Top/Right/Bottom/Left/Center |
| `GetConnectionPointByIndex(i, out)` | bounds-checked access to an anchor |
| `FindClosestConnectionPoint(pt, maxDist)` | index of the nearest anchor, `-1` if none |
| `MoveBy(delta)` | `m_position += delta` |

### Properties (non-virtual accessors, `noexcept` when trivial)

| Group | Members |
|-------|---------|
| Identity | `m_id` (`0` = not yet in a document) |
| Geometry | `m_position`, `m_width`, `m_height` (`GetLength/SetLength` alias height) |
| Colors | `m_color`, `m_outlineColor`, `m_fillColor`, `m_fontColor` |
| Style | `m_fillEnabled`, `m_strokeWidth`, `m_visible` |
| Text | `m_text`, `m_fontSize` |
| UI state | `m_selected` |

`protected: DrawTextContent(ctx, bounds)` draws the text centered when it is non-empty (helper for derived classes).

Default values: size 100x100, outline `DarkGray`, fill `SoftBlue`, stroke 2, font size 12.

## 3.4 Concrete shapes

| Class | Type | Specific members | Notes |
|-------|------|------------------|-------|
| `SquareShape` | `Square` | - | `DrawRect` |
| `RoundRectangleShape` | `RoundRectangle` | `m_cornerRadius` (`Get/SetCornerRadius`) | `DrawRoundedRect` |
| `CircleShape` | `Circle` | - | constructor `(cx, cy, radius)` in the canvas |
| `OvalShape` | `Oval` | - | `DrawEllipse` |
| `StarShape` | `Star` | `m_points` (min 3); `ComputeStarVertices()` | alternates outer/inner radius (inner = 0.45 * outer), starts pointing up (`-pi/2`) |
| `ArrowShape` | `Arrow` | `m_direction` (`ArrowDirection { Right, Left, Up, Down }`); `ComputeArrowVertices()` | polygon |
| `LineShape` | `Line` | caps, connected ids, anchor indexes | see below |
| `UmlClassShape`, `UmlInterfaceShape`, `UmlActorShape`, `UmlUseCaseShape`, `UmlNoteShape`, `UmlPackageShape` | `Custom` | - | declared by macro `UFMNEXT_DECLARE_UML_SHAPE` |
| `HousePictureShape`, `TreePictureShape`, `SunPictureShape`, `CloudPictureShape`, `MoonPictureShape`, `HeartPictureShape`, `FlowerPictureShape`, `CarPictureShape`, `BoatPictureShape`, `MountainPictureShape` | `Custom` | - | declared by `UFMNEXT_DECLARE_PICTURE_SHAPE` |

### `LineShape` in detail

A line reuses the base fields: `m_position` is the **start**, `(m_width, m_height)` is the **vector start -> end**
(possibly negative). Hence:

- `GetStartPoint() = m_position`, `GetEndPoint() = m_position + (m_width, m_height)`.
- `SetStartPoint(pt)` keeps the end fixed (recomputes the vector); `SetEndPoint(pt)` only changes the vector.
- `GetBoundingBox()` is padded by `max(10, 2*stroke)` so that thin or perfectly horizontal lines remain selectable.
- `HitTest` computes the distance from the point to the **segment** (projection parameter `t` clamped to `[0,1]` with `std::clamp`).
- `GetConnectionPoints()` returns only start (index 0) and end (index 1).
- `SetStartConnectedShapeId(id, anchor)`, `SetEndConnectedShapeId(id, anchor)`, `DisconnectStart()`, `DisconnectEnd()`.

### The two ways of implementing `Clone()`

1. Hand-written (`StarShape::Clone`): builds a new object and copies each property. Easy to read, easy to forget a field.
2. Copy constructor via `std::make_unique<ClassName>(*this)` (UML/picture macros): always complete.
   Prefer this for new shapes (Rule of Zero, see [09-cpp-cookbook.md](09-cpp-cookbook.md)).

### Macro-declared shapes

```cpp
#define UFMNEXT_DECLARE_UML_SHAPE(ClassName)                           \
class ClassName : public UmlShapeBase {                                \
public:                                                                \
    ClassName();                                                       \
    void Draw(IDrawingContext& ctx) const override;                    \
    std::string GetClassName() const override { return #ClassName; }   \
    std::unique_ptr<Shape> Clone() const override {                    \
        return std::make_unique<ClassName>(*this);                     \
    }                                                                  \
};
```
`#ClassName` is the preprocessor **stringizing operator**: it turns the identifier into the string literal used as factory key.

## 3.5 `ShapeFactory` ([ShapeFactory.h](../include/ufmnext/core/ShapeFactory.h))

| Member | Description |
|--------|-------------|
| `using Creator = std::function<std::shared_ptr<Shape>()>` | any callable producing a shape |
| `static ShapeFactory& Instance()` | Meyers singleton; first call runs `RegisterBuiltIns()` |
| `bool Register(name, creator)` | `false` if name empty, creator empty or **already registered** (`std::map::emplace(...).second`) |
| `bool IsRegistered(name) const` | |
| `std::shared_ptr<Shape> Create(name) const` | `nullptr` for unknown names |
| `std::vector<std::string> GetRegisteredNames() const` | sorted (map order) |
| `std::map<std::string, Creator> m_creators` | the registry |

Built-in registration uses a local macro:

```cpp
#define UFMNEXT_REGISTER(T) Register(#T, [] { return std::make_shared<T>(); })
UFMNEXT_REGISTER(StarShape);
#undef UFMNEXT_REGISTER
```

## 3.6 `Document` ([Document.h](../include/ufmnext/core/Document.h), [Document.cpp](../src/core/Document.cpp))

The model. Owns `std::vector<std::shared_ptr<Shape>> m_shapes` (**vector order = z-order**, last = top).

| Function | Behavior |
|----------|----------|
| `AddShape(shape)` | null -> `0`; assigns `m_nextId++` if id is 0, otherwise bumps `m_nextId` above the given id; sets modified; calls `UpdateConnectedLines()`; returns the id |
| `RemoveShape(id)` | `std::find_if` + `erase`; disconnects lines that referenced the shape |
| `Clear()` | empties the vector, resets ids |
| `FindShapeById(id)` | linear search |
| `FindShapeAt(pt, tol)` | iterates with **reverse iterators** so the top-most shape wins |
| `GetVisibleShapes(viewport)` | visible and intersecting the viewport (culling) |
| `SelectShape(id, clearOthers=true)`, `DeselectShape`, `ClearSelection` | selection flag lives in the shapes |
| `GetSelectedShapes()`, `GetPrimarySelectedShape()` | primary = first selected in z-order |
| `UpdateConnectedLines()` | see [02-architecture.md](02-architecture.md#28-connected-lines) |
| `BringToFront`, `SendToBack`, `BringForward`, `SendBackward` | reorder the vector |
| `SaveAsRawData()`, `LoadFromRawData(str)` | forward to `Serializer` |
| `IsModified()`, `SetModified()` | dirty flag (title bar `*`) |
| `PaperSize` | struct with default 1200 x 1400, tolerance-based `operator==` |

Complexity: most searches are $O(n)$ in the number of shapes; acceptable for this application size.

## 3.7 `Tracker` ([Tracker.h](../include/ufmnext/core/Tracker.h), [Tracker.cpp](../src/core/Tracker.cpp))

Static helper (no state) managing selection handles and snapping.

| Type / function | Description |
|-----------------|-------------|
| `enum class HandleId { None, Body, TopLeft, TopCenter, TopRight, MiddleLeft, MiddleRight, BottomLeft, BottomCenter, BottomRight, LineStart, LineEnd }` | |
| `TrackerHandle { id, position, rect }` | a handle and its clickable square |
| `SnapResult { snapped, targetShapeId, anchorIndex, anchorPosition }` | result of a snap query |
| `DefaultHandleSize = 8.0` | `static inline` data member (C++17) |
| `GetHandles(shape, size)` | 8 handles around the bounding box, or 2 (`LineStart`, `LineEnd`) for lines |
| `HitTestHandle(shape, pt, size)` | a handle id, `Body` if the point is on the shape, else `None` |
| `ResizeShape(shape, handle, currentPt, originalBounds, keepAspect)` | recomputes the rectangle from the **original bounds** and the cursor; min size 10; lines move the matching end |
| `MoveShape(shape, delta)` | calls `shape.MoveBy` |
| `FindSnapAnchor(pt, shapes, ignoreId, radius=16)` | nearest anchor of any other visible shape within the radius |

Resizing from the original bounds (not incrementally) avoids rounding drift: the result only depends on the current
cursor.

## 3.8 `Serializer` ([Serializer.h](../include/ufmnext/core/Serializer.h))

`SerializeDocumentToRawData`, `DeserializeDocumentFromRawData`, `SaveToFile`, `LoadFromFile` (all `static`).
Format description: [07-file-format.md](07-file-format.md).

## 3.9 `Palette`, `PaletteItem`, `PaletteLoader`
Described in depth in [05-palette-system.md](05-palette-system.md).

## 3.10 `PluginApi.h`
Defines `UFMNEXT_PLUGIN_EXPORT`, the two symbol names and the function pointer types `PluginRegisterFn`, `PluginPaletteJsonFn`.
See [05-palette-system.md](05-palette-system.md#58-plugin-palettes).
