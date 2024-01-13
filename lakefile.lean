import Lake
open Lake DSL

package sqlean where

target sqlean.o pkg : FilePath := do
  let oFile := pkg.buildDir / "c" / "sqlean.o"
  let srcJob ← inputFile <| pkg.dir / "c" / "sqlean.cpp"
  let weakArgs := #["-I", (← getLeanIncludeDir).toString]
  buildO "sqlean.cpp" oFile srcJob weakArgs #["-fPIC"] "c++" getLeanTrace

target libsqlean pkg : FilePath := do
  let name := nameToSharedLib "sqlean"
  let sqleanO ← fetch <| pkg.target ``sqlean.o
  buildLeanSharedLib (pkg.nativeLibDir / name) #[sqleanO]

module_facet sqlean mod : FilePath := libsqlean.fetch

@[default_target]
lean_lib Sqlean where
  precompileModules := true
  extraDepTargets := #[``libsqlean]
  nativeFacets := #[`sqlean, Module.oFacet]
  moreLinkArgs := #["-lsqlite3"]

lean_exe sqlean_test where
  root := `examples.test
  moreLinkArgs := #["-lsqlite3"]
