# 6. The Core GUI Drawing Loop: Mouse Events and Painting

Source: [DrawingCanvas.cpp](../src/gui/DrawingCanvas.cpp). Qt owns the main loop (`QApplication::exec()`); the canvas
reacts to events. There is no `while(true)` in the application code: the "loop" is
**event -> change the model -> `update()` -> Qt schedules `paintEvent` -> repaint**.

## 6.1 The loop at a glance

```plantuml
@startuml
start
repeat
  :OS / Qt event queue;
  split
    :mousePressEvent;
  split again
    :mouseMoveEvent;
  split again
    :mouseReleaseEvent;
  split again
    :paintEvent;
  end split
  :modify Document / interaction state;
  :update()  (request a repaint, coalesced by Qt);
repeat while (application running?)
stop
@enduml
```

Rules to remember:

1. Event handlers **never draw**; they change data and call `update()`.
2. Only `paintEvent` draws, always from the current state of `Document` + interaction members.
3. Qt merges several `update()` calls into a single `paintEvent`.

## 6.2 Coordinates

Every handler first converts the position: `const core::Point docPt = WidgetToDoc(event->position());`

```
docX = (widgetX - m_margin) / m_zoom        (m_margin = 40, m_zoom = 0.1 .. 5.0)
```
Tolerances in pixels are divided by `m_zoom` (e.g. `5.0 / m_zoom`) so that the clickable area is constant on screen.

## 6.3 Interaction state machine

`m_toolMode` (what the toolbox chose) and `m_dragMode` (what the mouse is doing right now) together form the state.

```plantuml
@startuml
[*] --> Idle
state Idle {
  [*] --> None
  None : m_dragMode = None
}
Idle --> Resize : press on a tracker handle\n(Select tool)
Idle --> Move : press on a shape\n(Select tool)
Idle --> SelectBox : press on empty space\n(Select tool)
Idle --> Create : press (CreateShape tool)

Move --> Move : mouseMove / MoveBy(delta) all selected\nUpdateConnectedLines
Resize --> Resize : mouseMove / Tracker::ResizeShape
SelectBox --> SelectBox : mouseMove / update m_selectBox
Create --> Create : mouseMove / size from drag rectangle\nor line end + snapping

Move --> Idle : release
Resize --> Idle : release
SelectBox --> Idle : release / select intersecting shapes
Create --> Idle : release / connect line end if snapped
@enduml
```

`DragMode::ConnectLine` is declared but not used.

## 6.4 Mouse press

```plantuml
@startuml
start
:if not left button or no document -> return;
:docPt = WidgetToDoc(pos)\nm_dragStartDocPt = m_lastDocPt = docPt;
if (m_toolMode == Select?) then (yes)
  :primary = GetPrimarySelectedShape();
  if (primary) then (yes)
    :handle = Tracker::HitTestHandle(primary, docPt, size/zoom);
  endif
  if (handle is a real handle (not None, not Body)?) then (yes)
    :m_dragMode = Resize\nm_activeHandle = handle\nm_activeShape = primary\nm_originalBounds = bbox;
  else (no)
    :hit = Document::FindShapeAt(docPt, 5/zoom);
    if (hit?) then (yes)
      if (not selected OR Ctrl) then (yes)
        :SelectShape(id, clearOthers = !Ctrl);
      endif
      :m_dragMode = Move; m_activeShape = hit;
      :emit selectionChanged;
    else (no)
      if (no Ctrl) then (yes)
        :ClearSelection();
      endif
      :m_dragMode = SelectBox\nm_selectBox = Rect(docPt, 0, 0);
      :emit selectionChanged;
    endif
  endif
else (CreateShape)
  :newShape = build by ShapeType (10x10 at docPt)\nor ShapeFactory::Create(m_createShapeClass);
  if (Line?) then (yes)
    :caps from tool; FindSnapAnchor for the start;\nSetStartConnectedShapeId;
  endif
  :apply current style (colors, stroke, font, text);
  :Document::AddShape; SelectShape;
  :m_activeShape = newShape; m_dragMode = Create;
  :emit selectionChanged;
endif
:update();
stop
@enduml
```

Key points:

