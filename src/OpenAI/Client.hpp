#include <string>

#include "boost/beast.hpp"

namespace OpenAI {
    struct Message {
        ::std::string Role;
        ::std::wstring Content;
    };

    class Client {
    public:
        inline Client() = default;
        Client(const Client&) = delete;
        Client(Client&&) = delete;

        ~Client() = default;

        Client& operator=(const Client&) = delete;
        Client& operator=(Client&&) = delete;

        template<typename Iterator>
        inline Message Complete(const ::std::string& model, Iterator begin, Iterator end) {

        }

    private:
        ::std::string 
    };
}