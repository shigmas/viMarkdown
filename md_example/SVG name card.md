# SVG name card

```SVG
<svg width="550" height="330" viewBox="0 0 550 330" xmlns="http://www.w3.org/2000/svg">
  <!-- 背景：深宇宙の漆黒 -->
  <defs>
    <radialGradient id="space" cx="50%" cy="50%" r="70%">
      <stop offset="0%" style="stop-color:#0a0e17" />
      <stop offset="100%" style="stop-color:#000000" />
    </radialGradient>
    
    <!-- 発光フィルター -->
    <filter id="neon" x="-20%" y="-20%" width="140%" height="140%">
      <feGaussianBlur stdDeviation="2" result="blur" />
      <feMerge>
        <feMergeNode in="blur" />
        <feMergeNode in="SourceGraphic" />
      </feMerge>
    </filter>
  </defs>

  <rect width="550" height="330" fill="url(#space)" rx="10" />

  <!-- 背景：星々 -->
  <g fill="white">
    <circle cx="50" cy="40" r="1" opacity="0.8" />
    <circle cx="150" cy="280" r="1.5" opacity="0.6" />
    <circle cx="450" cy="60" r="1" opacity="0.9" />
    <circle cx="520" cy="200" r="1" opacity="0.5" />
    <circle cx="300" cy="150" r="0.8" opacity="1" />
  </g>

  <!-- HUDフレーム：サイバーな枠線 -->
  <g stroke="#00d4ff" fill="none" stroke-width="1.5" filter="url(#neon)">
    <!-- 四隅のコーナーブラケット -->
    <path d="M 20 50 L 20 20 L 50 20" />
    <path d="M 500 20 L 530 20 L 530 50" />
    <path d="M 20 280 L 20 310 L 50 310" />
    <path d="M 500 310 L 530 310 L 530 280" />
    
    <!-- 六角形のグリッドパターン（さりげなく） -->
    <path d="M 450 150 L 470 138 L 490 150 L 490 174 L 470 186 L 450 174 Z" opacity="0.3" />
    <path d="M 490 150 L 510 138 L 530 150 L 530 174 L 510 186 L 490 174 Z" opacity="0.3" />
  </g>

  <!-- 組織名：VISUAL SOFTWARE LAB -->
  <g transform="translate(40, 50)">
    <text font-family="'MS Gothic'" font-size="12" font-weight="bold" fill="#00d4ff" filter="url(#neon)" letter-spacing="2">VISUAL SOFTWARE LABORATORY</text>
    <rect x="0" y="10" width="200" height="1" fill="#00d4ff" opacity="0.5" />
  </g>

  <!-- 肩書き：自宅研究員（首席） -->
  <g transform="translate(40, 110)">
    <rect width="110" height="20" fill="#00d4ff" fill-opacity="0.2" stroke="#00d4ff" />
    <text x="5" y="10" font-family="'MS Gothic'" font-size="12" fill="#fff" dominant-baseline="central">[CHIEF RESEARCHER]</text>
    <text x="120" y="10" font-family="'MS Gothic'" font-size="16" fill="#00d4ff" dominant-baseline="central" filter="url(#neon)">自宅研究員（首席）</text>
  </g>

  <!-- お名前：vivisuke (N.Tsuda) -->
  <text x="40" y="170" font-family="'MS Gothic'" font-size="44" font-weight="bold" fill="#fff" dominant-baseline="central" filter="url(#neon)">vivisuke (N.Tsuda)</text>

  <!-- スキャンライン風の装飾 -->
  <line x1="40" y1="210" x2="510" y2="210" stroke="#00d4ff" stroke-width="4" filter="url(#neon)" />
  <line x1="40" y1="210" x2="510" y2="210" stroke="#fff" stroke-width="1" opacity="0.8" />

  <!-- 連絡先：Email -->
  <g transform="translate(40, 250)">
    <text font-family="'MS Gothic'" font-size="14" fill="#00d4ff">ACCESS: ntsuda@master.email.ne.jp</text>
    <text x="0" y="25" font-family="'MS Gothic'" font-size="10" fill="#666">SECTOR: HOME_LAB // STATUS: ONLINE</text>
  </g>

  <!-- 右下のデータバー -->
  <g transform="translate(480, 260)" stroke="#00d4ff" fill="none">
    <rect x="0" y="0" width="4" height="20" fill="#00d4ff" />
    <rect x="8" y="5" width="4" height="15" fill="#00d4ff" />
    <rect x="16" y="2" width="4" height="18" fill="#00d4ff" />
    <rect x="24" y="8" width="4" height="12" fill="#00d4ff" />
    <text x="35" y="15" font-family="'MS Gothic'" font-size="8" stroke="none" fill="#00d4ff">v0.3.007_STABLE</text>
  </g>

</svg>
```
