#ifndef THORSANVIL_NISSE_MCP_MCPUTIL_H
#define THORSANVIL_NISSE_MCP_MCPUTIL_H

#include "NisseMCPConfig.h"
#include <ThorSerialize/Traits.h>

namespace ThorsAnvil::Nisse::MCP
{

enum class Protocol {v2024_11_05, v2025_03_26, v2025_06_18, v2025_11_25, v2026_07_28};
using ProtocolRange = std::pair<Protocol, Protocol>;

enum SessionState   {Invalid, Requested, Confirmed};

using Clock         = std::chrono::steady_clock;
using Time          = std::chrono::time_point<Clock>;
using Duration      = std::chrono::seconds;

}

ThorsAnvil_MakeEnumAlt(ThorsAnvil::Nisse::MCP::Protocol, "2024-11-05", "2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28");


#endif
