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
            : m_body{ body } {

        }

        void init(::boost::system::error_code& errorCode) {
            errorCode = {};
        }
    
        ::boost::optional<::std::pair<const_buffers_type, bool>> get(::boost::system::error_code& errorCode) {
            errorCode = {};
            
            size_t size{ 0 };
            for (auto current = ::std::begin(m_body); current != ::std::end(m_body); ++current) {
                m_buffer[size] = '&'; ++size;
                const ::std::string& key{ current->first };
                const ::std::string& value{ current->second };
                ::std::memcpy(m_buffer + size, key.data(), key.size()); size += key.size();
                m_buffer[size] = '='; ++size;
                ::std::memcpy(m_buffer + size, value.data(), value.size()); size += value.size();
            }
            
            return std::make_pair(
                boost::asio::const_buffer(m_buffer + 1, size - 1), 
                    false); 
        }
      private:
        const value_type& m_body;
        char m_buffer[32768]; 
    };
};

} // http
} // beast
} // eboost

#endif
