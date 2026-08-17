/* ======================================================================
   UI Engine & Interactivity Handlers
   ====================================================================== */
import { KEYBOARD_CATALOG } from "../config/catalog.js";
import { renderLayer } from "./renderer.js";

export function initUI(allLayers, grid, state) {
  const overlay = document.getElementById("overlay");
  const infoKey = document.getElementById("info-key");
  const infoNote = document.getElementById("info-note");
  const stageTitle = document.getElementById("stage-title");
  const stagePlatform = document.getElementById("stage-platform");
  const switchArrow = document.getElementById("switch-indicator-arrow");
  const switchText = document.getElementById("switch-indicator-text");
  const layerSidebarEl = document.getElementById("sidebar-layers");
  const mobilePickerEl = document.getElementById("mobile-layer-select");
  const modelSelectEl = document.getElementById("keyboard-select");
  const downloadLinkEl = document.getElementById("download-link");
  const themeToggleEl = document.getElementById("theme-toggle");
  const colorwaySlider = document.getElementById("colorway-slider");
  const colorwayLabel = document.getElementById("colorway-label");
  const colorwaySwatch = document.getElementById("colorway-swatch");
  const boardImg = document.getElementById("board-img");

  const COLORWAYS = [
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
      file: "assets/b1pro/KcB1Pro_ANSI_PI.svg",
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
      file: "assets/b1pro/Keychron_B1_Pro_ANSI.svg",
    },
  ];

  function showInfo(cell) {
    if (!cell || cell === null) {
      infoKey.textContent = "—";
      infoNote.innerHTML = "In use while in layer.";
      return;
    }
    if (cell.trans) {
      infoKey.textContent = "▼";
      infoNote.innerHTML =
        "Inherits whatever this position does on the <strong>Base Layer</strong>.";
      return;
    }
    infoKey.textContent = cell.t;
    let parts = [];
    if (cell.h) parts.push(`<span class="info-chip hold">Hold: ${cell.h}</span>`);
    if (cell.sh) parts.push(`<span class="info-chip shift">Shift → ${cell.sh}</span>`);
    if (cell.n) parts.push(`<span>${cell.n}</span>`);
    infoNote.innerHTML = parts.length
      ? parts.join(" &nbsp; ")
      : `Tap action on this layer.`;
  }

  function updateLayerUI(id) {
    const layer = allLayers[id];
    if (!layer) return;

    stageTitle.textContent = `${String(layer.num).padStart(2, "0")} · ${layer.name}`;
    stagePlatform.textContent = layer.platform === "mac" ? "macOS" : "Windows";

    if (layer.num >= 6) {
      switchArrow.textContent = "⚙";
      switchText.textContent = "Physical Win/Mac switch set to: WIN";
    } else {
      switchArrow.textContent = "";
      switchText.textContent = "Physical Win/Mac switch set to: MAC";
    }

    renderLayer(id, allLayers, grid, overlay, showInfo, state);

    document.querySelectorAll(".layer-btn").forEach((btn) => {
      btn.classList.toggle("active", btn.dataset.layer === id);
    });
    if (mobilePickerEl) mobilePickerEl.value = id;

    if (history.replaceState) {
      history.replaceState(null, "", "#" + id);
    }
  }

  function populateSidebar() {
    if (!layerSidebarEl) return;
    layerSidebarEl.innerHTML = "";

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
          btn.innerHTML = `<span class="num">${String(layer.num).padStart(2, "0")}</span><span>${layer.name}</span>`;
          btn.addEventListener("click", () => updateLayerUI(id));
          groupEl.appendChild(btn);
        });

      layerSidebarEl.appendChild(groupEl);
    });
  }

  function populateMobilePicker() {
    if (!mobilePickerEl) return;
    mobilePickerEl.innerHTML = "";
    Object.keys(allLayers).forEach((id) => {
      const layer = allLayers[id];
      const opt = document.createElement("option");
      opt.value = id;
      opt.textContent = `${layer.num} · ${layer.platform.toUpperCase()} ${layer.name}`;
      mobilePickerEl.appendChild(opt);
    });
    mobilePickerEl.addEventListener("change", (e) => updateLayerUI(e.target.value));
  }

  function populateModelCatalog() {
    if (!modelSelectEl) return;
    modelSelectEl.innerHTML = "";
    KEYBOARD_CATALOG.forEach((item) => {
      const opt = document.createElement("option");
      opt.value = item.id;
      opt.textContent = `${item.name} (${item.firmware})`;
      modelSelectEl.appendChild(opt);
    });

    modelSelectEl.addEventListener("change", (e) => {
      const item = KEYBOARD_CATALOG.find((k) => k.id === e.target.value);
      if (item && downloadLinkEl) {
        if (item.uf2Url) {
          downloadLinkEl.href = item.uf2Url;
          downloadLinkEl.style.display = "inline-flex";
          downloadLinkEl.textContent = `⬇ Firmware ${item.latestRelease}`;
        } else {
          downloadLinkEl.style.display = "none";
        }
      }
    });
  }

  function initColorways() {
    function setColorway(idx) {
      const cw = COLORWAYS[idx];
      if (!cw) return;
      document.documentElement.setAttribute("data-colorway", cw.id);
      if (colorwayLabel) colorwayLabel.textContent = cw.name;
      if (colorwaySwatch) colorwaySwatch.style.setProperty("--colorway-swatch-color", cw.swatch);
      if (boardImg) boardImg.src = cw.file;

      document.querySelectorAll(".colorway-ticks-labels span").forEach((sp, i) => {
        sp.classList.toggle("active", i === idx);
      });
    }

    if (colorwaySlider) {
      colorwaySlider.addEventListener("input", (e) => setColorway(parseInt(e.target.value, 10)));
      setColorway(parseInt(colorwaySlider.value, 10));
    }
  }

  function initThemeToggle() {
    if (!themeToggleEl) return;

    function applyTheme(theme) {
      document.documentElement.setAttribute("data-theme", theme);
      const icon = document.getElementById("theme-toggle-icon");
      const label = document.getElementById("theme-toggle-label");
      if (theme === "dark") {
        if (icon) icon.textContent = "☀️";
        if (label) label.textContent = "Light";
      } else {
        if (icon) icon.textContent = "🌙";
        if (label) label.textContent = "Dark";
      }
    }

    let theme = localStorage.getItem("theme") || "light";
    applyTheme(theme);

    themeToggleEl.addEventListener("click", () => {
      theme = theme === "dark" ? "light" : "dark";
      localStorage.setItem("theme", theme);
      applyTheme(theme);
    });
  }

  populateSidebar();
  populateMobilePicker();
  populateModelCatalog();
  initColorways();
  initThemeToggle();

  document.addEventListener("click", (e) => {
    if (!e.target.closest(".key") && state.selectedKey) {
      state.selectedKey.classList.remove("selected");
      state.selectedKey = null;
    }
  });

  const hashId = location.hash.replace("#", "");
  const initialLayer = allLayers[hashId] ? hashId : "m_base";
  updateLayerUI(initialLayer);
}
