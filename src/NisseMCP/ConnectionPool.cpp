#include "ConnectionPool.h"

using namespace ThorsAnvil::Nisse::MCP;

ConnectionPool::ConnectionPool(ThorsAnvil::ThorsSocket::SocketInit const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version)
    : socketInfo{socketInfo}
    , version{version}
{}

ConnectionPool::PoolItem ConnectionPool::getConnection()
{
    if (spare.size() != 0) {
        PoolItem result = spare.back();
        spare.pop_back();
        return result;
    }

    pool.emplace_back(socketInfo, version);
    return --(pool.end());
}

void ConnectionPool::returnConnection(PoolItem item)
{
    spare.emplace_back(item);
}
