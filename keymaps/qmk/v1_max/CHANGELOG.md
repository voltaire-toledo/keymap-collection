# Keychron V1 Max Firmware Changelog

No V1 Max firmware has been flashed or hardware-qualified in this project.
The B1 Pro/ZMK history is archived under `V1-MAX/legacy-b1pro-zmk/` and must
not be represented as a V1 release.

## Entry format

Create an entry before flashing a reviewed V1 Max candidate:

```markdown
## <revision> - YYYY-MM-DD

- Candidate source: `<repo-relative path>`
- Candidate SHA-256: `<hash>`
- Source commit: `<commit>`
- QMK target: `keychron/v1_max/<variant>:default`
- Build artifact: `<repo-relative path>`
- Build artifact SHA-256: `<hash>`
- Test unit: `<physical variant and identifier>`
- Flash method: STM32 DFU
- Observed result: pending
```
