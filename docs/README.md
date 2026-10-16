# UFMNext Documentation

UFMNext is a 2D drawing MDI studio written in **ISO C++17** with **Qt 6**. This documentation covers the two
source trees `include/ufmnext` and `src` (the `core` and `gui` modules), the palette system, and doubles as a
teaching course for junior developers.

## Reading order

| # | Document | Audience | Content |
|---|----------|----------|---------|
| 1 | [01-overview-and-build.md](01-overview-and-build.md) | everybody | Purpose, layout, build, run, tests |
| 2 | [02-architecture.md](02-architecture.md) | developers | Design, layers, class diagrams, design patterns, state and sequence diagrams |
| 3 | [03-core-api-reference.md](03-core-api-reference.md) | developers | Every class of `ufmnext::core`: members and essential functions |
| 4 | [04-gui-api-reference.md](04-gui-api-reference.md) | developers | Every class of `ufmnext::gui`: members, slots, signals |
| 5 | [05-palette-system.md](05-palette-system.md) | developers, designers | Palette JSON format, loading, samples, plugins |
| 6 | [06-gui-event-loop.md](06-gui-event-loop.md) | developers | Mouse press / move / release, paint loop, state machine |
| 7 | [07-file-format.md](07-file-format.md) | developers | The `.ufm` raw drawing format |
| 8 | [08-cpp17-teaching-guide.md](08-cpp17-teaching-guide.md) | juniors, trainers | C++17 techniques explained using this code base, exercises |
| 9 | [09-cpp-cookbook.md](09-cpp-cookbook.md) | juniors | Cookbook with index to C++ Core Guidelines and cppreference |
| - | [features1.md](features1.md) | everybody | Feature notes: creation, selection, clipboard, file I/O |

## Rendering the diagrams

Diagrams are written in PlantUML (` ```plantuml ` blocks). Render them with the VS Code extension
*PlantUML* (jebbs.plantuml, `Alt+D` to preview) or with `plantuml.jar` (`java -jar plantuml.jar docs/*.md`).
Mermaid is not used so that the same sources work for UML class, state and sequence diagrams.

## Conventions used in the code

- Namespaces: `ufmnext::core` (no Qt) and `ufmnext::gui` (Qt).
- Public member functions use `PascalCase` (`AddShape`), data members use the `m_` prefix, Qt slots/signals use `camelCase`.
- Header files live in `include/ufmnext/<module>/`, implementation in `src/<module>/`.
- Coordinates are `double` in **document space** (paper pixels at zoom 100%).
