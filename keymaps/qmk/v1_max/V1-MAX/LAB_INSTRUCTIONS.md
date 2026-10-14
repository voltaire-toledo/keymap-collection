# V1 Max ANSI — LAB_INSTRUCTIONS: Build and Flash

## Scope and authority

This document operationalizes one full candidate lifecycle — baseline,
candidate update, review, build, flash, hardware QC, promotion — into a
runnable sequence for the Keychron V1 Max ANSI encoder QMK project.

It is subordinate to, not a replacement for:

- `AGENTS.md` — candidate and release discipline, protected V1 Max behavior.
- `V1-MAX/QC-IN_PROGRESS/REVIEW-PROTOCOL.md` — required review checks.
- `V1-MAX/WSL-Prep.md` — one-time WSL/QMK environment preparation.
- `V1-MAX/QC-IN_PROGRESS/v0.24-b1-intent-v1max-ansi.md` — the prior reviewed
  candidate record and its behavioral-translation methodology.

Where this document and those disagree, those win. What this document adds is
the step those deliberately withheld: the actual DFU flash and the hardware QC
that must follow it.

## 0. Baseline declaration

`ZMK/B1PRO/QC-IN_PROGRESS/v0.25-candidate.keymap` is now the behavioral-intent
baseline, superseding the v0.24 reference used in the existing candidate
record. As with v0.24, it is **intent only** — not a QMK source file, not a
layout authority. The layout/matrix authority remains
`keyboards/keychron/v1_max/ansi_encoder/`, and the candidate remains
`keyboards/keychron/v1_max/ansi_encoder/keymaps/default/`.

### Delta review: v0.24 → v0.25

A line-by-line diff of the two ZMK files shows the v0.25 changes are almost
entirely internal ZMK cleanup, not new behavior to translate:

| v0.25 change (ZMK) | Effect on the QMK candidate |
| --- | --- |
| Header comment updated; whitespace fix on one `fn_layer_access` binding | None — no semantic change. |
| `layer_m_sym1` / `layer_m_sym1`(Windows `layer_w_sym1`) idle cells: `&none` → `&trans` | None — the keymap already uses QMK's transparent keycode (`_______`) in every equivalent cell. ZMK's `&trans` and QMK's `_______` are the same concept; candidate was never on `&none`'s QMK equivalent (`XXXXXXX`) here. |
| `layer_m_nav1` / `layer_w_nav1` idle cells: `&none` → `&trans`, plus one added `&kp SPACE` on the row ZMK enters this layer by holding the *Space* thumb key | None — candidate enters `MAC_NAV`/`WIN_NAV` by holding **Backslash** (`LT(MAC_NAV, KC_BSLS)`), not Space, so the thumb Space key is never shadowed and needs no passthrough. This ZMK-side add is specific to B1's own layer-tap mechanism. |
| `layer_m_func` row 3: adds explicit `1 2 3 4 5 6` beside the existing `7 8 9` numpad block, moves `DEL` after `BSPC`, and removes a duplicated `OUT_24G` binding | **No direct carry-over.** Candidate `MAC_FUNC` row 3 is already a deliberate, previously-reviewed substitution: Keychron RGB fine controls (`UG_TOGG/NEXT/VALU/HUEU/SATU/SPDU`, `UG_PREV/VALD/SPDD`) plus the `7 8 9`/`BSPC` numpad block and the Mac screenshot shortcut. B1 has no RGB hardware, so this row was never a literal position copy. Overwriting it with a `1–6` number overlay would remove reviewed, in-use RGB controls for a B1 artifact that was itself a duplicate-binding bug (the removed `OUT_24G`). **Recommendation: do not carry this cell forward.** Record the decision explicitly in the updated candidate record (Step 2) so it is an intentional exclusion, not a silent gap. |

**Conclusion:** no `keymap.c` edits are functionally required to move the
baseline from v0.24 to v0.25. The work in Steps 1–4 below is to (a) confirm
this conclusion under review rather than take it on faith, and (b) re-point
the candidate record at v0.25 as the reference. If a reviewer instead wants
the Mac Fn-layer `1–6` overlay added, treat that as a new, separately reviewed
behavior change (it touches more than 3 physical keys on a protected Fn
layer, so it requires the second independent review per
`REVIEW-PROTOCOL.md`), not a mechanical port of v0.25.

## 1. Prerequisites

- [ ] Physical test unit confirmed as the Keychron V1 Max **ANSI encoder**
      model (not ISO/JIS).
- [ ] WSL `Ubuntu-24.04` prepared per `WSL-Prep.md`: `QMK_HOME` exported and
      pointed at the native copy (`~/src/qmk_firmware-fork-keychron`), `qmk
      doctor` passing.
