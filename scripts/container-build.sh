#!/usr/bin/env bash
# Invoked by build-firmware.sh inside the selected toolchain image.
set -euo pipefail
cd /work
# Reused bind mounts can have a different mapped owner in a fresh container.
# Trust only repositories in this disposable, pinned-input workspace.
while IFS= read -r -d '' gitdir; do
  git config --global --add safe.directory "$(dirname "$gitdir")"
done < <(find /work -name .git -prune -print0)
checkout() {
  local url=$1 ref=$2 dest=$3
  if [[ ! -d "$dest/.git" ]]; then
    git init "$dest"
    git -C "$dest" remote add origin "$url"
  fi
  git -C "$dest" fetch --depth 1 origin "$ref"
  git -C "$dest" checkout --detach FETCH_HEAD
  test "$(git -C "$dest" rev-parse HEAD)" = "$ref"
}
case "$1" in
qmk)
  checkout https://github.com/Keychron/qmk_firmware.git 2b493ff95f2665db719b1e95c91babb5036e72f3 qmk
  cd qmk
  git submodule update --init --depth 1
  [[ "$QMK_OVERLAY" != /* && "/$QMK_OVERLAY/" != *"/../"* ]] || { echo 'QMK_OVERLAY must be relative to the repository' >&2; exit 2; }
  cp -a "/repo/$QMK_OVERLAY/." .
  make -j"$BUILD_JOBS" keychron/v1_max/ansi_encoder:default
  artifact=keychron_v1_max_ansi_encoder_default.bin
  test -s "$artifact"
  cp "$artifact" /output/
  ;;
zmk)
  checkout https://github.com/voltaire-toledo/submod-zmk.git 9df42d4e2c6362cf04df41218155e7aef4f02d18 zmk
  cd zmk
  cp /repo/scripts/zmk-west.yml app/west.yml
  if [[ ! -d .west ]]; then west init -l app; fi
  west update -n
  west zephyr-export
  for patch in 0001-esb-nrf-fix.patch 0002-zephyr-fix-clang-format-requirement.patch; do
    patchfile="/repo/keymaps/zmk/b1pro/patches/$patch"
    if git -C zephyr apply --check "$patchfile"; then
      git -C zephyr apply "$patchfile"
    elif git -C zephyr apply --reverse --check "$patchfile"; then
      echo "Already applied: $patch"
    else
      echo "Patch does not match pinned Zephyr: $patch" >&2; exit 1
    fi
  done
  cp /repo/keymaps/zmk/b1pro/patches/tdfu_prv.h app/src/dfu/tdfu_prv.h
  [[ "$ZMK_KEYMAP" != /* && "/$ZMK_KEYMAP/" != *"/../"* ]] || { echo 'ZMK_KEYMAP must be relative to the repository' >&2; exit 2; }
  cp "/repo/$ZMK_KEYMAP" app/boards/shields/keychron/b1/us/keychron_b1_us.keymap
  export SOURCE_DATE_EPOCH=1791108125
  west build -s app -b keychron -d build/b1pro -p always -o=-j"$BUILD_JOBS" -- -DSHIELD=keychron_b1_us -DBUILD_VERSION=c080d0853f98
  artifact=zmk_b1pro.uf2
  test -s build/b1pro/zephyr/zmk.uf2
  cp build/b1pro/zephyr/zmk.uf2 "/output/$artifact"
  ;;
esac
cd /output
sha256sum "$artifact" > "$artifact.sha256"
sha256sum -c "$artifact.sha256"
