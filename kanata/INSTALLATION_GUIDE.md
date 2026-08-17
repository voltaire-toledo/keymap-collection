# Kanata on macOS: Comprehensive Setup Guide

This guide details how to successfully install, configure, and persist `kanata` (v1.11.0+) on macOS using the Karabiner DriverKit.

## 1. Prerequisites

- **Kanata Binary:** Installed via Homebrew or manually (e.g. `/opt/homebrew/bin/kanata` or `/usr/local/bin/kanata`).
- **Karabiner Driver:** [Karabiner-VirtualHIDDevice-Manager](https://github.com/pqrs-org/Karabiner-DriverKit-VirtualHIDDevice).
  - *Note:* Even if you don't use Karabiner-Elements, you **must** have this driver installed.

## 2. Driver Setup & Activation

1.  **Install the Driver:** Run the `.pkg` installer from the link above.
2.  **Activate the Extension:**
    ```bash
    sudo /Applications/.Karabiner-VirtualHIDDevice-Manager.app/Contents/MacOS/Karabiner-VirtualHIDDevice-Manager activate
    ```
3.  **Approve:** Go to **System Settings > Privacy & Security** and click **Allow** for `pqrs.org`.
4.  **Verify:** Run `systemextensionsctl list`. You should see:
    `org.pqrs.Karabiner-DriverKit-VirtualHIDDevice [activated enabled]`

## 3. Configuration (`kanata.kbd`)

Critical settings for macOS to prevent keyboard lockout:

```lisp
(defcfg
  process-unmapped-keys yes ;; Essential: prevents blocking keys not in defsrc
  tap-hold-require-prior-idle 150 ;; Prevents accidental holds during fast typing
  
  macos-dev-names-include (
    "0xD4359520DA829EC8" ;; Use the unique hash of your internal keyboard
    "Karabiner DriverKit VirtualHIDKeyboard 1.8.0" ;; The output device
  )
)

(defsrc
  ;; MUST include every key you intend to use. 
  ;; Any key missing here will be BLOCKED unless process-unmapped-keys is yes.
  esc  f1   f2 ...
  caps a    s  ...
)
```

## 4. Navigation Layer (Caps Lock)

The navigation layer is now triggered by holding **Caps Lock**. 
- **Tap Caps Lock:** Sends `Escape`.
- **Hold Caps Lock:** Activates `nav` layer (Arrows, Home/End, etc.).

## 5. The "Secret Sauce": The Karabiner Daemon

`kanata` will fail with `connect_failed asio.system:2` if the Karabiner DriverKit Daemon is not running. It must be started as root.

To ensure the driver's communication channel is open, verify or create the system LaunchDaemon `/Library/LaunchDaemons/org.pqrs.Karabiner-DriverKit-VirtualHIDDevice-Daemon.plist`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>org.pqrs.Karabiner-DriverKit-VirtualHIDDevice-Daemon</string>
    <key>ProgramArguments</key>
    <array>
        <string>/Library/Application Support/org.pqrs/Karabiner-DriverKit-VirtualHIDDevice/Applications/Karabiner-VirtualHIDDevice-Daemon.app/Contents/MacOS/Karabiner-VirtualHIDDevice-Daemon</string>
    </array>
    <key>RunAtLoad</key><true/>
    <key>KeepAlive</key><true/>
</dict>
</plist>
```

## 6. Persistence & GUI Integration (`kanata-tray`)

To enable keyboard remapping on the **macOS login screen** and ensure process recovery, Kanata runs as a system LaunchDaemon (`com.kanata.service`). `kanata-tray` runs as a user LaunchAgent and is used strictly as a status/layer monitor.

### A. System LaunchDaemon (`com.kanata.service.plist`)

Create a system LaunchDaemon at `/Library/LaunchDaemons/com.kanata.service.plist`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.kanata.service</string>
    <key>ProgramArguments</key>
    <array>
        <string>/opt/homebrew/bin/kanata</string>
        <string>--cfg</string>
        <string>/Users/me/code/dotfiles/kanata/kanata.kbd</string>
        <string>--port</string>
        <string>5829</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <true/>
    <key>StandardOutPath</key>
    <string>/tmp/kanata.stdout.log</string>
    <key>StandardErrorPath</key>
    <string>/tmp/kanata.stderr.log</string>
</dict>
</plist>
```

Configure permissions and bootstrap it:
```bash
sudo chown root:wheel /Library/LaunchDaemons/com.kanata.service.plist
sudo chmod 644 /Library/LaunchDaemons/com.kanata.service.plist
sudo launchctl bootstrap system /Library/LaunchDaemons/com.kanata.service.plist
```

### B. Dummy Wrapper Configuration
Because `kanata-tray` expects to manage a child process, but we want it to act solely as a status monitor, configure it to run a dummy/sleep script instead.

Update `kanata-wrapper.sh` to run a sleep loop:
```bash
#!/bin/bash
# Dummy wrapper to keep kanata-tray happy while it monitors the daemon on port 5829.
exec sleep 99999999
```

### C. Kanata Tray Configuration
Ensure `~/Library/Application Support/kanata-tray/kanata-tray.toml` points to the dummy wrapper, and has `tcp_port` configured to connect to the daemon:

```toml
[defaults]
tcp_port = 5829

[presets.'Default Preset']
kanata_executable = '/Users/me/code/dotfiles/kanata/kanata-wrapper.sh'
kanata_config = '/Users/me/code/dotfiles/kanata/kanata.kbd'
autorun = true
```

### D. LaunchAgent (`com.kanata.tray.plist`)
To start the tray GUI automatically upon login, create a LaunchAgent at `~/Library/LaunchAgents/com.kanata.tray.plist`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.kanata.tray</string>
    <key>ProgramArguments</key>
    <array>
        <string>/opt/homebrew/bin/kanata-tray</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <false/>
    <key>LimitLoadToSessionType</key>
    <string>Aqua</string>
</dict>
</plist>
```

Bootstrap it using:
```bash
launchctl bootstrap gui/$(id -u) ~/Library/LaunchAgents/com.kanata.tray.plist
```

## 7. Troubleshooting

- **Check Service Status:** `sudo launchctl list | grep kanata`
- **Check TCP Connection:** `sudo lsof -i :5829` (Should show `kanata-tray` connected to `kanata`)
- **Emergency Exit:** Press `LControl + Space + Escape` to quit the underlying process.
- **Input Monitoring:** Ensure the `kanata` binary has **Input Monitoring** permissions in *System Settings > Privacy & Security > Input Monitoring*.
- **Daemon Logs:** View standard logs at `/tmp/kanata.stdout.log` and error logs at `/tmp/kanata.stderr.log`.