- [ ] Windows worktree synced to the native WSL copy (see `WSL-Prep.md` §7),
      excluding `.build/` and `.git/index.lock`, without `--delete`.
- [ ] `git status --short` clean (or only the intended candidate edit
      present) in the native WSL copy.

## 2. Update the candidate record

1. Copy `V1-MAX/QC-IN_PROGRESS/v0.24-b1-intent-v1max-ansi.md` to
   `V1-MAX/QC-IN_PROGRESS/v0.25-b1-intent-v1max-ansi.md`.
2. Update its "Intent reference" line to point at
   `ZMK/B1PRO/QC-IN_PROGRESS/v0.25-candidate.keymap`.
3. Add the delta-review table from Step 0 above (or link to this document),
   including the explicit exclusion decision for the Fn-row `1–6` overlay.
4. If `default/keymap.c`, `config.h`, or `rules.mk` are edited at all, recompute
   and record their SHA-256 (`sha256sum keymap.c config.h rules.mk`).
5. If no source file changes, state explicitly: "No source changes;
   v0.25 delta is fully absorbed by the existing v0.24 translation review."
   Keep the existing hashes.

## 3. Run the review gates

Per `REVIEW-PROTOCOL.md`:

1. Deterministic review: confirm `LAYOUT_ansi_82` argument counts (82 per
   layer), layer 0/2 DIP contract intact, no reintroduced B1 position
   numbers.
2. Independent review: required here because the Step 0 conclusion is a
   judgment call about excluded behavior on a protected Fn layer. A second
   reviewer must explicitly concur with the "no carry-over" decision (or
   direct an alternative, separately reviewed change).
3. Record both verdicts in `v0.25-b1-intent-v1max-ansi.md`.

Do not proceed to Step 5 until both verdicts are recorded as pass.

## 4. Commit the reviewed candidate

```bash
git add keyboards/keychron/v1_max/ansi_encoder/keymaps/default/ \
        V1-MAX/QC-IN_PROGRESS/v0.25-b1-intent-v1max-ansi.md
git commit -m "docs(v1-max): re-baseline candidate on B1 v0.25 intent"
```

Only a committed candidate may be built. An edit after this commit
invalidates the review per `REVIEW-PROTOCOL.md` and restarts Step 3.

## 5. Build in WSL

1. Re-sync Windows → WSL (candidate must already be committed):

   ```bash
   rsync -a --info=progress2 \
     --exclude='.build/' \
     --exclude='.git/index.lock' \
     /mnt/d/CODE/VT-Keymaps/VT-QMK-V1Max/qmk_firmware-fork-keychron/ \
     ~/src/qmk_firmware-fork-keychron/
   ```

2. Verify the native copy matches the committed, reviewed candidate:

   ```bash
   cd "$QMK_HOME"
   git status --short   # expect clean
   sha256sum keyboards/keychron/v1_max/ansi_encoder/keymaps/default/keymap.c \
              keyboards/keychron/v1_max/ansi_encoder/keymaps/default/config.h \
              keyboards/keychron/v1_max/ansi_encoder/keymaps/default/rules.mk
   ```

   Compare against the hashes recorded in
   `v0.25-b1-intent-v1max-ansi.md`. Stop if they differ.

3. Build only the reviewed candidate:

   ```bash
   qmk compile -kb keychron/v1_max/ansi_encoder -km default
   ```

   (Equivalent Make form: `make keychron/v1_max/ansi_encoder:default`.)

4. Confirm the build succeeded and locate the artifact — QMK reports the path
   on success, typically:

   ```text
   .build/keychron_v1_max_ansi_encoder_default.bin
   ```

## 6. Record the build artifact

```bash
sha256sum .build/keychron_v1_max_ansi_encoder_default.bin
```

Add the artifact path and hash to `v0.25-b1-intent-v1max-ansi.md`. A
successful compile is not hardware QC — it only proves the artifact exists
and matches the reviewed source.

## 7. Add the CHANGELOG entry (before flashing)

Add an entry to the root `CHANGELOG.md` with: candidate identity (commit
SHA), source hashes, build artifact path/hash, target unit (V1 Max V1M-D4
ANSI encoder), and a **pending** observed-result field. `AGENTS.md` requires
this to exist before any flash — the flash is not evidence until the
changelog and hardware result both exist.

## 8. Flash procedure (STM32 DFU)

The V1 Max ANSI encoder uses `stm32-dfu` (`keyboards/keychron/v1_max/info.json`).
Flashing does not require a separate bootloader flag — QMK's `:flash` target
and `qmk flash` invoke `dfu-util` for you.

