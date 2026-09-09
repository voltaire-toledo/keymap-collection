/* ======================================================================
   Multi-Keyboard & Firmware Release Catalog Configuration
   ====================================================================== */
export const KEYBOARD_CATALOG = [
  {
    id: "b1pro",
    name: "Keychron B1 Pro",
    firmware: "ZMK",
    layout: "ANSI 75%",
    repoUrl: "https://github.com/voltaire-toledo/Keychron-B1-Pro-Custom-Keymap",
    latestRelease: "v0.24",
    uf2Url: "https://github.com/voltaire-toledo/Keychron-B1-Pro-Custom-Keymap/releases/download/v0.24/v0.24-sync-layers.uf2",
    svgAssetDir: "assets/b1pro",
  },
  {
    id: "v1max",
    name: "Keychron V1 Max",
    firmware: "QMK",
    layout: "ANSI 75%",
    repoUrl: "https://github.com/voltaire-toledo/VT-QMK-V1Max",
    latestRelease: "v1.0",
    uf2Url: "",
    svgAssetDir: "assets/v1max",
  },
  {
    id: "kanata",
    name: "Kanata Portable Layer",
    firmware: "Kanata",
    layout: "ANSI / OS Native",
    repoUrl: "https://github.com/voltaire-toledo/kanata",
    latestRelease: "v1.0",
    uf2Url: "",
    svgAssetDir: "assets/kanata",
  },
];
