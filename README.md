# viMarkdown

-- visual Markdown editor --

[日本語版 (Japanese)](README_ja.md)

**viMarkdown** is a lightweight, high-performance Markdown editor built with **Qt6** and **C++**.  
It is designed for developers and writers who love efficiency.

<!--<img width="700" alt="image" src="./screen.png" />-->
![image](screen.png)


## Key Features

- **Perfect Sync Between Editor and Preview**
  Your cursor position and edits are synchronized in real-time. Experience intuitive editing while preserving the integrity of your Markdown structure.
- **Direct Editing in Preview**
  Beyond the editor, you can insert or delete text directly within the preview pane. Seamlessly edit your document in a true WYSIWYG fashion.
- **Outline-based Navigation**
  Effortlessly navigate and edit long documents. The outline view helps you maintain a clear grasp of your document structure at all times.
- **Unique Block Features (Keisen & CSV)**
  Supports text-based "Keisen" (ruled lines) and "CSV" blocks. Create complex diagrams and tables as easily as writing code, empowering you to structure your documents with ease.
- **Native Implementation with Qt6/C++**
  Built with Qt6/C++. Unlike Electron-based editors, viMarkdown delivers an exceptionally lightweight and high-performance experience.

## Development Phases

|ver.| Phase | Description | Schedule |
|----| :--- | :--- | :--- |
|0.0.xxx| prototype | Experimental implementation | Dec 2025 – |
|0.1.0xx| dev | Feature implementation & verification | Jan 2026 –  |
|0.1.1xx| alpha | Bug fixes and occasional feature additions | 10 Feb 2026 – |
|0.1.2xx| beta | Bug fixes only | 10 Mar 2026 –|
|0.2.xxx| rc | Bug fixes only (ensuring no side effects) | 21 Apr 2026 –  |
|0.2.xxx| Stable | Maintenance mode | End Apr 2026 – (**Current**)|
|0.3.0xx|dev|Feature implementation & verification|2026/08～|
|0.3.1xx|alpha|Bug fixes and occasional feature additions|2026/12～|

## Roadmap

| Ver. | Features / Overview | Schedule |
| :--- | :--- | :--- |
| 0.2 | Basic editor features, Basic Markdown, Ruled-line (Keisen) blocks, CSV blocks | Scheduled for release by the end of April 2026.|
| 0.4 | Vi key bindings, Japanese localization for menus and UI, regex search, grep, rectangular selection, SVG blocks?, usability and performance improvements, migration to CMake (Qt Creator / Mac / Linux? support) | Dev version starting around May 2026, Alpha version around Aug 2026, Stable release (Win/Mac) planned for Dec 2026. |
| 0.6 | Math formulas, Mind maps, Page view, Presentation mode? | TBD |
| 0.8 | Mermaid diagrams?, Direct editing in the previewer? | TBD |

\* This roadmap is current and subject to change without notice at the author's sole discretion.

## Build Instructions
### Visual Studio on Windows (Recommended)
This project is primarily developed and tested on Windows 11 using Visual Studio 2026 and Qt VS Tools.

**Prerequisites:**
- Visual Studio 2022 or later
- Qt VS Tools extension
- Qt 6 SDK (MSVC build)

**Build Steps for Visual Studio:**
1. Clone this repository.
2. Open `viMarkdown.slnx` in Visual Studio.
3. If necessary, configure the Qt VS Tools settings (Qt Versions) to match your installed Qt SDK.
4. Build the solution (`Ctrl` + `Shift` + `B`).

### Qt Creator (CMake) on macOS / Windows / Linux
Multi-platform builds using Qt Creator and CMake are also supported.
*Note: As of April 2025, builds have been verified on Windows 11 and macOS. The Linux environment is currently untested, so build errors may occur.*

**Build Steps for Qt Creator:**
1. Clone this repository.
2. Launch Qt Creator, select "Open Project", and choose `viMarkdown/CMakeLists.txt` from the repository.
3. When the Configure Project screen appears, check the Qt Kit you want to use (e.g., `Qt 6.x.x for macOS`) and click "Configure Project".
4. Build the project (Windows/Linux: `Ctrl` + `B`, Mac: `Cmd` + `B`).