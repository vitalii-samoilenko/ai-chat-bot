#ifndef ESQLITE3_ENSURE_SUCCESS_HPP
#define ESQLITE3_ENSURE_SUCCESS_HPP

#include <stdexcept>

#include <sqlite3.h>

void esqlite3_ensure_success(int error_code) {
    switch (error_code) {
    case SQLITE_OK:
    case SQLITE_ROW:
    case SQLITE_DONE:
        return;
    default:
        throw ::std::runtime_error{ ::sqlite3_errstr(error_code) };
    }
};

#endif
