/* ======================================================================
   VT Mello Keymaps - High-Performance Keyboard-Agnostic SVG Canvas Renderer
   Engineered with DocumentFragment Batching & Delegated Event Dispatching
   ====================================================================== */

const SVG_NS = "http://www.w3.org/2000/svg";
const XLINK_NS = "http://www.w3.org/1999/xlink";

const DEFAULT_KEY_RX = 3;
const DEFAULT_BOARD_RX = 8;

/**
 * @typedef {Object} CornerRadii
 * @property {number} tl - Top-left radius
 * @property {number} tr - Top-right radius
 * @property {number} br - Bottom-right radius
 * @property {number} bl - Bottom-left radius
 */

/**
 * @typedef {Object} KeyCell
 * @property {string} [t] - Primary tap label
 * @property {string} [h] - Hold / secondary action label
 * @property {string} [sh] - Shifted character label
 * @property {string} [n] - Documentation note / combo explanation
 * @property {string} [icon] - SVG symbol icon identifier
 * @property {boolean} [trans] - Transparent pass-through indicator
 * @property {string} [cls] - Custom CSS class name
 */

/**
 * Generates an SVG path data string for a rectangle with independent corner radii.
 * @param {number} x
 * @param {number} y
 * @param {number} w
 * @param {number} h
 * @param {number} tl
 * @param {number} tr
 * @param {number} br
 * @param {number} bl
 * @returns {string}
 */
export function roundedRectPath(x, y, w, h, tl, tr, br, bl) {
  return (
    `M${x + tl},${y}` +
    `H${x + w - tr}A${tr},${tr} 0 0 1 ${x + w},${y + tr}` +
    `V${y + h - br}A${br},${br} 0 0 1 ${x + w - br},${y + h}` +
    `H${x + bl}A${bl},${bl} 0 0 1 ${x},${y + h - bl}` +
    `V${y + tl}A${tl},${tl} 0 0 1 ${x + tl},${y}Z`
  );
}

/**
 * Estimates text width based on character count and font size.
 * @param {string} text
 * @param {number} fontSize
 * @returns {number}
 */
export function estWidth(text, fontSize) {
  return text.length * fontSize * 0.56;
}

/**
 * Fits a single line of text into maxWidth by incrementally reducing font size.
 * @param {string} text
 * @param {number} maxWidth
 * @param {number} maxFont
 * @param {number} minFont
 * @returns {number}
 */
export function fitSingleLine(text, maxWidth, maxFont, minFont) {
  let fs = maxFont;
  while (fs > minFont && estWidth(text, fs) > maxWidth) {
    fs -= 0.15;
  }
  return Math.max(fs, minFont);
}

/**
 * Wraps text into lines with overflow boundary safety and ellipsis truncation (TASK-18).
 * @param {string} text
 * @param {number} maxWidth
 * @param {number} fontSize
 * @param {number} maxLines
 * @returns {string[]}
 */
export function wrapToLines(text, maxWidth, fontSize, maxLines) {
  const words = text.split(" ");
  const lines = [];
  let currentLine = "";

  for (const word of words) {
    const candidate = currentLine ? `${currentLine} ${word}` : word;
    if (!currentLine || estWidth(candidate, fontSize) <= maxWidth) {
      currentLine = candidate;
    } else {
      lines.push(currentLine);
      currentLine = word;
    }
  }
  if (currentLine) lines.push(currentLine);

  if (lines.length <= maxLines) {
    return lines;
  }

  const head = lines.slice(0, maxLines - 1);
  let tail = lines.slice(maxLines - 1).join(" ");

  if (estWidth(tail, fontSize) > maxWidth) {
    while (tail.length > 1 && estWidth(`${tail}…`, fontSize) > maxWidth) {
      tail = tail.slice(0, -1).trim();
    }
    tail = `${tail}…`;
  }

  return [...head, tail];
}

