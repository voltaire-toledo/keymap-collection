# Kanata Configuration Manifest - April 19, 2026

This manifest tracks the iterative refinements made to the `kanata.kbd` configuration over the last 4 hours to achieve a balance between fast typing and reliable Home Row Mod behavior.

## 🛠 Core Layout Changes
- **Modifier Layout:** Finalized as **CGAS** (Control, GUI/Cmd, Alt, Shift).
- **Home Row Keys:** 
  - Left: A (Ctrl), S (Cmd), D (Alt), F (Shift)
  - Right: J (Shift), K (Alt), L (Cmd), ; (Ctrl)

## ⏱ Data-Driven Timing Calibration
Based on physical profiling of the user's typing rhythm (Averages ~130-160ms, Max linger ~248ms), the following timeouts were established:

| Key | Modifier | Tap Timeout | Hold Timeout |
| :--- | :--- | :--- | :--- |
| **A / ;** | **Ctrl** | 150ms | **325ms** |
| **S / L** | **Cmd** | 150ms | **275ms** |
| **D / K** | **Alt** | 150ms | **225ms** |
| **F / J** | **Shift** | 150ms | **195ms** |

## 🛡 Stability & Reliability Features
- **Bilateral Enforcement:** Restored `tap-hold-release-keys`. Modifiers only trigger when a key on the *opposite* hand is pressed, preventing same-hand rolling errors (e.g., "ask").
- **Bypass Toggle:** Implemented a persistent toggle for "Normal Mode."
  - **Trigger:** Tap **Right Command** key.
  - **Function:** Flips between `base` (Kanata) and `bypass` (Standard) layers.
- **Emergency Exit:** `LControl + Space + Escape` kills the process for an immediate system-managed restart.

## ⌨️ Typing Optimization (Ghost Key Fixes)
- **Spacebar Interference:** Removed `spc` from bilateral trigger lists. Fast typing like "lads fall" no longer results in "ladsfall".
- **Rolling-Safe Triggers:** Removed `n` and `m` from the right-hand trigger list. This allows natural rolls for words like "an", "many", and "example" without accidental modifier activation.
- **Permissive Holds:** (Ongoing) Investigating "flushing" behavior for long-held home row keys to ensure characters are never swallowed.

## 🚀 Future Roadmap: "Kanata Tuner"
- **Project Goal:** Build an application wrapper that allows users to iteratively tune their Home Row Mod timings through guided typing tests (similar to our Swift profiler).
- **Features:** 
  - Automated "Sensation" testing (identifying rolls vs. holds).
  - Visual dashboard for Avg/Max hold times per finger.
  - One-click "Apply & Restart" for `.kbd` files.
  - "Training Wheels" mode: Gradually tightens timeouts as user accuracy improves.

## 📂 File & Service Management
- **Service Control:** Migrated from LaunchDaemon to LaunchAgent running `kanata-tray` to enable GUI tray feedback and prevent boot-time race conditions. Controls are managed via user-domain `bootstrap` and `bootout` commands.
- **Manifest Location:** `~/CODE/dotfiles/kanata/config_manifest.md`
