# ZMK container build: Keychron B1 Pro

See [Build QMK and ZMK firmware in containers](CONTAINER-FIRMWARE-BUILDS.md)
for the maintained, portable setup, build, artifact verification, and failure
procedures. Run the commands in Bash (WSL2 on Windows).

The build uses the existing v0.25.8 candidate with board `keychron` and shield
`keychron_b1_us`. It produces `build_output/zmk/zmk_b1pro.uf2` and a SHA-256
sidecar. The previous Windows-only staging paths and unconditional historical
checksum comparison have been superseded by that procedure.
