```svg
<svg width="400" height="300" viewBox="0 0 400 300" xmlns="http://www.w3.org/2000/svg">
  <!-- 背景 -->
  <rect width="400" height="300" fill="#fcfcfc" />

  <!-- Vehicleクラス（親） -->
  <g transform="translate(125, 20)">
    <rect width="150" height="100" fill="white" stroke="#333" stroke-width="2" rx="5" />
    <!-- クラス名 -->
    <line x1="0" y1="35" x2="150" y2="35" stroke="#333" stroke-width="1" />
    <text x="75" y="18" font-family="'MS Gothic', sans-serif" font-size="16" font-weight="bold" text-anchor="middle" dominant-baseline="central">Vehicle (乗り物)</text>
    <!-- 属性・メソッド -->
    <text x="10" y="55" font-family="'MS Gothic', sans-serif" font-size="12" dominant-baseline="central">+ speed: int</text>
    <text x="10" y="80" font-family="'MS Gothic', sans-serif" font-size="12" dominant-baseline="central">+ move() : void</text>
  </g>

  <!-- 継承の矢印（白抜きの三角） -->
  <g stroke="#333" stroke-width="2" fill="none">
    <line x1="200" y1="120" x2="200" y2="180" />
    <polygon points="200,120 192,135 208,135" fill="white" />
  </g>

  <!-- Carクラス（子） -->
  <g transform="translate(125, 180)">
    <rect width="150" height="100" fill="white" stroke="#333" stroke-width="2" rx="5" />
    <!-- クラス名 -->
    <line x1="0" y1="35" x2="150" y2="35" stroke="#333" stroke-width="1" />
    <text x="75" y="18" font-family="'MS Gothic', sans-serif" font-size="16" font-weight="bold" text-anchor="middle" dominant-baseline="central">Car (自動車)</text>
    <!-- 属性・メソッド -->
    <text x="10" y="55" font-family="'MS Gothic', sans-serif" font-size="12" dominant-baseline="central">+ fuel: int</text>
    <text x="10" y="80" font-family="'MS Gothic', sans-serif" font-size="12" dominant-baseline="central">+ honk() : void</text>
  </g>
</svg>
```
