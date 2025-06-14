#ifndef EBOOST_BEAST_ENSURE_SUCCESS_HPP
#define EBOOST_BEAST_ENSURE_SUCCESS_HPP

#include <boost/beast/core/error.hpp>

namespace eboost {
namespace beast {

void ensure_success(::boost::beast::error_code error_code) {
    if (error_code) {
        throw ::boost::beast::system_error{ error_code } ;
    }
}

} // beast
} // eboost

#endif
