# Features 1 – Shape creation, selection, editing, clipboard and file I/O

Stack: ISO C++17, Qt6. Domain logic lives in `ufmnext_core` (no Qt); Qt code lives in `src/gui`.

## 1. Document model
`core::Document` owns `std::vector<std::shared_ptr<Shape>> m_shapes`. Every shape is inserted with `AddShape()`.
Rendering (`DrawingCanvas::paintEvent`) iterates the (visible) shapes of this vector and calls `Shape::Draw(IDrawingContext&)`;
`QtDrawingContext` adapts the abstract context to `QPainter`. Selection state is stored in each shape (`IsSelected`).

## 2. Creating shapes by drag
Pick a shape in the left toolbox (`ShapeToolboxWidget`, emits `toolChanged`). `DrawingCanvas::SetTool` stores it.
- *Mouse press* (`ToolMode::CreateShape`): a 10x10 shape is created at the click point, added to the document, `DragMode::Create`.
- *Mouse move*: the shape's position/size is recomputed from the press point and the cursor (lines update their end point, with snapping).
- *Mouse release*: the shape keeps the size reached.

## 3. Selection and tracker
In `ToolMode::Select` (toolbox "Select" item or the **Select** toolbar/Edit-menu button, `m_actSelectTool`), a press runs
`Document::FindShapeAt(pt)`; the hit shape is selected and `drawTracker()` paints a square with handles on each side/corner.
Pressing a handle (`Tracker::HitTestHandle`) starts `DragMode::Resize` (`Tracker::ResizeShape`).

## 4. Moving
Pressing on a shape starts `DragMode::Move`; each mouse move translates all selected shapes by the delta since the last event
(`MoveBy`), connected lines are updated; release ends the drag and the shape stays at its new position.
Pressing on an already-selected shape keeps the multi-selection so the group moves together.

## 5. Rubber-band multi-selection
Pressing on empty space in Select mode starts `DragMode::SelectBox`. A dashed rectangle (`m_selectBox`) is drawn while dragging;
on release every shape whose bounding box intersects it is selected. Ctrl keeps/extends the existing selection.

## 6. Context menu: Cut / Copy / Paste / Delete
`DrawingCanvas::contextMenuEvent` (right click) selects the shape under the cursor if needed and shows a `QMenu`.
- Copy: deep-clones selected shapes (`Shape::Clone`) into an application-wide clipboard vector of `shared_ptr` (works across MDI windows).
- Cut = Copy + `DeleteSelection`.
- Paste: clones the clipboard, assigns new ids (`SetId(0)`), offsets by 20 px per repeated paste, clears line connections to
  original shapes, adds and selects the copies.
The same operations are in the Edit menu/toolbar with standard shortcuts (`MainWindow`).

## 7. Load / Save As / Export PNG
File menu: **Load...**, **Save**, **Save As...** use `core::Serializer` (`Document::SaveAsRawData` / `LoadFromRawData`, `.ufm` text format)
with all shapes. **Export Diagram as PNG...** calls `DrawingCanvas::ExportToPng`, rendering the whole paper through `QPainter` into a `QImage`.

## 8. Files touched
`include/ufmnext/gui/DrawingCanvas.h`, `src/gui/DrawingCanvas.cpp`, `include/ufmnext/gui/MainWindow.h`, `src/gui/MainWindow.cpp`.
