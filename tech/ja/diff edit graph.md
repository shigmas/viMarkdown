# diff edit graph

```SVG
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 660 480" width="100%" height="100%" style="background-color: transparent;">
  <defs>
    <!-- ドロップシャドウ効果 -->
    <filter id="shadow" x="-5%" y="-5%" width="110%" height="110%">
      <feDropShadow dx="1" dy="2" stdDeviation="3" flood-opacity="0.08"/>
    </filter>
    
    <!-- パス矢印マーカー定義 -->
    <marker id="arrow-indigo" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 1.5 L 8 5 L 0 8.5 z" fill="#6366f1" />
    </marker>
    <marker id="arrow-blue" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 1.5 L 8 5 L 0 8.5 z" fill="#2563eb" />
    </marker>
    <marker id="arrow-rose" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 1.5 L 8 5 L 0 8.5 z" fill="#f43f5e" />
    </marker>
  </defs>

  <!-- メインコンテナ -->
  <rect width="660" height="480" rx="12" fill="#ffffff" stroke="#e2e8f0" stroke-width="1.5" filter="url(#shadow)"/>

  <!-- タイトル・軸ラベル -->
  <text x="235" y="32" font-family="MS Gothic" font-weight="bold" fill="#64748b" text-anchor="middle" font-size="13">X = "abcde" (Columns)</text>
  <text x="25" y="235" font-family="MS Gothic" font-weight="bold" fill="#64748b" text-anchor="middle" font-size="13" transform="rotate(-90, 25, 235)">Y = "abdce" (Rows)</text>

  <!-- ================= GRID AREA (x:70~400, y:70~400) ================= -->
  <g font-family="MS Gothic">
    <!-- 列ラベル (X座標) -->
    <text x="97.5" y="60" text-anchor="middle" font-weight="bold" fill="#475569" font-size="14">∅</text>
    <text x="152.5" y="60" text-anchor="middle" font-weight="bold" fill="#0f172a" font-size="14">a</text>
    <text x="207.5" y="60" text-anchor="middle" font-weight="bold" fill="#0f172a" font-size="14">b</text>
    <text x="262.5" y="60" text-anchor="middle" font-weight="bold" fill="#0f172a" font-size="14">c</text>
    <text x="317.5" y="60" text-anchor="middle" font-weight="bold" fill="#0f172a" font-size="14">d</text>
    <text x="372.5" y="60" text-anchor="middle" font-weight="bold" fill="#0f172a" font-size="14">e</text>

    <!-- 行ラベル (Y座標) -->
    <text x="50" y="102.5" text-anchor="middle" font-weight="bold" fill="#475569" font-size="14">∅</text>
    <text x="50" y="157.5" text-anchor="middle" font-weight="bold" fill="#0f172a" font-size="14">a</text>
    <text x="50" y="212.5" text-anchor="middle" font-weight="bold" fill="#0f172a" font-size="14">b</text>
    <text x="50" y="267.5" text-anchor="middle" font-weight="bold" fill="#0f172a" font-size="14">d</text>
    <text x="50" y="322.5" text-anchor="middle" font-weight="bold" fill="#0f172a" font-size="14">c</text>
    <text x="50" y="377.5" text-anchor="middle" font-weight="bold" fill="#0f172a" font-size="14">e</text>

    <!-- DP セル定義 (背景色・境界線) -->
    <!-- 行0 -->
    <rect x="70" y="70" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="125" y="70" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="180" y="70" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="235" y="70" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="290" y="70" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="345" y="70" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />

    <!-- 行1 -->
    <rect x="70" y="125" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="125" y="125" width="55" height="55" fill="#f0fdf4" stroke="#86efac" stroke-width="1.5" /> <!-- Match: a-a -->
    <rect x="180" y="125" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="235" y="125" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="290" y="125" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="345" y="125" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />

    <!-- 行2 -->
    <rect x="70" y="180" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="125" y="180" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="180" y="180" width="55" height="55" fill="#f0fdf4" stroke="#86efac" stroke-width="1.5" /> <!-- Match: b-b -->
    <rect x="235" y="180" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="290" y="180" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="345" y="180" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />

    <!-- 行3 -->
    <rect x="70" y="235" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="125" y="235" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="180" y="235" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="235" y="235" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="290" y="235" width="55" height="55" fill="#f0fdf4" stroke="#86efac" stroke-width="1.5" /> <!-- Match: d-d -->
    <rect x="345" y="235" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />

    <!-- 行4 -->
    <rect x="70" y="290" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="125" y="290" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="180" y="290" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="235" y="290" width="55" height="55" fill="#f0fdf4" stroke="#86efac" stroke-width="1.5" /> <!-- Match: c-c -->
    <rect x="290" y="290" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="345" y="290" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />

    <!-- 行5 -->
    <rect x="70" y="345" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="125" y="345" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="180" y="345" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="235" y="345" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="290" y="345" width="55" height="55" fill="#ffffff" stroke="#e2e8f0" stroke-width="1" />
    <rect x="345" y="345" width="55" height="55" fill="#f0fdf4" stroke="#86efac" stroke-width="1.5" /> <!-- Match: e-e -->
  </g>

  <!-- ================= PATH LINES (計算パス) ================= -->
  <!-- 共通開始パス: (0,0) -> (1,1) -> (2,2) -->
  <path d="M 97.5,97.5 L 145,145" stroke="#6366f1" stroke-width="3" fill="none" marker-end="url(#arrow-indigo)" />
  <path d="M 152.5,152.5 L 200,200" stroke="#6366f1" stroke-width="3" fill="none" marker-end="url(#arrow-indigo)" />

  <!-- パス 1 (上回り/Branch A): (2,2) -> (2,3) -> (3,4) -> (4,4) -->
  <path d="M 207.5,207.5 L 255,207.5" stroke="#2563eb" stroke-width="3" stroke-dasharray="1 0" fill="none" marker-end="url(#arrow-blue)" />
  <path d="M 262.5,207.5 L 310,255" stroke="#2563eb" stroke-width="3" stroke-dasharray="1 0" fill="none" marker-end="url(#arrow-blue)" />
  <path d="M 317.5,262.5 L 317.5,310" stroke="#2563eb" stroke-width="3" stroke-dasharray="1 0" fill="none" marker-end="url(#arrow-blue)" />

  <!-- パス 2 (下回り/Branch B): (2,2) -> (3,2) -> (4,3) -> (4,4) -->
  <path d="M 207.5,207.5 L 207.5,255" stroke="#f43f5e" stroke-width="3" stroke-dasharray="1 0" fill="none" marker-end="url(#arrow-rose)" />
  <path d="M 207.5,262.5 L 255,310" stroke="#f43f5e" stroke-width="3" stroke-dasharray="1 0" fill="none" marker-end="url(#arrow-rose)" />
  <path d="M 262.5,317.5 L 310,317.5" stroke="#f43f5e" stroke-width="3" stroke-dasharray="1 0" fill="none" marker-end="url(#arrow-rose)" />

  <!-- 共通終了パス: (4,4) -> (5,5) -->
  <path d="M 317.5,317.5 L 365,365" stroke="#6366f1" stroke-width="3" fill="none" marker-end="url(#arrow-indigo)" />


  <!-- ================= DP VALUES (セル値) ================= -->
  <!-- パス上の値を際立たせるため、白のアウトラインを伴う太文字で配置 -->
  <g font-family="MS Gothic" font-weight="bold" font-size="15" text-anchor="middle" fill="#0f172a" stroke="#ffffff" stroke-width="3.5" paint-order="stroke fill">
    <!-- 行 0 -->
    <text x="97.5" y="103">0</text> <text x="152.5" y="103" fill="#64748b" font-weight="normal">1</text> <text x="207.5" y="103" fill="#64748b" font-weight="normal">2</text> <text x="262.5" y="103" fill="#64748b" font-weight="normal">3</text> <text x="317.5" y="103" fill="#64748b" font-weight="normal">4</text> <text x="372.5" y="103" fill="#64748b" font-weight="normal">5</text>
    <!-- 行 1 -->
    <text x="97.5" y="158" fill="#64748b" font-weight="normal">1</text> <text x="152.5" y="158">0</text> <text x="207.5" y="158" fill="#64748b" font-weight="normal">1</text> <text x="262.5" y="158" fill="#64748b" font-weight="normal">2</text> <text x="317.5" y="158" fill="#64748b" font-weight="normal">3</text> <text x="372.5" y="158" fill="#64748b" font-weight="normal">4</text>
    <!-- 行 2 -->
    <text x="97.5" y="213" fill="#64748b" font-weight="normal">2</text> <text x="152.5" y="213" fill="#64748b" font-weight="normal">1</text> <text x="207.5" y="213">0</text> <text x="262.5" y="213">1</text> <text x="317.5" y="213" fill="#64748b" font-weight="normal">2</text> <text x="372.5" y="213" fill="#64748b" font-weight="normal">3</text>
    <!-- 行 3 -->
    <text x="97.5" y="268" fill="#64748b" font-weight="normal">3</text> <text x="152.5" y="268" fill="#64748b" font-weight="normal">2</text> <text x="207.5" y="268">1</text> <text x="262.5" y="268" fill="#64748b" font-weight="normal">2</text> <text x="317.5" y="268">1</text> <text x="372.5" y="268" fill="#64748b" font-weight="normal">2</text>
    <!-- 行 4 -->
    <text x="97.5" y="323" fill="#64748b" font-weight="normal">4</text> <text x="152.5" y="323" fill="#64748b" font-weight="normal">3</text> <text x="207.5" y="323" fill="#64748b" font-weight="normal">2</text> <text x="262.5" y="323">1</text> <text x="317.5" y="323">2</text> <text x="372.5" y="323" fill="#64748b" font-weight="normal">3</text>
    <!-- 行 5 -->
    <text x="97.5" y="378" fill="#64748b" font-weight="normal">5</text> <text x="152.5" y="378" fill="#64748b" font-weight="normal">4</text> <text x="207.5" y="378" fill="#64748b" font-weight="normal">3</text> <text x="262.5" y="378" fill="#64748b" font-weight="normal">2</text> <text x="317.5" y="378" fill="#64748b" font-weight="normal">3</text> <text x="372.5" y="378">2</text>
  </g>


  <!-- ================= RIGHT LEGEND AREA (x:425~645) ================= -->
  <!-- セクションタイトル -->
  <text x="425" y="35" font-family="MS Gothic" font-weight="bold" fill="#0f172a" font-size="15">LCS / SED 可視化</text>
  <text x="425" y="54" font-family="MS Gothic" fill="#64748b" font-size="11">対象: "abcde" vs "abdce"</text>
  <line x1="425" y1="65" x2="635" y2="65" stroke="#f1f5f9" stroke-width="1.5" />

  <!-- ステータスバッジ群 -->
  <g transform="translate(425, 78)" font-family="MS Gothic">
    <!-- SED バッジ -->
    <rect x="0" y="0" width="100" height="42" rx="6" fill="#eff6ff" stroke="#bfdbfe" stroke-width="1"/>
    <text x="50" y="16" font-size="9" fill="#2563eb" font-weight="bold" text-anchor="middle">SED (最短編集距離)</text>
    <text x="50" y="35" font-size="17" fill="#1e3a8a" font-weight="bold" text-anchor="middle">2</text>
    
    <!-- LCS バッジ -->
    <rect x="110" y="0" width="100" height="42" rx="6" fill="#f0fdf4" stroke="#bbf7d0" stroke-width="1"/>
    <text x="160" y="16" font-size="9" fill="#16a34a" font-weight="bold" text-anchor="middle">LCS (最長共通部分列)</text>
    <text x="160" y="35" font-size="17" fill="#14532d" font-weight="bold" text-anchor="middle">4</text>
  </g>

  <!--凡例項目 -->
  <g transform="translate(425, 138)" font-family="MS Gothic" font-size="11" fill="#334155">
    <!-- 共通パス -->
    <line x1="0" y1="10" x2="25" y2="10" stroke="#6366f1" stroke-width="3.5" />
    <text x="35" y="14" font-weight="bold" fill="#4f46e5">共通経路 (a, b / e)</text>
    
    <!-- パス1 -->
    <line x1="0" y1="30" x2="25" y2="30" stroke="#2563eb" stroke-width="3" stroke-dasharray="1 0" />
    <text x="35" y="34">経路 1 (LCS: "abde")</text>
    
    <!-- パス2 -->
    <line x1="0" y1="50" x2="25" y2="50" stroke="#f43f5e" stroke-width="3" stroke-dasharray="1 0" />
    <text x="35" y="54">経路 2 (LCS: "abce")</text>

    <!-- セル凡例 -->
    <rect x="0" y="70" width="16" height="16" rx="3" fill="#f0fdf4" stroke="#86efac" stroke-width="1.5" />
    <text x="25" y="82">一致セル (追加コスト 0)</text>
  </g>

  <!-- 具体的なアライメントの文字列表現 -->
  <g transform="translate(425, 245)" font-family="MS Gothic">
    <rect width="210" height="210" rx="8" fill="#f8fafc" stroke="#e2e8f0" stroke-width="1"/>
    <text x="12" y="20" font-weight="bold" fill="#334155" font-size="11">得られる最適なアライメント</text>
    
    <!-- アライメント 1 (経路 1) -->
    <rect x="12" y="32" width="186" height="74" rx="4" fill="#ffffff" stroke="#e0e7ff" stroke-width="1"/>
    <text x="22" y="48" font-size="11" font-weight="bold" fill="#2563eb">アライメント 1 (LCS: abde)</text>
    <text x="22" y="68" font-family="MS Gothic" font-size="13" fill="#0f172a" letter-spacing="3">X: a b c d <tspan fill="#f43f5e" font-weight="bold">-</tspan> e</text>
    <text x="22" y="85" font-family="MS Gothic" font-size="13" fill="#0f172a" letter-spacing="3">Y: a b <tspan fill="#f43f5e" font-weight="bold">-</tspan> d c e</text>
    <rect x="131" y="55" width="10" height="32" fill="#f43f5e" fill-opacity="0.1" rx="2"/>
    <rect x="85" y="55" width="10" height="32" fill="#f43f5e" fill-opacity="0.1" rx="2"/>

    <!-- アライメント 2 (経路 2) -->
    <rect x="12" y="118" width="186" height="74" rx="4" fill="#ffffff" stroke="#ffe4e6" stroke-width="1"/>
    <text x="22" y="134" font-size="11" font-weight="bold" fill="#f43f5e">アライメント 2 (LCS: abce)</text>
    <text x="22" y="154" font-family="MS Gothic" font-size="13" fill="#0f172a" letter-spacing="3">X: a b <tspan fill="#2563eb" font-weight="bold">-</tspan> c d e</text>
    <text x="22" y="171" font-family="MS Gothic" font-size="13" fill="#0f172a" letter-spacing="3">Y: a b d c <tspan fill="#2563eb" font-weight="bold">-</tspan> e</text>
    <rect x="85" y="141" width="10" height="32" fill="#2563eb" fill-opacity="0.1" rx="2"/>
    <rect x="131" y="141" width="10" height="32" fill="#2563eb" fill-opacity="0.1" rx="2"/>
  </g>
</svg>
```
