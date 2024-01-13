#include <lean/lean.h>
#include <sqlite3.h>
#include <stdlib.h>

static lean_external_class *g_sqlean_class;
static void sqlean_finalizer(void *) { /* We don't automatically close the db */
}
static void sqlean_foreach(void *, lean_obj_arg) {}

static lean_external_class *g_sqlean_stmt_class;
static void sqlean_stmt_finalizer(void *stmt) {
  sqlite3_finalize(static_cast<sqlite3_stmt *>(stmt));
}
static void sqlean_stmt_foreach(void *, lean_obj_arg) {}

static lean_external_class *g_sqlean_value_class;
static void sqlean_value_finalizer(void *value) {
  sqlite3_value_free(static_cast<sqlite3_value *>(value));
}
static void sqlean_value_foreach(void *, lean_obj_arg) {}

extern "C" LEAN_EXPORT lean_obj_res sqlean_initialize(lean_obj_arg /* w */) {
  if (!g_sqlean_class) {
    g_sqlean_class =
        lean_register_external_class(sqlean_finalizer, sqlean_foreach);
    g_sqlean_stmt_class = lean_register_external_class(sqlean_stmt_finalizer,
                                                       sqlean_stmt_foreach);
    g_sqlean_value_class = lean_register_external_class(sqlean_value_finalizer,
                                                        sqlean_value_foreach);
  }
  return lean_io_result_mk_ok(lean_box(0));
}

extern "C" LEAN_EXPORT lean_obj_res sqlean_open_v2(b_lean_obj_arg filename,
                                                   uint32_t flags,
                                                   lean_obj_arg /* w */) {
  sqlite3 *db = nullptr;
  sqlite3_open_v2(lean_string_cstr(filename), &db, flags, nullptr);
  return lean_io_result_mk_ok(lean_alloc_external(g_sqlean_class, db));
}

extern "C" LEAN_EXPORT lean_obj_res sqlean_prepare_v3(b_lean_obj_arg db,
                                                      b_lean_obj_arg sql,
                                                      uint8_t prepFLags,
                                                      lean_obj_arg /* w */) {
  sqlite3_stmt *stmt = nullptr;
  sqlite3_prepare_v3(static_cast<sqlite3 *>(lean_get_external_data(db)),
                     lean_string_cstr(sql), lean_string_byte_size(sql),
                     prepFLags, &stmt, nullptr);
  return lean_io_result_mk_ok(lean_alloc_external(g_sqlean_stmt_class, stmt));
}

extern "C" LEAN_EXPORT lean_obj_res sqlean_column_value(b_lean_obj_arg stmt,
                                                        uint16_t iCol,
                                                        lean_obj_arg /* w */) {
  sqlite3_value *value = sqlite3_value_dup(sqlite3_column_value(
      static_cast<sqlite3_stmt *>(lean_get_external_data(stmt)), iCol));
  return lean_io_result_mk_ok(lean_alloc_external(g_sqlean_value_class, value));
};

extern "C" LEAN_EXPORT lean_obj_res sqlean_step(b_lean_obj_arg stmt,
                                                lean_obj_arg /* w */) {
  sqlite3_step(static_cast<sqlite3_stmt *>(lean_get_external_data(stmt)));
  return lean_io_result_mk_ok(lean_box(0));
}

extern "C" LEAN_EXPORT lean_obj_res sqlean_reset(b_lean_obj_arg stmt,
                                                 lean_obj_arg /* w */) {
  sqlite3_reset(static_cast<sqlite3_stmt *>(lean_get_external_data(stmt)));
  return lean_io_result_mk_ok(lean_box(0));
}

extern "C" LEAN_EXPORT lean_obj_res sqlean_errmsg(b_lean_obj_arg db,
                                                  lean_obj_arg /* w */) {
  const char *msg =
      sqlite3_errmsg(static_cast<sqlite3 *>(lean_get_external_data(db)));
  return lean_io_result_mk_ok(lean_mk_string(msg));
}
