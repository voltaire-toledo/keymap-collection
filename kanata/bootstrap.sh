#!/bin/bash
# bootstrap.sh - Load Kanata-Tray LaunchAgent and Karabiner DriverKit Daemon

TRAY_PLIST_SOURCE="$(dirname "$0")/com.kanata.tray.plist"
TRAY_PLIST_DEST="$HOME/Library/LaunchAgents/com.kanata.tray.plist"
KARABINER_DAEMON_PLIST="/Library/LaunchDaemons/org.pqrs.Karabiner-DriverKit-VirtualHIDDevice-Daemon.plist"

echo "--------------------------------------------------------"
echo "Bootstrapping Kanata Tray service..."

# 0. Check and install Tray plist
if [ ! -f "$TRAY_PLIST_SOURCE" ]; then
    echo "Error: Source $TRAY_PLIST_SOURCE not found."
    exit 1
fi

mkdir -p "$HOME/Library/LaunchAgents"
cp "$TRAY_PLIST_SOURCE" "$TRAY_PLIST_DEST"
echo "Installed LaunchAgent to $TRAY_PLIST_DEST"

if [ ! -f "$KARABINER_DAEMON_PLIST" ]; then
    echo "Error: $KARABINER_DAEMON_PLIST not found."
    echo "Kanata requires the Karabiner DriverKit Daemon to function."
    echo "Please follow INSTALLATION_GUIDE.md to set it up."
    exit 1
fi

# 1. Ensure Karabiner Daemon is bootstrapped first
echo "Ensuring Karabiner DriverKit Daemon is running..."
sudo launchctl bootstrap system "$KARABINER_DAEMON_PLIST" 2>/dev/null || true
sudo launchctl kickstart -k system/org.pqrs.Karabiner-DriverKit-VirtualHIDDevice-Daemon 2>/dev/null || true

# 2. Unload Kanata-Tray if already loaded (ignore errors if not loaded)
echo "Reloading Kanata Tray LaunchAgent..."
launchctl bootout gui/$(id -u) "$TRAY_PLIST_DEST" 2>/dev/null || true

# 3. Bootstrap (Load) the service
launchctl bootstrap gui/$(id -u) "$TRAY_PLIST_DEST"

echo "--------------------------------------------------------"
echo "Kanata Tray is now bootstrapped and running."
echo "Note: If remapping doesn't work after reboot, check 'launchctl list | grep kanata'"
echo "--------------------------------------------------------"

launchctl list | grep kanata

