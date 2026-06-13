# Markdown Editor Landscape

```SVG
<svg xmlns="http://www.w3.org/2000/svg" width="900" height="550">

  <style>
    .axis {
      stroke: #333;
      stroke-width: 2;
    }
    .label {
      font-size: 16px;
      font-family: sans-serif;
    }
    .app {
      fill: #dbeafe;
      stroke: #2563eb;
      stroke-width: 2;
    }
    .vimd {
      fill: #dcfce7;
      stroke: #16a34a;
      stroke-width: 3;
    }
    .name {
      font-size: 14px;
      text-anchor: middle;
      font-family: sans-serif;
    }
    .note {
      font-size: 12px;
      font-family: sans-serif;
      fill: #555;
      text-anchor: middle;
    }
  </style>

  <!-- title -->
  <text x="450" y="40"
        font-size="26"
        font-weight="bold"
        text-anchor="middle">
    Markdown Editor Landscape (2026)
  </text>

  <!-- axes -->
  <line class="axis" x1="100" y1="450" x2="800" y2="450"/>
  <line class="axis" x1="100" y1="450" x2="100" y2="80"/>

  <!-- X-axis labels -->
  <text class="label" x="120" y="485">Simple</text>
  <text class="label" x="700" y="485">Feature Rich</text>

  <!-- Y-axis labels -->
  <text class="label"
        x="80" y="430"
        transform="rotate(-90 80 430)">
    General Users
  </text>

  <text class="label"
        x="40" y="170"
        transform="rotate(-90 80 170)">
    Developers / Power Users
  </text>

  <!-- Obsidian -->
  <circle class="app" cx="680" cy="180" r="45"/>
  <text class="name" x="680" y="185">Obsidian</text>

  <!-- VS Code -->
  <circle class="app" cx="760" cy="130" r="40"/>
  <text class="name" x="760" y="135">VS Code</text>

  <!-- Typora -->
  <circle class="app" cx="450" cy="260" r="35"/>
  <text class="name" x="450" y="265">Typora</text>

  <!-- MarkText -->
  <circle class="app" cx="350" cy="300" r="28"/>
  <text class="name" x="350" y="305">MarkText</text>

  <!-- Zettlr -->
  <circle class="app" cx="620" cy="230" r="25"/>
  <text class="name" x="620" y="235">Zettlr</text>

  <!-- Notion -->
  <circle class="app" cx="720" cy="330" r="40"/>
  <text class="name" x="720" y="335">Notion</text>

  <!-- viMarkdown -->
  <circle class="vimd" cx="520" cy="120" r="42"/>
  <text class="name" x="520" y="125">viMarkdown</text>

  <!-- annotation -->
  <text class="note" x="520" y="75">
    Vim + Preview + SVG
  </text>

</svg>
```