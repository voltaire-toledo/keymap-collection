/* ======================================================================
   VT Mello Keymaps - Modern UI Engine & View Controller
   Built with Reactive Unidirectional Flow & Event Delegation
   ====================================================================== */
import { KEYBOARD_CATALOG } from "../config/catalog.js";
import {
  renderLayer,
  attachDelegatedEvents,
  getCellFromLayer,
  resolveFirmwareMatrix,
  hasSuperpower,
} from "./renderer.js";
import { createStore } from "./store.js";

/**
 * Supported hardware colorways with matching SVG plate assets.
 */
export const COLORWAYS = Object.freeze([
  {
    id: "retro-red",
    name: "Retro Red",
    swatch: "#93162a",
    file: "assets/b1pro/KcB1Pro_ANSI_PRetro.svg",
  },
  {
    id: "retro-blue",
    name: "Retro Blue",
    swatch: "#9ca4b2",
    file: "assets/b1pro/KcB1Pro_ANSI_PRetro.svg",
  },
  {
    id: "space-gray",
    name: "Space Gray",
    swatch: "#3b3b3d",
    file: "assets/b1pro/KcB1Pro_ANSI_PSG.svg",
  },
  {
    id: "ivory-white",
    name: "Ivory White",
    swatch: "#fdfdfc",
    file: "assets/b1pro/KcB1Pro_ANSI_PI.svg",
  },
]);

/**
 * Escapes HTML entities to prevent XSS.
 * @param {any} str
 * @returns {string}
 */
export function escapeHtml(str) {
  if (!str) return "";
  return String(str)
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#039;");
}

/**
 * Categorizes the functional role of a keycap binding.
 * @param {Object} cell
 * @returns {string}
 */
export function getKeyRole(cell) {
  if (cell.icon?.startsWith("layer-") || (cell.t && /^(?:⇒|=>)/.test(cell.t))) return "Layer Activation";
  if (cell.h && cell.t) {
    if (/(?:sym|nav|func|macro|base|reserved)/i.test(cell.h)) return "Layer Hold Switch";
    return "Dual-Role Key";
  }
  if (cell.icon) return "Media Icon";
  if (cell.sh) return "Shifted Key";
  return "Standard Binding";
}

/**
 * Formats user-friendly documentation note for a key binding.
 * @param {Object} cell
 * @returns {string}
 */
export function getKeyNote(cell) {
  if (cell.n) return escapeHtml(cell.n);
  if (cell.t && /^(?:⇒|=>)\s*Base/i.test(cell.t)) {
    return "Tap to activate and return to the <strong>Base Layer</strong>.";
  }
  if (cell.t && /^(?:⇒|=>)\s*([A-Za-z0-9_-]+)/i.test(cell.t)) {
    const match = cell.t.match(/^(?:⇒|=>)\s*([A-Za-z0-9_-]+)/i);
    return `Tap to activate and switch to the <strong>${escapeHtml(match[1])} Layer</strong>.`;
  }
  if (cell.h && cell.t) {
    if (/nav/i.test(cell.h)) {
      return `Single tap emits <strong>${escapeHtml(cell.t)}</strong>. Hold momentarily activates the <strong>Navigation Layer</strong>.`;
    }
    if (/func/i.test(cell.h)) {
      return `Single tap emits <strong>${escapeHtml(cell.t)}</strong>. Hold momentarily activates the <strong>Function Layer</strong>.`;
    }
    if (/sym/i.test(cell.h)) {
      return `Single tap emits <strong>${escapeHtml(cell.t)}</strong>. Hold momentarily activates the <strong>Symbols Layer</strong>.`;
    }
    const cleanHold = cell.h.replace(/^[→⇒=>\s]+/, "");
    return `Single tap emits <strong>${escapeHtml(cell.t)}</strong>. Hold emits <strong>${escapeHtml(cleanHold)}</strong>.`;
  }
  if (cell.t) {
    return `Standard keypress action for <strong>${escapeHtml(cell.t)}</strong> on this layer.`;
  }
  return "Active key binding on this layer.";
}