/**
 * Appends an SVG text node to a parent group.
 * @param {SVGElement} g
 * @param {number} x
 * @param {number} y
 * @param {number} size
 * @param {string} cls
 * @param {string} str
 * @returns {SVGTextElement}
 */
export function addText(g, x, y, size, cls, str) {
  const textEl = document.createElementNS(SVG_NS, "text");
  textEl.setAttribute("x", x.toFixed(2));
  textEl.setAttribute("y", y.toFixed(2));
  textEl.setAttribute("font-size", size.toFixed(2));
  textEl.classList.add(cls);
  textEl.textContent = str;
  g.appendChild(textEl);
  return textEl;
}

/**
 * Calculates font size and wrapped lines for primary tap text.
 * @param {string} text
 * @param {number} maxW
 * @param {number} h
 * @returns {{ font: number, lines: string[] }}
 */
function computeFittedLines(text, maxW, h) {
  const maxLines = h > 15 ? 3 : 2;
  const initialFont = Math.max(3.4, Math.min(7.3, maxW * 0.32));

  if (estWidth(text, initialFont) <= maxW) {
    return { font: initialFont, lines: [text] };
  }

  let fitFont = initialFont;
  let fittedLines = [text];

  for (let attempt = 0; attempt < 6; attempt++) {
    fittedLines = wrapToLines(text, maxW, fitFont, maxLines);
    const widest = Math.max(...fittedLines.map((line) => estWidth(line, fitFont)));
    if (widest <= maxW) break;

    fitFont = Math.max(3.0, fitFont * (maxW / widest) * 0.94);
    if (fitFont <= 3.0) {
      fittedLines = wrapToLines(text, maxW, fitFont, maxLines);
      break;
    }
  }

  return { font: fitFont, lines: fittedLines };
}

/**
 * Renders an SVG icon keycap action.
 * @param {SVGElement} g
 * @param {string} iconId
 * @param {number} cx
 * @param {number} tapCenterY
 * @param {number} maxW
 * @param {number} tapRegionH
 */
function renderKeyIcon(g, iconId, cx, tapCenterY, maxW, tapRegionH) {
  const size = Math.min(maxW, tapRegionH) * 0.86 * 0.6;
  const useEl = document.createElementNS(SVG_NS, "use");
  useEl.setAttributeNS(XLINK_NS, "href", `#icon-${iconId}`);
  useEl.setAttribute("href", `#icon-${iconId}`);
  useEl.setAttribute("x", (cx - size / 2).toFixed(2));
  useEl.setAttribute("y", (tapCenterY - size / 2).toFixed(2));
  useEl.setAttribute("width", size.toFixed(2));
  useEl.setAttribute("height", size.toFixed(2));
  useEl.classList.add("key-tap");
  g.appendChild(useEl);
}

/**
 * Renders primary tap text onto a key.
 * @param {SVGElement} g
 * @param {string} text
 * @param {number} cx
 * @param {number} tapCenterY
 * @param {number} maxW
 * @param {number} h
 */
function renderTapText(g, text, cx, tapCenterY, maxW, h) {
  const { font, lines } = computeFittedLines(text, maxW, h);
  const lineH = font * 1.08;
  const blockTop = tapCenterY - ((lines.length - 1) * lineH) / 2;

  lines.forEach((line, i) => {
    addText(g, cx, blockTop + i * lineH, font, "key-tap", line);
  });
}

/**
 * Renders secondary hold badges or top-right shifted symbols.
 * @param {SVGElement} g
 * @param {KeyCell} cell
 * @param {{ cx: number, x: number, y: number, w: number, h: number, padX: number }} dims
 */
function renderSecondaryBadge(g, cell, { cx, x, y, w, h, padX }) {
  if (cell.h) {
    const holdMaxW = w - Math.min(1.6, w * 0.06) * 2;
    const holdFont = fitSingleLine(cell.h, holdMaxW, Math.min(5.6, w * 0.24), 3.2);
    addText(g, cx, y + h * 0.78, holdFont, "key-hold", cell.h);
    return;
  }

  if (cell.sh) {
    const shiftFont = Math.min(4.2, w * 0.18);
    addText(g, x + w - padX - 1.5, y + padX + 2, shiftFont, "key-shift", cell.sh);
  }
}

