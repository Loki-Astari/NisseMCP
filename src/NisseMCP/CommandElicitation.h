#ifndef THORSANVIL_NISSE_MCP_COMMAND_ELICITATION_H
#define THORSANVIL_NISSE_MCP_COMMAND_ELICITATION_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"
#include "CommandTask.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#elicitation/create
namespace ThorsAnvil::Nisse::MCP::Command
{
    // https://modelcontextprotocol.io/specification/2025-11-25/schema#elicitrequestformparams
    struct ElicitRequestFormParams
    {
        OptTaskMetadata     task;
        OptMeta /*vera*/    _meta;
        OptString           mode; // “form”;
        std::string         message;
        Schema              requestedSchema;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#elicitrequesturlparams
    struct ElicitRequestURLParams
    {
        OptTaskMetadata     task;
        OptMeta /*vera*/    _meta;
        std::string         mode; // “url”;
        std::string         message;
        std::string         elicitationId;
        std::string         url;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#elicitrequestparams
    using ElicitRequestParams   = std::variant<ElicitRequestFormParams, ElicitRequestURLParams>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#elicitrequest
    struct ElicitRequest
    {
        std::string         jsonrpc; // “2.0”;
        RequestId           id;
        std::string         method; // “elicitation/create”;
        ElicitRequestParams params;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#elicitrequestparams
    enum class Action {accept, decline, cancel};
    struct ElicitContent
    {
        // [key: string]: string | number | boolean | string[] };
    };
    using OptElicitContent  = std::optional<ElicitContent>;
    struct ElicitResult
    {
        OptMeta /*vera*/    _meta;
        Action              action;
        OptElicitContent    content;
        // [key: string]: unknown;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#booleanschema
    struct BooleanSchema
    {
        std::string         type; // “boolean”;
        OptString           title;
        OptString           description;
        OptBool             isDefault;  // Actual => default;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#enumschema
    struct LegacyTitledEnumSchema
    {
        std::string         type; // “string”;
        OptString           title;
        OptString           description;
        VecString           enums;   // Actual enum
        OptVecString        enumNames;
        OptString           isDefault; // Actual default
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#numberschema
    struct NumberSchema
    {
        std::string         type; // “number” | “integer”;
        OptString           title;
        OptString           description;
        OptNumber           minimum;
        OptNumber           maximum;
        OptNumber           isDefault;  // Actual default
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#singleselectenumschema
    enum class Format {uri, email, date, date_time}; // date_time => date-time
    using OptFormat         = std::optional<Format>;
    struct StringSchema
    {
        std::string         type; // “string”;
        OptString           title;
        OptString           description;
        OptNumber           minLength;
        OptNumber           maxLength;
        OptFormat           format;
        OptString           isDefault;  // Actual default
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#titledmultiselectenumschema
    struct ItemInfo
    {
        std::string         isConst; // Actual const
        std::string         title;
    };
    using VecItemInfo       = std::vector<ItemInfo>;
    struct AnyItems
    {
        VecItemInfo         anyOf;
    };
    struct TitledMultiSelectEnumSchema
    {
        std::string         type; // “array”;
        OptString           title;
        OptString           description;
        OptNumber           minItems;
        OptNumber           maxItems;
        AnyItems            items;
        OptVecString        isDefault; // ACtual default
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#titledsingleselectenumschema
    struct TitledSingleSelectEnumSchema
    {
        std::string         type; // “string”;
        OptString           title;
        OptString           description;
        VecItemInfo         oneOf;
        OptString           isDefault; // Actual default
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#untitledmultiselectenumschema
    struct EnumItem
    {
        std::string         type; // “string”;
        VecString           enums; // Actual enum
    };
    using VecEnumItem       = std::vector<EnumItem>;
    struct UntitledMultiSelectEnumSchema
    {
        std::string         type; // “array”;
        OptString           title;
        OptString           description;
        OptNumber           minItems;
        OptNumber           maxItems;
        VecEnumItem         items;
        OptVecString        isDefault; // Actual default
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#untitledsingleselectenumschema
    struct UntitledSingleSelectEnumSchema
    {
        std::string         type; // “string”;
        OptString           title;
        OptString           description;
        VecString           enums; // Actual enum
        OptString           isDefault; // Actual default
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#multiselectenumschema
    using MultiSelectEnumSchema     = std::variant<UntitledMultiSelectEnumSchema, TitledMultiSelectEnumSchema>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#singleselectenumschema
    using SingleSelectEnumSchema    = std::variant<UntitledSingleSelectEnumSchema, TitledSingleSelectEnumSchema>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#enumschema
    using EnumSchema                = std::variant<SingleSelectEnumSchema, MultiSelectEnumSchema, LegacyTitledEnumSchema>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#primitiveschemadefinition
    using PrimitiveSchemaDefinition = std::variant<StringSchema, NumberSchema, BooleanSchema, EnumSchema>;

}

#endif