/**
 * Formats Markdown changelog text securely into structured HTML.
 * @param {Object|null} release
 * @returns {string}
 */
export function formatChangelogHtml(release) {
  if (!release?.changelog?.length) {
    return `
      <h4>Coming Soon</h4>
      <p>Changelog and release notes for this keyboard configuration are currently being prepared.</p>
    `;
  }

  const notesHtml = release.notes
    ? `<p style="color: var(--text-muted); margin-bottom: 1rem;"><em>${escapeHtml(release.notes)}</em></p>`
    : "";

  const listItems = release.changelog
    .map((line) => {
      const safeText = escapeHtml(line);
      const bolded = safeText.replace(/\*\*(.*?)\*\*/g, "<strong>$1</strong>");
      return `<li>${bolded}</li>`;
    })
    .join("");

  return `
    <h4>${escapeHtml(release.name)}</h4>
    ${notesHtml}
    <ul>${listItems}</ul>
  `;
}

/**
 * Initializes and binds the complete User Interface and Event Loop.
 * @param {Record<string, Object>} allLayers - Complete dictionary of layer maps
 * @param {Array<Array<Array<number>>>} grid - Geometry bounding grid
 * @param {ReturnType<createStore>} [store] - Reactive application store
 */
export function initUI(allLayers, grid, externalStore) {
  // ---------------------------------------------------------------------------
  // 1. Cached DOM Elements
  // ---------------------------------------------------------------------------
  const dom = {
    overlay: document.getElementById("overlay"),
    stageTitle: document.getElementById("stage-title"),
    stagePlatform: document.getElementById("stage-platform"),
    switchArrow: document.getElementById("switch-indicator-arrow"),
    switchText: document.getElementById("switch-indicator-text"),
    layerSidebarEl: document.getElementById("sidebar-layers"),
    mobilePickerEl: document.getElementById("mobile-layer-select"),
    modelSelectEl: document.getElementById("keyboard-select"),
    versionSelectEl: document.getElementById("version-select"),
    changelogBtn: document.getElementById("changelog-btn"),
    downloadLinkEl: document.getElementById("download-link"),
    downloadText: document.getElementById("download-text"),
    themeToggleEl: document.getElementById("theme-toggle"),
    themeToggleIcon: document.getElementById("theme-toggle-icon"),
    themeToggleLabel: document.getElementById("theme-toggle-label"),
    colorwaySlider: document.getElementById("colorway-slider"),
    colorwayLabel: document.getElementById("colorway-label"),
    colorwaySwatch: document.getElementById("colorway-swatch"),
    boardImg: document.getElementById("board-img"),
    stageCard: document.querySelector(".stage-card"),
    switchIndicator: document.querySelector(".switch-indicator"),
    boardWrap: document.querySelector(".board-wrap"),
    legend: document.querySelector(".legend"),
    infoPanel: document.getElementById("info-panel"),

    // Key Inspector Properties
    infoKey: document.getElementById("info-key"),
    infoPos: document.getElementById("info-pos"),
    infoTitle: document.getElementById("info-title"),
    infoContext: document.getElementById("info-context"),
    infoTap: document.getElementById("info-tap"),
    infoHold: document.getElementById("info-hold"),
    infoShift: document.getElementById("info-shift"),
    infoNote: document.getElementById("info-note"),

    // Firmware Action Matrix
    matrixTap: document.getElementById("matrix-tap"),
    matrixHold: document.getElementById("matrix-hold"),
    matrixDance: document.getElementById("matrix-dance"),
    matrixLong: document.getElementById("matrix-long"),
    matrixCombo: document.getElementById("matrix-combo"),
    powersStatus: document.getElementById("info-powers-status"),
    cellMatrixTap: document.getElementById("cell-matrix-tap"),
    cellMatrixHold: document.getElementById("cell-matrix-hold"),
    cellMatrixDance: document.getElementById("cell-matrix-dance"),
    cellMatrixLong: document.getElementById("cell-matrix-long"),
    cellMatrixCombo: document.getElementById("cell-matrix-combo"),

    // Changelog Modal
    changelogModal: document.getElementById("changelog-modal"),
    modalTitle: document.getElementById("modal-title"),
    modalBadge: document.getElementById("modal-badge"),
    modalBody: document.getElementById("modal-body"),
    modalMeta: document.getElementById("modal-meta"),
    modalCloseBtn: document.getElementById("modal-close-btn"),
    modalDismissBtn: document.getElementById("modal-dismiss-btn"),
  };

  // ---------------------------------------------------------------------------
  // 2. Initial State Setup (TASK-12, TASK-17)
  // ---------------------------------------------------------------------------
  const activeDocColorway = document.documentElement.getAttribute("data-colorway") || "space-gray";
  const initialColorwayIdx = Math.max(0, COLORWAYS.findIndex((c) => c.id === activeDocColorway));

  const urlHash = location.hash.replace("#", "");
  const defaultKeyboard = KEYBOARD_CATALOG[0];
  const initialLayerId = allLayers[urlHash]
    ? urlHash
    : defaultKeyboard?.defaultLayer || Object.keys(allLayers)[0] || "m_base";

  const store =
    externalStore ||
    createStore({
      keyboardId: defaultKeyboard.id,
      layerId: initialLayerId,
      releaseId: defaultKeyboard.releases?.[0]?.id || "",
      colorwayIndex: initialColorwayIdx,
      theme: localStorage.getItem("theme") || "light",
      selectedCoord: null,
      selectedCell: null,
      hoveredCoord: null,
      hoveredCell: null,
    });

  // ---------------------------------------------------------------------------
  // 3. View Renderers
  // ---------------------------------------------------------------------------

  function renderInspector(cell, activeLayer) {
    if (!dom.infoKey || !dom.infoTap || !dom.infoHold || !dom.infoShift || !dom.infoNote) return;

    const updateMatrix = (matrix) => {
      if (!dom.matrixTap) return;
      dom.matrixTap.textContent = matrix.tap;
      dom.matrixHold.textContent = matrix.hold;
      dom.matrixDance.textContent = matrix.dance;
      dom.matrixLong.textContent = matrix.long;
      dom.matrixCombo.innerHTML = matrix.combo;

      const setCellState = (cellEl, valEl, val) => {
        if (!cellEl || !valEl) return;
        const hasAction = Boolean(val && val !== "—");
        cellEl.classList.toggle("has-action", hasAction);
        valEl.classList.toggle("active", hasAction);
      };

      setCellState(dom.cellMatrixTap, dom.matrixTap, matrix.tap);
      setCellState(dom.cellMatrixHold, dom.matrixHold, matrix.hold);
      setCellState(dom.cellMatrixDance, dom.matrixDance, matrix.dance);
      setCellState(dom.cellMatrixLong, dom.matrixLong, matrix.long);
      setCellState(dom.cellMatrixCombo, dom.matrixCombo, matrix.combo);

      if (dom.powersStatus) {
        if (matrix.isSpecial) {
          dom.powersStatus.textContent = "✦ Superpowered Key";
          dom.powersStatus.classList.add("has-powers");
        } else {
          dom.powersStatus.textContent = "Standard Key";
          dom.powersStatus.classList.remove("has-powers");
        }
      }
    };

    if (!cell) {
      dom.infoKey.textContent = "—";
      if (dom.infoPos) dom.infoPos.textContent = "No Key Selected";
      if (dom.infoTitle) dom.infoTitle.textContent = "Key Inspector";
      if (dom.infoContext) dom.infoContext.textContent = "Hover or click any key";
      dom.infoTap.textContent = "—";
      dom.infoHold.textContent = "—";
      dom.infoShift.textContent = "—";
      dom.infoNote.innerHTML =
        "Hover or click any key on the keyboard to inspect its bindings and firmware behavior.";
      updateMatrix({
        tap: "—",
        hold: "—",
        dance: "—",
        long: "—",
        combo: "—",
        isSpecial: false,
      });
      return;
    }

    if (cell.trans) {
      dom.infoKey.textContent = "▼";
      if (dom.infoPos) dom.infoPos.textContent = "Transparent";
      if (dom.infoTitle) dom.infoTitle.textContent = "Pass-Through (&trans)";
      if (dom.infoContext) dom.infoContext.textContent = "Inherits Base Layer";
      dom.infoTap.innerHTML = "<code>&trans</code> Fallback";
      dom.infoHold.textContent = "—";
      dom.infoShift.textContent = "—";
      dom.infoNote.innerHTML =
        "Inherits whatever this physical position outputs on the <strong>Base Layer</strong>.";
      updateMatrix({
        tap: "Inherits Base Layer (&trans)",
        hold: "—",
        dance: "—",
        long: "—",
        combo: "—",
        isSpecial: false,
      });
      return;
    }

    dom.infoKey.textContent = cell.t || (cell.icon ? "★" : "—");
    if (dom.infoPos) dom.infoPos.textContent = getKeyRole(cell);
    const isLayerSwitch = cell.icon?.startsWith("layer-") || (cell.t && /^(?:⇒|=>)/.test(cell.t));
    if (isLayerSwitch && dom.infoTitle) {
      const match = (cell.t || "").match(/^(?:⇒|=>)\s*([A-Za-z0-9_-]+)/i);
      const targetName = match ? `${match[1]} Layer` : "Target Layer";
      dom.infoTitle.textContent = `Activate ${targetName}`;
    } else if (dom.infoTitle) {
      dom.infoTitle.textContent = cell.h ? `${cell.t || "Key"}  /  ${cell.h}` : (cell.t || "Key Binding");
    }

    if (dom.infoContext) {
      dom.infoContext.textContent = activeLayer
        ? `${activeLayer.name} (${activeLayer.platform.toUpperCase()})`
        : "Active Layer";
    }

    dom.infoTap.innerHTML = cell.t
      ? `<code>${escapeHtml(cell.t)}</code>`
      : (cell.icon ? `<code>#icon-${escapeHtml(cell.icon)}</code>` : "—");

    dom.infoHold.innerHTML = cell.h
      ? `<span class="info-chip hold">Hold: ${escapeHtml(cell.h)}</span>`
      : "—";

    dom.infoShift.innerHTML = cell.sh
      ? `<span class="info-chip shift">Shift: ${escapeHtml(cell.sh)}</span>`
      : "—";

    dom.infoNote.innerHTML = getKeyNote(cell);

    // Populate Firmware Action Matrix
    const matrix = resolveFirmwareMatrix(cell);
    updateMatrix(matrix);
  }

  function renderStage(state) {
    const { keyboardId, layerId } = state;
    const keyboard = KEYBOARD_CATALOG.find((k) => k.id === keyboardId);

    if (keyboardId === "b1pro") {
      const csStage = document.getElementById("coming-soon-stage");
      if (csStage) csStage.remove();

      [dom.switchIndicator, dom.boardWrap, dom.infoPanel, dom.legend, dom.stagePlatform].forEach(
        (el) => el && (el.style.display = "")
      );
      if (dom.colorwaySwatch) dom.colorwaySwatch.style.display = "";
      if (dom.colorwaySlider) dom.colorwaySlider.style.display = "";

      if (dom.layerSidebarEl && !dom.layerSidebarEl.querySelector(".layer-btn")) {
        populateSidebar();
      }

      const layer = allLayers[layerId];
      if (layer) {
        const iconId = getLayerIconId(layer);
        const layerDisplayName = layer.name.toLowerCase().endsWith("layer")
          ? layer.name
          : `${layer.name} Layer`;

        dom.stageTitle.innerHTML = `
          <span class="stage-title-icon" aria-hidden="true">
            <svg viewBox="0 0 24 24" width="42" height="42"><use href="#${iconId}"></use></svg>
          </span>
          <span class="stage-title-text">${String(layer.num).padStart(2, "0")} · ${escapeHtml(layerDisplayName)}</span>
        `;
        dom.stagePlatform.textContent = layer.platform === "mac" ? "macOS" : "Windows";

        // TASK-16: Derive switch indicator strictly from platform, eliminating magic numbers
        const isWindowsMode = layer.platform === "win";
        dom.switchArrow.textContent = isWindowsMode ? "⚙" : "";
        dom.switchText.textContent = isWindowsMode
          ? "Physical Win/Mac switch set to: WIN"
          : "Physical Win/Mac switch set to: MAC";

        // Render Layer with DocumentFragment batching
        renderLayer(layer, grid, dom.overlay, {
          selectedCoord: state.selectedCoord,
        });

        // Sync Sidebar & Mobile Picker
        document.querySelectorAll(".layer-btn").forEach((btn) => {
          btn.classList.toggle("active", btn.dataset.layer === layerId);
        });
        if (dom.mobilePickerEl) dom.mobilePickerEl.value = layerId;

        if (history.replaceState) {
          history.replaceState(null, "", `#${layerId}`);
        }
      }
    } else {
      // Coming soon stage for non-b1pro targets
      [dom.switchIndicator, dom.boardWrap, dom.infoPanel, dom.legend].forEach(
        (el) => el && (el.style.display = "none")
      );
      if (dom.colorwaySwatch) dom.colorwaySwatch.style.display = "none";
      if (dom.colorwaySlider) dom.colorwaySlider.style.display = "none";

      if (dom.layerSidebarEl) {
        dom.layerSidebarEl.innerHTML = `
          <div class="sidebar-header">
            <svg class="sidebar-header-icon" viewBox="0 0 24 24" width="22" height="22" aria-hidden="true">
              <use href="#icon-layers-stack"></use>
            </svg>
            <span class="sidebar-header-title">Layers</span>
          </div>
          <div class="sidebar-group">
            <div class="sidebar-group-label">Catalog</div>
            <div class="sidebar-coming-soon">Coming Soon</div>
          </div>
        `;
      }

      if (dom.stageTitle) dom.stageTitle.textContent = "Coming Soon";
      if (dom.stagePlatform) {
        dom.stagePlatform.textContent = keyboard ? keyboard.firmware : "Coming Soon";
        dom.stagePlatform.style.display = "";
      }

      let csStage = document.getElementById("coming-soon-stage");
      if (!csStage && dom.stageCard) {
        csStage = document.createElement("div");
        csStage.id = "coming-soon-stage";
        csStage.className = "coming-soon-stage";
        dom.stageCard.appendChild(csStage);
      }

      if (csStage) {
        csStage.style.display = "block";
        csStage.innerHTML = `
          <div class="coming-soon-card">
            <span class="coming-soon-badge">Coming Soon</span>
            <h3>${keyboard ? keyboard.name : "Keyboard"} (${keyboard ? keyboard.firmware : ""})</h3>
            <p>Interactive keymap visualization and firmware assets for this keyboard target are currently under development.</p>
          </div>
        `;
      }
    }
  }

  function renderReleaseUI(release) {
    if (!dom.downloadLinkEl) return;
    const hasDownload = Boolean(release?.zipUrl && release.zipUrl !== "#");

    if (hasDownload) {
      dom.downloadLinkEl.href = release.zipUrl;
      dom.downloadLinkEl.style.display = "inline-flex";
      if (dom.downloadText) {
        dom.downloadText.textContent = `Download ${release.version} (.zip)`;
      } else {
        dom.downloadLinkEl.textContent = `⬇ Download ${release.version} (.zip)`;
      }
    } else {
      dom.downloadLinkEl.style.display = "none";
    }
  }

  function renderColorway(idx) {
    const cw = COLORWAYS[idx];
    if (!cw) return;
    document.documentElement.setAttribute("data-colorway", cw.id);
    if (dom.colorwayLabel) dom.colorwayLabel.textContent = cw.name;
    if (dom.colorwaySwatch) dom.colorwaySwatch.style.setProperty("--colorway-swatch-color", cw.swatch);
    if (dom.boardImg) dom.boardImg.src = cw.file;
    if (dom.colorwaySlider && dom.colorwaySlider.value !== String(idx)) {
      dom.colorwaySlider.value = String(idx);
    }
  }

  function renderTheme(theme) {
    document.documentElement.setAttribute("data-theme", theme);
    const isDark = theme === "dark";
    if (dom.themeToggleIcon) dom.themeToggleIcon.textContent = isDark ? "☀️" : "🌙";
    if (dom.themeToggleLabel) dom.themeToggleLabel.textContent = isDark ? "Light" : "Dark";
  }

  // ---------------------------------------------------------------------------
  // 4. Modal Handlers
  // ---------------------------------------------------------------------------

  function openChangelogModal() {
    const state = store.getState();
    const keyboard = KEYBOARD_CATALOG.find((k) => k.id === state.keyboardId);
    if (!keyboard) return;

    const release = keyboard.releases?.find((r) => r.id === state.releaseId) || keyboard.releases?.[0] || null;

    if (dom.modalTitle) {
      dom.modalTitle.textContent = release ? `${keyboard.name} · ${release.version}` : `${keyboard.name} Changelog`;
    }
    if (dom.modalBadge) {
      dom.modalBadge.textContent = release ? release.badge : keyboard.firmware;
    }
    if (dom.modalMeta) {
      dom.modalMeta.textContent = release
        ? `Release Date: ${release.date} · Status: ${release.status.toUpperCase()}`
        : "";
    }
    if (dom.modalBody) {
      dom.modalBody.innerHTML = formatChangelogHtml(release);
    }

    if (dom.changelogModal) {
      dom.changelogModal.classList.add("open");
      dom.changelogModal.setAttribute("aria-hidden", "false");
    }
  }

  function closeChangelogModal() {
    if (dom.changelogModal) {
      dom.changelogModal.classList.remove("open");
      dom.changelogModal.setAttribute("aria-hidden", "true");
    }
  }

  // ---------------------------------------------------------------------------
  // 5. Reactive Store Subscriptions
  // ---------------------------------------------------------------------------

  store.subscribe((state, prevState) => {
    // Stage or Layer Change
    if (
      state.layerId !== prevState.layerId ||
      state.keyboardId !== prevState.keyboardId ||
      state.selectedCoord !== prevState.selectedCoord
    ) {
      renderStage(state);
    }

    // Info Inspector State Machine (Hover > Selected > Default)
    if (
      state.hoveredCell !== prevState.hoveredCell ||
      state.selectedCell !== prevState.selectedCell ||
      state.layerId !== prevState.layerId
    ) {
      const activeCell = state.hoveredCell ?? state.selectedCell ?? null;
      const activeLayer = allLayers[state.layerId];
      renderInspector(activeCell, activeLayer);
    }

    // Colorway Change
    if (state.colorwayIndex !== prevState.colorwayIndex) {
      renderColorway(state.colorwayIndex);
    }

    // Theme Change
    if (state.theme !== prevState.theme) {
      renderTheme(state.theme);
    }

    // Selection Highlighting Toggle
    if (state.selectedCoord !== prevState.selectedCoord) {
      dom.overlay?.querySelectorAll(".key.selected").forEach((el) => {
        el.classList.remove("selected");
      });

      if (state.selectedCoord) {
        const { row, col, sub } = state.selectedCoord;
        const selector =
          sub !== undefined
            ? `.key[data-row="${row}"][data-col="${col}"][data-sub="${sub}"]`
            : `.key[data-row="${row}"][data-col="${col}"]`;
        const targetEl = dom.overlay?.querySelector(selector);
        targetEl?.classList.add("selected");
      }
    }
  });

  // ---------------------------------------------------------------------------
  // 6. Event Wiring & Bootstrap
  // ---------------------------------------------------------------------------

  // Delegated Key Canvas Events (TASK-20)
  if (dom.overlay) {
    attachDelegatedEvents(dom.overlay, {
      onHover({ row, col, sub }) {
        const activeLayer = allLayers[store.getState().layerId];
        const cell = getCellFromLayer(activeLayer, row, col, sub);
        store.setState({ hoveredCoord: { row, col, sub }, hoveredCell: cell });
      },
      onLeave() {
        store.setState({ hoveredCoord: null, hoveredCell: null });
      },
      onSelect({ row, col, sub }) {
        const state = store.getState();
        const isSame =
          state.selectedCoord &&
          state.selectedCoord.row === row &&
          state.selectedCoord.col === col &&
          state.selectedCoord.sub === sub;

        if (isSame) {
          store.setState({ selectedCoord: null, selectedCell: null });
        } else {
          const activeLayer = allLayers[state.layerId];
          const cell = getCellFromLayer(activeLayer, row, col, sub);
          store.setState({
            selectedCoord: { row, col, sub },
            selectedCell: cell,
          });
        }
      },
    });
  }

  // Document-wide click deselect
  document.addEventListener("click", (e) => {
    if (!e.target.closest(".key")) {
      const state = store.getState();
      if (state.selectedCoord !== null) {
        store.setState({ selectedCoord: null, selectedCell: null });
      }
    }
  });

  // Colorway Slider Event
  if (dom.colorwaySlider) {
    dom.colorwaySlider.addEventListener("input", (e) => {
      store.setState({ colorwayIndex: parseInt(e.target.value, 10) });
    });
  }

  // Theme Toggle
  if (dom.themeToggleEl) {
    dom.themeToggleEl.addEventListener("click", () => {
      const current = store.getState().theme;
      const nextTheme = current === "dark" ? "light" : "dark";
      localStorage.setItem("theme", nextTheme);
      store.setState({ theme: nextTheme });
    });
  }

  // Populate Catalogs & Selectors
  function populateVersionSelector(keyboardId) {
    if (!dom.versionSelectEl) return;
    dom.versionSelectEl.innerHTML = "";

    const keyboard = KEYBOARD_CATALOG.find((k) => k.id === keyboardId);
    const releases = keyboard?.releases || [];

    if (!releases.length) {
      const opt = document.createElement("option");
      opt.value = "";
      opt.textContent = "Coming Soon";
      dom.versionSelectEl.appendChild(opt);
      dom.versionSelectEl.disabled = true;
      renderReleaseUI(null);
      return;
    }

    dom.versionSelectEl.disabled = false;
    const defaultRel = releases.find((r) => r.default) || releases[0];

    releases.forEach((rel) => {
      const opt = document.createElement("option");
      opt.value = rel.id;
      opt.textContent = rel.name;
      if (rel.id === defaultRel.id) opt.selected = true;
      dom.versionSelectEl.appendChild(opt);
    });

    store.setState({ releaseId: defaultRel.id });
    renderReleaseUI(defaultRel); // TASK-13: Immediately initialize download link on load
  }

  if (dom.modelSelectEl) {
    dom.modelSelectEl.innerHTML = "";
    KEYBOARD_CATALOG.forEach((item) => {
      const opt = document.createElement("option");
      opt.value = item.id;
      opt.textContent = `${item.name} (${item.firmware})`;
      dom.modelSelectEl.appendChild(opt);
    });

    dom.modelSelectEl.addEventListener("change", (e) => {
      const keyboardId = e.target.value;
      store.setState({ keyboardId });
      populateVersionSelector(keyboardId);
    });
  }

  if (dom.versionSelectEl) {
    dom.versionSelectEl.addEventListener("change", (e) => {
      const releaseId = e.target.value;
      const keyboard = KEYBOARD_CATALOG.find((k) => k.id === store.getState().keyboardId);
      const rel = keyboard?.releases?.find((r) => r.id === releaseId) || null;
      store.setState({ releaseId });
      renderReleaseUI(rel);
    });
  }

  // Resolve corresponding isometric layer icon symbol
  function getLayerIconId(layer) {
    const name = (layer.name || "").toLowerCase();
    if (name.includes("sym")) return "icon-layer-symbols";
    if (name.includes("nav")) return "icon-layer-navigation";
    if (name.includes("func")) return "icon-layer-function";
    if (name.includes("macro")) return "icon-layer-macro";
    if (name.includes("reserved") || name.includes("uat")) return "icon-layer-reserved";
    return "icon-layer-base";
  }

  // Populate Layer Sidebar & Header with Multi-Colored Isometric Stack
  function populateSidebar() {
    if (!dom.layerSidebarEl) return;
    dom.layerSidebarEl.innerHTML = "";

    // Sidebar Nav Header with Universal Multi-Colored Isometric Stack
    const headerEl = document.createElement("div");
    headerEl.className = "sidebar-header";
    headerEl.innerHTML = `
      <svg class="sidebar-header-icon" viewBox="0 0 24 24" width="22" height="22" aria-hidden="true">
        <use href="#icon-layers-stack"></use>
      </svg>
      <span class="sidebar-header-title">Layers</span>
    `;
    dom.layerSidebarEl.appendChild(headerEl);

    const groups = [
      { name: "macOS", platform: "mac" },
      { name: "Windows", platform: "win" },
    ];

    groups.forEach((group) => {
      const groupEl = document.createElement("div");
      groupEl.className = "sidebar-group";

      const labelEl = document.createElement("div");
      labelEl.className = "sidebar-group-label";
      labelEl.textContent = group.name;
      groupEl.appendChild(labelEl);

      Object.keys(allLayers)
        .filter((id) => allLayers[id].platform === group.platform)
        .forEach((id) => {
          const layer = allLayers[id];
          const btn = document.createElement("button");
          btn.className = "layer-btn";
          btn.type = "button";
          btn.dataset.layer = id;
          btn.innerHTML = `<span class="num">${String(layer.num).padStart(2, "0")}</span><span>${escapeHtml(layer.name)}</span>`;
          btn.addEventListener("click", () => {
            store.setState({ layerId: id, selectedCoord: null, selectedCell: null });
          });
          groupEl.appendChild(btn);
        });

      dom.layerSidebarEl.appendChild(groupEl);
    });
  }

  populateSidebar();

  if (dom.mobilePickerEl) {
    dom.mobilePickerEl.innerHTML = "";
    Object.keys(allLayers).forEach((id) => {
      const layer = allLayers[id];
      const opt = document.createElement("option");
      opt.value = id;
      opt.textContent = `${layer.num} · ${layer.platform.toUpperCase()} ${layer.name}`;
      dom.mobilePickerEl.appendChild(opt);
    });
    dom.mobilePickerEl.addEventListener("change", (e) => {
      store.setState({ layerId: e.target.value, selectedCoord: null, selectedCell: null });
    });
  }

  // Modal Listeners
  if (dom.changelogBtn) dom.changelogBtn.addEventListener("click", openChangelogModal);
  if (dom.modalCloseBtn) dom.modalCloseBtn.addEventListener("click", closeChangelogModal);
  if (dom.modalDismissBtn) dom.modalDismissBtn.addEventListener("click", closeChangelogModal);
  if (dom.changelogModal) {
    dom.changelogModal.addEventListener("click", (e) => {
      if (e.target === dom.changelogModal) closeChangelogModal();
    });
  }
  document.addEventListener("keydown", (e) => {
    if (e.key === "Escape" && dom.changelogModal?.classList.contains("open")) {
      closeChangelogModal();
    }
  });

  // Initial Sync
  populateVersionSelector(store.getState().keyboardId);
  renderColorway(store.getState().colorwayIndex);
  renderTheme(store.getState().theme);
  renderStage(store.getState());
  renderInspector(null, allLayers[store.getState().layerId]);

  return store;
}
