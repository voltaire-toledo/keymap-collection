# Antigravity Kanata Configuration Log (AGY.md)

This log tracks the specifications, configuration state, historical design choices, discrepancies, and task lists for the Kanata keyboard customizer setup on macOS. It is updated by Antigravity (AGY) to coordinate with the user and other agents.

---

## 🖥️ System & Environment Specifications

- **OS:** macOS (Apple Silicon/Intel)
- **Primary Keyboard:** MacBook Internal Keyboard + Keychron V1 Max
- **Virtual Driver:** Karabiner DriverKit VirtualHIDDevice (`org.pqrs.Karabiner-DriverKit-VirtualHIDDevice`)
- **Kanata Path:** `/opt/homebrew/bin/kanata`
- **Configuration Path:** `~/CODE/dotfiles/kanata/kanata.kbd`
- **Service Management:** 
  - Core service managed by `kanata-tray` (runs in user space, executes `kanata` via a passwordless `sudo` wrapper).
  - LaunchAgent file exists at `~/Library/LaunchAgents/com.kanata.tray.plist` (automates starting the system tray icon on user login).
- **Log Files:**
  - Standard system logging for the LaunchAgent, or standard output when running `kanata-tray` manually.

---

## ⌨️ Layout & Configuration Analysis (`kanata.kbd`)

The configuration is built around **Home Row Mods (HRM)** with **Bilateral Enforcement** and a dedicated **Navigation Layer** mapped to Caps Lock.

### 1. Home Row Mods (CGAS)
Home row modifiers are configured with `tap-hold-release-keys`. A modifier only triggers if a key on the **opposite** hand is pressed and released while the home row key is held. This prevents accidental modifier activation during same-hand rolls (e.g., typing "ask").

```
Left Hand (Pinky -> Index):       Right Hand (Index -> Pinky):
  A    -> Control (lctl)            J    -> Shift (rsft)
  S    -> Command (lmet)            K    -> Alt (ralt)
  D    -> Alt (lalt)                L    -> Command (lmet)
  F    -> Shift (lsft)              ;    -> Control (rctl)
```

#### Modifiers Timings & Calibration
Timings are optimized based on the physical dexterity and speed of different fingers:

| Key(s) | Finger | Modifier | Tap Timeout | Hold Timeout | Logic |
| :--- | :--- | :--- | :---: | :---: | :--- |
| `F` / `J` | Index | Shift | 225ms | 175ms | **Fast/Agile:** Shorter hold timeout for rapid capitalization. |
| `D` / `K` | Middle | Alt | 320ms | 300ms | **Powerhouse:** standard typing speeds. |
| `S` / `L` | Ring | Cmd | 320ms | 300ms | **Strong:** standard typing speeds. |
| `A` / `;` | Pinky | Ctrl | 320ms | 300ms | **Anchor:** standard typing speeds. |

### 2. Base Layer Layout
- **Caps Lock:** Dual-function. Tapping sends `Escape`; holding activates the **Hyper Key modifier** (`Ctrl+Cmd+Alt+Shift`), useful for mapping global shortcuts in terminal emulators (Ghostty/iTerm2) and tools like Raycast.
- **Spacebar:** Dual-function. Tapping sends standard `Space`; holding activates the **Navigation Layer** (using standard `tap-hold` with a `175ms` delay to prevent swallowed spaces or roll misfires during fast typing).
- **Fn Key:** Toggles the `fn-base` layer, which contains functional keys (`F1` - `F12`) and the bypass layer toggles.

### 3. Navigation Layer (Hold Spacebar)
When the Spacebar is held, the right hand acts as an inverted-T arrow cluster and general navigation/editing pad:
- **Movement:**
  - `I` -> Up
  - `J` -> Left
  - `K` -> Down
  - `L` -> Right
  - `H` -> Home
  - `;` -> End
  - `M` -> Page Up (`pgup`)
  - `,` -> Page Down (`pgdn`)
- **Editing:**
  - `U` -> Delete (`del`)
  - `O` -> Backspace (`bspc`)
- **Left-Hand Shortcuts (Direct Commands):**
  - `Q` -> `Cmd+Q` (Quit)
  - `W` -> `Cmd+W` (Close)
  - `X` -> `Cmd+X` (Cut)
  - `C` -> `Cmd+C` (Copy)
  - `V` -> `Cmd+V` (Paste)
  - `.` -> `Alt+.`

---

## 🔍 Historical Evolutions & Discrepancies

A comparison between the active `kanata.kbd`, the historic `kanata.kbd.bak`, and documentation (`CHECKPOINT.md` and `config_manifest.md`) reveals important design evolutions and undocumented changes:

### 1. Shift Responsiveness (`F` and `J` keys)
- **Checkpoint Claim:** `CHECKPOINT.md` states `F/J` were switched to `tap-hold-press` (200ms) for instant activation.
- **Actual File:** `kanata.kbd` uses `tap-hold-release-keys` (225ms tap / 175ms hold) with opposite-hand lists.
- **Rationale:** `tap-hold-press` does not support bilateral restriction. Without it, same-hand rolls (like typing "fun" or "if") would trigger accidental shifts. Keeping `tap-hold-release-keys` but shortening the hold timeout to 175ms resolved the latency while maintaining rolling safety.

