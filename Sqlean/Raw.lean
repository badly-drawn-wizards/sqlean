import Sqlean.ModuleRuntime

initialize sqlean_initialize

opaque Db : Type
opaque Stmt : Type

@[extern "sqlean_open_v2"] opaque open_v2 (filename : @& String) (flags : UInt32 := 0x10006) : IO Db
@[extern "sqlean_prepare_v3"] opaque prepare_v3 (db : @& Db) (sql : @& String) (prepFlags : UInt8 := 0) : IO Stmt
@[extern "sqlean_step"] opaque step (stmt : @& Stmt) : BaseIO Unit
@[extern "sqlean_errmsg"] opaque errmsg (db : @& Db) : BaseIO String


