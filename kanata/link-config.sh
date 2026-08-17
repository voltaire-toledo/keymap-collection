#!/bin/sh
set -eu

usage() {
  cat <<'EOF'
Usage: link-config.sh [--target darwin|linux] [--force]

Creates the local Kanata config symlink for this operating system.

Defaults:
  darwin -> ~/Library/Application Support/kanata/kanata.kbd
  linux  -> ${XDG_CONFIG_HOME:-~/.config}/kanata/kanata.kbd

Options:
  --target <os>  Override OS detection. Supported: darwin, linux.
  --force        Replace an existing destination file or symlink.
  -h, --help     Show this help.
EOF
}

target_os=""
force="false"

while [ "$#" -gt 0 ]; do
  case "$1" in
    --target)
      if [ "$#" -lt 2 ]; then
        echo "error: --target requires darwin or linux" >&2
        exit 1
      fi
      target_os="$2"
      shift
      ;;
    --force)
      force="true"
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "error: unknown option: $1" >&2
      usage >&2
      exit 1
      ;;
  esac
  shift
done

if [ -z "$target_os" ]; then
  case "$(uname -s)" in
    Darwin) target_os="darwin" ;;
    Linux) target_os="linux" ;;
    *)
      echo "error: unsupported OS: $(uname -s)" >&2
      exit 1
      ;;
  esac
fi

script_dir="$(CDPATH='' cd -- "$(dirname "$0")" && pwd)"

case "$target_os" in
  darwin)
    source_file="$script_dir/kanata.darwin"
    dest_dir="$HOME/Library/Application Support/kanata"
    ;;
  linux)
    source_file="$script_dir/kanata.linux"
    dest_dir="${XDG_CONFIG_HOME:-$HOME/.config}/kanata"
    ;;
  *)
    echo "error: unsupported target: $target_os" >&2
    exit 1
    ;;
esac

dest_file="$dest_dir/kanata.kbd"

if [ ! -f "$source_file" ]; then
  echo "error: source config does not exist: $source_file" >&2
  echo "create it first, then rerun this script" >&2
  exit 1
fi

mkdir -p "$dest_dir"

if [ -L "$dest_file" ] || [ -e "$dest_file" ]; then
  current_target=""
  if [ -L "$dest_file" ]; then
    current_target="$(readlink "$dest_file" || true)"
  fi

  if [ "$current_target" = "$source_file" ]; then
    echo "unchanged: $dest_file -> $source_file"
    exit 0
  fi

  if [ "$force" != "true" ]; then
    echo "error: destination already exists: $dest_file" >&2
    echo "rerun with --force to replace it" >&2
    exit 1
  fi

  rm -f "$dest_file"
fi

ln -s "$source_file" "$dest_file"
echo "linked: $dest_file -> $source_file"
