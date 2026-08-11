#ifndef THORSANVIL_NISSE_MCP_CONNECTION_POOL
#define THORSANVIL_NISSE_MCP_CONNECTION_POOL

#include "NisseMCPConfig.h"

#include "NisseHTTP/ClientHTTP.h"

#include <vector>
#include <list>

namespace ThorsAnvil::Nisse::MCP
{

class ConnectionFromPool;
class ConnectionPool
{

    using Pool = std::list<ThorsAnvil::Nisse::HTTP::ClientHTTP>;
    using PoolItem = Pool::iterator;
    using SpareItem = std::vector<PoolItem>;

    ThorsAnvil::ThorsSocket::SocketInit     socketInfo;
    ThorsAnvil::Nisse::HTTP::Version        version;
    Pool                                    pool;
    SpareItem                               spare;

    public:
        ConnectionPool(ThorsAnvil::ThorsSocket::SocketInit const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version);

    private:
        friend class ConnectionFromPool;
        PoolItem getConnection();
        void     returnConnection(PoolItem item);
};

class ConnectionFromPool
{
    ConnectionPool&             pool;
    ConnectionPool::PoolItem    item;
    public:
        ConnectionFromPool(ConnectionPool& pool)
            : pool{pool}
            , item{pool.getConnection()}
        {}
        ~ConnectionFromPool()                               {pool.returnConnection(item);}
        operator ThorsAnvil::Nisse::HTTP::ClientHTTP&()     {return *item;}
        ThorsAnvil::Nisse::HTTP::ClientHTTP& operator*()    {return *item;}
        ThorsAnvil::Nisse::HTTP::ClientHTTP* operator->()   {return &(*item);}
};

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "ConnectionPool.source"
#endif

#endif
