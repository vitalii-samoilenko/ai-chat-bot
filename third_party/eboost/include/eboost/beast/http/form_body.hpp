#ifndef EBOOST_BEAST_HTTP_FORM_BODY_HPP
#define EBOOST_BEAST_HTTP_FORM_BODY_HPP

#include <boost/beast/http.hpp>
#include <boost/asio/buffer.hpp>

namespace eboost {
namespace beast {
namespace http {

template<typename Range>
struct form_body {
    using value_type = Range;

    struct writer {
        using const_buffers_type = ::boost::asio::const_buffer;

        template<bool isRequest, class Fields>
        writer(const ::boost::beast::http::header<isRequest, Fields>&, const value_type& body)
            : _body{ body } {

        }

        void init(::boost::system::error_code& error_code) {
            error_code = {};
        }
    
        ::boost::optional<::std::pair<const_buffers_type, bool>> get(::boost::system::error_code& error_code) {
            error_code = {};
            
            size_t size{ 0 };
            for (auto const &key_value : _body) {
                _buffer[size] = '&'; ++size;
                auto const &key{ key_value.first };
                auto const &value{ key_value.second };
                ::std::memcpy(_buffer + size, key.data(), key.size()); size += key.size();
                _buffer[size] = '='; ++size;
                ::std::memcpy(_buffer + size, value.data(), value.size()); size += value.size();
            }
            
            return std::make_pair(
                boost::asio::const_buffer(_buffer + 1, size - 1), 
                    false); 
        }
      private:
        const value_type& _body;
        char _buffer[32768]; 
    };
};

} // http
} // beast
} // eboost

#endif
