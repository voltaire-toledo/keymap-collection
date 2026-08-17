/* ======================================================================
   Keyboard-Agnostic SVG Canvas Renderer & Path Builder
   ====================================================================== */
import { RX, BOARD_CORNER_RX, STACKED_ROW, STACKED_COL } from "../config/grid_b1pro.js";

const svgNS = "http://www.w3.org/2000/svg";

export function roundedRectPath(x, y, w, h, tl, tr, br, bl) {
  return (
    `M${x + tl},${y}` +
    `H${x + w - tr}A${tr},${tr} 0 0 1 ${x + w},${y + tr}` +
    `V${y + h - br}A${br},${br} 0 0 1 ${x + w - br},${y + h}` +
    `H${x + bl}A${bl},${bl} 0 0 1 ${x},${y + h - bl}` +
    `V${y + tl}A${tl},${tl} 0 0 1 ${x + tl},${y}Z`
  );
}

export function estWidth(text, fontSize) {
  return text.length * fontSize * 0.56;
}

export function fitSingleLine(text, maxWidth, maxFont, minFont) {
  let fs = maxFont;
  while (fs > minFont && estWidth(text, fs) > maxWidth) fs -= 0.15;
  return Math.max(fs, minFont);
}

export function wrapToLines(text, maxWidth, fontSize, maxLines) {
  const words = text.split(" ");
  const lines = [];
  let cur = "";
  words.forEach((word) => {
    const test = cur ? cur + " " + word : word;
    if (!cur || estWidth(test, fontSize) <= maxWidth) cur = test;
    else {
      lines.push(cur);
      cur = word;
    }
  });
  if (cur) lines.push(cur);
  if (lines.length > maxLines) {
    const head = lines.slice(0, maxLines - 1);
    const tail = lines.slice(maxLines - 1).join(" ");
    return [...head, tail];
  }
  return lines;
}

export function addText(g, x, y, size, cls, str) {
  const t = document.createElementNS(svgNS, "text");
  t.setAttribute("x", x);
  t.setAttribute("y", y);
  t.setAttribute("font-size", size.toFixed(2));
  t.classList.add(cls);
  t.textContent = str;
  g.appendChild(t);
  return t;
}

export function buildCellGroup(cell, x, y, w, h, corners, extraClass, showInfoCallback, state) {
  corners = corners || { tl: RX, tr: RX, br: RX, bl: RX };
  const g = document.createElementNS(svgNS, "g");
  g.classList.add("key");
  if (extraClass) g.classList.add(...extraClass.split(" ").filter(Boolean));

  const isEmpty = cell === null || cell === undefined;
  const isTrans = cell && cell.trans;
  if (isEmpty) g.classList.add("inert");

  const keyPath = document.createElementNS(svgNS, "path");
  keyPath.setAttribute(
    "d",
    roundedRectPath(x, y, w, h, corners.tl, corners.tr, corners.br, corners.bl)
  );
  keyPath.classList.add("key-cover");
  g.appendChild(keyPath);

  const cx = x + w / 2;
  const padX = Math.min(2.2, w * 0.08);
  const maxW = w - padX * 2;
  const hasHold = cell && cell.h;
  const hasShift = cell && cell.sh;

  if (isTrans) {
    addText(g, cx, y + h / 2, Math.min(6.2, h * 0.32), "key-trans", "▼");
  } else if (!isEmpty) {
    const tapRegionH = hasHold ? h * 0.62 : h * 0.92;
    const tapCenterY = hasHold ? y + h * 0.34 : y + h / 2;

    if (cell.icon) {
      const size = Math.min(maxW, tapRegionH) * 0.86 * 0.6;
      const use = document.createElementNS(svgNS, "use");
      use.setAttributeNS("http://www.w3.org/1999/xlink", "href", "#icon-" + cell.icon);
      use.setAttribute("href", "#icon-" + cell.icon);
      use.setAttribute("x", (cx - size / 2).toFixed(2));
      use.setAttribute("y", (tapCenterY - size / 2).toFixed(2));
      use.setAttribute("width", size.toFixed(2));
      use.setAttribute("height", size.toFixed(2));
      use.classList.add("key-tap");
      g.appendChild(use);
    } else {
      let font = Math.min(7.3, w * 0.32, tapRegionH * 0.62);
      font = Math.max(font, 3.4);
      let lines = [cell.t];
      if (estWidth(cell.t, font) > maxW) {
        const maxLines = h > 15 ? 3 : 2;
        let fitFont = font;
        for (let attempt = 0; attempt < 6; attempt++) {
          lines = wrapToLines(cell.t, maxW, fitFont, maxLines);
          const widest = Math.max(...lines.map((l) => estWidth(l, fitFont)));
          if (widest <= maxW) break;
          fitFont = Math.max(3.0, fitFont * (maxW / widest) * 0.94);
          if (fitFont <= 3.0) {
            lines = wrapToLines(cell.t, maxW, fitFont, maxLines);
            break;
          }
        }
        font = fitFont;
      }
      const lineH = font * 1.08;
      const blockTop = tapCenterY - ((lines.length - 1) * lineH) / 2;
      lines.forEach((line, i) =>
        addText(g, cx, blockTop + i * lineH, font, "key-tap", line)
      );
    }

    if (hasHold) {
      const holdMaxW = w - Math.min(1.6, w * 0.06) * 2;
      const holdFont = fitSingleLine(cell.h, holdMaxW, Math.min(5.6, w * 0.24), 3.2);
      addText(g, cx, y + h * 0.78, holdFont, "key-hold", cell.h);
    } else if (hasShift) {
      const shiftFont = Math.min(4.2, w * 0.18);
      addText(g, x + w - padX - 1.5, y + padX + 2, shiftFont, "key-shift", cell.sh);
    }
  }

  g.addEventListener("mouseenter", () => showInfoCallback(cell));
  g.addEventListener("click", (e) => {
    e.stopPropagation();
    if (state.selectedKey) state.selectedKey.classList.remove("selected");
    if (state.selectedKey === g) {
      state.selectedKey = null;
      return;
    }
    g.classList.add("selected");
    state.selectedKey = g;
    showInfoCallback(cell);
  });
  return g;
}