/**
 * Dispatches keycap content rendering (pass-through, icon, tap, hold, shift).
 * @param {SVGElement} g
 * @param {KeyCell|null} cell
 * @param {{ cx: number, x: number, y: number, w: number, h: number, padX: number, maxW: number }} dims
 */
function renderKeyContent(g, cell, dims) {
  if (!cell) return;

  const { cx, y, h, maxW } = dims;

  if (cell.trans) {
    addText(g, cx, y + h / 2, Math.min(6.2, h * 0.32), "key-trans", "▼");
    return;
  }

  const hasHold = Boolean(cell.h);
  const tapRegionH = hasHold ? h * 0.62 : h * 0.92;
  const tapCenterY = hasHold ? y + h * 0.34 : y + h / 2;

  if (cell.icon) {
    renderKeyIcon(g, cell.icon, cx, tapCenterY, maxW, tapRegionH);
  } else if (cell.t) {
    renderTapText(g, cell.t, cx, tapCenterY, maxW, h);
  }

  renderSecondaryBadge(g, cell, dims);
}

/**
 * Calculates outer keyboard perimeter corner radii dynamically (TASK-15).
 * @param {number} rowIdx
 * @param {number} colIdx
 * @param {number} rowCount
 * @param {number} colCount
 * @param {number} [defaultRx]
 * @param {number} [boardCornerRx]
 * @returns {CornerRadii}
 */
export function getBoardCornerRadii(
  rowIdx,
  colIdx,
  rowCount,
  colCount,
  defaultRx = DEFAULT_KEY_RX,
  boardCornerRx = DEFAULT_BOARD_RX
) {
  const corners = { tl: defaultRx, tr: defaultRx, br: defaultRx, bl: defaultRx };
  const isTop = rowIdx === 0;
  const isBottom = rowIdx === rowCount - 1;
  const isLeft = colIdx === 0;
  const isRight = colIdx === colCount - 1;

  if (isTop && isLeft) corners.tl = boardCornerRx;
  if (isTop && isRight) corners.tr = boardCornerRx;
  if (isBottom && isLeft) corners.bl = boardCornerRx;
  if (isBottom && isRight) corners.br = boardCornerRx;

  return corners;
}

/**
 * Builds an SVG `<g class="key">` element with boundary path and metadata attributes.
 * @param {KeyCell|null} cell
 * @param {number} x
 * @param {number} y
 * @param {number} w
 * @param {number} h
 * @param {CornerRadii} corners
 * @param {string} extraClass
 * @param {number} rowIdx
 * @param {number} colIdx
 * @param {number|undefined} [subIdx]
 * @returns {SVGGElement}
 */
export function buildCellGroup(
  cell,
  x,
  y,
  w,
  h,
  corners,
  extraClass,
  rowIdx,
  colIdx,
  subIdx
) {
  const effectiveCorners = corners || {
    tl: DEFAULT_KEY_RX,
    tr: DEFAULT_KEY_RX,
    br: DEFAULT_KEY_RX,
    bl: DEFAULT_KEY_RX,
  };

  const g = document.createElementNS(SVG_NS, "g");
  g.classList.add("key");
  if (extraClass) {
    g.classList.add(...extraClass.split(" ").filter(Boolean));
  }

  // Data attributes for O(1) delegated event lookup
  g.dataset.row = String(rowIdx);
  g.dataset.col = String(colIdx);
  if (subIdx !== undefined) {
    g.dataset.sub = String(subIdx);
  }

  if (cell === null || cell === undefined) {
    g.classList.add("inert");
  }

  // Key outline / plate boundary
  const keyPath = document.createElementNS(SVG_NS, "path");
  keyPath.setAttribute(
    "d",
    roundedRectPath(
      x,
      y,
      w,
      h,
      effectiveCorners.tl,
      effectiveCorners.tr,
      effectiveCorners.br,
      effectiveCorners.bl
    )
  );
  keyPath.classList.add("key-cover");
  g.appendChild(keyPath);

  // Geometry dimensions
  const padX = Math.min(2.2, w * 0.08);
  const dims = {
    cx: x + w / 2,
    x,
    y,
    w,
    h,
    padX,
    maxW: w - padX * 2,
  };

  renderKeyContent(g, cell, dims);
  return g;
}

