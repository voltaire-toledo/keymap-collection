#pragma once

// Preserve the B1 Pro intent for bilateral home-row modifiers: same-hand rolls
// stay taps, while opposite-hand nested presses can resolve as modifiers.
#define CHORDAL_HOLD
#define PERMISSIVE_HOLD_PER_KEY
#define TAPPING_TERM_PER_KEY
#define QUICK_TAP_TERM_PER_KEY
#define FLOW_TAP_TERM 150
#define ONESHOT_TIMEOUT 1000
#define DYNAMIC_KEYMAP_LAYER_COUNT 12
