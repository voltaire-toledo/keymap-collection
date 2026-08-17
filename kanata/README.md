# Kanata Setup for macOS (Tahoe v26)

This directory contains a macOS Kanata setup focused on home-row mods, a navigation layer, and a bypass mode for troubleshooting.

## Contents

- `kanata.kbd`: Main Kanata configuration.
- `kanata.kbd.bak`: Backup config.
- `install.sh` and `install_agy.sh`: Installer helpers.
- `bootstrap.sh`: Bootstrap helper script.
- `com.kanata.tray.plist`: launchd LaunchAgent service definition for kanata-tray.
- `INSTALLATION_GUIDE.md`, `CHECKPOINT.md`, `AddToDump.md`, `config_manifest.md`: Notes and operational docs.
- `profiler.swift` and `profiler_script.py`: Profiling/support scripts.

---

## 1. Installation

### Step 1: Run the Installer

```bash
chmod +x install.sh
./install.sh
```

### Step 2: Approve Karabiner Driver (if prompted)

1. Open **System Settings** > **Privacy & Security**.
2. In the **Security** section, allow blocked software from `pqrs.org`.
3. Authenticate to complete approval.

### Step 3: Grant Input Monitoring

1. Open **System Settings** > **Privacy & Security** > **Input Monitoring**.
2. Enable your terminal app.
3. If prompted, enable the Kanata binary as well.

---

## 2. Configuration Notes

<<<<<<< Updated upstream
### Active `defcfg` behavior

- `process-unmapped-keys yes`
- `concurrent-tap-hold yes`
- `macos-dev-names-include` currently includes:
  - Apple internal keyboard hash string
  - Karabiner DriverKit virtual keyboard
  - Synergy mouse
  - USB/wireless receiver-style names used in this environment

Use this command to inspect the current device names on your host:
=======
### Local Config Symlink

The repo stores platform-specific source configs, while the local machine uses
the normal `kanata.kbd` filename:

- macOS: `~/Library/Application Support/kanata/kanata.kbd`
- Linux: `${XDG_CONFIG_HOME:-~/.config}/kanata/kanata.kbd`

Create or refresh the symlink with:

```bash
./link-config.sh
```

Use `--target darwin` or `--target linux` to override OS detection, and
`--force` to replace an existing local config link or file.

### Step 1: Discover Device Names
Kanata needs exact strings to include or exclude specific keyboards. Run:
>>>>>>> Stashed changes

```bash
kanata --list
```

Then update `macos-dev-names-include` in `kanata.kbd` as needed.

### Layer model in `kanata.kbd`

- `base`: Main typing layer with bilateral home-row mods.
- `nav`: Navigation/editing layer toggled from Caps behavior.
- `bypass`: Plain typing layer with no home-row mods.
- `fn-base`: Function layer reachable from base.
- `fn-byp`: Function layer reachable from bypass.

### Important key behaviors

- `caps_nav` is `tap-hold 200 150 esc (layer-toggle nav)`:
  - Tap Caps for `Esc`.
  - Hold Caps to toggle `nav`.
- Home-row mods use `tap-hold-release-keys` bilateral enforcement:
  - Left hand mods (`a/s/d/f`) activate only with right-hand key interaction.
  - Right hand mods (`j/k/l/;`) activate only with left-hand key interaction.
- Fn key behavior:
  - In `base`, Fn toggles `fn-base`.
  - In `bypass`, Fn toggles `fn-byp`.
  - In each Fn layer, the key at the right edge of the third row switches between `base` and `bypass`.

## 3. Running Kanata

The setup runs **Kanata** as a system-wide LaunchDaemon (`com.kanata.service`), which enables keyboard remapping on the **macOS login screen** and automatically restarts the process if it terminates.

`kanata-tray` is used strictly as a status and layer monitor. It runs a dummy sleep script that keeps the tray active while it connects to Kanata's TCP port `5829`.

### Launching the Services

To bootstrap the tray agent and register the LaunchDaemon:

```bash
# Load the kanata-tray LaunchAgent
launchctl bootstrap gui/$(id -u) ~/Library/LaunchAgents/com.kanata.tray.plist

# Load the system-wide kanata LaunchDaemon (requires sudo)
sudo launchctl bootstrap system /Library/LaunchDaemons/com.kanata.service.plist
```

For manual debugging/runs of the daemon, you can start or stop the service via launchctl:
```bash
sudo launchctl kickstart -k system/com.kanata.service
```


## Keyboard Layout

### Home Row Mods Visualization (CGAS)

