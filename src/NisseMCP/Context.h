#ifndef THORSANVIL_NISSE_MCP_CONTEXT_H
#define THORSANVIL_NISSE_MCP_CONTEXT_H

#include <ostream>
#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{

    enum State {OK, ErrorReported, ErrorNoInput, ErrorItem};

    class Context
    {
        std::ostream&   output;
        State           state = OK;
        std::size_t     count;
        bool            stream;
        public:
            Context(std::ostream& output)
                : output(output)
                , state(State::OK)
                , count(0)
                , stream(false)
            {}
            ~Context()
            {
                // Close the output array.
                if (stream && count > 0) {
                    output << "]";
                }
            }
            std::ostream&  error(State newState)
            {
                if (newState != State::ErrorItem) {
                    state = newState;
                }
                std::string_view sep = !stream ? "" : (count == 0) ? "[" : ",";
                ++count;
                return output << sep;
            }
            void serverSideStream()
            {
                stream = true;
            }
            std::ostream&  addItem()
            {
                std::string_view sep = !stream ? "" : (count == 0) ? "[" : ",";
                ++count;
                return output << sep;
            }
    };
}

#endif
