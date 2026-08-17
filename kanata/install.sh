#!/bin/bash

# Exit on error
set -e

echo "Starting Kanata and Karabiner Driver installation..."

# 1. Install Kanata
echo "Downloading latest Kanata binary for macOS..."
KANATA_URL=$(curl -s https://api.github.com/repos/jtroo/kanata/releases/latest | grep "browser_download_url.*kanata_macos_arm64" | cut -d : -f 2,3 | tr -d \")
# Fallback to x86_64 if arm64 not found or if on Intel (though we assume Apple Silicon for Tahoe v26)
if [ -z "$KANATA_URL" ]; then
    KANATA_URL=$(curl -s https://api.github.com/repos/jtroo/kanata/releases/latest | grep "browser_download_url.*kanata_macos" | head -n 1 | cut -d : -f 2,3 | tr -d \")
fi

curl -L "$KANATA_URL" -o kanata_bin
chmod +x kanata_bin
sudo mv kanata_bin /usr/local/bin/kanata

echo "Kanata installed to /usr/local/bin/kanata"

# 2. Install Karabiner-DriverKit-VirtualHIDDevice
echo "Downloading latest Karabiner-DriverKit-VirtualHIDDevice..."
DRIVER_URL=$(curl -s https://api.github.com/repos/pqrs-org/Karabiner-DriverKit-VirtualHIDDevice/releases/latest | grep "browser_download_url.*pkg" | cut -d : -f 2,3 | tr -d \")

curl -L "$DRIVER_URL" -o driver.pkg
echo "Installing driver.pkg (will require sudo)..."
sudo installer -pkg driver.pkg -target /
rm driver.pkg

# 3. Activate Driver
echo "Activating Karabiner-VirtualHIDDevice..."
if [ -f "/Applications/.Karabiner-VirtualHIDDevice-Manager.app/Contents/MacOS/Karabiner-VirtualHIDDevice-Manager" ]; then
    /Applications/.Karabiner-VirtualHIDDevice-Manager.app/Contents/MacOS/Karabiner-VirtualHIDDevice-Manager activate
else
    echo "Warning: Activation manager not found at expected path. Please check /Applications."
fi

echo "--------------------------------------------------------"
echo "Installation complete!"
echo "NEXT STEPS:"
echo "1. Go to System Settings > Privacy & Security."
echo "2. Scroll to 'Security' and click 'Allow' for the pqrs.org extension."
echo "3. Grant 'Input Monitoring' permission to your Terminal app."
echo "4. Run 'kanata --list' to find your keyboard names."
echo "5. Update kanata.kbd and run 'sudo kanata --cfg kanata.kbd'."
echo "--------------------------------------------------------"
