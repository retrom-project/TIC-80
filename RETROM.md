# Retrom Web core

This fork follows upstream main at the exact commit recorded in `retrom-fork.json`. The mirror branch stays upstream-only. Retrom changes live on the maintenance line and feature branches.

Run `.github/rpg-runtime/build-candidate.sh /absolute/empty/directory` with Docker as a non-root user. It builds with pinned Emscripten 4.0.10, runs owned native lifecycle/state regressions, copies `tic80-retrom.mjs`, `tic80-retrom.wasm` and complete license texts, and emits `retrom-core-candidate.json` with source/submodule identities and byte hashes. Cores are built explicitly; the runtime consumes the result without compiling sources.

The ES module default is an Emscripten factory with a per-instance heap. ABI 1 exposes load, step, RGBA pixels, interleaved signed 16-bit stereo samples, readiness, save, restore and stop. Inputs are sampled once per emulated 60 Hz frame. Payloads are limited to 4 MiB. Host envelopes bind state to core and cartridge digest.

TIC-80 renders the active 240 × 136 area and outputs 44.1 kHz audio. Binary `.tic` cartridges are supported; chunk lengths are validated before calling the upstream loader. Compiled language backends: Lua, JavaScript, Wren, MoonScript, Fennel, Squirrel and WebAssembly. Other backends, the studio/editor and multi-cartridge projects are outside this build.

`tic80-pmem-v1` stores exactly 1024 bytes of native persistent memory. It is GAME_SAVE, not an execution snapshot: restore must happen before the first tick/BOOT, and the game decides how to use pmem. A game that never writes pmem has no gameplay progress to restore.

Candidate validation does not publish a release. After real Retrom import, review preview, publishing, Launch and fresh-instance restore acceptance, an explicitly authorized release can populate the declared `rpg-runtime-release.json` asset and pin the final core commit/tag in the runtime. Downloaded third-party games are local test inputs and must not enter Git or release assets.

Formal release tags are annotated and must point to a tested commit on `retrom/g4aba09c98f1e`. The release workflow reruns native regressions, validates the exact Web ABI and asset allowlist, and publishes the release descriptor consumed by retrom-runtime.
