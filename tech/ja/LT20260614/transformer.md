# transformer

```SVG
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 800 600" width="800" height="600">
  <!-- 背景（ダークテーマ用に落ち着いたネイビーを設定） -->
  <rect width="100%" height="100%" fill="#1a1b26" rx="12"/>
  
  <!-- 定義（グラデーションと矢印マーカー） -->
  <defs>
    <!-- Encoder用の青〜紫のグラデーション -->
    <linearGradient id="encoderGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#4158D0" />
      <stop offset="100%" stop-color="#C850C0" />
    </linearGradient>
    <!-- Decoder用のオレンジ〜紫のグラデーション -->
    <linearGradient id="decoderGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#FFCC70" />
      <stop offset="100%" stop-color="#C850C0" />
    </linearGradient>
    <!-- コンポーネントボックスの背景色 -->
    <linearGradient id="boxGrad" x1="0%" y1="0%" x2="100%" y2="0%">
      <stop offset="0%" stop-color="#1f2335" />
      <stop offset="100%" stop-color="#24283b" />
    </linearGradient>
    <!-- 接続線用の矢印 -->
    <marker id="arrow-blue" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 1.5 L 10 5 L 0 8.5 z" fill="#7db9f5" />
    </marker>
    <marker id="arrow-orange" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 1.5 L 10 5 L 0 8.5 z" fill="#ff9e64" />
    </marker>
  </defs>

  <!-- タイトル -->
  <text x="400" y="45" font-family="'Segoe UI', Roboto, sans-serif" font-size="22" font-weight="bold" fill="#ffffff" text-anchor="middle">Transformer Architecture</text>
  
  <!-- ==================== ENCODER (LEFT) ==================== -->
  <!-- Encoderの外枠 -->
  <rect x="70" y="110" width="300" height="380" fill="none" stroke="#4158D0" stroke-width="2" stroke-dasharray="5,5" rx="10"/>
  <text x="85" y="135" font-family="'Segoe UI', Roboto, sans-serif" font-size="14" font-weight="bold" fill="#7db9f5">Encoder (x N)</text>

  <!-- Input Embedding -->
  <g transform="translate(110, 430)">
    <rect width="220" height="40" fill="url(#boxGrad)" stroke="#4158D0" stroke-width="1.5" rx="5"/>
    <text x="110" y="24" font-family="sans-serif" font-size="12" fill="#c0caf5" text-anchor="middle">Input Embedding</text>
  </g>
  <text x="220" y="525" font-family="sans-serif" font-size="12" fill="#a9b1d6" text-anchor="middle">Inputs</text>
  <path d="M 220, 510 L 220, 475" stroke="#7db9f5" stroke-width="2" marker-end="url(#arrow-blue)"/>

  <!-- Positional Encoding (Encoder) -->
  <g transform="translate(320, 410) scale(0.6)">
    <!-- サイン波をパスで表現 -->
    <path d="M0 20 Q 15 5, 30 20 T 60 20" fill="none" stroke="#f7768e" stroke-width="3"/>
    <text x="-45" y="22" font-family="sans-serif" font-size="15" fill="#f7768e" font-weight="bold">PE</text>
  </g>
  <path d="M 335, 430 L 290, 430" stroke="#f7768e" stroke-dasharray="3,3" stroke-width="1.5"/>

  <!-- Multi-Head Attention (Enc) -->
  <g transform="translate(110, 310)">
    <rect width="220" height="45" fill="url(#encoderGrad)" rx="5"/>
    <text x="110" y="27" font-family="sans-serif" font-size="13" font-weight="bold" fill="#ffffff" text-anchor="middle">Multi-Head Attention</text>
  </g>
  <path d="M 220, 430 L 220, 360" stroke="#7db9f5" stroke-width="2" marker-end="url(#arrow-blue)"/>

  <!-- Feed Forward (Enc) -->
  <g transform="translate(110, 190)">
    <rect width="220" height="45" fill="url(#boxGrad)" stroke="#4158D0" stroke-width="1.5" rx="5"/>
    <text x="110" y="27" font-family="sans-serif" font-size="13" fill="#c0caf5" text-anchor="middle">Feed Forward</text>
  </g>
  <path d="M 220, 310 L 220, 240" stroke="#7db9f5" stroke-width="2" marker-end="url(#arrow-blue)"/>

  <!-- ==================== DECODER (RIGHT) ==================== -->
  <!-- Decoderの外枠 -->
  <rect x="430" y="110" width="300" height="380" fill="none" stroke="#ff9e64" stroke-width="2" stroke-dasharray="5,5" rx="10"/>
  <text x="445" y="135" font-family="'Segoe UI', Roboto, sans-serif" font-size="14" font-weight="bold" fill="#ff9e64">Decoder (x N)</text>

  <!-- Output Embedding -->
  <g transform="translate(470, 430)">
    <rect width="220" height="40" fill="url(#boxGrad)" stroke="#ff9e64" stroke-width="1.5" rx="5"/>
    <text x="110" y="24" font-family="sans-serif" font-size="12" fill="#c0caf5" text-anchor="middle">Output Embedding</text>
  </g>
  <text x="580" y="525" font-family="sans-serif" font-size="12" fill="#a9b1d6" text-anchor="middle">Outputs (shifted right)</text>
  <path d="M 580, 510 L 580, 475" stroke="#ff9e64" stroke-width="2" marker-end="url(#arrow-orange)"/>

  <!-- Positional Encoding (Decoder) -->
  <g transform="translate(415, 410) scale(0.6)">
    <path d="M0 20 Q 15 5, 30 20 T 60 20" fill="none" stroke="#f7768e" stroke-width="3"/>
    <text x="75" y="22" font-family="sans-serif" font-size="15" fill="#f7768e" font-weight="bold">PE</text>
  </g>
  <path d="M 445, 430 L 490, 430" stroke="#f7768e" stroke-dasharray="3,3" stroke-width="1.5"/>

  <!-- Masked Multi-Head Attention -->
  <g transform="translate(470, 340)">
    <rect width="220" height="45" fill="url(#boxGrad)" stroke="#ff9e64" stroke-width="1.5" rx="5"/>
    <text x="110" y="27" font-family="sans-serif" font-size="13" fill="#c0caf5" text-anchor="middle">Masked Multi-Head Attn</text>
  </g>
  <path d="M 580, 430 L 580, 390" stroke="#ff9e64" stroke-width="2" marker-end="url(#arrow-orange)"/>

  <!-- Multi-Head Attention (Dec) -->
  <g transform="translate(470, 250)">
    <rect width="220" height="45" fill="url(#decoderGrad)" rx="5"/>
    <text x="110" y="27" font-family="sans-serif" font-size="13" font-weight="bold" fill="#ffffff" text-anchor="middle">Multi-Head Attention</text>
  </g>
  <path d="M 580, 340 L 580, 300" stroke="#ff9e64" stroke-width="2" marker-end="url(#arrow-orange)"/>

  <!-- Feed Forward (Dec) -->
  <g transform="translate(470, 160)">
    <rect width="220" height="45" fill="url(#boxGrad)" stroke="#ff9e64" stroke-width="1.5" rx="5"/>
    <text x="110" y="27" font-family="sans-serif" font-size="13" fill="#c0caf5" text-anchor="middle">Feed Forward</text>
  </g>
  <path d="M 580, 250 L 580, 210" stroke="#ff9e64" stroke-width="2" marker-end="url(#arrow-orange)"/>

  <!-- ==================== CROSS ATTENTION CONNECTION ==================== -->
  <!-- Encoderの出力からDecoderのMulti-Head AttentionへのKeys & Valuesのパス -->
  <path d="M 220, 190 L 220, 150 L 400, 150 L 400, 272 L 465, 272" fill="none" stroke="#bb9af7" stroke-dasharray="4,4" stroke-width="2" marker-end="url(#arrow-blue)"/>
  <text x="310" y="142" font-family="sans-serif" font-size="11" fill="#bb9af7" text-anchor="middle">Keys &amp; Values</text>

  <!-- ==================== TOP OUTPUT ==================== -->
  <!-- Linear & Softmax -->
  <g transform="translate(470, 50)">
    <rect width="220" height="40" fill="url(#boxGrad)" stroke="#f7768e" stroke-width="1.5" rx="5"/>
    <text x="110" y="24" font-family="sans-serif" font-size="12" fill="#c0caf5" text-anchor="middle">Linear &amp; Softmax</text>
  </g>
  <path d="M 580, 160 L 580, 95" stroke="#ff9e64" stroke-width="2" marker-end="url(#arrow-orange)"/>
  
  <text x="580" y="20" font-family="sans-serif" font-size="12" fill="#a9b1d6" text-anchor="middle">Output Probabilities</text>
  <path d="M 580, 50 L 580, 28" stroke="#f7768e" stroke-width="2" marker-end="url(#arrow-orange)"/>
</svg>
```
