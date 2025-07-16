#include "re2/re2.h"
#include "sqlite3.h"

extern "C" {
    void esqlite3_regexp(::sqlite3_context *context, int argc, ::sqlite3_value *argv[]) {
        ::std::string_view pattern{ 
            reinterpret_cast<const char *>(::sqlite3_value_text(argv[0])),
            static_cast<size_t>(::sqlite3_value_bytes(argv[0]))
        };
        ::std::string_view value{
            reinterpret_cast<const char *>(::sqlite3_value_text(argv[1])),
            static_cast<size_t>(::sqlite3_value_bytes(argv[1]))
        };
        ::sqlite3_result_int(context, ::RE2::PartialMatch(value, pattern));
    };
};