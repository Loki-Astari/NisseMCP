#ifndef THORSANVIL_NISSE_MCP_COMMAND_COMMON_H
#define THORSANVIL_NISSE_MCP_COMMAND_COMMON_H

#include "JsonRPC.h"

#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>

#include <vector>
#include <optional>
#include <any>

// https://modelcontextprotocol.io/specification/2025-11-25/schema#common-types
namespace ThorsAnvil::Nisse::MCP::Command
{
    using OptBool           = std::optional<bool>;

    using OptString         = std::optional<std::string>;

    using VecString         = std::vector<std::string>;
    using OptVecString      = std::optional<VecString>;

    using Number            = int;
    using OptNumber         = std::optional<Number>;

    using OptAny            = std::optional<std::any>;

    enum class Role {user, assistant};
    using VecRole           = std::vector<Role>;
    using OptVecRole        = std::optional<VecRole>;

    struct PaginatedRequestParams
    {};
    using OptPaginatedRequestParams = std::optional<PaginatedRequestParams>;

    struct RequestParams
    {};
    using OptRequestParams              = std::optional<RequestParams>;

    struct Properties
    {
        // : { [key: string]: PrimitiveSchemaDefinition };
    };
    struct Schema
    {
        OptString           $schema;
        std::string         type; //  “object”;
        Properties          properties;
        OptVecString        required;
    };

    struct Annotations
    {
        OptVecRole          audience;
        OptNumber           priority;
        OptString           lastModified;
    };
    using OptAnnotations    = std::optional<Annotations>;

    using Cursor            = std::string;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#error
    struct Error
    {
        Number              code;
        std::string         message;
        OptString           data;           // String for now. Can be more complicated. See: Error docs.
    };

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

    enum class LoggingLevel {debug, info, notice, warning, error, critical, alert, emergency };

    using ProgressToken     = JsonRPC::SerializableId;
    using OptProgressToken  = std::optional<ProgressToken>;
    using RequestId         = JsonRPC::RequestId;
    using OptRequestId      = std::optional<RequestId>;

    struct Meta
    {
        OptProgressToken    progressToken;
        // [key: string]: unknown
    };
    using OptMeta           = std::optional<Meta>;

    struct Result
    {
        OptMeta /*vera*/    _meta;
        // [key: string]: unknown;
    };

    using EmptyResult       = Result;

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Icon,                 src, mimeType, sizes, theme);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Meta,                 progressToken);

#endif