/**
 * Retrieves a key cell from a layer using grid coordinates in O(1).
 * @param {Object} layer
 * @param {number} row
 * @param {number} col
 * @param {number|string|undefined} [sub]
 * @returns {KeyCell|null}
 */
export function getCellFromLayer(layer, row, col, sub) {
  if (!layer?.rows?.[row]) return null;
  const cell = layer.rows[row][col];
  if (Array.isArray(cell) && sub !== undefined && sub !== null && sub !== "") {
    return cell[Number(sub)] ?? null;
  }
  return cell ?? null;
}

/**
 * Standard classification helper for B1 Pro / 75% ANSI layouts.
 * @param {number} rowIdx
 * @param {number} colIdx
 * @param {number} rowCount
 * @param {number} colCount
 * @param {KeyCell|null} [cell]
 * @returns {string}
 */
export function defaultKeyClassifier(rowIdx, colIdx, rowCount, colCount, cell) {
  if (cell?.cls) return cell.cls;

  const isEscOrEnter =
    (rowIdx === 0 && (colIdx === 0 || colIdx === colCount - 1)) ||
    (rowIdx === 3 && (colIdx === 0 || colIdx === colCount - 1));

  if (isEscOrEnter) return "esc-ent-group";

  const isUtility =
    rowIdx === 0 ||
    colIdx === 0 ||
    (rowIdx === 1 && colIdx === colCount - 1) ||
    (rowIdx === 2 && colIdx === colCount - 1) ||
    (rowIdx === 4 && colIdx === colCount - 1) ||
    (rowIdx === rowCount - 1 && [1, 2, 4, 5].includes(colIdx));

  return isUtility ? "utility-group" : "";
}

/**
 * Renders a complete keyboard layer onto an SVG container using DocumentFragment (TASK-20).
 * Completely keyboard-agnostic (TASK-14, TASK-15).
 * @param {Object} layer - Layer definition containing rows
 * @param {Array<Array<Array<number>>>} grid - Geometry grid array
 * @param {SVGElement} overlayEl - Target SVG container
 * @param {Object} [options]
 * @param {(row: number, col: number, rowCount: number, colCount: number, cell: KeyCell|null) => string} [options.keyClassifier]
 * @param {{ row: number, col: number, sub?: number }|null} [options.selectedCoord]
 */