```text
  LEFT HAND                                       RIGHT HAND
  +-----+-----+-----+-----+               +-----+-----+-----+-----+
  |  A  |  S  |  D  |  F  |               |  J  |  K  |  L  |  ;  |
  +-----+-----+-----+-----+               +-----+-----+-----+-----+
  | ^Ctl| Cmd | Alt | Sft |               | Sft | Alt | Cmd | ^Ctl|
  +-----+-----+-----+-----+               +-----+-----+-----+-----+
     C     G     A     S                     S     A     G     C
```

### Full Home Row Context

This view shows where the modifiers sit within the standard ANSI home row:

```text
+------+------+------+------+------+------+------+------+------+------+------+------+--------+
| Caps |  A   |  S   |  D   |  F   |  G   |  H   |  J   |  K   |  L   |  ;   |  '   | Return |
|      | ^Ctl | Cmd  | Alt  | Sft  |      |      | Sft  | Alt  | Cmd  | ^Ctl |      |        |
+------+------+------+------+------+------+------+------+------+------+------+------+--------+
```

### Nav layer highlights

```text
  ROW 3
  +-------+-------+-------+-------+-------+
  |   Q   |   W   |   U   |   I   |   O   |
  +-------+-------+-------+-------+-------+
  | Cmd+Q | Cmd+W |  Del  |  Up   | Bspc  |
  +-------+-------+-------+-------+-------+

  ROW 4
  +-------+-------+-------+-------+-------+-------+-------+-------+-------+
  |   A   |   S   |   D   |   F   |   H   |   J   |   K   |   L   |   ;   |
  +-------+-------+-------+-------+-------+-------+-------+-------+-------+
  | Ctrl  | Cmd   | Alt   | Shift | Home  | Left  | Down  | Right | End   |
  +-------+-------+-------+-------+-------+-------+-------+-------+-------+

  ROW 5
  +-------+-------+-------+-------+-------+-------+
  |   X   |   C   |   V   |   N   |   M   |   .   |
  +-------+-------+-------+-------+-------+-------+
  | Cmd+X | Cmd+C | Cmd+V | PgDn  | PgUp  | Opt+. |
  +-------+-------+-------+-------+-------+-------+
```

### Bypass layer

```text
  ┏━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┓
  ┃   Esc   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃  Dele   ┃
  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃
  ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┛
     ┏━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┓
     ┃   Tab   ┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃
     ┃    _    ┃  Cmd+Q  ┃  Cmd+W  ┃    _    ┃    _    ┃    _    ┃    _    ┃  Del ⌦  ┃   ⬆︎     ┃ BkSp  ⌫ ┃    _    ┃    _    ┃    _    ┃    _    ┃
     ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┛
       ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━━━┓
       ┃  Esc(T)   ┃    A    ┃    S    ┃    D    ┃    F    ┃    G    ┃    H    ┃    J    ┃    K    ┃    L    ┃    :    ┃    '    ┃  Return   ┃
       ┃ Nav(Hold) ┃   Ctl   ┃   Cmd   ┃   Alt   ┃   Sft   ┃    _    ┃  Home   ┃    ⬅︎    ┃   ⬇︎     ┃   ➡︎     ┃   End   ┃    _    ┃     _     ┃
       ┗━━━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━━━┛
          ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━━━┓
          ┃   Shift   ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃    N    ┃    M    ┃    <    ┃    >    ┃    /    ┃   Shift   ┃
          ┃     _     ┃    _    ┃  Cmd+X  ┃  Cmd+C  ┃  Cmd+V  ┃    _    ┃    _    ┃ ↖↖ PgUp ┃ PgDn ↘↘ ┃  Alt+.  ┃    _    ┃     _     ┃
          ┗━━━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━━━┛
             ┏━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┓
             ┃   Fn    ┃    ^    ┃    ⌥    ┃    ⌘    ┃                Space                ┃    ⌘    ┃    ⌥    ┃
             ┃    _    ┃    _    ┃    _    ┃    _    ┃                  _                  ┃    _    ┃    _    ┃
             ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┛
```

---

## Troubleshooting

### Synergy erratic mouse behavior

If pointer behavior becomes jumpy with Synergy:

1. On the Synergy server, enable **Use relative mouse moves**.
2. Re-check included device names with `kanata --list` and adjust `macos-dev-names-include`.

### Bluetooth reconnect edge case

If a keyboard reconnect causes remaps to stop, restart Kanata.

---

## Process Daemon Recovery

By running `kanata` as a system LaunchDaemon with `KeepAlive` enabled, `launchd` will automatically restart `kanata` if the process crashes or terminates. This ensures high availability and eliminates the need to manually restart the remapper from the terminal.
