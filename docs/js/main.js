/* ======================================================================
   VT Keymaps - Main Application Bootstrap & ES Module Entry Point
   ====================================================================== */
import { GRID_B1PRO } from "./config/grid_b1pro.js";
import { B1PRO_MAC_LAYERS } from "./layers/b1pro_mac_layers.js";
import { B1PRO_WIN_LAYERS } from "./layers/b1pro_win_layers.js";
import { initUI } from "./engine/ui.js";

/**
 * Merged B1 Pro layers catalog.
 */
const ALL_B1PRO_LAYERS = Object.freeze({
  ...B1PRO_MAC_LAYERS,
  ...B1PRO_WIN_LAYERS,
});

// Bootstrap application on DOM ready
document.addEventListener("DOMContentLoaded", () => {
  initUI(ALL_B1PRO_LAYERS, GRID_B1PRO);
});
