# VT Mello Keymap Collection

This repository serves as the central hub for my custom keyboard firmware and keymaps. It manages two separate Git submodules for different keyboard ecosystems:

## 🎛️ [QMK Firmware (Keychron V1 Max)](./QMK)
Contains the custom "mello" keymap, QMK build configurations, and developer lab documentation tailored specifically for the **Keychron V1 Max** (ANSI Encoder).
- **Core Keyboard:** Keychron V1 Max
- **Location:** `./QMK`

## ⌨️ [ZMK Firmware (Keychron B1 Pro)](./ZMK)
Contains the custom keymap, Zephyr workspace scaffolding, and Windows PowerShell build scripts tailored specifically for the **Keychron B1 Pro**.
- **Core Keyboard:** Keychron B1 Pro
- **Location:** `./ZMK`

---
> [!NOTE] 
> The `QMK` and `ZMK` directories are Git submodules tracking forks of the upstream Keychron repositories. Ensure you initialize and update submodules after cloning this repository (`git submodule update --init --recursive`).
