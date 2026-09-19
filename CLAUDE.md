# inverted-pendulum-bot — Claude Code Instructions

This file is a concise, task-oriented guide for Claude and AI agents to be
immediately productive in this repository. The repository was bootstrapped from
`embedded-pro/embedded-scaffold`: the `example_*` components exist to demonstrate
the conventions and are expected to be replaced by the inverted pendulum bot's own
components.

## 1. Big-Picture Architecture

**Purpose**: firmware for a self-balancing inverted pendulum robot, written in C++
with strict real-time and memory constraints for a resource-constrained
microcontroller (ARM Cortex-M4, NUCLEO-WB55RG), with a host build for off-target
testing.

**Major components**:
- `core/` — reusable libraries only (no application entry points).
  - `core/platform_abstraction/` — the `platform::Platform` interface (the seam
    between portable logic and hardware) plus a `PlatformMock` for tests.
  - `core/cli/` — the headline portable application (LED blink + UART CLI),
    written against `platform::Platform` and unit-tested on the host with the mock.
  - `core/example_component/` — a trivial `interfaces/` + `implementations/` +
    `test/` example showing the component conventions.
- `targets/` — application entry points and platform-specific implementations.
  - `targets/cli/Main.cpp` — one platform-agnostic entry point reused for
    host/st; it includes `PLATFORM_IMPL_HEADER` (set per platform in
    `targets/CMakeLists.txt`) to construct the selected `PlatformImpl`.
  - `targets/platform_implementations/{host,st}/PlatformImpl.*` — concrete
    `platform::Platform` implementations (host stubs; NUCLEO-WB55RG).
  - The active platform is chosen by `INVERTED_PENDULUM_BOT_TARGET_PLATFORM`.
- `tools/` — host-side developer tools that reuse `core/` libraries
  (`tools/example_tool/`). Built only for the host.
- `integration_tests/` — BDD tests (cucumber-cpp / Gherkin): `features/`, `steps/`,
  `support/`, `hooks/`, `main/`.
- `infra/numerical-toolbox/` — generic numerical algorithms (PID, filters,
  fixed-point helpers).
- `infra/embedded-infra-lib/` — infrastructure: bounded containers, build helpers
  (`emil_*` CMake functions), toolchain cmake pieces.
- `infra/hal/st/` — the STM32 hardware abstraction layer.

## 2. Critical Developer Workflows

```bash
# Clone (with submodules)
git clone --recursive <repo>

# Configure & build host (recommended first step)
cmake --preset host
cmake --build --preset host-Debug

# Run unit + integration tests
ctest --preset host

# Build the cli firmware for the board (.elf/.hex)
cmake --preset NUCLEO-WB55RG
cmake --build --preset NUCLEO-WB55RG-Debug

# Coverage build
cmake --preset coverage
cmake --build --preset coverage
```

Configure presets (`CMakePresets.json`): `host`, `host-single-Debug`, `windows`,
`coverage`, `NUCLEO-WB55RG`.

## 3. Project-Specific Constraints (must follow)

### Memory — No Heap in Embedded/Real-Time Code

The no-heap rule applies to `core/` runtime code, `targets/`, and any ISR-reachable
path. Host tools, simulators, and test code may use normal heap patterns.

- No `new`, `delete`, `malloc`, `free`, `std::make_unique`, `std::make_shared`
- No `std::vector` → use `infra::BoundedVector<T>::WithMaxSize<N>`
- No `std::string` → use `infra::BoundedString::WithStorage<N>`
- No `std::deque`/`std::list`/`std::map`/`std::set` → use bounded alternatives from `embedded-infra-lib`
- No recursion in embedded/runtime paths (stack must be predictable)
- No `virtual ~Dtor() = 0` (pure virtual destructors) — adds flash/RAM overhead.
  Use a protected non-virtual destructor on interfaces (see
  `core/example_component/interfaces/Accumulator.hpp`).

### Real-Time — Hot Paths

- No virtual dispatch or blocking calls in ISR-reachable hot paths.
- Apply `OPTIMIZE_FOR_SPEED` (from `numerical/math/CompilerOptimizations.hpp`) and,
  where appropriate, `#pragma GCC optimize("O3", "fast-math")` to hot-path files.