export function renderLayer(layer, grid, overlayEl, options = {}) {
  if (!layer || !grid || !overlayEl) return;

  const fragment = document.createDocumentFragment();
  const rowCount = grid.length;
  const classifier = options.keyClassifier || defaultKeyClassifier;
  const selected = options.selectedCoord;

  layer.rows.forEach((row, rowIdx) => {
    const colCount = row.length;
    row.forEach((cell, colIdx) => {
      const box = grid[rowIdx][colIdx];
      // TASK-14: Pure structural stacked-cell detection; zero imported B1Pro constants
      const isStackedCell = Array.isArray(box[0]);

      if (isStackedCell) {
        const [topBox, bottomBox] = box;
        const [topCell, bottomCell] = Array.isArray(cell) ? cell : [cell, null];

        const topGroup = buildCellGroup(
          topCell,
          topBox[0],
          topBox[1],
          topBox[2] - topBox[0],
          topBox[3] - topBox[1],
          { tl: DEFAULT_KEY_RX, tr: DEFAULT_KEY_RX, br: 1, bl: 1 },
          "utility-group",
          rowIdx,
          colIdx,
          0
        );

        const bottomGroup = buildCellGroup(
          bottomCell,
          bottomBox[0],
          bottomBox[1],
          bottomBox[2] - bottomBox[0],
          bottomBox[3] - bottomBox[1],
          { tl: 1, tr: 1, br: DEFAULT_KEY_RX, bl: DEFAULT_KEY_RX },
          "utility-group",
          rowIdx,
          colIdx,
          1
        );

        if (selected && selected.row === rowIdx && selected.col === colIdx) {
          if (selected.sub === 0) topGroup.classList.add("selected");
          if (selected.sub === 1) bottomGroup.classList.add("selected");
        }

        fragment.appendChild(topGroup);
        fragment.appendChild(bottomGroup);
        return;
      }

      const [x0, y0, x1, y1] = box;
      // TASK-15: Dynamic corner radii derived from grid dimensions
      const corners = getBoardCornerRadii(rowIdx, colIdx, rowCount, colCount);
      const extraClass = classifier(rowIdx, colIdx, rowCount, colCount, cell);

      const cellGroup = buildCellGroup(
        cell,
        x0,
        y0,
        x1 - x0,
        y1 - y0,
        corners,
        extraClass,
        rowIdx,
        colIdx
      );

      if (
        selected &&
        selected.row === rowIdx &&
        selected.col === colIdx &&
        selected.sub === undefined
      ) {
        cellGroup.classList.add("selected");
      }

      fragment.appendChild(cellGroup);
    });
  });

  // Atomic off-DOM replacement (single reflow/repaint, zero listener leaks)
  overlayEl.replaceChildren(fragment);
}

/**
 * Attaches permanent delegated event listeners to the SVG overlay container (TASK-20).
 * @param {SVGElement} overlayEl
 * @param {{
 *   onHover: (target: { keyEl: SVGGElement, row: number, col: number, sub?: number }) => void,
 *   onLeave: () => void,
 *   onSelect: (target: { keyEl: SVGGElement, row: number, col: number, sub?: number }, evt: MouseEvent) => void
 * }} handlers
 * @returns {() => void} Cleanup teardown function
 */
export function attachDelegatedEvents(overlayEl, handlers) {
  let activeHoverEl = null;

  function handlePointerMove(e) {
    const keyEl = e.target.closest(".key");
    if (!keyEl || !overlayEl.contains(keyEl)) {
      if (activeHoverEl) {
        activeHoverEl = null;
        handlers.onLeave();
      }
      return;
    }

    if (activeHoverEl === keyEl) return;
    activeHoverEl = keyEl;

    const row = Number(keyEl.dataset.row);
    const col = Number(keyEl.dataset.col);
    const sub = keyEl.dataset.sub !== undefined ? Number(keyEl.dataset.sub) : undefined;

    handlers.onHover({ keyEl, row, col, sub });
  }

  function handlePointerLeave(e) {
    const related = e.relatedTarget?.closest?.(".key");
    if (!related && activeHoverEl) {
      activeHoverEl = null;
      handlers.onLeave();
    }
  }

  function handleClick(e) {
    const keyEl = e.target.closest(".key");
    if (!keyEl || !overlayEl.contains(keyEl)) return;
    e.stopPropagation();

    const row = Number(keyEl.dataset.row);
    const col = Number(keyEl.dataset.col);
    const sub = keyEl.dataset.sub !== undefined ? Number(keyEl.dataset.sub) : undefined;

    handlers.onSelect({ keyEl, row, col, sub }, e);
  }

  overlayEl.addEventListener("pointerover", handlePointerMove);
  overlayEl.addEventListener("pointerout", handlePointerLeave);
  overlayEl.addEventListener("click", handleClick);

  return () => {
    overlayEl.removeEventListener("pointerover", handlePointerMove);
    overlayEl.removeEventListener("pointerout", handlePointerLeave);
    overlayEl.removeEventListener("click", handleClick);
  };
}
