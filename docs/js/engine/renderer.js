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
/**
 * Resolves a layer icon ID if the key represents a layer transition.
 * @param {KeyCell} cell
 * @returns {string|null}
 */
function resolveLayerIcon(cell) {
  if (cell.icon && cell.icon.startsWith("layer-")) return cell.icon;
  if (!cell.t) return null;

  const text = cell.t.trim();
  const match = text.match(/^(?:⇒|=>)\s*([A-Za-z0-9_-]+)/i);
  if (!match) return null;

  const target = match[1].toLowerCase();
  if (target.includes("base")) return "layer-base";
  if (target.includes("func")) return "layer-function";
  if (target.includes("sym")) return "layer-symbols";
  if (target.includes("nav")) return "layer-navigation";
  if (target.includes("macro")) return "layer-macro";
  if (target.includes("reserved") || target.includes("uat")) return "layer-reserved";

  return null;
}

/**
 * Resolves a secondary hold icon definition (standard key icon or layer sheet icon).
 * Specifically handles:
 * - Position 00 (Esc holding Caps Lock -> icon: "caps-lock")
 * - Position 41 (\ holding Navigation -> layer icon: "layer-navigation")
 * - Position 42 (Caps Lock / Esc holding Function -> layer icon: "layer-function")
 * - Position 70 (Space holding Navigation -> layer icon: "layer-navigation")
 * - Position 72 (Fn holding Function -> layer icon: "layer-function")
 * - Home-row G and H keys for Symbols layer -> layer icon: "layer-symbols"
 * @param {KeyCell} cell
 * @returns {{ iconId: string, isLayer: boolean }|null}
 */
