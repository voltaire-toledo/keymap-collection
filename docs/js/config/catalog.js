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
    svgAssetDir: "assets/b1pro",
    defaultLayer: "m_base",
    releases: [
      {
        id: "v0.25.1",
        version: "v0.25.1",
        name: "v0.25.1 (Upcoming Candidate)",
        badge: "Candidate",
        status: "upcoming",
        date: "2026-09-17",
        zipUrl: "https://github.com/voltaire-toledo/Keychron-B1-Pro-Custom-Keymap/releases/download/v0.25.1/v0.25.1-candidate.zip",
        zipFilename: "v0.25.1-candidate.zip",
        notes: "Candidate built and linted; awaiting final hardware approval.",
        changelog: [
          "**Status:** Keymap linted & compiled (`v0.25.1-build_5.uf2`); pending hardware QC.",
          "**HRM Retro-Tap:** Enabled retro-tap on homerow mods (lhm and rhm) so releasing held mods without chording emits the tap character.",
          "**HRM Positional Triggers:** Fixed row-boundary index shifts in hold-trigger-key-positions.",
          "**HRM Timing Relaxations:** Relaxed timings by +75ms (tapping-term-ms = 275ms, quick-tap-ms = 250ms).",
          "**NAV Layer Spacebar:** Bound pos 53 in NAV1 (Layers 3 & 9) to &trans for seamless repeat.",
          "**Battery Indicator:** Directly bound key B on Function layers (1 & 7) to &out OUT_BAT.",
          "**Windows GPU Reset:** Bound key V on Win Func layer (Layer 7) to Win + Ctrl + Shift + B.",
          "**LED Notifications:** Pulsing layer hold patterns: White (Func), Green (Symbol), Purple (Nav), Orange (Macro); double Cyan flash for Windows Key Lock."
        ]
      },
      {
        id: "v0.25",
        version: "v0.25",
        name: "v0.25 (Unified - Recalled)",
        badge: "Recalled",
        status: "recalled",
        date: "2026-09-08",
        zipUrl: "https://github.com/voltaire-toledo/Keychron-B1-Pro-Custom-Keymap/releases/download/v0.25/v0.25-unified.zip",
        zipFilename: "v0.25-unified.zip",
        notes: "Recalled on 2026-09-12 due to timing sensitivities; superseded by v0.25.1.",
        changelog: [
          "**Notice:** Recalled on 2026-09-12; please use v0.24 (stable) or upcoming v0.25.1.",
          "**BASE Layers (0 & 6):** Fn key: 1-tap TG(FUNC), 2-tap OSL(MCRO), Hold MO(FUNC).",
          "**FUNC Layers (1 & 7):** Esc pos 0 tap returns to Base, 3s hold toggles F-row exchange (&change).",
          "**SYMBOL Layers (2 & 8):** Fn row set to F1-F12, Num row to F13-F24. Opening brackets < [ { ( on ASDF; closing brackets and auto-closing macros on JKLM.",
          "**Combos:** Fn + J + Z (3s hold) for factory recovery reset; Fn + B for battery LED; Fn + Win (3s hold) for Win Key Lock."
        ]
      },
      {
        id: "v0.24",
        version: "v0.24",
        name: "v0.24 (Sync Layers - Active)",
        badge: "Active",
        status: "active",
        default: true,
        date: "2026-08-11",
        zipUrl: "https://github.com/voltaire-toledo/Keychron-B1-Pro-Custom-Keymap/releases/download/v0.24/v0.24-sync-layers.zip",
        zipFilename: "v0.24-sync-layers.zip",
        notes: "Current stable production firmware with passed hardware QC.",
        changelog: [
          "**Status:** Hardware QC PASSED on 2026-08-11. Active stable keymap release.",
          "**Mac Function-Row Swap:** F1–F12 moved to Fn layer; Mac media and brightness controls placed on Base layer.",
          "**Mac Shortcuts:** Cmd+W on pos 30; Cmd+C and Cmd+V tap-dances; Ctrl+Cmd+Space emoji picker on R_SHIFT.",
          "**Windows Base Modifiers:** Corrected Win / Alt modifier order and positions across Windows base.",
          "**Windows Function Layer:** Mapped Task View (Win+Tab), File Explorer (Win+E), Lock PC (Win+L), and direct Bluetooth/2.4G profile toggles."
        ]
      },
      {
        id: "factory",
        version: "factory",
        name: "Factory (Keychron v1.0.3)",
        badge: "Factory OEM",
        status: "factory",
        date: "2024-05-15",
        zipUrl: "https://github.com/voltaire-toledo/Keychron-B1-Pro-Custom-Keymap/releases/download/factory/zmk_b1pro_us_v1.0.3.zip",
        zipFilename: "zmk_b1pro_us_v1.0.3.zip",
        notes: "Original out-of-the-box factory firmware baseline from Keychron.",
        changelog: [
          "**Status:** Official Keychron Factory OEM Firmware (v1.0.3).",
          "Standard Keychron 4-layer ANSI configuration (Mac Base, Mac Fn, Win Base, Win Fn).",
          "Factory Bluetooth channels 1–3 and 2.4GHz dongle pairing profiles.",
          "Standard media function key assignments on F-row."
        ]
      }
    ]
  },
  {
    id: "v1max",
    name: "Keychron V1 Max",
    firmware: "QMK",
    layout: "ANSI 75%",
    repoUrl: "https://github.com/voltaire-toledo/VT-QMK-V1Max",
    svgAssetDir: "assets/v1max",
    releases: [
      {
        id: "rev1",
        version: "Rev 1",
        name: "Rev 1 (Mello Keymap - 12 Layers)",
        badge: "Pending QC",
        status: "upcoming",
        date: "2026-08-20",
        zipUrl: "#",
        zipFilename: "keychron_v1_max_ansi_encoder_mello.zip",
        notes: "Candidate built for STM32; hardware QC pending.",
        changelog: [
          "Candidate Identity: Keychron V1 Max ANSI Encoder (Mello Keymap - 12 Layers).",
          "Target Unit: Keychron V1 Max (STM32 DFU).",
          "Artwork & layout pending asset permissions."
        ]
      }
    ]
  },
  {
    id: "kanata",
    name: "Kanata Portable Layer",
    firmware: "Kanata",
    layout: "ANSI / OS Native",
    repoUrl: "https://github.com/voltaire-toledo/kanata",
    svgAssetDir: "assets/kanata",
    releases: [
      {
        id: "v1.0",
        version: "v1.0",
        name: "v1.0 (Portable Configuration)",
        badge: "In Development",
        status: "upcoming",
        date: "2026-09-01",
        zipUrl: "#",
        zipFilename: "kanata-mello-config.zip",
        notes: "Cross-platform daemon configuration.",
        changelog: [
          "Cross-platform OS-level interceptor keymap.",
          "Matches B1 Pro homerow-mod and navigation layer conventions."
        ]
      }
    ]
  }
];
