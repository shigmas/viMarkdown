```SVG
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 740 460" width="100%" height="100%" style="background-color: transparent;">
  <defs>
    <!-- ドロップシャドウ効果 -->
    <filter id="shadow" x="-5%" y="-5%" width="110%" height="110%">
      <feDropShadow dx="1" dy="2" stdDeviation="3" flood-opacity="0.08"/>
    </filter>
    
    <!-- 矢印マーカーの定義 -->
    <marker id="arrow-slate" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 1.5 L 8 5 L 0 8.5 z" fill="#94a3b8" />
    </marker>
    <marker id="arrow-indigo" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 1.5 L 8 5 L 0 8.5 z" fill="#6366f1" />
    </marker>
  </defs>

  <!-- メインコンテナ -->
  <rect width="740" height="460" rx="12" fill="#ffffff" stroke="#e2e8f0" stroke-width="1.5" filter="url(#shadow)"/>

  <!-- タイトルセクション -->
  <text x="370" y="32" font-family="MS Gothic" font-weight="bold" fill="#0f172a" font-size="16" text-anchor="middle">vi/ex 自動テストの仕組みとAI協調ループ</text>
  <text x="370" y="52" font-family="MS Gothic" fill="#64748b" font-size="11" text-anchor="middle">viMarkdownにおける 「1文字のズレも許さない」 テスト駆動開発の実態</text>

  <!-- ================= BLOCK 1: テスト記述の工夫 (x: 25, y: 80) ================= -->
  <g transform="translate(25, 80)" font-family="MS Gothic">
    <rect width="210" height="320" rx="8" fill="#f8fafc" stroke="#cbd5e1" stroke-width="1.5"/>
    <text x="105" y="25" text-anchor="middle" font-weight="bold" font-size="13" fill="#1e293b">1. テスト表現の工夫</text>
    
    <!-- カーソルの表現 -->
    <rect x="15" y="45" width="180" height="45" rx="4" fill="#ffffff" stroke="#e2e8f0"/>
    <text x="25" y="62" font-size="11" fill="#0f172a" font-weight="bold">カーソル位置の可視化</text>
    <text x="25" y="79" font-size="12" fill="#64748b">"h<tspan fill="#e11d48" font-weight="bold">┃</tspan>ello\n" <tspan font-size="10">(┃で表現)</tspan></text>

    <!-- 選択範囲の表現 -->
    <rect x="15" y="100" width="180" height="45" rx="4" fill="#ffffff" stroke="#e2e8f0"/>
    <text x="25" y="117" font-size="11" fill="#0f172a" font-weight="bold">選択範囲（ビジュアル）</text>
    <text x="25" y="134" font-size="12" fill="#64748b">"<tspan fill="#2563eb" font-weight="bold">《</tspan>h<tspan fill="#e11d48" font-weight="bold">┃</tspan>e<tspan fill="#2563eb" font-weight="bold">》</tspan>llo\n" <tspan font-size="9">(《 》で表現)</tspan></text>

    <!-- コード例 -->
    <rect x="15" y="155" width="180" height="150" rx="4" fill="#0f172a" stroke="#1e293b"/>
    <text x="25" y="175" font-size="9" fill="#38bdf8" font-weight="bold">// C++ テストケース例</text>
    <text x="25" y="195" font-size="9" fill="#94a3b8">{ "Move cursor left",</text>
    <text x="25" y="210" font-size="9" fill="#e2e8f0">  "h<tspan fill="#f43f5e">┃</tspan>ello\n",</text>
    <text x="25" y="225" font-size="9" fill="#94a3b8">  {</text>
    <text x="25" y="240" font-size="9" fill="#34d399">    "h", "<tspan fill="#f43f5e">┃</tspan>hello\n"</text>
    <text x="25" y="255" font-size="9" fill="#94a3b8">  }</text>
    <text x="25" y="270" font-size="9" fill="#94a3b8">},</text>
    <text x="25" y="290" font-size="9" fill="#64748b" font-style="italic">※1行ごとに状態を比較</text>
  </g>

  <!-- コネクタ 1 -> 2 -->
  <path d="M 235,240 L 259,240" stroke="#94a3b8" stroke-width="2.5" fill="none" marker-end="url(#arrow-slate)"/>

  <!-- ================= BLOCK 2: 組み合わせと実行 (x: 265, y: 80) ================= -->
  <g transform="translate(265, 80)" font-family="MS Gothic">
    <rect width="210" height="320" rx="8" fill="#f8fafc" stroke="#cbd5e1" stroke-width="1.5"/>
    <text x="105" y="25" text-anchor="middle" font-weight="bold" font-size="13" fill="#1e293b">2. テスト実行と検証</text>

    <!-- 組み合わせ爆発バッジ -->
    <rect x="15" y="45" width="180" height="55" rx="6" fill="#eff6ff" stroke="#bfdbfe" stroke-width="1"/>
    <text x="105" y="62" font-size="10" fill="#2563eb" font-weight="bold" text-anchor="middle">組み合わせ爆発への対応</text>
    <text x="105" y="78" font-size="11" fill="#1e3a8a" font-weight="bold" text-anchor="middle">Operator × Motion × Count</text>
    <text x="105" y="92" font-size="8" fill="#1e40af" text-anchor="middle">手動検証不可能な膨大なパターン</text>

    <!-- 実行の流れ -->
    <g transform="translate(15, 115)">
      <rect width="180" height="190" rx="4" fill="#ffffff" stroke="#e2e8f0"/>
      <text x="90" y="20" font-size="11" font-weight="bold" fill="#334155" text-anchor="middle">自動テストランナー</text>
      
      <!-- Before -->
      <rect x="10" y="32" width="160" height="24" rx="3" fill="#f1f5f9" stroke="#cbd5e1" stroke-width="0.5"/>
      <text x="18" y="48" font-size="10" fill="#475569">Before   : h<tspan fill="#f43f5e" font-weight="bold">┃</tspan>ello\n</text>

      <!-- Commands -->
      <text x="90" y="76" font-size="10" font-weight="bold" fill="#4f46e5" text-anchor="middle">⬇️ Command: 'h' 実行</text>

      <!-- Expected -->
      <rect x="10" y="94" width="160" height="24" rx="3" fill="#f0fdf4" stroke="#bbf7d0" stroke-width="0.5"/>
      <text x="18" y="110" font-size="10" fill="#166534">Expected : <tspan fill="#f43f5e" font-weight="bold">┃</tspan>hello\n</text>

      <!-- vs -->
      <text x="90" y="132" font-size="9" fill="#94a3b8" text-anchor="middle">VS (不一致でアサート)</text>

      <!-- Actual -->
      <rect x="10" y="142" width="160" height="24" rx="3" fill="#fff1f2" stroke="#fecaca" stroke-width="0.5"/>
      <text x="18" y="158" font-size="10" fill="#991b1b">Actual   : h<tspan fill="#f43f5e" font-weight="bold">┃</tspan>ello\n</text>
      <text x="18" y="180" font-size="8" fill="#ef4444" font-weight="bold">※ 1文字のズレを即時検知！</text>
    </g>
  </g>

  <!-- コネクタ 2 -> 3 -->
  <path d="M 475,240 L 499,240" stroke="#94a3b8" stroke-width="2.5" fill="none" marker-end="url(#arrow-slate)"/>

  <!-- ================= BLOCK 3: 判定とAIループ (x: 505, y: 80) ================= -->
  <g transform="translate(505, 80)" font-family="MS Gothic">
    <rect width="210" height="320" rx="8" fill="#f8fafc" stroke="#cbd5e1" stroke-width="1.5"/>
    <text x="105" y="25" text-anchor="middle" font-weight="bold" font-size="13" fill="#1e293b">3. 判定 ＆ AI協調解決</text>

    <!-- 合格判定 -->
    <rect x="15" y="45" width="180" height="35" rx="4" fill="#f0fdf4" stroke="#bbf7d0" stroke-width="1"/>
    <text x="105" y="66" font-size="10" font-weight="bold" fill="#166534" text-anchor="middle">🟢 合格 (Vimおじさんも安心)</text>

    <!-- 不合格判定 -->
    <rect x="15" y="90" width="180" height="35" rx="4" fill="#fef2f2" stroke="#fecaca" stroke-width="1"/>
    <text x="105" y="111" font-size="10" font-weight="bold" fill="#991b1b" text-anchor="middle">🔴 不合格 (AI協調レビューへ)</text>

    <!-- AI協調レビューパネル -->
    <rect x="15" y="135" width="180" height="170" rx="6" fill="#ffffff" stroke="#e2e8f0" stroke-width="1.5"/>
    <text x="90" y="155" font-size="11" font-weight="bold" fill="#4f46e5" text-anchor="middle">🤖 AI協調レビュー体制</text>
    
    <!-- Gemini -->
    <rect x="25" y="165" width="160" height="38" rx="4" fill="#f8fafc" stroke="#e2e8f0"/>
    <text x="32" y="179" font-size="9" font-weight="bold" fill="#0ea5e9">Gemini先生</text>
    <text x="32" y="193" font-size="9" fill="#475569">・テストケースの一次生成</text>

    <!-- チャッピー & Claude -->
    <rect x="25" y="210" width="160" height="38" rx="4" fill="#f8fafc" stroke="#e2e8f0"/>
    <text x="32" y="224" font-size="9" font-weight="bold" fill="#10b981">チャッピー ＆ Claude先生</text>
    <text x="32" y="238" font-size="9" fill="#475569">・テストの評価・修正・レビュー</text>

    <!-- 診断 -->
    <rect x="25" y="255" width="160" height="42" rx="4" fill="#fef3c7" stroke="#fde68a"/>
    <text x="32" y="269" font-size="9" font-weight="bold" fill="#b45309">失敗時のトリアージ</text>
    <text x="32" y="281" font-size="8" fill="#78350f">C++の実装バグか、テストデータの</text>
    <text x="32" y="291" font-size="8" fill="#78350f">誤りかをAIが特定して修正案提示</text>
  </g>

  <!-- ================= FEEDBACK LOOP (C -> A) ================= -->
  <!-- 画面下部を通る破線のループアロー -->
  <path d="M 610,400 L 610,430 L 130,430 L 130,400" stroke="#6366f1" stroke-width="2.5" stroke-dasharray="4 4" fill="none" marker-end="url(#arrow-indigo)" />
  
  <!-- ループ上の説明バッジ -->
  <rect x="245" y="418" width="250" height="24" rx="12" fill="#ffffff" stroke="#6366f1" stroke-width="1.5" />
  <text x="370" y="434" font-family="MS Gothic" font-size="10" fill="#4f46e5" font-weight="bold" text-anchor="middle">AIの修正提案 ➔ コード/テスト修正 ➔ 再テスト</text>
</svg>
```
