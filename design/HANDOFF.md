# Keymap Explorer redesign — handoff from Cowork (2026-09-28)

Context carried over from a Claude Cowork design session. Read this, then `design/prototype/index.html`
(open it in a browser) and the screenshots in `design/screenshots/`.

## Why
- Joint effort with **Keychron**: showcase their newest keyboards, show adoption in the enthusiast community.
- Users know **VIA**. Stay familiar, fix where VIA fails.
- Core problem: one key can have 4–5 functions (tap, shift, double/triple tap, hold, long-press, per layer, combos). Current site can't communicate that.

## Hard constraints
- **Use Keychron's original SVGs** (`docs/assets/b1pro/KcB1Pro_ANSI_*.svg`, viewBox `0 0 388 173`) as board art. Legends are an overlay using `GRID_B1PRO` coords. Prototype draws its own board only because the cloud session couldn't read these files — replace that part.
- **Support all colorways** (`COLORWAYS` in `docs/js/engine/ui.js`): Space Gray (PSG), Ivory White (PI), Retro Red + Retro Blue (both PRetro, differ via CSS on `.esc-ent-group` / `.utility-group`). Legend colors need per-colorway tokens (ivory = dark legends).
- Layer **name first**; index number secondary or hidden ("Base 0" confuses normal users).

## Visual style
- Glass-cockpit, image-led, uncluttered. Keyboard render = hero (~70% first viewport).
- **No card soup.** No bordered/rounded/shadowed layout containers, never nested. One recessed panel only (the context panel under the board). Separate regions with spacing, alignment, 1px low-contrast hairlines. Radius only on physical/interactive objects (keycaps, swatches, buttons).
- Group with typography: small uppercase mono labels. Fonts in prototype: Bricolage Grotesque (display), IBM Plex Sans (body), IBM Plex Mono (data).

## Layout (VIA-familiar)
1. Header line: title, keyboard / OS / release selectors (underlined, borderless), "Get this layout" button tinted by colorway.
2. Instrument strip above board: layer tabs (color dot + name, underline active; hover = how to reach) | lens control.
3. Board (hero).
4. Strip below board: annunciators (Layer, Held keys, Mods order, Shift, Peek — small LEDs) | colorway livery chips.
5. Context panel (only panel): tabs Key / How to read a key / Flash. Later align to VIA words: Keymap, Macros, Lighting, Save·Flash. Keycode picker groups (when editing arrives): Basic, Media, Macro, Layers, Special, Lighting.

## Keycap anatomy (fixed positions)
| Zone | Gesture | Style |
|---|---|---|
| Center | Tap | largest; tap-to-layer `⇒ Layer` in layer hue |
| Top-right | Shift + tap | blue |
| Top-left | Multi-tap | pink, superscript count `²)` `³()` |
| Top-left (alt) | One-shot layer | `⇢Macro` in target layer hue |
| Bottom band | Hold | gray = modifier; filled with layer hue = layer |
| Bottom band, striped | Long-press | time + result (`3 s ▸ Bootloader`) |
| Dashed bridge between keys + pill | Combo | pill shows hold time (`3 s ⇄`) |

Two color systems never mix: **layer hues** (Base gray, Fn teal, Sym amber, Nav violet, Macro lime) only for layer bands/tabs/held keys; **gesture hues** (shift blue, multi-tap pink) only for corner legends and glyphs. Never recolor Keychron's plate.

Gesture glyphs = Morse-style timing diagrams: tap 1 pulse, 2×/3× pulses, hold long bar + tapping-term tick, long-press faded bar + end tick, shift low bar + pulse, one-shot pulse + dashed arrow, combo two parallel bars + end tick.

Key states per layer: transparent = outline + 40% ghost of base legend; no action = flat, no legend; held (layer activator) = filled with layer hue, pressed, "held".

## Interactions
- Hover inspects, click pins. **Hold ≥230 ms on a layer-hold key = peek** that layer until release (mirrors hold-tap).
- Lenses: All / Tap / Shift / Multi-tap / Hold / Combo with counts. Single lens shows only that output, dims rest. Physical Shift key = temporary Shift lens.
- Context panel Key tab: gesture rows [glyph | name + one-line description | output chip] + "Same key, every layer" rows [layer dot | name + how to reach | resolved output + gesture chips]; click row → switch layer.
- Combo rows have "Try it" → board flashes 3× then applies effect.

## Combo: modifier swap (planned firmware feature)
- Hold **Left ⌥ + Left ⌘ for 3 s** → lights flash 3×, ⌥ and ⌘ swap (Mac order ∧ ⌥ ⌘ ⇄ Windows order Ctrl Win Alt). Replaces dedicated Mac/Win layers.
- UI: dashed bridge + `3 s ⇄` pill; swapped keys marked `⇄`; "Mods: Mac/Win order" annunciator.
- **Open:** does right ⌘ swap too? Base layer only?

## Data model change
Stop storing gestures as free-text notes (`n: "Double-tap: pair BT 1"`). Per key:
`t` tap, `sh` shifted, `x2`/`x3` multi-tap, `h` modifier hold, `hl` layer hold, `lp`+`lpt` long-press, `to` tap→layer, `os` one-shot layer, `n` note.
Combos separate: `{keys:['5-1','5-2'], layer:'base', hold:'3 s', out:'Swap ⌘ ⇄ ⌥', fx:'Lights flash 3×', n}`.
Firmware-agnostic; compile/display to QMK / ZMK / Kanata syntax as secondary text.

## Where VIA fails → our answer
1. Not the exact layout → vendor SVG + per-board grid; ZMK physical-layout `keys` data where available.
2. Colorways → exact SVG per colorway, remembered per device, incl. knob.
3. Knobs (V1 Max) → draw real knob at true position, ring labels CCW / CW / press for active layer; dial diagram in panel; "same knob, every layer" list. Source: `assets/v1_max_ansi_encoder.json`.
4. Layer keys confuse beginners → plain verbs first ("Hold for Symbols", "Tap to switch to Fn", "Next key only", "Tap key / hold layer"), firmware syntax (`MO(2)`, `&mo SYM`) secondary; add **Layer Map** view (Base → layers, arrows labeled by key + gesture); peek + held keys teach by doing.
5. Weak bridge firmware↔UI → one model, plain language primary.
6. Plug-in-first → preview-first; connection = annunciator, not gate. ZMK Studio: BLE only on Linux/native apps, else USB; needs `&studio_unlock` — highlight that key on the board.

## Prototype status
- `design/prototype/index.html` — v2 (cardless cockpit). Self-contained; uses B1 Pro macOS v0.24 layer data restructured to the new model.
- `design/prototype/v1-index.html` — v1 (card-heavy, for contrast only; do not copy its surfaces).
- Placeholders: download disabled; flash step 3 copy generic; retro colorway plate/cap colors approximated.

## Suggested next steps
1. Plan (don't code yet): map prototype concepts onto `docs/js/engine/{renderer,ui,store}.js`; SVG board + overlay.
2. Migrate `docs/js/layers/*.js` to structured gesture fields + `COMBOS`.
3. Add V1 Max with knob.
4. Layer Map view.
Track via `backlog` CLI per CLAUDE.md.
