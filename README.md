# Real-Time Rendering 2021 (RTR2021)

[![C](https://img.shields.io/badge/Language-C%20%2F%20C%2B%2B-blue.svg?logo=c%2B%2B)](https://en.wikipedia.org/wiki/C%2B%2B)
[![OpenGL](https://img.shields.io/badge/Graphics-OpenGL%204.6%20%2F%20GLES%203.0%20%2F%20WebGL%202.0-5586a4.svg?logo=opengl)](https://www.opengl.org/)
[![Direct3D 11](https://img.shields.io/badge/Graphics-Direct3D%2011-0078d7.svg?logo=windows)](https://learn.microsoft.com/en-us/windows/win32/direct3d11/atoc-dx-graphics-direct3d-11)
[![CUDA](https://img.shields.io/badge/Compute-NVIDIA%20CUDA-76b900.svg?logo=nvidia)](https://developer.nvidia.com/cuda-toolkit)
[![OpenCL](https://img.shields.io/badge/Compute-OpenCL-3b82f6.svg?logo=khronos)](https://www.khronos.org/opencl/)
[![Platforms](https://img.shields.io/badge/Platforms-Windows%20%7C%20Linux%20%7C%20Android%20%7C%20WebGL%20%7C%20macOS%20%7C%20iOS-black.svg)](#cross-platform-concept-matrix)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

A comprehensive real-time rendering, high-performance computing, and cross-platform systems engineering portfolio developed by **Shubham Jadhav** during the **RTR 2021 (Batch 4)** masterclass mentored by **Dr. Vijay D. Gokhale** at **AstroMediComp**.

This repository documents foundational mathematics, low-level native windowing engines, programmable shader pipelines, hardware-accelerated compute kernels, and standalone graphics capstones written natively from scratch across **6 operating systems**.

---

## Table of Contents

- [Architectural Overview](#architectural-overview)
- [Cross-Platform Concept Matrix](#cross-platform-concept-matrix)
- [Featured Capstone Projects](#featured-capstone-projects)
  - [1. Group Finale Project: PENETRALIA](#1-group-finale-project-penetralia)
  - [2. FFP DS Project: Asa He Chanchal Maan](#2-ffp-ds-project-asa-he-chanchal-maan)
- [Standalone Demos & Effects Portfolio](#standalone-demos--effects-portfolio)
- [High-Performance Computing & GPGPU](#high-performance-computing--gpgpu)
- [Core Foundations & Computer Science](#core-foundations--computer-science)
- [Build and Execution Guide](#build-and-execution-guide)
- [Acknowledgements](#acknowledgements)

---

## Architectural Overview

The repository is structured into five cohesive modules spanning foundations, graphics APIs, heterogeneous compute, and production-grade capstone scenes:

```
RTR2021/
├── 01-Foundations/                 # Multi-language HelloWorld, Pure C curriculum, CS parsers, Art & Math, GLUT
│   ├── 01-HelloWorld/              # C, C++, Python, Java, Win32 SDK, x86/x64 NASM Assembly
│   ├── 02-C/                       # 17 Assignment series, homeworks, and practice programs
│   ├── 03-ComputerScience/         # Recursive Descent Parser for C Identifiers
│   ├── 04-ArtAndMathematics/      # Perspectives (1-point, 2-point), geometric formulae, storyboards
│   ├── 05-Blender/                 # 3D modeling and projection studies
│   ├── 06-GLUT/                    # FreeGLUT implementations (Windows & Linux)
│   └── 07-TemplatesAndClassDemos/  # Native window templates, rotating cameras, viewport slicing
│
├── 02-OpenGL/                      # Multi-platform native OpenGL implementations
│   ├── 01-Windows/                 # Win32 SDK foundations, Fixed Function (FFP), Programmable Pipeline (PP)
│   ├── 02-Linux/                   # Native Xlib/GLX windowing, FFP, and Programmable Pipeline (PP)
│   ├── 03-Android/                 # NDK, Native Activity, Java lifecycle, OpenGL ES 3.0 (42 assignments)
│   ├── 04-WebGL/                   # HTML5 Canvas, JavaScript, WebGL 2.0 (36 assignments)
│   ├── 05-macOS/                   # Cocoa, Objective-C++, OpenGL 4.1 Core Profile (39 assignments)
│   └── 06-iOS/                     # UIKit, GLKit, Objective-C++, OpenGL ES 3.0 (35 assignments)
│
├── 03-DirectX/                     # Native Microsoft DirectX 11 pipeline
│   └── Direct3D11/                 # 37 progressive HLSL/C++ assignments (Device, SwapChain, OM, VS, PS, GS, HS, DS)
│
├── 04-HighPerformanceComputing/    # Heterogeneous Parallel Computing & Graphics Interop
│   ├── CUDA/                       # Vector addition, matrix multiplication, device query
│   ├── OpenCL/                     # OpenCL kernel dispatch across CPU/GPU devices
│   └── OpenGL-Interop/             # Zero-copy GPU compute-to-graphics sine wave animation (CUDA & OpenCL)
│
└── 05-ProjectsAndDemos/            # 36 Standalone Mini-Projects, Capstones & Effects
    ├── 01-WorldClock to 32-Study/  # Standalone effects: Grass, Terrain, Starfield, Ocean, NURBS, Water, Fog
    ├── 33-BirthdayDemo_YogeshDada/ # Interactive 3D birthday celebration demo
    ├── 34-BirthdayDemo_VipinDada/  # Cryptex puzzle & historical palace scene
    ├── 35-FFP_DSProject_.../       # "Asa He Chanchal Maan" FFP hierarchical animation capstone
    ├── 36-GroupFinale_.../         # "PENETRALIA" RTR4 HULL Group Finale Production
    └── Study_LearnOpenGL/          # Modern PBR, Bloom, Model Loading reference implementations
```

---

## Cross-Platform Concept Matrix

Every rendering concept is implemented natively using the platform's respective windowing system and shader pipeline:

| Rendering Concept / Milestone | Windows Win32 | Linux Xlib/GLX | Android NDK | WebGL 2.0 | macOS Cocoa | iOS UIKit | Direct3D 11 |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Native Window & Event Loop** | `01-SDK/01` | `01-SDK/01` | `01-09` | `01-05` | `01-06` | `01-04` | `01-02` |
| **Orthographic Projection** | `PP/03` | `PP/03` | `10` | `06` | `07` | `05` | `03` |
| **Perspective Projection** | `PP/04` | `PP/04` | `11` | `07` | `08` | `06` | `04` |
| **Colored 2D Geometry** | `PP/05-09` | `PP/05-09` | `12-16` | `08-12` | `09-13` | `07-11` | `05-11` |
| **2D / 3D Rotations (Euler)** | `PP/10-15` | `PP/10-14` | `17-21` | `13-17` | `14-18` | `12-16` | `12-18` |
| **Texture Mapping & Sampling** | `PP/16-19` | `PP/15-18` | `22-25` | `18-21` | `19-22` | `17-20` | `19-24` |
| **Procedural Checkerboard** | `PP/18` | `PP/17` | `24` | `20` | `21` | `19` | `21` |
| **Diffuse Lighting (Lambertian)**| `PP/21-23` | `PP/20-22` | `27-29` | `23-25` | `24-26` | `22-24` | `26-28` |
| **Per-Vertex Lighting (Gouraud)**| `PP/24` | `PP/23` | `30` | `26` | `27` | `25` | `29` |
| **Per-Fragment Lighting (Phong)**| `PP/25` | `PP/24` | `31` | `27` | `28` | `26` | `30` |
| **Per-Vertex / Per-Fragment Toggle** | `PP/26` | `PP/25` | `32` | `28` | `29` | `27` | `31` |
| **Multi-Light Simulation (2 Lights)** | `PP/27` | `PP/26` | `33` | `29` | `30` | `28` | `32` |
| **Multi-Light Simulation (3 Lights)** | `PP/28` | `PP/27` | `34` | `30` | `31` | `29` | `33` |
| **Material Properties (24 Spheres)** | `PP/29` | `PP/28` | `35` | `31` | `32` | `30` | `34` |
| **Hierarchical Matrix Stacks** | `PP/31-32` | `PP/30-31` | `37-38` | `33-34` | `34-35` | `32-33` | — |
| **3D Model Loading (Assimp/OBJ)** | `PP/33` | `PP/32` | — | — | — | — | — |
| **Tessellation Shaders (Hull/Domain)**| `PP/34` | `PP/33` | `39` | — | `36` | — | `35` |
| **Geometry Shader (Amplification)** | `PP/35` | `PP/34` | `40` | — | `37` | — | `36` |
| **Interleaved Vertex Buffers** | `PP/36` | `PP/35` | `41` | `35` | `38` | `34` | `37` |
| **Framebuffer Objects (FBO / RTT)** | `PP/37` | `PP/36` | `42` | `36` | `39` | `35` | — |

---

## Featured Capstone Projects

### 1. Group Finale Project: PENETRALIA
> **Directory**: [`05-ProjectsAndDemos/36-GroupFinale_RTR4HullProject_PENETRALIA/`](05-ProjectsAndDemos/36-GroupFinale_RTR4HullProject_PENETRALIA/)  
> **Team HULL**: Shubham Jadhav, Amit Sambhaji, Anusha Gugle, Ashwini Bhutare, Dnyaneshwari Mundhe, Shekhar Shinde, Varad Lanke, Vipin Kodag, Yogesh Ahire.

PENETRALIA is the culmination capstone of RTR 2021—a full-scale, synchronized multi-scene cinematic real-time rendering engine featuring:
- **CUDA-Accelerated Ocean FFT Simulation**: High-frequency Phillips spectrum ocean waves generated using `cufft` (Fast Fourier Transforms) with dynamic normal calculation and Jacobians.
- **Tessellated Terrain Displacement Mapping**: Multi-LOD terrain rendering driven by 16-bit heightmaps and procedural splatmaps.
- **Atmospheric Scattering & Sky Rendering**: Rayleigh and Mie physical atmospheric light scattering equations integrated in real-time GLSL shaders.
- **Post-Processing Pipeline**: Two-pass Gaussian Bloom, HDR tone mapping, and Multisample Anti-Aliasing (MSAA) using custom Framebuffer Objects (FBO).
- **Procedural Particle Systems**: Directional and sideways rain simulation, flow-mapped vegetation/grass instancing.
- **Hierarchical Scene Synchronizer**: Central clock synchronizing camera fly-throughs, light transitions, and 3D audio tracks.

### 2. FFP DS Project: Asa He Chanchal Maan
> **Directory**: [`05-ProjectsAndDemos/35-FFP_DSProject_AsaHeChanchalMaan/`](05-ProjectsAndDemos/35-FFP_DSProject_AsaHeChanchalMaan/)

A comprehensive Fixed Function Pipeline graphics and data structures production:
- Complete scene architecture rendered without programmable shaders, executing complex matrix hierarchies manually via `glPushMatrix` and `glPopMatrix`.
- Custom mathematical Bézier curves and trajectory interpolations for animated eagle flight, flowing river geometry, mountain terrain, and foliage.
- Full texture mapping, multi-level coordinate transformations, and synced audio narration.

---

## Standalone Demos & Effects Portfolio

A rich suite of 34 standalone specialized mini-projects and effects developed during research:

| ID | Project / Demo Name | Core Graphics & Mathematical Techniques |
| :---: | :--- | :--- |
| `01` | **WorldClock** | Dynamic time-zone geometric clock with mathematical hand rotation calculations |
| `02` | **SolarSystem** | Hierarchical orbital mechanics, planetary spin, tilt, and multi-texturing |
| `03-04` | **Static & Dynamic India** | Dynamic typography, Bézier curve aircraft trajectory, tricolor particle smoke trails |
| `05` | **Fulpakhru** | Biomechanical flapping wing animation with rhythmic sinusoids and alpha blending |
| `06` | **Fog** | Classic OpenGL depth-buffer exponential and linear fog density modeling |
| `07` | **Arrow** | 3D cubic trajectory modeling with dynamic orientation vector calculations |
| `08` | **Cryptex** | Multi-cylinder mechanical lock simulation with independent rotary dials |
| `09` | **Tree_Bamboo** | Cylindrical instancing with segmented stalk textures and leaf geometry |
| `10` | **CubeMap** | Seamless 6-face environment mapping for skyboxes and reflection probes |
| `11` | **Eagle** | Multi-component articulated bird model with synchronized wing flap cycles |
| `12` | **Humanoid** | Articulated skeletal hierarchy with joint limits and limb matrix stacks |
| `13` | **NURBS_Surface** | Non-Uniform Rational B-Splines surface generation and evaluator pipelines |
| `14` | **FadeInOut** | Framebuffer alpha transitions and smooth luminance interpolation |
| `15` | **Font** | Bitmap and outline font glyph rasterization in 3D world space |
| `16-17` | **Hut & Trees** | Architectural spatial modeling and procedural forest distribution |
| `18` | **Water** | Dynamic sinusoidal wave displacement with specular highlight reflections |
| `19` | **Tree_Main** | Recursive procedural fractal tree generation with branching heuristics |
| `20` | **Effect_Grass** | Vertex-shader wind displacement animation with flowmap perturbation |
| `21-22` | **Terrain Displacement & HeightMap** | Elevation mapping from 16-bit textures using normal reconstruction |
| `23` | **Effect_StarField** | Compute/point sprite 3D starfield warp simulation with perspective scaling |
| `24` | **Effect_ParticleSystem** | GPU-friendly particle emitters with life-cycle tracking and velocity dampening |
| `25` | **Effect_DistanceFog** | Shader-based atmospheric distance attenuation and fog color bleeding |
| `26` | **Effect_MultiTexturing** | Multi-unit texture blending using blend masks and detail normal maps |
| `27` | **Effect_OceanSimulation** | Fast ocean water mesh simulation with specular sun reflection highlights |
| `28-29` | **WebGL EndCredit & Room** | WebGL 2.0 browser-based interactive room and textured typography scene |
| `30-32` | **Graphics Study Series** | Reference implementations from OpenGL Red Book 9, Orange Book 3, and WebGL |
| `33-34` | **Birthday Demos** | Interactive tribute demos with Cryptex puzzle mechanics and particle effects |

---

## High-Performance Computing & GPGPU

Located in [`04-HighPerformanceComputing/`](04-HighPerformanceComputing/), these modules explore data-parallel algorithms and hardware interop:

- **NVIDIA CUDA**:
  - Grid, Block, and Thread hierarchy optimization for parallel array manipulation.
  - Tiled matrix multiplication utilizing on-chip Shared Memory to maximize memory bandwidth.
  - Comprehensive device query utilities inspecting CUDA compute capability, warp sizes, and SM counts.
- **Khronos OpenCL**:
  - OpenCL context, command queue, program build, and kernel execution lifecycle.
  - Device dispatch targeting heterogeneous architectures (CPUs and discrete GPUs).
- **Zero-Copy Compute-to-Graphics Interop**:
  - Direct sharing of OpenGL Vertex Buffer Objects (VBO) with CUDA (`cudaGraphicsGLRegisterBuffer`) and OpenCL (`clCreateFromGLBuffer`).
  - Dynamic GPU sine wave calculation mutating 3D vertex positions entirely in VRAM without CPU-GPU readback bottlenecks.

---

## Core Foundations & Computer Science

Located in [`01-Foundations/`](01-Foundations/):
- **Systems Hello World**: Low-level Hello World implementations in pure C, modern C++, Python, Java, native Win32 SDK, and raw x86_64 NASM Assembly.
- **Master C Curriculum**: Over 17 structured assignment series covering pointers, pointer-to-pointer indirection, memory management (`malloc`, `calloc`, `realloc`), bitwise manipulations, custom struct layouts, and file I/O.
- **Recursive Descent Parser**: A compiler frontend parser written in C that tokenizes and verifies grammar for C identifiers using syntax trees.
- **Art & Mathematics**: Studies in vanishing point convergence (1-point and 2-point perspective projections), hand-shaded 3D emoji light studies, and complete storyboard/screenplay design.

---

## Build and Execution Guide

### Windows (Win32 & DirectX 11)
Open the **x64 Native Tools Command Prompt for VS**:
```cmd
cd 02-OpenGL\01-Windows\02-PP\01-OpenGLExtensions
build.bat
OGL.exe
```

### Linux (Xlib / GLX)
Ensure OpenGL and X11 development headers are installed:
```bash
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev libx11-dev
cd 02-OpenGL/02-Linux/02-PP/01-OpenGLExtension
./build.sh
./OGL
```

### Android
Open the target directory in **Android Studio** (or build via Gradle wrapper):
```bash
cd 02-OpenGL/03-Android/09-Bluescreen
./gradlew assembleDebug
```

### WebGL
Host the project directory using any static HTTP server (e.g., Python):
```bash
cd 02-OpenGL/04-WebGL/05-Bluescreen
python3 -m http.server 8080
# Open http://localhost:8080 in any WebGL 2.0 supported browser
```

### macOS & iOS
- **macOS**: Compile via the included shell script or open in Xcode:
  ```bash
  cd 02-OpenGL/05-macOS/06-Bluescreen
  ./build.sh
  open OGL.app
  ```
- **iOS**: Open the `.xcodeproj` file inside Xcode, select an iOS Simulator or connected device, and press **Run** (Cmd + R).

---

## Acknowledgements

- **Dr. Vijay D. Gokhale**: For his visionary teaching, unwavering mentorship, and for instilling the values of clean systems engineering and real-time graphics architecture.
- **AstroMediComp**: For conducting the transformative Real-Time Rendering 2021 program.
- **Mrs. Gokhale Ma'am**: For teaching the foundational principles of art, light, perspective, and screenplay design.
- **Team HULL**: To all team members of the PENETRALIA group finale for their collaboration and shared passion for graphics programming.
