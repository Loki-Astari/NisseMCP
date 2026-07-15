#ifndef THORSANVIL_NISSE_MCP_COMMAND_CONTENT_H
#define THORSANVIL_NISSE_MCP_COMMAND_CONTENT_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#content
namespace ThorsAnvil::Nisse::MCP::Command
{
    using Audio = std::vector<char>;    // Base 64 encoded.

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#audiocontent
    struct AudioContent
    {
        Audio               type;
        std::string         data;
        std::string         mimeType;
        OptAnnotations      annotations;
        OptMeta /*vera*/    _meta;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#blobresourcecontents
    struct BlobResourceContents
    {
        std::string         uri;
        OptString           mimeType;
        OptMeta /*vera*/    _meta;
        std::string         blob;
    };


    // https://modelcontextprotocol.io/specification/2025-11-25/schema#imagecontent
    struct ImageContent
    {
        std::string         type; // “image”;
        std::string         data;
        std::string         mimeType;
        OptAnnotations      annotations;
        OptMeta /*vera*/    _meta;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#resourcelink
    struct ResourceLink
    {
        std::string         type; // “resource_link”;
        OptVecIcon          icons;
        std::string         name;
        OptString           title;
        std::string         uri;
        OptString           description;
        OptString           mimeType;
        OptAnnotations      annotations;
        OptNumber           size;
        OptMeta /*vera*/    _meta;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#textcontent
    struct TextContent
    {
        std::string         type; // “text”;
        std::string         text;
        OptAnnotations      annotations;
        OptMeta /*vera*/    _meta;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#textresourcecontents
    struct TextResourceContents
    {
        std::string         uri;
        OptString           mimeType;
        OptMeta /*vera*/    _meta;
        std::string         text;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#embeddedresource
    using EmbededResourceData          = std::variant<TextResourceContents, BlobResourceContents>;
    struct EmbeddedResource
    {
        std::string         type; // “resource”;
        EmbededResourceData resource;
        OptAnnotations      annotations;
        OptMeta /*vera*/    _meta;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#contentblock
    using ContentBlock      = std::variant<TextContent, ImageContent, AudioContent, ResourceLink, EmbeddedResource>;
    using VecContentBlock   = std::vector<ContentBlock>;

}

#endif
