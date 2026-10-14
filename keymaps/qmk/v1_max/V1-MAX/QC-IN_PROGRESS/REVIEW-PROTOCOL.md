# V1 Max QMK Review Protocol

## Review identity

The reviewed identity is the candidate commit plus SHA-256 for every
candidate-bundle file: `keymap.c`, required `rules.mk`, any `config.h`, and
any changed shared source. An edit invalidates validation and review.

## Required checks

1. Confirm the ANSI encoder target and `LAYOUT_ansi_82` macro against the
   physical unit.
2. Compare each changed visible layout argument with the selected vendor
   baseline; do not reuse B1 position numbers.
3. Review layer indices with the V1 DIP callback: Mac base is 0, Windows base
   is 2 in the stock design.
4. Review Fn, DFU recovery, wireless controls, encoder maps, RGB, and the
   VIA/raw-HID policy.
5. Review every feature flag and timing/hold callback, including Chordal Hold
   or Tap Dance where enabled.
6. Record build-input and output hashes and cumulative expected hardware tests.
7. Confirm the VIA policy and the required pre-QC state procedure; do not
   silently reset a user's keyboard state.

One senior review follows deterministic validation. A second independent review
is mandatory for layer order, protected paths, shared runtime code, or three or
more physical-key changes. Candidate approval precedes any DFU flash.
