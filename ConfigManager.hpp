#pragma once
#include <iostream>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <vector>

#include "Exception.h"
#include "StringUtilities.h"

#define RESOLVER_CONF_FILE "/etc/resolv.conf"

class ConfigManager
{
public:
    ConfigManager();
    ~ConfigManager();
    void parseArgs(int argc, char * argv[]);
    int getResolverPort() const;
    const std::string & getResolverIPString() const;
    in_addr_t getResolverIPInetAddr() const;

private:
    std::string mResolverIP;
    unsigned mResolverPort;

    void parseResolver(std::string & aResolverIP);
};
