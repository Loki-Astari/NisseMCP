#ifndef THORSANVIL_NISSE_MCP_COMMAND_COMMON_H
#define THORSANVIL_NISSE_MCP_COMMAND_COMMON_H

#include "NisseMCPConfig.h"
#include "JsonRPC.h"

#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>

#include <optional>
#include <string>
#include <vector>
#include <any>
#include <variant>
#include <map>
#include <memory>

// https://modelcontextprotocol.io/specification/2025-11-25/schema#common-types
namespace ThorsAnvil::Nisse::MCP::Command
{

struct Object {};
using OptObject         = std::optional<Object>;

using RequestId         = JsonRPC::RequestId;
using OptRequestId      = std::optional<RequestId>;

using OptBool           = std::optional<bool>;

using OptString         = std::optional<std::string>;

using VecString         = std::vector<std::string>;
using OptVecString      = std::optional<VecString>;

using Number            = int;
using OptNumber         = std::optional<Number>;

enum class Theme {light, dark};
using OptTheme          = std::optional<Theme>;

struct Icon
{
    std::string         src;
    OptString           mimeType;
    OptVecString        sizes;
    OptTheme            theme;
};
using VecIcon           = std::vector<Icon>;
using OptVecIcon        = std::optional<VecIcon>;

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Object);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Icon,                 src, mimeType, sizes, theme);

#endif
