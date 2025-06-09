#ifndef EBOOST_BEAST_ENSURE_SUCCESS_HPP
#define EBOOST_BEAST_ENSURE_SUCCESS_HPP

#include <boost/beast/core/error.hpp>

namespace eboost {
namespace beast {

void ensure_success(::boost::beast::error_code errorCode) {
    if (errorCode) {
        throw ::boost::beast::system_error{ errorCode } ;
    }
}

} // beast
} // eboost

#endif
