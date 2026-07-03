# GenericContainer Core: C++20 Modernization, Bug Fixes & Catch2 Test Battery

> **STATUS: COMPLETED** (July 2026). All six phases landed on `develop`:
> C++20 flip -> characterization battery (1679 assertions, zero skips) ->
> variant/Box storage rewrite -> Eigen matrices + size_t + checked narrowing ->
> API polish (GenericError, operator==, span, concepts, fork removal) ->
> serialization hardening + sanitizer CI. All 11 recorded bugs fixed and
> covered by regressions; wire format byte-identical (golden pin).

## Context

The core of GenericContainer (~14k lines: `GenericContainer.hh` 5071 + `GenericContainer.cc` 3760 + Promote/Serialize/Support/Tables/Collapse) is a pre-C++17 design: a raw tagged union (`DataStorage`, 21 type tags) with manual `new`/`delete`, ~15 duplicated 21-case switches, and `unsigned` sizes everywhere. Code review found real memory-safety bugs (below). Goal: fix the bugs, rewrite storage on `std::variant` + RAII, modernize the API to C++20, and build a complete Catch2 v3 unit-test battery.

**User decisions (binding):** Catch2 v3 via FetchContent · free to break C++ API (in-repo interface layers updated to keep building; C ABI of `GenericContainerInterface_C.h` / FFI stays frozen) · keep `operator[]` auto-convert/destroy semantics (document + test) · full modernization scope · **replace custom `mat_type` with Eigen dense matrices** (Eigen becomes a public dependency).

## Bugs found (verified first-hand)

1. **Exception safety** — `allocate_*` (src/GenericContainer.cc:785-970) sets `m_data_type` *before* `new`; on `bad_alloc` the destructor deletes a stale union pointer → double-free/UB.
2. **Copy-assignment UAF** — `operator=` (include/GenericContainer/GenericContainer.hh:3832) is `clear(); from_gc(a);` with no self/alias check: `gc = gc` or `gc = gc["key"]` reads freed memory.
3. **`mat_type` publicly inherits `std::vector`** (GenericContainer.hh:170); `unsigned nr*nc` multiply overflows silently. Storage is **column-major** (`at(i + j*m_num_rows)`, GenericContainer.cc:147-171) while docs claim row-major. → Fixed by replacing `mat_type` with Eigen (below).
4. **Unchecked narrowing** — `get_value<int_type>` does unchecked `static_cast<int>(m_data.l)` (GenericContainer.cc:1633); REAL→int paths (`copyto_vec_*`, promote) check only `isInteger()`, no range check → `1e300` is UB.
5. **Const-correctness holes hidden by the union** — `push_bool(bool) const` mutates (GenericContainer.cc:1198); `get_map_bool(initializer_list) const` uses `(*m_data.m)[key]` which can insert (GenericContainer.cc:2326).
6. **`unsigned` size truncation** (`get_num_elements`, `operator[]`, `set_*` sizes) for containers >4G elements.
7. **Serialization**: `int32_t` offsets (2 GB limit, silent truncation in `mem_size`), no pre-check of matrix `nr*nc` vs remaining buffer (giant alloc before failing).
8. Minor: `GC_ASSERT` builds an ostringstream per call site; `exception(ost.str().data())` sloppiness; `using TypeAllowed = enum class ...`, `using string = string;` self-alias; Windows `#ifdef` forks for `simple_data`/`get_pointer`; redundant recursive pre-clear in `clear()`; duplicate `reserve()` calls in Promote.
9. **`promote_to_vector` scalar cases read repurposed union storage** (found by Phase 1 tests): `set_vector(1); (*this)[0] = m_data.i;` reads the union member *after* it became the vector pointer — garbage values for arithmetic scalars, use-after-free for STRING/COMPLEX (GenericContainerPromote.cc ~1876-1925). The VEC_* cases save the pointer first and are correct. → Fixed by Phase 2; test `[promote][newbehavior]` unskips.
10. **Union type confusion in matrix promotions** (found by Phase 1 tests): `promote_to_mat_int` (VEC_BOOL/VEC_INTEGER cases, lines ~1546/1555), `promote_to_mat_long` (VEC_BOOL, ~1608), `promote_to_mat_complex` (all four vec cases, ~1802-1829) write elements through `m_data.m_r` (matrix-of-real) while the union holds a different matrix type — copy-paste from `promote_to_mat_real`, UB. → Fixed by Phase 2; test `[promote][newbehavior]` unskips.
11. **`de_serialize` silently ignores an invalid type tag** (no `default` in the tag switch) — corrupted input yields an untouched/empty container with no error. → Fixed in Phase 5; test `[serialize][newbehavior]` unskips.

## Design

