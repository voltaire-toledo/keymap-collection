#!/usr/bin/env bash

set -euo pipefail

readonly KANATA_API="https://api.github.com/repos/jtroo/kanata/releases/latest"
readonly DRIVER_API="https://api.github.com/repos/pqrs-org/Karabiner-DriverKit-VirtualHIDDevice/releases/latest"
readonly DRIVER_TEAM_ID="G43BCU2T37"
readonly INSTALL_DIR="/usr/local/bin"

fail() {
    printf 'Error: %s\n' "$1" >&2
    exit 1
}

latest_tag() {
    local tag
    tag=$(curl -fsSL "$1" | awk -F '"' '/"tag_name"[[:space:]]*:/ { print $4; exit }')
    [[ "$tag" =~ ^v[0-9]+([.-][[:alnum:].-]+)*$ ]] || fail "Could not read a valid release tag from $1"
    printf '%s\n' "$tag"
}

verify_kanata_archive() {
    local asset="$1"
    local archive="$2"
    local sums="$3"
    local expected

    expected=$(awk -v name="$asset" '$2 == name { print $1; exit }' "$sums")
    [[ "$expected" =~ ^[[:xdigit:]]{64}$ ]] || fail "No valid SHA-256 entry found for $asset"
    printf '%s  %s\n' "$expected" "$archive" | shasum -a 256 -c -
}

[[ "$(uname -s)" == "Darwin" ]] || fail "This installer supports macOS only"

case "$(uname -m)" in
    arm64)
        kanata_asset="macos-binaries-arm64.zip"
        kanata_binary="kanata_macos_arm64"
        ;;
    x86_64)
        kanata_asset="macos-binaries-x64.zip"
        kanata_binary="kanata_macos_x64"
        ;;
    *)
        fail "Unsupported macOS architecture: $(uname -m)"
        ;;
esac

command -v curl >/dev/null || fail "curl is required"
command -v shasum >/dev/null || fail "shasum is required"
command -v unzip >/dev/null || fail "unzip is required"
command -v pkgutil >/dev/null || fail "pkgutil is required"
command -v spctl >/dev/null || fail "spctl is required"

tmp_dir=$(mktemp -d "${TMPDIR:-/tmp}/kanata-install.XXXXXX")
trap 'rm -rf "$tmp_dir"' EXIT

printf 'Resolving the latest Kanata release...\n'
kanata_tag=$(latest_tag "$KANATA_API")
kanata_base="https://github.com/jtroo/kanata/releases/download/$kanata_tag"
kanata_archive="$tmp_dir/$kanata_asset"
kanata_sums="$tmp_dir/sha256sums"

curl -fsSL "$kanata_base/sha256sums" -o "$kanata_sums"
curl -fsSL "$kanata_base/$kanata_asset" -o "$kanata_archive"
verify_kanata_archive "$kanata_asset" "$kanata_archive" "$kanata_sums"

unzip -p "$kanata_archive" "$kanata_binary" > "$tmp_dir/kanata"
[[ -s "$tmp_dir/kanata" ]] || fail "The verified archive did not contain $kanata_binary"
chmod 0755 "$tmp_dir/kanata"

printf 'Resolving the latest Karabiner virtual HID driver release...\n'
driver_tag=$(latest_tag "$DRIVER_API")
driver_version=${driver_tag#v}
driver_asset="Karabiner-DriverKit-VirtualHIDDevice-$driver_version.pkg"
driver_url="https://github.com/pqrs-org/Karabiner-DriverKit-VirtualHIDDevice/releases/download/$driver_tag/$driver_asset"
driver_pkg="$tmp_dir/$driver_asset"
curl -fsSL "$driver_url" -o "$driver_pkg"

signature=$(pkgutil --check-signature "$driver_pkg") || fail "The driver package signature is invalid"
printf '%s\n' "$signature"
grep -Fq "($DRIVER_TEAM_ID)" <<< "$signature" || fail "The driver package signer does not match the expected developer team"
spctl --assess --type install --verbose "$driver_pkg" || fail "Gatekeeper rejected the driver package"

printf 'Installing verified Kanata binary...\n'
sudo mkdir -p "$INSTALL_DIR"
sudo install -m 0755 "$tmp_dir/kanata" "$INSTALL_DIR/kanata"

printf 'Installing verified Karabiner virtual HID driver...\n'
sudo installer -pkg "$driver_pkg" -target /

printf 'Activating Karabiner-VirtualHIDDevice...\n'
manager="/Applications/.Karabiner-VirtualHIDDevice-Manager.app/Contents/MacOS/Karabiner-VirtualHIDDevice-Manager"
if [[ -x "$manager" ]]; then
    "$manager" activate
else
    printf 'Warning: Activation manager was not found at the expected path. Check /Applications.\n' >&2
fi

cat <<'EOF'
--------------------------------------------------------
Installation complete!
NEXT STEPS:
1. Go to System Settings > General > Login Items & Extensions > Driver Extensions.
2. Enable the Karabiner-VirtualHIDDevice extension if macOS requests it.
3. Grant Input Monitoring permission to your terminal or Kanata service.
4. Run `kanata --list` to find your keyboard names.
5. Update your Kanata configuration and start Kanata with that config.
--------------------------------------------------------
EOF
