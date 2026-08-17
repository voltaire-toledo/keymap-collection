# Product Requirements Document (PRD): Kanata to QMK Migration

## 1. Objective
To translate an advanced Kanata software-level keyboard configuration into a native QMK firmware implementation. This ensures hardware-level portability while maintaining the precise ergonomic and performance characteristics of the original setup.

## 2. Target Audience & Constraints
- **Hardware:** 65%+ layout keyboards (e.g., Keychron V1 Max).
- **OS Target:** macOS (primary).
- **Constraints:** Maximum 8 QMK Layers available.

## 3. Core Features & UX Goals
1. **Home Row Mods (HRM):** Seamless modifier access (Ctrl, Opt, Cmd, Shift) on the home row without sacrificing typing speed. Must feel snappy and resist accidental misfires during fast bursts.
2. **SpaceFn (Layer Toggle):** Spacebar must function as a standard space on tap, but instantly toggle the Navigation layer when held and combined with another key.
3. **Bilateral Symbol Layers:** Holding the left index finger should reveal symbols on the right hand, and vice versa.
4. **Mac-Centric Nav Layer:** Left-hand cluster for Undo/Cut/Copy/Paste, right-hand cluster for Vim-style inverted-T arrows (IJKL).
5. **Muggle Mode:** A fallback plain-text mode (no modifiers on alpha keys) for gaming or guest use.

## 4. Success Criteria
- The engineering team successfully flashes a QMK firmware that behaves identically to the `4review.kbd` Kanata profile.
- Typing speed is unaffected by HRM misfires.
- All macros (e.g., `000`, `R$`) function natively.