### Storage: `Box<T>` + `std::variant` (replaces union + tag)

New private detail (in `GenericContainer.hh` private section or new `include/GenericContainer/GenericContainerDetails.hh`):

- `Box<T>`: deep-copying owner over `std::unique_ptr<T>` (copy ctor clones, moves defaulted, `operator*` **propagates const** — fixes bug 5 structurally). Complete type even for incomplete `T` → solves both recursive-type problems (`std::map` with incomplete value type is not guaranteed; by-value `vector<GenericContainer>` inside the variant of the class being defined is thin ice).
- `Data = std::variant<std::monostate, pointer_type, bool_type, int_type, long_type, real_type, Box<complex_type>, Box<string_type>, Box<vec_*>…, Box<mat_*>…, Box<vector_type>, Box<map_type>>` — **variant index == `GC_type` enum value**, locked by 21 `static_assert`s. `get_type()` = `static_cast<GC_type>(m_data.index())`.
- Boxing exactly what the union heap-allocates keeps `sizeof(GenericContainer)` at **16 bytes** (no bloat in `vector<GenericContainer>`), same allocation profile as today.
- **Never-valueless invariant**: alternatives are only changed via `reset_to<T>()` (construct `Box` first, then nothrow move-assign into the variant) — on `bad_alloc` the old value survives (strong guarantee; kills bug 1). `static_assert(is_nothrow_move_constructible_v<Data>)`.
- Special members: copy ctor `= default` (deep via Box); **copy assignment = copy-and-swap** (kills bug 2, incl. aliasing); move ops reset source to monostate; `swap` = `m_data.swap`; dtor `= default` (`clear()` becomes `emplace<monostate>()`).
- Access core: `ref_or_throw<T>(where)` (via `std::get_if`, throws on mismatch — replaces `ck` + 40 near-identical `get_*` bodies), `ensure<T>()` (reuse-or-reset — replaces `allocate_*`).
- Visitation: `overloaded{}` + a `visit_data(f)` unboxing helper kill the ~15 duplicated 21-case switches (`clear`, `get_num_elements`, `num_rows/cols`, `dump`, `info`, `mem_size`, serialize dispatch, `compare_content`, `to_gc`/`from_gc` → thin wrappers over `operator=`). Compiler-enforced exhaustiveness. Hot scalar paths use `std::get_if` directly, not `std::visit`.

### Matrices: replace `mat_type` with Eigen dense matrices (user decision)

Drop the custom `mat_type` class entirely; matrices become Eigen dynamic dense matrices:

- `template <class T> using mat_type = Eigen::MatrixX<T>;` (alias keeps `mat_type<T>` spellings compiling) → `mat_int_type = Eigen::MatrixX<int_type>`, `mat_long_type = Eigen::MatrixX<long_type>`, `mat_real_type = Eigen::MatrixXd`, `mat_complex_type = Eigen::MatrixXcd`.
- **Eigen is column-major by default — identical to the current storage layout**, so serialization (rows, cols, raw `data()` in storage order) keeps the wire format byte-for-byte. This kills bug 3 (vector inheritance, unsigned `nr*nc` overflow — Eigen uses `Eigen::Index`/ptrdiff_t and checks) and fixes the row-major doc lie by construction.
- `Box<Eigen::MatrixX<T>>` in the variant: MatrixX is a 24-byte value type (ptr + 2×Index), deep-copying, no fixed-size alignment concerns (dynamic matrices self-align). `sizeof(GenericContainer)` stays 16.
- Core header includes only `<Eigen/Core>`. Eigen becomes a **PUBLIC dependency** of the library targets (accepted compile-time cost): FetchContent pin Eigen 3.4 with `EIGEN_BUILD_DOC/TESTING/PKGCONFIG` off (or a plain INTERFACE target over the fetched include dir, marked SYSTEM to silence Eigen-vs-new-clang warnings); install/export must propagate `Eigen3::Eigen`.
- API mapping in core code: `num_rows()/num_cols()` → `rows()/cols()` (GC-level `num_rows()`/`num_cols()` methods stay, returning `size_t` with centralized `Eigen::Index`↔`size_t` casts); element loops over `.data()`+`size()` or `reshaped()`; `get_column(j)` → `.col(j)` copy (plus zero-copy `.col(j)` for users, since they now hold real Eigen types); `resize(nr,nc)` native.
- GC's checked accessors (`get_*_at(i,j,where)`) bounds-check **before** indexing (Eigen `operator()` is assert-only in debug, UB in release).
- Deep equality: Eigen has no `bool operator==`; equality helper per matrix type = dims equal && `A.cwiseEqual(B).all()` (NaN ≠ NaN, as expected), used by `Box`/variant `operator==`.
- Users gain full Eigen linear algebra on `get_mat_real()` results — the main point of this choice.