1. **Back up recovery knowledge first, not the device**: the bootloader entry
   path below is independent of whatever application firmware is on the
   board, so a bad candidate flash is always recoverable by re-entering DFU
   and reflashing the known-good vendor `keychron` build
   (`qmk compile -kb keychron/v1_max/ansi_encoder -km keychron`, kept from the
   Developer Lab's safe build exercise).

2. Disconnect the USB cable. Toggle the keyboard's mode switch to **Cable**.

3. Enter DFU using either:
   - Hold **Esc** while reconnecting the USB cable, or
   - Press the reset button underneath the **Space bar** while the cable is
     connected.

4. Confirm the host sees the bootloader:

   ```bash
   dfu-util -l
   ```

   Expect an STM32 DFU device to be listed. If nothing appears, re-check
   Cable mode and the DFU entry gesture before proceeding — do not attempt a
   flash without a visible DFU device.

5. Flash the reviewed candidate:

   ```bash
   qmk flash -kb keychron/v1_max/ansi_encoder -km default
   ```

   (Equivalent Make form: `make keychron/v1_max/ansi_encoder:default:flash`.)

6. Wait for the tool to report completion and for the keyboard to
   re-enumerate as a normal USB HID device. Toggle the mode switch back to
   your working connection mode (Bluetooth/2.4G/Cable) as needed.

## 9. Hardware QC checklist

Complete every item below before this candidate can be promoted. Record
pass/fail for each in `v0.25-b1-intent-v1max-ansi.md`:

- [ ] Mac/Windows DIP switch selects `MAC_BASE`/`MAC_FUNC` and
      `WIN_BASE`/`WIN_FUNC` correctly (layers 0/2 contract).
- [ ] Fn access on both sides (`TD(TD_FN_MAC)` / `TD(TD_FN_WIN)`): tap for
      one-shot Macro layer, hold for the Fn layer, both release cleanly.
- [ ] `ESC_CAPS` custom behavior: tap → Esc, hold ≥600 ms → Caps Lock.
- [ ] Home-row mods (`HRM_*`) on both hands: taps produce letters, holds
      produce modifiers, Chordal Hold/permissive-hold behaves as expected on
      same-hand rolls vs. opposite-hand chords.
- [ ] All tap dances: `TD_MINS`, `TD_EQL`, `TD_PAREN`, `TD_BRACE`,
      `TD_BRACK`, `TD_ANGLE` (1-tap/2-tap/3-tap where applicable),
      `TD_BOOT` (3 s hold reaches bootloader).
- [ ] Wireless: all three BT host slots (`BT_HST1-3`), 2.4G (`P2P4G`),
      battery indication (`BAT_LVL`).
- [ ] RGB: toggle, hue/sat/val/speed increase and decrease, on both Mac and
      Windows Fn layers.
- [ ] Encoder: volume on base layers, RGB value on Fn layers, press-to-mute
      (or documented press behavior).
- [ ] Mouse layers (`MAC_NAV`/`WIN_NAV`): movement, wheel, buttons 1–3,
      acceleration steps, plus the Mission Control / window-manager
      shortcuts on that layer.
- [ ] Macro/UAT layers: return-to-base works; UAT layers remain otherwise
      inert as intentionally designed.
- [ ] VIA / raw HID policy behaves as selected (`VIA_ENABLE = yes`); no
      unexpected reset of user-configured state.
- [ ] Boot/DFU recovery re-tested after the candidate is running (Cable +
      Esc, and reset-under-Space), confirming recovery remains independent
      of the flashed application.
- [ ] Normal USB input (non-wireless) confirmed after recovery.

## 10. Promotion

Only after the tester explicitly reports all Step 9 checks pass:

1. Create `V1-MAX/releases/<revision>-<short-description>/`.
2. Move the exact reviewed source bundle (`keymap.c`, `config.h`,
   `rules.mk`), the built artifact and its hash sidecar, and the completed
   `v0.25-b1-intent-v1max-ansi.md` record into it.
3. Update the matching root `CHANGELOG.md` entry with the archived path and
   the observed hardware result.
4. Do not overwrite a reviewed candidate record — each promoted revision gets
   its own directory, matching the existing `releases/v0.23`, `v0.24`
   convention.

## Rollback / recovery

The STM32 DFU bootloader is independent of the application firmware. If the
flashed candidate misbehaves, repeat Step 8's DFU entry (Cable mode +
Esc while connecting, or reset under Space) and flash the last known-good
build — the stock `keychron` keymap, or a previously promoted
release under `V1-MAX/releases/`. This is always available regardless of
what firmware is currently on the board.