function resolveHoldIcon(cell) {
  if (cell.holdIcon) {
    const isLayer = cell.holdIcon.startsWith("layer-");
    return { iconId: cell.holdIcon, isLayer };
  }
  if (!cell.h) return null;

  const h = cell.h.trim().toLowerCase();
  if (/caps/i.test(h)) return { iconId: "caps-lock", isLayer: false };
  if (/sym/i.test(h)) return { iconId: "layer-symbols", isLayer: true };
  if (/nav/i.test(h)) return { iconId: "layer-navigation", isLayer: true };
  if (/func/i.test(h)) return { iconId: "layer-function", isLayer: true };
  if (/macro/i.test(h)) return { iconId: "layer-macro", isLayer: true };
  if (/base/i.test(h)) return { iconId: "layer-base", isLayer: true };
  if (/reserved|uat/i.test(h)) return { iconId: "layer-reserved", isLayer: true };

  return null;
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
  const isLayerIcon = iconId.startsWith("layer-");
  const size = isLayerIcon
    ? Math.min(maxW * 0.52, tapRegionH * 0.68)
    : Math.min(maxW, tapRegionH) * 0.86 * 0.6;

  const useEl = document.createElementNS(SVG_NS, "use");
  useEl.setAttributeNS(XLINK_NS, "href", `#icon-${iconId}`);
  useEl.setAttribute("href", `#icon-${iconId}`);
  useEl.setAttribute("x", (cx - size / 2).toFixed(2));
  useEl.setAttribute("y", (tapCenterY - size / 2).toFixed(2));
  useEl.setAttribute("width", size.toFixed(2));
  useEl.setAttribute("height", size.toFixed(2));
  useEl.classList.add(isLayerIcon ? "key-layer-icon" : "key-tap");
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
 * @param {number} [fontOverride]
 */
function renderTapText(g, text, cx, tapCenterY, maxW, h, fontOverride) {
  const { font: computedFont, lines } = computeFittedLines(text, maxW, h);
  const font = fontOverride || computedFont;
  const lineH = font * 1.08;
  const blockTop = tapCenterY - ((lines.length - 1) * lineH) / 2;

  lines.forEach((line, i) => {
    addText(g, cx, blockTop + i * lineH, font, "key-tap", line);
  });
}

/**
/**
 * Detects if a key is an F1-F12 function key with a media/action symbol.
 * @param {KeyCell} cell
 * @returns {{ fText: string, iconId: string|null, symbolText: string|null }|null}
 */
function getFunctionKeyInfo(cell) {
  if (!cell) return null;

  // Case 1: cell.sh is F1-F12 (standard Base layer F-row)
  if (cell.sh && /^F(?:1[0-2]|[1-9])$/i.test(cell.sh.trim())) {
    const fText = cell.sh.trim().toUpperCase();
    return {
      fText,
      iconId: cell.icon || null,
      symbolText: !cell.icon && cell.t && cell.t !== fText ? cell.t : null,
    };
  }

  // Case 2: cell.t is F1-F12 and has an icon
  if (cell.t && /^F(?:1[0-2]|[1-9])$/i.test(cell.t.trim()) && cell.icon) {
    return {
      fText: cell.t.trim().toUpperCase(),
      iconId: cell.icon,
      symbolText: null,
    };
  }

  return null;
}

/**
 * Renders secondary hold badges or top-right shifted symbols.
 * @param {SVGElement} g
 * @param {KeyCell} cell
 * @param {{ cx: number, x: number, y: number, w: number, h: number, padX: number, maxW: number }} dims
 */
function renderSecondaryBadge(g, cell, { cx, x, y, w, h, padX, maxW }) {
  const holdIconInfo = resolveHoldIcon(cell);
  if (holdIconInfo) {
    const iconSize = Math.min(10.0, maxW * 0.52, h * 0.44);
    const iconCenterY = y + h * 0.69;
    const useEl = document.createElementNS(SVG_NS, "use");
    useEl.setAttributeNS(XLINK_NS, "href", `#icon-${holdIconInfo.iconId}`);
    useEl.setAttribute("href", `#icon-${holdIconInfo.iconId}`);
    useEl.setAttribute("x", (cx - iconSize / 2).toFixed(2));
    useEl.setAttribute("y", (iconCenterY - iconSize / 2).toFixed(2));
    useEl.setAttribute("width", iconSize.toFixed(2));
    useEl.setAttribute("height", iconSize.toFixed(2));
    useEl.classList.add(
      holdIconInfo.isLayer ? "key-layer-icon" : "key-icon",
      "key-hold-icon"
    );
    g.appendChild(useEl);
    return;
  }

  if (cell.h) {
    const holdMaxW = w - Math.min(1.6, w * 0.06) * 2;
    const holdFont = fitSingleLine(cell.h, holdMaxW, Math.min(5.6, w * 0.24), 3.2);
    addText(g, cx, y + h * 0.76, holdFont, "key-hold", cell.h);
    return;
  }

  if (cell.sh) {
    const shiftFont = Math.min(4.2, w * 0.18);
    addText(g, x + w - padX - 1.5, y + padX + 2, shiftFont, "key-shift", cell.sh);
  }
}

/**
 * Detects if a key cell has "hidden powers" beyond standard tap or momentary hold.
 * (Tap-dance, combo participation, deep long-hold 3s+, or explicit powers metadata).
 * @param {KeyCell|null} cell
 * @returns {boolean}
 */
export function hasSuperpower(cell) {
  if (!cell) return false;
  if (cell.powers) return true;
  if (!cell.n) return false;
  const n = cell.n.toLowerCase();
  return (
    n.includes("double-tap") ||
    n.includes("tap-dance") ||
    n.includes("combo") ||
    n.includes("hold 3s") ||
    n.includes("hold 5s") ||
    n.includes("long-hold") ||
    n.includes("bootloader") ||
    n.includes("reset")
  );
}

/**
 * Renders the accessible superpower badge in the corner where the geometric dog-ear would be.
 * @param {SVGElement} g
 * @param {KeyCell} cell
 * @param {{ x: number, y: number, w: number, h: number }} dims
 */
export function renderSuperpowerBadge(g, cell, { x, y, w, h }) {
  if (!hasSuperpower(cell)) return;

  const badgeSize = Math.min(6.8, w * 0.28, h * 0.28);
  const badgeX = x + 1.2;
  const badgeY = y + 1.2;

  const useEl = document.createElementNS(SVG_NS, "use");
  useEl.setAttributeNS(XLINK_NS, "href", "#icon-dogear-badge");
  useEl.setAttribute("href", "#icon-dogear-badge");
  useEl.setAttribute("x", badgeX.toFixed(2));
  useEl.setAttribute("y", badgeY.toFixed(2));
  useEl.setAttribute("width", badgeSize.toFixed(2));
  useEl.setAttribute("height", badgeSize.toFixed(2));
  useEl.classList.add("key-superpower-dogear", "key-superpower-badge");
  g.appendChild(useEl);
}

/**
 * Resolves the complete 5-tier firmware action matrix for a key cell.
 * @param {KeyCell|null} cell
 * @returns {{ tap: string, hold: string, dance: string, long: string, combo: string, isSpecial: boolean }}
 */
export function resolveFirmwareMatrix(cell) {
  if (!cell) {
    return {
      tap: "—",
      hold: "—",
      dance: "—",
      long: "—",
      combo: "—",
      isSpecial: false,
    };
  }

  // If cell defines explicit powers object
  if (cell.powers) {
    const p = cell.powers;
    const comboStr = Array.isArray(p.combos)
      ? p.combos.map((c) => `+ [${c.withKeys.join(" + ")}] ⇒ ${c.action}`).join("<br>")
      : (p.combo || "—");

    return {
      tap: p.tap || cell.t || "—",
      hold: p.hold || cell.h || "—",
      dance: p.doubleTap || p.tapDance || "—",
      long: p.longHold || p.deepHold || "—",
      combo: comboStr,
      isSpecial: true,
    };
  }

  // Otherwise, intelligently extract from cell fields & documentation notes
  const tapStr = cell.t || (cell.icon ? `Icon (#icon-${cell.icon})` : "—");
  const holdStr = cell.h || "—";
  let danceStr = "—";
  let longStr = "—";
  let comboStr = "—";
  let isSpecial = false;

  if (cell.n) {
    const n = cell.n;
    // Check for double-tap / tap dance
    const dtMatch = n.match(/(?:Double-tap|Tap-dance|2x tap):\s*([^.]+)/i);
    if (dtMatch) {
      danceStr = dtMatch[1].trim();
      isSpecial = true;
    }

    // Check for deep hold (3s+, 5s+, bootloader, reset)
    const lhMatch = n.match(/(?:Hold\s*\d+s|Long-hold|Deep-hold|Bootloader|Reset):\s*([^.]+)/i);
    if (lhMatch) {
      longStr = lhMatch[1].trim();
      isSpecial = true;
    } else if (/bootloader|reset/i.test(n)) {
      longStr = "Hold 5s: Firmware Reset / Bootloader";
      isSpecial = true;
    }

    // Check for combos
    const cbMatch = n.match(/(?:Combo|Chord):\s*([^.]+)/i);
    if (cbMatch) {
      comboStr = cbMatch[1].trim();
      isSpecial = true;
    }
  }

  // Known firmware chord mappings on standard layers
  if (cell.t === "Q" || cell.t === "W") {
    comboStr = "+ [Q + W] simultaneously ⇒ Esc (Quick Escape Chord)";
    isSpecial = true;
  } else if (cell.t === "J" || cell.t === "K") {
    comboStr = "+ [J + K] simultaneously ⇒ Enter (Quick Enter Chord)";
    isSpecial = true;
  } else if (cell.t === "Tab" || cell.t === "\\") {
    comboStr = "+ [Tab + \\] simultaneously ⇒ Toggle Nav Layer Lock";
    isSpecial = true;
  }

  return {
    tap: tapStr,
    hold: holdStr,
    dance: danceStr,
    long: longStr,
    combo: comboStr,
    isSpecial,
  };
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

  // Function keys (F1-F12 with symbols): F1-12 text at 4.80, bold & centered, symbols at 10.0 height
  const fKey = getFunctionKeyInfo(cell);
  if (fKey) {
    const fTextY = y + h * 0.28;
    addText(g, cx, fTextY, 4.8, "key-tap", fKey.fText);

    const symCenterY = y + h * 0.69;
    if (fKey.iconId) {
      const symSize = 10.0;
      const useEl = document.createElementNS(SVG_NS, "use");
      useEl.setAttributeNS(XLINK_NS, "href", `#icon-${fKey.iconId}`);
      useEl.setAttribute("href", `#icon-${fKey.iconId}`);
      useEl.setAttribute("x", (cx - symSize / 2).toFixed(2));
      useEl.setAttribute("y", (symCenterY - symSize / 2).toFixed(2));
      useEl.setAttribute("width", symSize.toFixed(2));
      useEl.setAttribute("height", symSize.toFixed(2));
      useEl.classList.add("key-icon");
      g.appendChild(useEl);
    } else if (fKey.symbolText) {
      addText(g, cx, symCenterY, 5.8, "key-tap", fKey.symbolText);
    }
    renderSuperpowerBadge(g, cell, dims);
    return;
  }

  const holdIconInfo = resolveHoldIcon(cell);
  const hasHold = Boolean(cell.h);
  const isDualRole = hasHold || Boolean(holdIconInfo);
  const tapRegionH = isDualRole ? h * 0.62 : h * 0.92;
  const tapCenterY = isDualRole ? y + h * 0.28 : y + h / 2;

  const layerIconId = resolveLayerIcon(cell);
  const iconToRender = cell.icon || layerIconId;

  if (iconToRender) {
    renderKeyIcon(g, iconToRender, cx, tapCenterY, maxW, tapRegionH);
  } else if (cell.t) {
    const fontOverride = isDualRole ? 4.8 : undefined;
    renderTapText(g, cell.t, cx, tapCenterY, maxW, h, fontOverride);
  }

  renderSecondaryBadge(g, cell, dims);
  renderSuperpowerBadge(g, cell, dims);
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
    (rowIdx === 0 && colIdx === 0) ||
    (rowIdx === 3 && colIdx === colCount - 1);

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
