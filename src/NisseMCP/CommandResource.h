#ifndef THORSANVIL_NISSE_MCP_COMMAND_RESOURCE_H
#define THORSANVIL_NISSE_MCP_COMMAND_RESOURCE_H

#include "CommandCommon.h"
#include "CommandContent.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#resources/list
// https://modelcontextprotocol.io/specification/2025-11-25/schema#resources/read
// https://modelcontextprotocol.io/specification/2025-11-25/schema#resources/subscribe
// https://modelcontextprotocol.io/specification/2025-11-25/schema#resources/templates/list
// https://modelcontextprotocol.io/specification/2025-11-25/schema#resources/unsubscribe
namespace ThorsAnvil::Nisse::MCP::Command
{
    // https://modelcontextprotocol.io/specification/2025-11-25/schema#listresourcesrequest
    struct ListResourcesRequest
    {
        std::string                     jsonrpc; // “2.0”;
        RequestId                       id;
        OptPaginatedRequestParams       params;
        std::string                     method; // “resources/list”;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#resource
    struct Resource
    {
        OptVecIcon                      icons;
        std::string                     name;
        OptString                       title;
        std::string                     uri;
        OptString                       description;
        OptString                       mimeType;
        OptAnnotations                  annotations;
        OptNumber                       size;
        OptMeta             /*vera*/    _meta;
    };
    using VecResource                   = std::vector<Resource>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#listresourcesresult
    struct ListResourcesResult
    {
        OptMeta             /*vera*/    _meta;
        OptString                       nextCursor;
        VecResource                     resources;
        // [key: string]: unknown;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#readresourcerequestparams
    struct ReadResourceRequestParams
    {
        OptMeta             /*vera*/    _meta;
        std::string                     uri;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#readresourcerequest
    struct ReadResourceRequest
    {
        std::string                     jsonrpc; // “2.0”;
        RequestId                       id; // RequestId;
        std::string                     method; // “resources/read”;
        ReadResourceRequestParams       params;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#readresourceresult
    using Content                       = std::variant<TextResourceContents, BlobResourceContents>;
    using VecContent                    = std::vector<Content>;
    struct ReadResourceResult
    {
        OptMeta             /*vera*/    _meta;
        VecContent                      contents;
        // [key: string]: unknown;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#subscriberequestparams
    struct SubscribeRequestParams
    {
        OptMeta             /*vera*/    _meta;
        std::string                     uri;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#subscriberequest
    struct SubscribeRequest
    {
        std::string                     jsonrpc; // “2.0”;
        RequestId                       id;
        std::string                     method; // “resources/subscribe”;
        SubscribeRequestParams          params;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#listresourcetemplatesrequest
    struct ListResourceTemplatesRequest
    {
        std::string                     jsonrpc; // “2.0”;
        RequestId                       id;
        OptPaginatedRequestParams       params;
        std::string                     method; // “resources/templates/list”;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#resourcetemplate
    struct ResourceTemplate
    {
        OptVecIcon                      icons;
        std::string                     name;
        OptString                       title;
        std::string                     uriTemplate;
        OptString                       description;
        OptString                       mimeType;
        OptAnnotations                  annotations;
        OptMeta             /*vera*/    _meta;
    };
    using VecResourceTemplate           = std::vector<ResourceTemplate>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#listresourcetemplatesresult
    struct ListResourceTemplatesResult
    {
        OptMeta             /*vera*/    _meta;
        OptString                       nextCursor;
        VecResourceTemplate             resourceTemplates;
        // [key: string]: unknown;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#unsubscriberequestparams
    struct UnsubscribeRequestParams
    {
        OptMeta             /*vera*/    _meta;
        std::string                     uri;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#unsubscriberequest
    struct UnsubscribeRequest
    {
        std::string                     jsonrpc; // “2.0”;
        RequestId                       id;
        std::string                     method; // “resources/unsubscribe”;
        UnsubscribeRequestParams        params;
    };

}

#endif
