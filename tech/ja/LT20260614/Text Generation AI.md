# Text Generation AI

```SVG
<svg xmlns="http://www.w3.org/2000/svg" width="900" height="420">
  <defs>
    <marker id="arrow" markerWidth="10" markerHeight="10"
            refX="8" refY="3" orient="auto">
      <path d="M0,0 L0,6 L9,3 z" fill="#444"/>
    </marker>
  </defs>

  <style>
    .box {
      fill: #eef5ff;
      stroke: #3366cc;
      stroke-width: 2;
      rx: 8;
    }
    .title {
      font-size: 16px;
      font-weight: bold;
      text-anchor: middle;
    }
    .year {
      font-size: 12px;
      fill: #666;
      text-anchor: middle;
    }
    .arrow {
      stroke: #444;
      stroke-width: 2;
      fill: none;
      marker-end: url(#arrow);
    }
  </style>

  <text x="450" y="35"
        font-size="24"
        font-weight="bold"
        text-anchor="middle">
    Evolution of Text Generation AI
  </text>

  <!-- RNN -->
  <rect class="box" x="40" y="150" width="130" height="70"/>
  <text class="title" x="105" y="180">RNN</text>
  <text class="year" x="105" y="205">~2014</text>

  <!-- LSTM -->
  <rect class="box" x="220" y="150" width="130" height="70"/>
  <text class="title" x="285" y="180">LSTM</text>
  <text class="year" x="285" y="205">2014-2017</text>

  <!-- Seq2Seq -->
  <rect class="box" x="400" y="150" width="130" height="70"/>
  <text class="title" x="465" y="180">Seq2Seq</text>
  <text class="year" x="465" y="205">+ Attention</text>

  <!-- Transformer -->
  <rect class="box" x="580" y="150" width="130" height="70"/>
  <text class="title" x="645" y="180">Transformer</text>
  <text class="year" x="645" y="205">2017</text>

  <!-- GPT -->
  <rect class="box" x="760" y="80" width="120" height="70"/>
  <text class="title" x="820" y="110">GPT</text>
  <text class="year" x="820" y="135">2018-</text>

  <!-- LLM -->
  <rect class="box" x="760" y="240" width="120" height="70"/>
  <text class="title" x="820" y="270">Modern LLM</text>
  <text class="year" x="820" y="295">ChatGPT / Gemini</text>

  <!-- arrows -->
  <line class="arrow" x1="170" y1="185" x2="220" y2="185"/>
  <line class="arrow" x1="350" y1="185" x2="400" y2="185"/>
  <line class="arrow" x1="530" y1="185" x2="580" y2="185"/>

  <line class="arrow" x1="710" y1="175" x2="760" y2="115"/>
  <line class="arrow" x1="710" y1="195" x2="760" y2="275"/>

  <text x="645" y="105"
        font-size="12"
        text-anchor="middle">
    Decoder-only
  </text>

  <text x="645" y="330"
        font-size="12"
        text-anchor="middle">
    RLHF / Tool Use / Reasoning
  </text>
</svg>
```
