# GenericContainer

[![CI](https://github.com/pbosetti/GenericContainer/actions/workflows/ci.yml/badge.svg)](https://github.com/pbosetti/GenericContainer/actions/workflows/ci.yml)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.25%2B-informational.svg)
[![License: BSD 2-Clause](https://img.shields.io/badge/License-BSD--2--Clause-brightgreen.svg)](LICENSE.md)

**A single C++ type that holds anything** — booleans, numbers, strings,
Eigen matrices, and arbitrarily nested vectors/maps of itself — with typed
accessors, deep copies that are actually safe, and opt-in bridges to
**JSON** (via `nlohmann::json`), **TOML**, **YAML**, and **Lua**.

```cpp
GenericContainer gc;
gc["name"]      = "probe-7";
gc["active"]    = true;
gc["readings"]  = vec_real_type{ 1.5, 2.75, 3.0 };
gc["meta"]["id"] = 42;

std::cout << gc["readings"].get_real_at( 1 ) << "\n"; // 2.75
```

---

## Contents

- [GenericContainer](#genericcontainer)
  - [Contents](#contents)
  - [Why](#why)
  - [Quick start](#quick-start)
  - [Building](#building)
  - [Using it from your own CMake project](#using-it-from-your-own-cmake-project)
    - [Option A — `FetchContent`](#option-a--fetchcontent)
    - [If other subprojects also fetch GenericContainer](#if-other-subprojects-also-fetch-genericcontainer)
    - [Option B — an installed package](#option-b--an-installed-package)
  - [Supported types](#supported-types)
  - [JSON — `nlohmann::json`](#json--nlohmannjson)
  - [Evaluating expressions with Expressionist](#evaluating-expressions-with-expressionist)
  - [TOML](#toml)
  - [YAML](#yaml)
  - [Lua](#lua)
  - [Other language bindings](#other-language-bindings)
  - [Testing](#testing)
  - [License](#license)

---

## Why

Scripting-language interop, config files, and RPC payloads all need the
same thing: a value that can be *any* type, including nested structures of
itself, discovered at runtime. `GenericContainer` is that value —
implemented as a `std::variant` under the hood (deep-copying, no raw
unions, no manual memory management), with:

- **Safe by construction.** Copy assignment is copy-and-swap (safe under
  self-assignment and aliasing); numeric conversions are range-checked and
  throw `GenericError` instead of silently truncating.
- **Real matrices.** `mat_type<T>` *is* an `Eigen::Matrix<T, Dynamic,
  Dynamic>` — no copying out to use linear algebra on stored data.
- **Modular format support.** JSON/TOML/YAML/Lua bridges are separate,
  opt-in libraries. Link only what you need; the core has no format
  dependencies at all.
- **A stable C ABI and FFI layer** (`GenericContainerInterface_C.h`,
  `GenericContainer_ffi.h`) for embedding in non-C++ hosts.

## Quick start

```cpp
#include "GenericContainer/GenericContainer.hh"
using namespace GC_namespace;

GenericContainer gc;
gc["one"]  = 1;                   // integer
gc["two"]  = true;                // boolean
gc["three"] = 1.4;                // floating point
gc["four"] = "pippo";             // string
gc["five"].set_vec_int( 10 );     // vector of 10 integers

std::cout << gc["one"].get_int()     << '\n';
std::cout << gc["two"].get_bool()    << '\n';
std::cout << gc["three"].get_real()  << '\n';
std::cout << gc["four"].get_string() << '\n';

vec_int_type & v = gc["five"].get_vec_int();
v[3] = 99;
```

Vectors and maps of `GenericContainer` nest freely, which is how you build
arbitrary tree-shaped data:

```cpp
auto & list = gc["items"].set_vector( 2 );
list[0]["id"]   = 1;
list[0]["tags"] = vec_string_type{ "a", "b" };
list[1]["id"]   = 2;

for ( auto const & item : gc["items"].get_vector() )
  std::cout << item["id"].get_int() << '\n';
```

Matrices are Eigen matrices — use them directly:

```cpp
mat_real_type & M = gc["M"].set_mat_real( 3, 3 );
M.setIdentity();
Eigen::Vector3d x = M * Eigen::Vector3d{ 1, 2, 3 };
```

See [`examples/`](examples/) for complete, compilable programs.

## Building

Requires **CMake ≥ 3.25** and a **C++20** compiler (Clang, GCC, or MSVC).
All dependencies are fetched automatically via `FetchContent`:
[Eigen](https://eigen.tuxfamily.org) 5.0.0 is always fetched fresh (pinned,
for version consistency — it's never looked up as an installed package);
[toml++](https://github.com/marzer/tomlplusplus) and
[fkYAML](https://github.com/fktn-k/fkYAML) prefer an already-installed
package and only fetch as a fallback; [Lua](https://www.lua.org) has no
CMake package story upstream, so it's always built from the official
source tarball.

```sh
cmake -S . -B build                             # configure (Release by default)
cmake --build build                             # build
ctest --test-dir build                          # run the test suite
cmake --install build --prefix /path/to/prefix  # install
cmake --build build --target package            # cpack: .tar.gz / .zip
```

> **NOTE**: the `build.sh` and `build.ps1` scripts provide easier single command builds for the lazy 🫣.

Useful configure-time options:

| Option | Default | Effect |
|---|---|---|
| `BUILD_SHARED_LIBS` | `OFF` | Build shared (`.so`/`.dylib`/`.dll`) instead of static libraries |
| `BUILD_TESTING` | `ON` | Build the Catch2 test suite |
| `GENERIC_CONTAINER_INSTALL` | `ON` (top-level) | Generate install/export/package rules |
| `GENERIC_CONTAINER_ENABLE_YAML` | `ON` | Build `GenericContainer::Yaml` |
| `GENERIC_CONTAINER_ENABLE_TOML` | `ON` | Build `GenericContainer::Toml` |
| `GENERIC_CONTAINER_ENABLE_LUA` | `ON` | Build `GenericContainer::Lua` |

Turning a backend off skips fetching its dependency entirely:

```sh
cmake -S . -B build -DGENERIC_CONTAINER_ENABLE_LUA=OFF -DGENERIC_CONTAINER_ENABLE_YAML=OFF
```

## Using it from your own CMake project

### Option A — `FetchContent`

```cmake
include( FetchContent )
FetchContent_Declare(
  GenericContainer
  GIT_REPOSITORY https://github.com/pbosetti/GenericContainer.git
  GIT_TAG        develop   # pin to a commit or tag for reproducible builds
)
set( BUILD_TESTING OFF )   # don't build GenericContainer's own test suite as a dependency
FetchContent_MakeAvailable( GenericContainer )

add_executable( my_app main.cc )
target_link_libraries( my_app PRIVATE
  GenericContainer::GenericContainer
  GenericContainer::Toml   # only what you need; each is optional
)
```

Eigen comes along for free — `GenericContainer` always fetches its own
pinned copy and exposes it globally as `Eigen3::Eigen`. If your own code
only touches Eigen types *through* `GenericContainer` (e.g. `mat_real_type`
returned from a container), you don't need to reference Eigen at all: the
include path already reaches you transitively through
`GenericContainer::GenericContainer`'s public link. If you use Eigen
directly in a target that doesn't otherwise depend on `GenericContainer`,
link it explicitly:

```cmake
target_link_libraries( my_app PRIVATE Eigen3::Eigen )
```

(must come after `FetchContent_MakeAvailable( GenericContainer )`, which is
what defines the target.)

### If other subprojects also fetch GenericContainer

`FetchContent` deduplicates by declared name: if your project pulls in
other subprojects that *also* `FetchContent_Declare(GenericContainer ...)`
(directly or transitively), it's fetched, configured, and built exactly
once no matter how many times `FetchContent_MakeAvailable(GenericContainer)`
is called — confirmed with a real three-level build (a top project plus two
sub-libraries, each independently fetching `GenericContainer`). Eigen is
deduplicated the same way, as a consequence: its own `FetchContent_Declare`
call lives inside `GenericContainer`'s `CMakeLists.txt`, which only
actually runs once.

Two things worth knowing about this:

- **It only works through `FetchContent`.** A subproject that instead
  vendors `GenericContainer` via a plain
  `add_subdirectory(path/to/GenericContainer)` bypasses the deduplication
  bookkeeping entirely; adding a second copy anywhere in the tree hard-fails
  configure with `add_library cannot create target "GenericContainer"
  because another target with the same name already exists`.
- **Version mismatches are resolved silently, with no warning.** If your
  project and a subproject pin different `GIT_TAG`s, CMake uses whichever
  `FetchContent_Declare` call it processes *first* in configure order and
  silently discards the rest — the subproject that asked for a different
  version gets no diagnostic at all, it just quietly builds against
  whichever version won. Verified on CMake 4.2.1: pinning `develop` at the
  top level and the years-old `1.0.0` tag in a subproject, the subproject
  received `develop` with no indication anything was overridden.

If subprojects might reasonably disagree on the version, declare
`GenericContainer` in your own top-level `CMakeLists.txt` **before**
`add_subdirectory`/`FetchContent`-ing anything that also depends on it —
that makes your pin the one that deterministically wins, rather than
leaving the outcome to directory-traversal order:

```cmake
# Top-level CMakeLists.txt, before any add_subdirectory()/FetchContent
# call that might pull in something also depending on GenericContainer.
FetchContent_Declare( GenericContainer GIT_REPOSITORY ... GIT_TAG ... )
FetchContent_MakeAvailable( GenericContainer )

add_subdirectory( third_party/some_lib_that_also_uses_GenericContainer )
```

### Option B — an installed package

After `cmake --install` (see [Building](#building)), a normal
`find_package` picks up the core library plus whichever backends were
enabled at install time:

```cmake
find_package( GenericContainer REQUIRED )

add_executable( my_app main.cc )
target_link_libraries( my_app PRIVATE
  GenericContainer::GenericContainer
  GenericContainer::Yaml
  GenericContainer::Lua
)
```

Point CMake at a non-system install with
`-DCMAKE_PREFIX_PATH=/path/to/prefix`.

## Supported types

| Category | Types |
|---|---|
| Scalar | pointer, boolean, integer, long integer, real, complex, string |
| Vector | of pointer, boolean, integer, real, complex, string |
| Matrix | of integer, long integer, real, complex (Eigen-backed) |
| Recursive | vector of `GenericContainer`, map of `GenericContainer` |

The recursive types are what let you model arbitrary tree-shaped data —
the same mechanism that underlies the JSON/TOML/YAML bridges below.

## JSON — `nlohmann::json`

For projects already using
[`nlohmann::json`](https://github.com/nlohmann/json), a header-only
adapter is available:

```cpp
#include "GenericContainer/GenericContainerInterface_nlohmann.hh"
```

Including this header (and only this header) enables conversion through
the standard `adl_serializer` customization point, so a `GenericContainer`
converts to/from `nlohmann::json` like any other supported type:

```cpp
GenericContainer gc; gc["a"] = 42;
nlohmann::json j = gc;                             // to_json
GenericContainer gc2 = j.get<GenericContainer>();  // from_json
```

`GenericContainer` has no hard dependency on `nlohmann::json` — only
translation units that include this header need `nlohmann/json.hpp` on
their include path (e.g. fetched separately via `FetchContent`).
Homogeneous numeric JSON arrays decode directly into
`vec_int_type`/`vec_real_type` (and, when possible, matrices) rather than
a generic vector of `GenericContainer`, so downstream calls like
`copyto_vec_real` get the fast representation.

## Evaluating expressions with Expressionist

JSON data may contain algebraic expressions stored as tagged strings,
e.g. `"$a + b"`.
[`Expressionist`](https://github.com/pbosetti/Expressionist) is a
header-only C++20 library that evaluates such expressions in place on a
`nlohmann::json` object — variables are simply other keys of the same
JSON tree, resolved in dependency order. Combined with the adapter above,
it's a three-step conversion:

```cpp
#include "GenericContainer/GenericContainerInterface_nlohmann.hh"
#include <expressionist.hpp>

std::string text = R"({
  "a": 1,
  "b": 2,
  "c": "$a + b",
  "g": "$sin(pi / 3) * b"
})";

Expressionist::Expressionist ex( text );
ex.evaluate(); // resolve "$..." in place
GenericContainer gc = ex.object().get<GenericContainer>(); //->GenericContainer

std::cout << gc["c"].get_int()  << '\n'; // 3
std::cout << gc["g"].get_real() << '\n'; // 1.732...
```

Requires a C++20 compiler and its own `FetchContent`, the same way
`nlohmann::json` does.

## TOML

`GenericContainer::Toml` reads and writes [TOML](https://toml.io/) via
[toml++](https://github.com/marzer/tomlplusplus), through two ordinary
member functions:

```cpp
#include "GenericContainer/GenericContainer.hh"
using namespace GC_namespace;

GenericContainer gc;
gc["name"] = "probe-7";
gc["gain"] = 3.5;

std::ostringstream out;
gc.to_toml( out );

GenericContainer gc2;
std::istringstream in( out.str() );
gc2.from_toml( in );  // -> true on success
```

`GenericContainerInterface_toml.hh` adds thin file/string convenience
wrappers (`file_TOML_to_GC`, `TOML_to_GC`, `GC_to_TOML`) if you'd rather
not touch streams directly. Link `GenericContainer::Toml`.

## YAML

`GenericContainer::Yaml` does the same for [YAML](https://yaml.org/) via
[fkYAML](https://github.com/fktn-k/fkYAML):

```cpp
#include "GenericContainer/GenericContainer.hh"
using namespace GC_namespace;

GenericContainer gc;
gc["items"] = vec_int_type{ 1, 2, 3 };

std::ostringstream out;
gc.to_yaml( out );

GenericContainer gc2;
std::istringstream in( out.str() );
gc2.from_yaml( in );
```

`GenericContainerInterface_yaml.hh` provides the matching
`file_YAML_to_GC` / `YAML_to_GC` / `GC_to_YAML` convenience wrappers.
Link `GenericContainer::Yaml`.

> **A NOTE REGARDING TOML/YAML**: Expressionist only supports JSON, so if you want to evaluate expressions in TOML/YAML, use those libraries' won conversion to JSON, then run Expressionist on the JSON, then load to `GenericContainer` as above.

## Lua

`GenericContainer::Lua` embeds Lua 5.4 and exchanges data with it through
global variables:

```cpp
#include "GenericContainer/GenericContainerInterface_lua.hh"
using namespace GC_namespace;

LuaInterpreter lua;

GenericContainer gc; gc = 7;
lua.GC_to_global( gc, "y" );              // GenericContainer -> Lua global
lua.execute( "z = y * 6" );               // run Lua code

GenericContainer result;
lua.global_to_GC( "z", result );          // Lua global -> GenericContainer
std::cout << result.get_number() << '\n'; // 42
```

`LuaInterpreter` also exposes `call()` (invoke a Lua function with
arguments packed in a `GenericContainer`), `do_file()`, and
`interactive()` (a full REPL entry point for embedding a Lua console in
your own tool). Link `GenericContainer::Lua`.

## Other language bindings

The [`matlab/`](matlab/) and [`ruby/`](ruby/) directories hold a MATLAB
mex interface and a Ruby FFI wrapper. Neither is wired into the CMake
build above, and `matlab/Compile.m`'s hardcoded source paths predate the
recent `interfaces/` reorganization — expect to patch paths before
either builds again.

## Testing

The test suite uses [Catch2](https://github.com/catchorg/Catch2) v3,
fetched automatically:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## License

[BSD 2-Clause](LICENSE.md).