### 2. Spacebar & Navigation Layer
- **Checkpoint Claim:** Spacebar acts as a tap-hold key (Tap: Space, Hold: Navigation Layer).
- **Actual File:** Navigation layer is triggered by holding **Caps Lock**. Spacebar is a standard, raw space key.
- **Rationale:** Making the spacebar a tap-hold key introduced a delay before spaces appeared on the screen, causing swallowed spaces and typing lag. Moving the Navigation layer to Caps Lock and restoring spacebar to its standard layout solved this.

### 3. Bypass Toggle ("Normal Mode")
- **Manifest Claim:** The bypass toggle is triggered by tapping the **Right Command** key.
- **Actual File:** The bypass toggle is triggered by pressing **`Fn + \`**.
  - Holding `Fn` activates `fn-base` or `fn-byp`.
  - Tapping `\` (which maps to `@to_byp` / `@to_base`) switches the system layer permanently.
- **Status:** The active implementation uses `Fn + \`. The manifest is outdated.

### 4. Right-Hand Triggers (`N` and `M` keys)
- **Manifest Claim:** `N` and `M` were removed from the right-hand trigger list to allow safe rolls ("an", "many", "example").
- **Actual File:** `n` and `m` are still present in `right-hand-keys` inside `kanata.kbd`.
- **Status:** They remain in the bilateral list. If same-hand rolls on the left hand (e.g. holding `a` and typing `n`) trigger accidental modifiers, `n` and `m` should be removed from `$right-hand-keys`.

### 5. Bilateral Trigger Inversion Bug (RESOLVED)
- **Issue:** Typing `Ctrl+l` (holding `a`, tapping `l`) produced `al`, and typing `Ctrl+f` (holding `;`, tapping `f`) produced `;f`.
- **Cause:** The `$left-hand-keys` and `$right-hand-keys` lists were completely inverted in the mod definitions. Left-hand modifiers (`a_mod` etc.) were configured with `$right-hand-keys` as their `tap-keys`, meaning opposite-hand keys forced a tap (e.g. holding `a` and pressing `l` forced `a` to tap, producing `al`), while same-hand keys triggered the modifier.
- **Resolution:** Corrected the trigger lists: left-hand mods now use `$left-hand-keys` (forcing same-hand keys to tap, protecting rolls) and right-hand mods now use `$right-hand-keys`. Opposite-hand modifiers now trigger instantly.

---

## 📋 Task List & Backlog

- [ ] **Bypass Toggle Realignment:** Clarify with the user if they want the Bypass Toggle on **Right Command** (as stated in the manifest) or if they prefer to keep it on **`Fn + \`** (active implementation).
- [x] **Bilateral Trigger Correction:** Resolved the inversion bug where same-hand rolls triggered modifiers and opposite-hand typing triggered taps.
- [x] **Prior-Idle Research:** Researched `tap-hold-require-prior-idle` (introduced in development version v1.12.0) and confirmed it is not yet supported in the stable v1.11.0 binary.
- [x] **Spacebar Navigation Trigger:** Configured Spacebar to act as `tap-hold` for the Navigation layer (using a safe `175ms` delay to prevent rolls).
- [x] **Caps Lock Hyper Modifier:** Configured Caps Lock to act as a Hyper key on hold and Escape on tap.
- [ ] **Terminal Shortcut Integration:** Verify integration of the Caps Lock Hyper key with Ghostty or iTerm2 (e.g., mapping `Hyper + Key` to terminal pane splits or tabs).
- [ ] **Documentation Sync:** Align `CHECKPOINT.md` and `config_manifest.md` with the finalized design once the above adjustments are decided.

---

## 🤝 Handoff Information

- **To run Kanata manually for debugging:**
  ```bash
  sudo /opt/homebrew/bin/kanata -c ~/CODE/dotfiles/kanata/kanata.kbd
  ```
- **To restart the service through the tray wrapper:**
  ```bash
  launchctl kickstart -k gui/$(id -u)/com.kanata.tray
  ```
  *(Note: Since Kanata is managed by the `kanata-tray` user LaunchAgent, restarting this agent kills and restarts both the tray app and the underlying Kanata process. You can also reload config or restart it directly from the macOS menu bar tray icon.)*
- **Key Files to watch:**
  - Configuration: [kanata.kbd](file:///Users/me/CODE/dotfiles/kanata/kanata.kbd)
  - Documentation: [README.md](file:///Users/me/CODE/dotfiles/kanata/README.md) | [CHECKPOINT.md](file:///Users/me/CODE/dotfiles/kanata/CHECKPOINT.md) | [config_manifest.md](file:///Users/me/CODE/dotfiles/kanata/config_manifest.md)
