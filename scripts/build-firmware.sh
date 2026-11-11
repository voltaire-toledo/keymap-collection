#!/usr/bin/env bash
# Host entry point; all downloads and compiler tools run in the container.
set -euo pipefail
repo=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
target=${1:-}
case "$target" in
  qmk) image=qmkfm/qmk_cli@sha256:b7d7fa8fb4432b569931de5ad59098cb788f440ed61a62c5126746b71aee0f4a ;;
  zmk) image=zmkfirmware/zmk-build-arm@sha256:1fa70cf58594942f90a96059a3b135ac1843af3b4fb6a46c7bb39f29f9de661b ;;
  *) echo "Usage: bash scripts/build-firmware.sh {qmk|zmk}" >&2; exit 2 ;;
esac
jobs=${BUILD_JOBS:-2}
[[ "$jobs" =~ ^[1-9][0-9]*$ ]] || { echo 'BUILD_JOBS must be a positive integer' >&2; exit 2; }
docker info >/dev/null
mkdir -p "$repo/.firmware-build/$target" "$repo/build_output/$target"
args=(--rm --entrypoint bash
  --mount "type=bind,src=$repo,dst=/repo,readonly"
  --mount "type=bind,src=$repo/.firmware-build/$target,dst=/work"
  --mount "type=bind,src=$repo/build_output/$target,dst=/output"
  -e "BUILD_JOBS=$jobs"
  -e "QMK_OVERLAY=${QMK_OVERLAY:-keymaps/qmk/v1_max/qmk_overlay}"
  -e "ZMK_KEYMAP=${ZMK_KEYMAP:-keymaps/zmk/b1pro/B1PRO/QC-IN_PROGRESS/v0.25.8-candidate.keymap}")
# The QMK image also copies its result to its configured userspace directory.
if [[ "$target" == qmk ]]; then
  args+=(--mount "type=bind,src=$repo/build_output/qmk,dst=/qmk_userspace")
fi
# Optional combined CA bundle for managed environments with an HTTPS proxy.
if [[ -n "${FIRMWARE_CA_BUNDLE:-}" ]]; then
  args+=(--mount "type=bind,src=$FIRMWARE_CA_BUNDLE,dst=/run/firmware-ca.pem,readonly"
    -e GIT_SSL_CAINFO=/run/firmware-ca.pem -e SSL_CERT_FILE=/run/firmware-ca.pem
    -e REQUESTS_CA_BUNDLE=/run/firmware-ca.pem -e PIP_CERT=/run/firmware-ca.pem)
fi
docker run "${args[@]}" "$image" /repo/scripts/container-build.sh "$target"
