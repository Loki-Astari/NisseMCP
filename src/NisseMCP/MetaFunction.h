#ifndef THORSANVIL_NISSE_MCP_METAFUNCTION_H
#define THORSANVIL_NISSE_MCP_METAFUNCTION_H

#include "NisseMCPConfig.h"
#include "JsonRPC.h"

#include <functional>
#include <type_traits>

namespace ThorsAnvil::Nisse::MCP
{

struct Context;

// Map an `operator()` signature to its single parameter.
// Catch-all: no `Param` member. Only the `P const&` single-parameter forms
// below define `Param`, so anything else (by-value, multi-arg, zero-arg)
// is a clean SFINAE-friendly rejection rather than a hard error.
template<typename F>
struct ParamOfSignature
{};
template<typename R, typename C, typename P>
struct ParamOfSignature<R(C::*)(Context&, JsonRPC::OptRequestId, P const&) const>
{
    using Param = P;
};
template<typename R, typename C, typename P>
struct ParamOfSignature<R(C::*)(Context&, JsonRPC::OptRequestId, P const&)>
{
    using Param = P;
};

// Primary template: for callables (lambdas, functors) deduce from operator().
// Delegating to ParamOfSignature (not FirstParam) means an unrecognised
// signature stops here with no `Param` instead of recursing into a hard error.
template<typename T>
struct FirstParam : ParamOfSignature<decltype(&std::remove_reference_t<T>::operator())>
{};

// Raw function type.
template<typename R, typename P>
struct FirstParam<R(Context&, JsonRPC::OptRequestId, P const&)>
{
    using Param = P;
};
// std::function.
template<typename R, typename P>
struct FirstParam<std::function<R(Context&, JsonRPC::OptRequestId, P const&)>>
{
    using Param = P;
};

template<typename C>
concept HadSingleParam = requires {typename FirstParam<std::remove_reference_t<C>>::Param;};

}

#endif
