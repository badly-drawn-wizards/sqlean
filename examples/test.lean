import Sqlean.Raw

def main : IO Unit := do
  let db ← open_v2 "test.db"
  IO.println (←errmsg db)
  let stmt ← prepare_v3 db "CREATE TABLE LOL (id int primary key);"
  IO.println (←errmsg db)
  step stmt
  IO.println (←errmsg db)

/- #eval do -/
/-   main -/
