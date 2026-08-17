# Kanata Knowledge Base Items

## Restarting Kanata Service (macOS)
To apply configuration changes when kanata is managed by `kanata-tray` via `launchctl`:
```bash
# Force a restart of the user LaunchAgent
launchctl kickstart -k gui/$(id -u)/com.kanata.tray
```
*Note: The `-k` flag kills the existing process to ensure a fresh start.*

## Checking Kanata Logs (macOS)
Since macOS does not use `journalctl`, you can check the output using the unified logging system:
```bash
log show --predicate 'process == "kanata" or process == "kanata-tray"' --last 5m
```
Alternatively, if you run the wrapper or `kanata-tray` manually in a terminal, logs are output directly to stderr/stdout.

## Verifying Service Status
Check if the user-level LaunchAgent is loaded and running:
```bash
launchctl list | grep kanata
```
Expected output: `<PID>  0  com.kanata.tray`

To find the running processes:
```bash
ps -o ppid,pid,command -ax | grep -E "kanata|tray"
```

## Service Configuration (macOS)
Kanata is run in the user session via the `kanata-tray` wrapper, which uses a passwordless `sudo` configuration to gain root access to the Karabiner VirtualHID Device driver.
- **LaunchAgent Path:** `~/Library/LaunchAgents/com.kanata.tray.plist`
- **Tray Config Path:** `~/Library/Application Support/kanata-tray/kanata-tray.toml`
- **Sudo Config Path:** `/etc/sudoers.d/kanata` (contains `YOUR_USER ALL=(ALL) NOPASSWD: /opt/homebrew/bin/kanata`)
- **Wrapper Executable:** `/Users/YOUR_USER/CODE/dotfiles/kanata/kanata-wrapper.sh` (called by the tray app)

## Bootstrapping Kanata-Tray
To manually bootstrap (load) the user service or force a fresh start:
```bash
# Load / Reload LaunchAgent
launchctl bootout gui/$(id -u) ~/Library/LaunchAgents/com.kanata.tray.plist 2>/dev/null || true
launchctl bootstrap gui/$(id -u) ~/Library/LaunchAgents/com.kanata.tray.plist
```

---

## 📝 Lessons Learned: Plist Deletion & Dual Service Conflicts

### 1. Why `com.kanata.service.plist` (LaunchDaemon) was Deleted
If a system-level LaunchDaemon (`com.kanata.service.plist`) existed earlier but was deleted, it was likely due to:
* **Migration to `kanata-tray`:** The `kanata` service cannot be run simultaneously as a system LaunchDaemon and via a user-session LaunchAgent (`kanata-tray`). The system daemon would grab the Karabiner HID device driver and bind to the TCP control port (`5829`), causing `kanata-tray` to fail when launching its own managed instance. Thus, the system plist was deleted to prevent dual-capture and resource conflict.
* **macOS System Update / Security Cleanup:** System-level LaunchDaemons running as root that reference configuration files in user home directories (`/Users/YOUR_USER/...`) are flagged by macOS security tools or third-party cleaning applications (like CleanMyMac, Onyx, or system update scripts) and may be automatically cleaned up.
* **Manual Correction:** The user or a bootstrap script previously removed it to cleanly pivot to the graphical tray configuration.

### 2. Device Grabbing & TCP Port Binding
* **Dual-Instance Conflict:** Only one instance of `kanata` can connect to the virtual HID driver and bind to port `5829` (the TCP port used by the tray app).
* **Fix:** Ensure no system-wide LaunchDaemons (`/Library/LaunchDaemons/com.kanata.service.plist`) are running or loaded, and rely entirely on the LaunchAgent (`~/Library/LaunchAgents/com.kanata.tray.plist`).
