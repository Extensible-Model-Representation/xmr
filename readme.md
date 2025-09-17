# Extensible Model Representation (XMR)

### Project Requirements
- git ≥ 2.43.5  
- CMake ≥ 3.25.0  
- C++ ≥ 20  

---

### Building the Project
1. Configure:
```bash
cmake -S ./xmr -B ./build
```
2. Build:
```bash
cmake --build ./build
```

---

### Running the Tool
After building, an **XMR** binary will be in `./build`. Run it with:

```bash
./build/XMR -f <model-input>
```

- **`-f <path>` (required):** Path to the model metadata input file.  
- If no other options are supplied, XMR assumes a **Papyrus UML XMI** input and generates **C++**.  
- The output defaults to `a.cpp` in the current working directory.

#### Optional arguments
- **`-o <path>`:** Set a custom output file name.  
- **`-g <shared-lib>`:** Load a custom **generator** at runtime. The library must export `create_generator` and `destroy_generator` entry points that construct/destroy an `IGenerator` instance.  
- **`-p <shared-lib>`:** Load a custom **parser** at runtime. The library must export `create_parser` and `destroy_parser` entry points that construct/destroy an `IParser` instance.  

---

### Quick Usage Examples (CLI only)
- **Default Papyrus XMI → C++ (to `a.cpp`):**
```bash
./build/XMR -f MySystem.xmi
```
- **Custom output file:**
```bash
./build/XMR -f MySystem.xmi -o out/SystemImpl.cpp
```
- **Custom parser and generator plugins:**
```bash
./build/XMR -f model.any -p ./parsers/libmyparser.so -g ./generators/librustgen.so -o out/main.rs
```

---

### Project Background (Thesis)
XMR is based on the Santa Clara University senior thesis **“XMR: Extensible Model Representation”** by **Jason Cisneros** and **Lucas Van Der Heijden** (Date of Award: **June 10, 2025**). The work targets **safety-critical domains (e.g., aerospace)** where V&V is essential, and manual alignment between validated high-level models and low-level implementations is **tedious, time-consuming, and error-prone**.  

XMR provides an **open-source** path to **automatically generate code directly from UML models**, reducing human error and cost, and improving configurability compared to proprietary toolchains such as **MATLAB/Simulink**.

📄 [Read the thesis here](https://scholarcommons.scu.edu/cseng_senior/342/)  

> **Recommended citation:**  
> Cisneros, Jason and Van Der Heijden, Lucas, “XMR: Extensible Model Representation” (2025). *Computer Science and Engineering Senior Theses*, 342.  

