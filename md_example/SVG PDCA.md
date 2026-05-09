# SVG PDCA

```SVG
<svg width="550" height="400" viewBox="0 0 550 400" xmlns="http://www.w3.org/2000/svg">
  <rect width="550" height="400" fill="#f8f9fa" />

  <defs>
    <polygon id="arrow" points="-10,-6 0,0 -10,6" fill="#666" />
  </defs>

  <!-- 中央のガイド（基準位置 250, 200） -->
  <circle cx="250" cy="200" r="100" fill="none" stroke="#ddd" stroke-dasharray="5,5" />

  <!-- PLAN (Top) -->
  <g transform="translate(250, 70)">
    <circle r="45" fill="#3498db" stroke="#2980b9" stroke-width="2" />
    <text font-family="Arial" font-size="16" font-weight="bold" fill="white" text-anchor="middle" dy=".35em">PLAN</text>
    <!-- 右側に説明 -->
    <text x="55" font-family="Arial" font-size="12" fill="#555" text-anchor="start" dy=".35em">目標の設定・計画</text>
  </g>

  <!-- DO (Right) -->
  <g transform="translate(380, 200)">
    <circle r="45" fill="#2ecc71" stroke="#27ae60" stroke-width="2" />
    <text font-family="Arial" font-size="16" font-weight="bold" fill="white" text-anchor="middle" dy=".35em">DO</text>
    <!-- 右側に説明 -->
    <text x="55" font-family="Arial" font-size="12" fill="#555" text-anchor="start" dy=".35em">計画の実行</text>
  </g>

  <!-- CHECK (Bottom) -->
  <g transform="translate(250, 330)">
    <circle r="45" fill="#f1c40f" stroke="#f39c12" stroke-width="2" />
    <text font-family="Arial" font-size="16" font-weight="bold" fill="white" text-anchor="middle" dy=".35em">CHECK</text>
    <!-- 右側に説明 -->
    <text x="55" font-family="Arial" font-size="12" fill="#555" text-anchor="start" dy=".35em">結果の確認・評価</text>
  </g>

  <!-- ACT (Left) -->
  <g transform="translate(120, 200)">
    <circle r="45" fill="#e74c3c" stroke="#c0392b" stroke-width="2" />
    <text font-family="Arial" font-size="16" font-weight="bold" fill="white" text-anchor="middle" dy=".35em">ACT</text>
    <!-- 左側に説明（text-anchor="end"） -->
    <text x="-55" font-family="Arial" font-size="12" fill="#555" text-anchor="end" dy=".35em">改善策の検討・実施</text>
  </g>

  <!-- つなぎの線と矢印（位置を微調整） -->
  <path d="M 310,85 Q 365,115 375,145" fill="none" stroke="#666" stroke-width="3" />
  <use href="#arrow" transform="translate(375, 145) rotate(70)" />

  <path d="M 380,255 Q 370,285 310,315" fill="none" stroke="#666" stroke-width="3" />
  <use href="#arrow" transform="translate(310, 315) rotate(160)" />

  <path d="M 190,315 Q 135,285 125,255" fill="none" stroke="#666" stroke-width="3" />
  <use href="#arrow" transform="translate(125, 255) rotate(250)" />

  <path d="M 120,145 Q 130,115 190,85" fill="none" stroke="#666" stroke-width="3" />
  <use href="#arrow" transform="translate(190, 85) rotate(340)" />

</svg>```
