# V1 Max Candidates in Progress

Store unpassed QMK candidates here as one record per immutable candidate
identity. The record must identify the ANSI encoder target, candidate commit,
SHA-256 for every source-bundle file (`keymap.c` and any `rules.mk`, `config.h`,
or runtime changes), baseline commit/file hashes, changed physical layout
arguments, expected tests, reviews, build artifact/hash, DFU result, and final
hardware result. Do not overwrite a reviewed candidate.

Compilation and a DFU transfer are not QC. Promote only after explicit
hardware PASS, moving the exact source bundle, artifact/hash sidecar, and
complete record to `V1-MAX/releases/`.