### API modernization (breaking allowed)

- Sizes → `std::size_t`: `get_num_elements`, `num_rows/num_cols`, `operator[](i)`, `operator()(i)`, `get_*_at`, `set_vector/set_vec_*/set_mat_*`. Serialization wire format keeps `int32_t` externally with checked narrowing (`std::in_range`).
- Central `checked_narrow<To>(from, where)`: float sources require `isInteger(x)` **and** exact-representable range bounds; integer sources via `std::in_range` (fixes bug 4 in `get_value`, `get_as_*`, `copyto_vec_*/mat_*`, promote paths).
- Error handling: keep macro names `GC_ASSERT`/`GC_DO_ERROR`, re-implement over a variadic `[[noreturn]] gc_error(args...)` (fold-expression `<<`, cold, `[[unlikely]]`); new `class GenericError : std::runtime_error` (still catchable as `runtime_error`; public static `exception()` kept). No `std::format`/fmt needed. Call sites: mechanical `<<`→`,` pass (~450 sites).
- C++20 surface: `[[nodiscard]]` on getters; `noexcept` audit; `std::integral` concepts on index/resize templates; constrained `get_pointer<T>` (`is_pointer_v`); `std::span` overloads for `serialize`/`de_serialize`; deep `operator==` (variant+Box equality); `constexpr to_string(GC_type)`; `inline` `stream_number_precision`; delete Windows `#ifdef` forks and odd aliases; remove deprecated camelCase methods (`numRows`, `readFormattedData`, …) and update their only users (examples/example07.cc, example10.cc, examples/test_all.cc).
- Serialization hardening: `mem_size` computes in `uint64_t` and throws if >`INT32_MAX`; `de_serialize` pre-checks `nr*nc*elem_size <= remaining` before allocating.

### Interface-layer updates (compile-gate driven)

`unsigned NR{ M.num_rows() }` braced-init becomes a hard narrowing error → precise fix list: src_yaml_interface/to_yaml.cc:157-178, src_json_interface/to_json.cc:164-171, include/GenericContainer/GenericContainerInterface_nlohmann.hh:128-168, toml/lua counter types, `GenericContainerInterface_C.cc` internal casts (**.h untouched — C ABI frozen**), `GenericContainer_ffi.cc` recompile-only. Matlab/Ruby interfaces are not in the CMake build — best-effort grep only.

## Implementation phases (gate after each: `cmake -Bbuild -GNinja && cmake --build build && ctest --test-dir build --output-on-failure`; phases 2-5 also ASan+UBSan locally; push per phase → 3-OS CI)

**Phase 0 — Toolchain**: `cxx_std_17`→`cxx_std_20` in CMakeLists.txt:194,216; scrub the `-std=c++17` injected by cmake_utils' CMakeLists-cflags.txt (`string(REPLACE …)` after include). Gate: existing build+tests green on macOS/Linux/Windows CI.

**Phase 1 — Characterization test battery (safety net first)**: new `tests/` dir, Catch2 v3 via FetchContent (`Catch2WithMain`, `catch_discover_tests`), single `gc_unit_tests` executable linked to the static lib; wired inside the existing `UTILS_ENABLE_TESTS AND top-level` block (CMakeLists.txt:234). Suites written against **current** behavior; new-behavior cases (bug fixes) written but tagged/`SKIP()`ed:

| File | Covers |
|---|---|
| test_scalar_types.cc | set/get/ctor/= for all 8 scalar tags; wrong-type get throws; get_type/empty |
| test_vector_types.cc | 7 vec types; push_* growth & promotion-on-push; get_*_at; vector\<bool\> quirks |
| test_matrix_types.cc | dims, (i,j), **column-major layout pinned via data()**, get_row/column, equality helper, Eigen interop (assign MatrixXd into GC, mutate via returned reference, run an Eigen op on it) — Phase 1 pins GC-level behavior only (accessors + serialization), not mat_type member API, since that changes in Phase 3 |
| test_map_vector_nesting.cc | deep trees; get_keys/exists/must_exists/get_if_exists/get_map_*; erase; merge; 10k-deep recursion smoke |
| test_copy_move.cc | deep-copy independence (21 types); move leaves NOTYPE; swap; **regressions: `gc=gc`, `gc=gc["key"]`, self-feed set_vec_int(gc.get_vec_int())** |
| test_operators.cc | kept auto-convert semantics of `operator[]` (destroy+convert, auto-resize) pinned by test; const [] / operator() throw paths; `where` context in messages |
| test_promotion.cc | full promote matrix, ladders scalar→vec→mat, idempotence, illegal promotions throw |
| test_conversion_checks.cc | boundary values: ±2^31 real rejected, 1e300/NaN/Inf rejected, LONG>INT32_MAX rejected, imag≠0 rejected |
| test_serialization.cc | round-trip all types + nested; 200 seeded random GCs property round-trip; `mem_size()==buffer`; **golden byte-vector pin (wire compat)**; truncated-at-every-prefix + corrupted-tag failure loops |
| test_collapse.cc / test_tables.cc / test_support.cc | Collapse cases; write_table/read_formatted_data round trips (examples/example07_data.txt fixture); from_file dispatch, string_escape |
| test_errors.cc / test_print_dump.cc | GenericError is-a runtime_error; message content; info/dump/print/to_yaml smoke on maximal container |
| test_c_interface.cc / test_ffi.cc | C API round trips + error codes; gc_from_json/gc_to_json, gc_last_error, NULL-handle safety |
| test_nlohmann_interface.cc / test_json_yaml_toml.cc | json↔GC structural round trips (examples/data.json, test.yml, settings.toml) |

