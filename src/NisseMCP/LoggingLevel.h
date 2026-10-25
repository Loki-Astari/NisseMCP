#ifndef THORSANVIL_NISSE_MCP_LOGGING_LEVEL_H
#define THORSANVIL_NISSE_MCP_LOGGING_LEVEL_H

#include "NisseMCPConfig.h"
#include "ThorsLogging/ThorsLogging.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#logging/setlevel
namespace ThorsAnvil::Nisse::MCP
{

enum class LoggingLevel
{
    emergency       = ThorsAnvil::ThorsVerbosity::Emergency,
    alert           = ThorsAnvil::ThorsVerbosity::Alert,
    critical        = ThorsAnvil::ThorsVerbosity::Critical,
    error           = ThorsAnvil::ThorsVerbosity::Error,
    warning         = ThorsAnvil::ThorsVerbosity::Warning,
    notice          = ThorsAnvil::ThorsVerbosity::Notice,
    info            = ThorsAnvil::ThorsVerbosity::Info,
    debug           = ThorsAnvil::ThorsVerbosity::Debug,
};

}

#endif
