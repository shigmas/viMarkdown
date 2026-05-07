# SVG-2

```SVG
<svg width="300" height="120" xmlns="http://www.w3.org/2000/svg">

  <!-- グラデーション定義 -->
  <defs>
    <linearGradient id="grad1" x1="0%" y1="0%" x2="0%" y2="100%">
      <stop offset="0%" stop-color="#00f2fe" />
      <stop offset="100%" stop-color="#4facfe" />
    </linearGradient>
  <radialGradient id="rad1">
    <stop offset="0%" stop-color="yellow"/>
    <stop offset="100%" stop-color="green"/>
  </radialGradient>
  <!-- 矢印ヘッド定義 -->
    <marker id="arrow" markerWidth="10" markerHeight="10"
            refX="8" refY="3" orient="auto" markerUnits="strokeWidth">
      <path d="M0,0 L0,6 L9,3 z" fill="red"/>
    </marker>
  </defs>

  <!-- 矩形 -->
  <rect x="20" y="20" width="260" height="80" rx="12"
        fill="url(#grad1)" stroke="#333" stroke-width="2"/>
  <rect width="100" height="50" rx="10" fill="url(#rad1)"/>
  <!-- 矢印線 -->
  <line x1="200" y1="10" x2="10"  y2="80"
        stroke="red" stroke-width="4" marker-end="url(#arrow)"/>
</svg>
```