Keep example01-18 as CTest smoke tests; retire `test_all` from CTest in Phase 6.

**Phase 2 — Storage rewrite** (single PR, public signatures still unchanged to isolate risk): Box/variant/special members/`ref_or_throw`/`ensure`/`reset_to`; port all `m_data.x` accesses across GenericContainer.cc, Promote, Serialize, Collapse, Tables, Support; const-hole fixes (`push_*` become non-const; `find` in const map queries); `from_gc`/`to_gc` as wrappers. Un-skip bug-1/2/5 tests. Gate incl. golden serialization bytes unchanged + sanitizers.

**Phase 3 — Eigen matrix migration + size_t migration + checked_narrow**: add Eigen FetchContent as PUBLIC dep; swap `mat_*_type` aliases to `Eigen::MatrixX<T>`; adapt matrix code paths in GenericContainer.cc, Serialize (raw `data()` write — golden bytes must stay identical), Promote, Collapse, Tables; apply §API sizes; update interface-layer call sites listed above (`num_rows()`→`rows()` where they touch matrix objects directly); un-skip conversion-check tests.

**Phase 4 — API polish**: gc_error/GenericError + mechanical macro-call conversion; `operator==`; span overloads; nodiscard/noexcept/concepts/constexpr; Windows-fork and camelCase removal (update example07/10, test_all); column-major doc fix. Watch MSVC on CI closely this phase.

**Phase 5 — Serialization hardening**: uint64 mem_size + overflow throw; matrix-dim pre-check; enable corrupted-buffer test loop.

**Phase 6 — Cleanup & CI**: retire `test_all` from CTest; add ASan+UBSan Debug job (ubuntu+macos, `-fsanitize=address,undefined -fno-sanitize-recover=all`) to .github/workflows/ci.yml; README note on breaking changes.

## Verification

- Every phase: full configure/build/ctest (Ninja, Debug) locally on macOS/Clang; CI matrix (macos/ubuntu-22.04/windows) is the portability gate.
- Phase 1's battery is the behavioral baseline: the variant rewrite must land green against it.
- Serialization wire format pinned by a golden byte-vector test generated from the *old* code in Phase 1 and asserted unchanged through Phase 5.
- ASan+UBSan runs prove the fixed UAF/double-free/narrowing bugs and guard the rewrite.

## Top risks & mitigations

- **MSVC quirks** (variant CTAD, requires-in-lambda, flag ordering): Phase 0 CI gate first; conservative `if constexpr` dispatch; keep `#pragma warning(disable:4661)`; possibly `/bigobj` for the test executable once Eigen is in.
- **`std::visit` compile-time/code-size blowup**: visitors only where switches existed; `get_if` on hot paths; measure GenericContainer.cc compile time before/after.
- **Eigen as public dependency**: compile-time cost on every consumer TU (accepted by user decision); include only `<Eigen/Core>`; mark Eigen includes SYSTEM to suppress its warnings on new Clang/GCC; installed CMake package must require/propagate Eigen3.
- **Hidden reliance on mat_type being a vector** (`size()`, iterators, `push_back`?): compile gate catches every in-repo use during Phase 3; Eigen provides `size()`/`data()` but not vector iteration semantics — adapt with `reshaped()`/raw loops.
- **Eigen::Index signedness** (ptrdiff_t vs size_t): centralize casts in the GC-level `num_rows/num_cols/get_*_at` accessors; enable `-Wsign-conversion` on core to catch strays.
- **Behavioral drift in kept semantics**: characterization tests written *before* the rewrite; dump/print tests structural not byte-exact; serialization byte-exact.
- **vector\<bool\> proxy vs generic visitors**: `if constexpr` special-casing; dedicated test sections.
