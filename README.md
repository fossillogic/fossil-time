# **Fossil Time by Fossil Logic**

Fossil Time is a modern, portable C library built for lossless, precision-aware time and date computation. It models time as an explicit, maximally expressive structure—from calendar fields to SI sub-second units down to yoctoseconds—governed by a precision mask that allows partial, fuzzy, or fully scientific timestamps. Its modular API provides reliable sleep routines, robust parsing and formatting, leap-year and weekday logic, monotonic timers for accurate interval measurement, and explicit types for durations and spans. String-based identifiers for formats, units, and operations provide clarity, safety, and extensibility—eliminating fragile enum coupling while enabling dynamic reasoning and AI-assisted workflows. Designed for embedded systems, servers, CLI tools, and high-integrity applications, Fossil Time emphasizes deterministic behavior, composability, and maintainable code without sacrificing human readability or scientific rigor.

## Key Features

1. **Lossless, Precision-Aware Time Model**  
    Represents time with a maximally expressive structure, from calendar fields down to SI sub-second units (milliseconds to yoctoseconds), preserving exact precision without hidden rounding.

2. **Precision Mask Semantics**  
    Every field is governed by a bitmask that explicitly defines what is known, enabling partial dates, fuzzy timestamps, scientific measurements, and AI-friendly reasoning.

3. **String-Based Identifiers**  
    Uses string IDs for formats, units, and operations instead of fragile enums, improving extensibility, safety, introspection, and dynamic integration.

4. **Robust Parsing & Formatting Engine**  
    Supports deterministic, mask-aware parsing and formatting with ISO, log, and customizable representations that respect actual precision.

5. **Monotonic & High-Resolution Timing**  
    Provides reliable monotonic clocks for accurate elapsed-time measurement, benchmarking, and interval tracking.

6. **Explicit Duration & Interval Types**  
    Distinguishes clearly between instants, durations, and spans, reducing ambiguity and improving correctness in time arithmetic.

7. **Calendar & Chronological Logic**  
    Built-in support for leap years, weekdays, yeardays, and calendar-safe calculations without relying on platform quirks.

8. **Cross-Platform Consistency**  
    Delivers uniform behavior across Windows, macOS, Linux, and embedded environments with predictable results.

9. **Modular, Composable Architecture**  
    Designed as independent components that can be integrated selectively, making it suitable for embedded systems, servers, and CLI tools.

10. **Deterministic, AI-Ready Design**  
     Structured for machine reasoning, validation, and symbolic manipulation—making it well-suited for intelligent systems, code generation, and advanced automation workflows.

## Getting Started

## ***Prerequisites***

To get started, ensure you have the following installed:

- **Meson Build System**: If you don’t have Meson `1.8.0` or newer installed, follow the installation instructions on the official [Meson website](https://mesonbuild.com/Getting-meson.html).

### Adding Dependency

#### Adding via Meson Git Wrap

To add a git-wrap, place a `.wrap` file in `subprojects` with the Git repo URL and revision, then use `dependency('fossil-time')` in `meson.build` so Meson can fetch and build it automatically.

#### Integrate the Dependency:

Add the `fossil-time.wrap` file in your `subprojects` directory and include the following content:

```ini
[wrap-git]
url = https://github.com/fossillogic/fossil-time.git
revision = v0.1.1

[provide]
dependency_names = fossil-time
```

**Note**: For the best experience, always use the latest releases. Visit the [releases](https://github.com/fossillogic/fossil-time/releases) page for the latest versions.

## Configure Options

- **Running Tests**: Enable testing by configuring with `-Dwith_test=enabled`.

Example:

```sh
meson setup builddir -Dwith_test=enabled
```

### Tests Double as Samples

The project is designed so that **test cases serve two purposes**:

- ✅ **Unit Tests** – validate the framework’s correctness.  
- 📖 **Usage Samples** – demonstrate how to use these libraries through test cases.  

This approach keeps the codebase compact and avoids redundant “hello world” style examples.  
Instead, the same code that proves correctness also teaches usage.  

This mirrors the **Meson build system** itself, which tests its own functionality by using Meson to test Meson.  
In the same way, Fossil Logic validates itself by demonstrating real-world usage in its own tests via Fossil Test.  

```bash
meson test -C builddir -v
```

Running the test suite gives you both verification and practical examples you can learn from.

## Contributing and Support

For contributions, issues, or support, please open an issue on the project repository or visit the [Fossil Logic Docs](https://fossillogic.com/docs) for more information. Contributions and feedback are always appreciated.
