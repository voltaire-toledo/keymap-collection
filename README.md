# Keymap Collection

This repository serves as the central hub for my custom keyboard firmware and keymaps. It manages two separate Git submodules for different keyboard ecosystems:

## [QMK Firmware (Keychron V1 Max)](./QMK)
Contains the custom keymap, QMK build configurations, and developer lab documentation tailored specifically for the **Keychron V1 Max** (ANSI Encoder).
- **Core Keyboard:** Keychron V1 Max
- **Location:** `./QMK`

## [ZMK Firmware (Keychron B1 Pro)](./ZMK)
Contains the custom keymap, Zephyr workspace scaffolding, and Windows PowerShell build scripts tailored specifically for the **Keychron B1 Pro**.
- **Core Keyboard:** Keychron B1 Pro
- **Location:** `./ZMK`

## Developer quick start

This section is for contributors working with the firmware and keymap source in
this repository. To download prebuilt firmware instead, use the repository's
GitHub Releases.

The `QMK` and `ZMK` directories are Git submodules, so clone the repository
using either of these options.

Clone everything in one command:

```bash
git clone --recurse-submodules https://github.com/voltaire-toledo/keymap-collection.git
cd keymap-collection
```

Or initialize the submodules after a normal clone:

```bash
git clone https://github.com/voltaire-toledo/keymap-collection.git
cd keymap-collection
git submodule update --init --recursive
```

Both commands check out the exact QMK and ZMK commits pinned by this
repository. If you intentionally need to advance the submodules to the latest
commits on their configured tracking branches, run:

```bash
git submodule update --remote --recursive
```

Review and commit the resulting submodule-pointer changes deliberately.
