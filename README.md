# Terminal Minecraft

A voxel engine that renders **entirely in the Windows terminal - no GPU**. The whole graphics pipeline runs on the CPU: a software rasterizer written from scratch in **C++17**, feeding a procedurally generated, infinite (chunk-streamed) world with terrain, biomes, rivers, caves and trees.

The core trick: it uses the half-block character `▄` (U+2584) with independent foreground/background colors to **double the vertical resolution**, each terminal cell becomes two vertical pixels.

Built from scratch as a personal deep-dive into real-time rendering, concurrency and low-level performance. It expands on the earlier [2D Console Graphics Engine](https://github.com/PedroDNBR/2d-console-engine-project).

---

## Why this project is interesting

There's no engine and no graphics library doing the heavy lifting. Every stage of the pipeline - projection, clipping, rasterization, culling, meshing, world generation and the threading that drives it, is implemented by hand. That makes it a from-first-principles exercise in exactly the areas that matter for engine and rendering work.

**Tech stack**
- **Language:** Modern C++ (C++17) - `constexpr`, `unique_ptr`/`shared_ptr`, templates, RAII
- **Platform:** Windows Console API (`WriteConsoleOutput`, `CHAR_INFO`), `QueryPerformanceCounter` for profiling
- **Dependencies:** none for graphics - everything implemented from scratch

---

## Rendering - software rasterizer from scratch

A full CPU rasterization pipeline, no GPU involved:

- Perspective projection with world → camera → screen space transforms
- Near-plane clipping
- Depth buffer (z-buffer) for correct occlusion
- Scanline polygon fill
- Backface culling and frustum-plane culling to discard invisible geometry before rasterization
- Output to the terminal via half-block characters (`▄`) with fg/bg colors, doubling effective vertical resolution

## Voxel meshing

- **Greedy meshing** - merges coplanar adjacent block faces into larger rectangles, drastically cutting the quad count sent to the rasterizer.

## Procedural world generation

- Custom noise from scratch: **value noise**, smooth noise (bilinear interpolation), and **fractal noise (FBM)** with octaves
- Multi-field heightmap (continentalness, erosion, peaks, valleys) with amplitude/frequency calibration
- **Biomes** driven by temperature and humidity fields
- **Rivers** via curl noise (two converging fields)
- **Caves** from trilinearly-interpolated 3D noise, plus **worm caves** (directed random walk carving tunnels)
- Deterministic tree placement

## Concurrency / multithreading

- Worker **thread pool** for parallel chunk generation and meshing
- **Lock-free MPSC queue** for delivering generated chunks back to the main thread
- `std::shared_mutex` (shared/exclusive locks) over the chunk map, with condition variables and thread-safe queues
- Meshing runs **outside the lock** using `shared_ptr` snapshots, eliminating contention
- **Deterministic PRNG** based on spatial hashing (prime multiplication + avalanche mixing) with a "bit budget" to extract multiple independent values from a single hash - guarantees identical regeneration of any chunk, which is essential in a multithreaded context

---

## Engineering problems solved

The most representative part of the project - the debugging and optimization work, done with tools rather than intuition:

- **Heap-buffer-overflow**, diagnosed with **AddressSanitizer**, traced back to an unclamped negative height corrupting the heap.
- **Data race** on a `shared_ptr` inside a shared `unordered_map` accessed from multiple threads - an intermittent crash surfacing in `shared_ptr`'s refcount decrement (`_Decref`), fixed by correcting the sharing model.
- **Lock contention** causing micro-stutters, resolved by restructuring the meshing pipeline to run outside the lock.
- **Data-oriented / cache optimization:** hot/cold path separation, memory layout (row-major, iteration order vs. cache lines), struct packing (compact quad from **64 → ~7 bytes**), and a contiguous list of visible meshes instead of iterating an `unordered_map`.
- **Profiling with `QueryPerformanceCounter`** to time each frame stage and find the real bottleneck - the terminal output syscall - instead of optimizing by guesswork.

---

## Skills demonstrated

- Low-level graphics (rasterization, projection, culling, z-buffer) with no libraries
- Concurrent programming (locks, lock-free queues, thread-safe design)
- Profiling-driven performance work with CPU-architecture awareness (cache, memory layout)
- Memory and concurrency debugging with professional tooling (ASan)
- Applied math (procedural noise, interpolation, trigonometry, vector algebra)
- Idiomatic C++ and systems design

---

## Building

Open `terminal-minecraft.sln` in Visual Studio (MSVC, C++17) and build. Run from a Windows terminal. To maximize performance, run it on Release mode

> Movement: W A S D

> Camera Look: I J K L

> Fly up: Space bar

> Fly down: Q

---

## Project structure

```
Engine/          Core engine systems
Renderer/        Software rasterizer and terminal output
World/           Chunk generation, meshing, noise, biomes
Game/Entities/   Game-side entities
main.cpp         Entry point
```

---

## Development progress

Screenshots from the build history, in rough chronological order:

<table>
  <tr>
    <td align="center">
      <strong>Initial rendering test</strong><br>
      <img src="https://i.imgur.com/VjWnsK7.png" width="100%">
    </td>
    <td align="center">
      <strong>Vertex projection</strong><br>
      <img src="https://i.imgur.com/awexG8D.png" width="100%">
    </td>
  </tr>
  <tr>
    <td align="center">
      <strong>Face projection and scan line</strong><br>
      <img width="100%" src="https://i.imgur.com/cmHNZ2z.png" />
    </td>
    <td align="center">
      <strong>Initial chunk generation</strong><br>
      <img width="100%" src="https://i.imgur.com/c3NivvE.png" />
    </td>
  </tr>
   <tr>
    <td align="center">
      <strong>Local chunk occlusion culling</strong><br>
      <img width="100%" src="https://i.imgur.com/au3v6qx.png" />
    </td>
    <td align="center">
      <strong>Basic pseudorandom terrain elevation</strong><br>
      <img width="100%" src="https://i.imgur.com/lQr4VIF.png" />
    </td>
  </tr>
    <td align="center">
        <strong>Chunk generation around player position</strong><br>
        <img width="100%" src="https://i.imgur.com/PWCt7qK.png" />
      </td>
      <td align="center">
        <strong>Chunk unloading</strong><br>
        <img width="100%" src="https://i.imgur.com/b4oqJHg.png" />
      </td>
    <tr>
    </tr>
      <td align="center">
        <strong>Greedy Meshing and Wireframe Rendering</strong><br>
        <img width="100%" src="https://i.imgur.com/yYs1X2t.png" />
      </td>
      <td align="center">
        <strong>Improved multithreading and terrain generation overhaul</strong><br>
        <img width="100%" src="https://i.imgur.com/tCX8vio.png" />
      </td>
    <tr>
    </tr>
      <td align="center">
        <strong>Tree Generation</strong><br>
        <img width="100%" src="https://i.imgur.com/VTrYyGo.png" />
      </td>
       <td align="center">
        <strong>Worm caves carving</strong><br>
        <img width="100%" src="https://i.imgur.com/IMvC2rt.png" />
      </td>
    <tr>
    </tr>
      <td align="center">
        <strong>Biome definition based on temperature and humidity</strong><br>
        <img width="100%" src="https://i.imgur.com/Kg2gliT.png" />
      </td>
      <td align="center">
        <strong>VT rendering with full RGB Colors and compiled time color palette generation</strong><br>
        <img width="100%" src="https://i.imgur.com/I0yFRJG.png" />
      </td>
    <tr>

    
</table>
<br/>
