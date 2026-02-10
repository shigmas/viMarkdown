# viMarkdown

-- visual Markdown editor --

[日本語版 (Japanese)](README_ja.md)

**viMarkdown** is a lightweight, high-performance Markdown editor built with **Qt6** and **C++**.  
It is designed for developers and writers who love efficiency.

<img width="500" alt="image" src="./screen.png" />


## Features

- **Fast & Native:** Built with Qt6 for lightning-fast performance compared to Electron-based editors.
- **Tree View Navigation:** Manage your files and outline structure easily.
- **Tabbed Interface:** Edit multiple files simultaneously.
- **Markdown Preview:** Live preview of your Markdown content.
- **Keisen Block:** Create class diagrams and UI mockups with ease using text-based characters.

## Development Phases

|ver.| Phase | Description | Schedule |
|----| :--- | :--- | :--- |
|0.0.xxx| prototype | Experimental implementation | Dec 2025 – |
|0.1.0xx| dev | Feature implementation & verification | Jan 2026 –  |
|0.1.1xx| alpha | Bug fixes and occasional feature additions | 10 Feb 2026 –(**Current**) |
|0.1.2xx| beta | Bug fixes only | Mar 2026 – |
|0.2.xxx| rc | Bug fixes only (ensuring no side effects) | Mid-Mar 2026 – |
|0.2.xxx| Stable | Maintenance mode | Apr 2026 – |

## Roadmap

| Ver. | Features / Overview | Schedule |
| :--- | :--- | :--- |
| 0.2 | Basic editor features, Basic Markdown, Ruled-line (Keisen) blocks, CSV blocks | Scheduled for release around April 2026 |
| 0.4 | vi commands, Japanese language support (UI/menus), Usability improvements, Performance optimizations | Dev version starts April 2026; Release date TBD |
| 0.6 | Math formulas, Mind maps, Page view, Presentation mode? | TBD |
| 0.8 | Mermaid diagrams?, Direct editing in the previewer? | TBD |

\* This roadmap is current and subject to change without notice at the author's sole discretion.

## How to build

### Windows (Recommended)
The project is currently developed and tested on **Windows 11** using **Visual Studio 2026** with **Qt VS Tools**.

**Prerequisites:**
- **Visual Studio 2026** or later
- **Qt VS Tools** extension
- **Qt 6 SDK** (MSVC build)

**Build Instructions:**
1. Clone the repository.
2. Open `viMarkdown.sln` in Visual Studio.
3. If prompted, configure the Qt Version in the Qt VS Tools settings to match your installed Qt SDK.
4. **Build Solution** (Ctrl+Shift+B).

### Linux / macOS / Qt Creator
**Help Wanted:**
Currently, build verification on Linux, macOS, or using Qt Creator (via `.pro` or `CMakeLists.txt`) is **untested**.
If you are using these environments, you may need to generate the project files manually. Contributions to add `CMake` or `qmake` support are highly welcome!