- A selected shape is tested for **handle hits first**, otherwise the tracker handles (which lie on or outside the border) could never be grabbed.
- Pressing an **already selected** shape without Ctrl does not change the selection, so a multi-selection can be moved as a group.
- New shapes start at 10 x 10 and are resized by the following moves; this gives the "drag to size" feel.
- Circles are created with center `(x+5, y+5)` and radius 5.

## 6.5 Mouse move

```plantuml
@startuml
start
:docPt = WidgetToDoc(pos);
:emit mouseMoved(docPt)  -> status bar;
switch (m_dragMode)
case (SelectBox)
  :m_selectBox = normalized rect(start, docPt);\nupdate();
case (Move)
  :delta = docPt - m_lastDocPt;
  :for every selected shape: MoveBy(delta);
  :Document::UpdateConnectedLines();
  :SetModified(true); m_lastDocPt = docPt;
  :update(); emit documentModified;
case (Resize)
  :Tracker::ResizeShape(shape, handle, docPt, m_originalBounds);
  :UpdateConnectedLines(); SetModified; update(); emit documentModified;
case (Create)
  if (line?) then (yes)
    :m_currentSnapResult = FindSnapAnchor(docPt, shapes, line id, 16/zoom);
    :SetEndPoint(snapped ? anchorPosition : docPt);
  else (no)
    :w = |docPt.x - start.x|, h = |docPt.y - start.y|;
    :position = (min x, min y); SetWidth(w); SetHeight(h);
  endif
  :SetModified; update(); emit documentModified;
case (None)
  :Select tool: pick cursor from the handle under the mouse\n(resize arrows, move cross, arrow);
endswitch
stop
@enduml
```

Notes:

- `setMouseTracking(true)` in the constructor makes `mouseMoveEvent` fire even with no button pressed (needed for hover cursors and the status bar).
- **Move is incremental** (`docPt - m_lastDocPt`, then `m_lastDocPt = docPt`) while **Resize is absolute** (computed from `m_originalBounds` captured at press time).
- Because `Create` takes `abs()` of the size and `min()` of the corner, the user can drag in any direction.

## 6.5.1 Cursor feedback table

| Handle under the cursor | Cursor |
|-------------------------|--------|
| TopLeft, BottomRight | `Qt::SizeFDiagCursor` |
| TopRight, BottomLeft | `Qt::SizeBDiagCursor` |
| TopCenter, BottomCenter | `Qt::SizeVerCursor` |
| MiddleLeft, MiddleRight | `Qt::SizeHorCursor` |
| Body | `Qt::SizeAllCursor` |
| none | `Qt::ArrowCursor` |

## 6.6 Mouse release

```plantuml
@startuml
start
if (not left button?) then (yes)
  stop
endif
if (m_dragMode == SelectBox) then (yes)
  :for each shape: if bbox.Intersects(m_selectBox)\n  SelectShape(id, clearOthers=false);
  :emit selectionChanged;
endif
if (m_dragMode == Create and shape is Line and snapped) then (yes)
  :line->SetEndConnectedShapeId(target, anchorIndex);
endif
:m_currentSnapResult.snapped = false;\nm_dragMode = None;\nm_activeHandle = None;
:update();
stop
@enduml
```

The tool mode is **not** reset: after creating a shape, the same tool stays active so several shapes can be drawn in a row.
Clicking **Select** (button, toolbar or Edit menu) returns to selection mode.

## 6.7 Complete interaction sequence: draw then move a shape

```plantuml
@startuml
actor User
participant "Qt event loop" as Q
participant DrawingCanvas as DC
participant Document as D
participant Tracker as T

== Create (tool = Square) ==
User -> Q : press at (100,100)
Q -> DC : mousePressEvent
DC -> D : AddShape(new SquareShape 10x10)
DC -> D : SelectShape(id)
DC -> Q : update()
Q -> DC : paintEvent
User -> Q : move to (180,160)
Q -> DC : mouseMoveEvent
DC -> DC : SetPosition(100,100) SetWidth(80) SetHeight(60)
DC -> Q : update()
User -> Q : release
Q -> DC : mouseReleaseEvent
DC -> DC : m_dragMode = None

== Move (tool = Select) ==
User -> Q : press inside the square
Q -> DC : mousePressEvent
DC -> T : HitTestHandle(primary, pt)  -> Body
DC -> D : FindShapeAt(pt)
DC -> DC : m_dragMode = Move
User -> Q : move +10,+5
Q -> DC : mouseMoveEvent
DC -> D : (selected shapes) MoveBy(delta); UpdateConnectedLines()
DC -> Q : update() ; emit documentModified
User -> Q : release
DC -> DC : m_dragMode = None
@enduml
```

