# UFNext Application

## What UFMNext does

- Draws vector shapes (square, rounded rectangle, circle, oval, line with caps, star, block arrow, UML shapes, pictures) on a paper sheet.
- Supports select, move, resize (tracker handles), rubber-band multi-selection, cut/copy/paste/delete, z-order, zoom, grid.
- Connects lines to shape anchors (snapping); connected lines follow their shapes.
- Saves/loads a text format (`.ufm`), exports PNG, prints.
- Shape **palettes** are described in JSON files and can be extended by plugins (shared libraries).
- Three UI themes (Dark, Light, Modern Blue).

## Initial Prompt

I want the skeleton of a 2D Drawing application written in ISO C++ 17 with QT with cmake support and editing in Visual Studio 2022 SLN file (the solution). I want an MDI application that can display drawing shapes. I want the concept of Shape class that has the virtual method Draw() ans each shape has its own implementation of the Draw() Method in override. I would like to have a panel on the left of the application that display various shapes in a grid list with differnts basic shapes (scare, round, line, circle, ovale, star, etc) like in office shared tools. Each shape has a derived class of Shape class. Each shape has basic common properties (color, fill color, outline color, text, font size, font color, fill enablesd true or false, visbible true our false, an ID, a position (x,y) and a width and a length. I would like the application have a toolbar and a menu bar. It should be pretty well disgned either the UI and the global aspect of the application mainframe. I want theme support. Everything should handled by QT6. I want support for print preview, export diagram as PNG, Save As Raw Data and also the need of rednering fast, only what is visible and the paper size should be parametrized to be 1200 \* 1400 or any resolution; It also must has support for zoom in and out. The drawing surface can accept drawing shape, moving shape or resizing shapes. Ad also we woul dlike to have connection between lines shapes (arrow, dot, etc) ans other shapes via the Tracker .
I need a seperation between QT plumbing dans the ISO C++c ode that we build. You do your job and let others doing the migration. You wanted to play, it's done. don't be ashamed, you aare a warrior, Pic the American. Go ! create the files ans the solution with also tets.


## Chat

I need to be able to select a shape for the left panel and click on the right pane to insert it. Dragging the mouse with still left mouse click and moving the mouse to size the shape. If I release the left mouse button, the shape is well sized and it's ok. I can select a shape by clicking on the display view. If a shape is on the click point coordiante (x, y), I want the shape to be selected by the tracker. The tracker has point on each sie of the square and I can resize the shape by clicking on any point of the shape to resize it. Then if I click on an objet, it should be selected and then will clicking and mouving the move, i should move the shape untile I release the click and then, the shape is on a new position. I should be also able to do copy, cut , paste operations on the shapes using a popup menu when I have a slected shape using right mouse click, then the popup menu appears. I should also be able to select multiple shapes by drawing a rectangle area in the select feature. The feature need a "select" button on the toolbar. Next, all the shapes should inserted in a vector of shared\_ptr\<Shape\>. The Draw process itearate on the shapes vector and draw each one. I should be also able to load and save diagrams with all shapes from the file menu load and save as and also export the content of the drawing area as export PNG image. Go ! We are still using iso c++17 and Qt6. Put a features1.md in the docs top level folder where you describe a technical documentation as markdown file where you explain how you did it.


## Chat

In the left pane of the mainframe, there is the drawing shape widget. Inside there is the Shapes palette. I would like to add a palette for UML shapes. I would like to add also another palette provided by shapes that are pictures (10 items). I also would like a button to load palette shapes from custom plugin. So there is a need to have a plug-in palette. Each palette should be described into a json file that contains the name of the shape derived class name and I need a factory that can create a shape by it's name. Go ! support for json should be nlohmann json.