- State explicit cycle budgets for timing-critical loops and verify with
  `arm-none-eabi-objdump -d -C`.

### Other Constraints

- Prefer fixed-size integer types (`uint8_t`, `int32_t`).
- Keep non-trivial logic in `.cpp` files; small `inline`/`constexpr` helpers in
  headers are allowed (and common in hot paths).
- Prefer `{}` initialization over `()`.
- `const` on all non-mutating methods; `constexpr` for constants and lookup tables.

## 4. Naming Conventions

- **Classes/Methods**: `PascalCase` (`AccumulatorImpl`, `Add()`, `Total()`)
- **Member variables**: `camelCase` (`total`, `initialized`)
- **Namespaces**: lowercase (`example`, `platform`, `integration`)
- **CMake libraries**: `inverted_pendulum_bot.<area>.<layer>`

## 5. Brace Style — Allman, 4-Space Indent

```cpp
namespace example
{
    class AccumulatorImpl
        : public Accumulator
    {
    public:
        void Add(int32_t value) override;
        int32_t Total() const override;

    private:
        int32_t total{ 0 };
    };
}
```

## 6. Patterns & Code Locations

- **New core component**: copy `core/example_component/`; keep interfaces in
  `interfaces/`, implementations in `implementations/`, tests in
  `implementations/test/`.
- **New application**: write the logic in `core/` against `platform::Platform` (see
  `core/cli/`); add a thin `targets/<app>/Main.cpp` that includes
  `PLATFORM_IMPL_HEADER` and constructs it. Unit-test the logic on the host with
  `core/platform_abstraction/test_doubles/PlatformMock.hpp`.
- **New platform peripheral**: add a method to `platform::Platform`, implement it in
  each `targets/platform_implementations/<platform>/PlatformImpl`, and update the mock.
- **New board**: copy a `PlatformImpl` (e.g. `st/`), wire its peripherals via
  `infra/hal/<vendor>`, and add a preset in `CMakePresets.json`.
- **New tool**: copy `tools/example_tool/`.
- **Include paths** are rooted at the repository root, e.g.
  `#include "core/example_component/implementations/AccumulatorImpl.hpp"`.

## 7. Testing

- Unit tests: `core/<component>/implementations/test/Test{Name}.cpp`, registered
  with `emil_add_test` / `emil_build_for`.
- Integration tests: cucumber-cpp under `integration_tests/`. Steps bind Gherkin
  lines via `GIVEN`/`WHEN`/`THEN`/`STEP`; share state through the cucumber `context`.
- Use `TEST_F` for fixtures; `TYPED_TEST` for multi-type tests.
- **`testing::StrictMock<>`** for ALL mock instances — `NiceMock`/`NaggyMock` are FORBIDDEN.
- Fixture class inside an anonymous `namespace {}`; test macros outside.
- Arrange-Act-Assert; `EXPECT_NEAR` with explicit tolerance for floating-point.

## 8. Documentation — Always Updated

- Documentation-first: update the relevant doc in `documentation/` before or
  alongside behavioural changes.
- `documentation/templates/` — starting points for new docs; CI validates
  architecture/design/theory docs and the requirements schema.
- All visuals must use Mermaid code blocks or ASCII art — no external image references.

## 9. Build System

- Presets are the primary interface (`CMakePresets.json`).
- Embedded toolchains: `infra/embedded-infra-lib/cmake/toolchain-*.cmake`.
- `compile_commands.json` is generated in build dirs for language servers/analysis.

## 10. Design Principles

- **SOLID** and **DRY**: reuse `infra/numerical-toolbox/` (PID, filters, transforms).
- **Constructor injection**: hardware dependencies via the platform `Board`/factory,
  never global state.
- **Error handling**: `std::optional<T>` for nullable returns; error codes/enums in
  embedded/runtime code (no exceptions); `assert()` for debug preconditions.

## Available Agents

Use the agents in `.claude/agents/` for structured development workflows:

- **orchestrator** — triage and route incoming development tasks
- **planner** — create detailed implementation plans before writing code
- **executor** — implement code changes following all project constraints
- **reviewer** — review code changes against project standards