export function renderLayer(id, layers, grid, overlayEl, showInfoCallback, state) {
  state.currentLayerId = id;
  const layer = layers[id];
  if (!layer) return;
  overlayEl.innerHTML = "";
  state.selectedKey = null;

  layer.rows.forEach((row, rowIdx) => {
    const lastCol = row.length - 1;
    row.forEach((cell, colIdx) => {
      const box = grid[rowIdx][colIdx];

      if (rowIdx === STACKED_ROW && colIdx === STACKED_COL && Array.isArray(cell)) {
        const [topBox, bottomBox] = box;
        overlayEl.appendChild(
          buildCellGroup(cell[0], topBox[0], topBox[1], topBox[2] - topBox[0], topBox[3] - topBox[1], { tl: RX, tr: RX, br: 1, bl: 1 }, "utility-group", showInfoCallback, state)
        );
        overlayEl.appendChild(
          buildCellGroup(cell[1], bottomBox[0], bottomBox[1], bottomBox[2] - bottomBox[0], bottomBox[3] - bottomBox[1], { tl: 1, tr: 1, br: RX, bl: RX }, "utility-group", showInfoCallback, state)
        );
      } else {
        const [x0, y0, x1, y1] = box;
        const w = x1 - x0;
        const h = y1 - y0;

        const corners = { tl: RX, tr: RX, br: RX, bl: RX };
        if (rowIdx === 0 && colIdx === 0) corners.tl = BOARD_CORNER_RX;
        if (rowIdx === 0 && colIdx === lastCol) corners.tr = BOARD_CORNER_RX;
        if (rowIdx === 5 && colIdx === 0) corners.bl = BOARD_CORNER_RX;
        if (rowIdx === 5 && colIdx === lastCol) corners.br = BOARD_CORNER_RX;

        let extra = "";
        if (
          (rowIdx === 0 && (colIdx === 0 || colIdx === 13)) ||
          (rowIdx === 3 && (colIdx === 0 || colIdx === 12))
        )
          extra = "esc-ent-group";
        else if (
          rowIdx === 0 ||
          colIdx === 0 ||
          (rowIdx === 1 && colIdx === 13) ||
          (rowIdx === 2 && colIdx === 13) ||
          (rowIdx === 4 && colIdx === 11) ||
          (rowIdx === 5 && (colIdx === 1 || colIdx === 2 || colIdx === 4 || colIdx === 5))
        )
          extra = "utility-group";

        overlayEl.appendChild(
          buildCellGroup(cell, x0, y0, w, h, corners, extra, showInfoCallback, state)
        );
      }
    });
  });
}