## 6.8 The paint pass

```cpp
void DrawingCanvas::paintEvent(QPaintEvent* event) {
    QPainter painter(this);                                 // RAII: ends painting in the destructor
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), palette().window());           // workspace background
    drawPaper(painter);                                     // shadow + white sheet (widget coords)

    // visible region in document units -> culling
    const QRect clipRect = event->rect();
    const core::Point tl = WidgetToDoc(clipRect.topLeft());
    const core::Point br = WidgetToDoc(clipRect.bottomRight());
    const core::Rect visibleDocRect(tl.x, tl.y, br.x - tl.x, br.y - tl.y);

    painter.save();
    painter.translate(m_margin, m_margin);                  // from here on: document coordinates
    painter.scale(m_zoom, m_zoom);
    painter.setClipRect(QRectF(0, 0, paper.width, paper.height));

    if (m_showGrid) drawGrid(painter);

    QtDrawingContext ctx(&painter);
    for (const auto& shape : m_document->GetVisibleShapes(visibleDocRect))
        shape->Draw(ctx);                                   // polymorphic call, z-order = vector order

    for (const auto& shape : visibleShapes)
        if (shape->IsSelected()) { drawTracker(painter, *shape); drawConnectionPoints(painter, *shape); }

    if (m_dragMode == DragMode::SelectBox) { /* dashed translucent rectangle */ }
    if (m_currentSnapResult.snapped)       drawSnapIndicator(painter, m_currentSnapResult.anchorPosition);
    painter.restore();
}
```

Layer order (bottom to top): workspace, shadow, paper, grid, shapes, trackers + anchors, rubber band, snap indicator.
Pen widths of overlays use `1.0 / m_zoom` to stay one pixel wide after the `scale`.

## 6.9 Other events

| Event | Behavior |
|-------|----------|
| `mouseDoubleClickEvent` | `FindShapeAt`; opens `QInputDialog::getText`; sets the shape text |
| `wheelEvent` | Ctrl + wheel: `SetZoom(m_zoom * 1.15^steps)`; otherwise default (scroll area scrolls) |
| `contextMenuEvent` | selects the shape under the cursor if it is not selected; menu Cut/Copy/Paste/Delete enabled by selection/clipboard |

### Clipboard sequence

```plantuml
@startuml
participant DrawingCanvas as DC
participant "g_clipboard" as CB
participant Document as D
== Copy ==
DC -> D : GetSelectedShapes()
loop each
  DC -> CB : push_back(shared_ptr(shape->Clone()))
end
DC -> DC : m_pasteCount = 0
== Paste ==
DC -> DC : ++m_pasteCount ; offset = 20 * m_pasteCount
DC -> D : ClearSelection()
loop each clipboard shape
  DC -> DC : copy = src->Clone(); copy->SetId(0); copy->MoveBy(offset, offset)
  DC -> DC : if LineShape: drop start/end connections
  DC -> D : AddShape(copy)  (new id)
  DC -> D : SelectShape(id, false)
end
@enduml
```

## 6.10 Snapping algorithm (`Tracker::FindSnapAnchor`)

For the cursor point, iterate over all visible shapes except the one being drawn, over each one's connection points,
and keep the closest whose distance is below `snapRadius` (`16 / zoom`). Complexity $O(n \cdot a)$ with $a$ anchors per
shape. When `snapped`, the line end is placed exactly on the anchor, an indicator ring is painted, and on release the
`(shapeId, anchorIndex)` pair is stored so the line follows the shape later.

## 6.11 Debugging tips for the interaction code

- Put a breakpoint in `mousePressEvent` and inspect `m_dragMode` / `m_toolMode`.
- If a shape cannot be grabbed, check its `HitTest` and `GetBoundingBox` and the tolerance `5.0 / m_zoom`.
- If nothing repaints, check that `update()` is called (handlers never paint directly).
- If handles look huge/small when zooming, a size was not divided by `m_zoom`.
