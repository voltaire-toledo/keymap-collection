# Keymap Review Protocol

## Review identity

The candidate SHA-256 is the immutable review identity. Any keymap edit after
a review invalidates that verdict and requires deterministic validation and
review again.

## Review count

- Use one senior Keychron/ZMK review after deterministic validation by
  default.
- Require a second independent review for protected direct controls, physical
  Fn/boot/reset behavior, layer structure, combos, shared/runtime code or
  behavior, or changes to three or more physical positions.

## Required checks

Each reviewer must inspect and record:

1. Candidate path and SHA-256.
2. Most recent QA-passed baseline path, revision, hash, and source identity.
3. Exact candidate-to-baseline diff by zero-based physical position.
4. All behavior definitions, combos, includes, constants, and formatting-only
   changes.
5. Layer count and ordered binding count for every layer.
6. The complete direct-control map at positions 77 through 81 on every layer.
7. Every candidate-related code-file change outside the keymap, including an
   explicit statement when firmware source and shared DTS code are unchanged.
8. Proposed isolated build-input provenance and hash check.
9. A final PASS or BLOCK verdict with unresolved findings.

## Gate

A candidate is available for user approval only when deterministic validation
passes and every required reviewer passes the same candidate hash with no
unresolved finding. Compilation remains blocked until the user approves that
exact reviewed hash.
