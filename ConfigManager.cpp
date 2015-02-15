#include "ConfigManager.hpp"

ConfigManager::ConfigManager()
{
    mResolverPort = -1;
    mResolverIP.clear();
}

ConfigManager::~ConfigManager()
{

}

void ConfigManager::parseArgs(int aArgc, char * aArgv[])
{
    // Attempt to find the port and IP of the DNS resolver on the command line.
    // if(aArgc < 3)
    // {
    //     std::cerr << "Not enough arguments. Ex: DNSClient -p 53 [-ip 127.0.0.1]\n";
    //     exit(-1);
    // }
    for(int i = 1; i + 1 < aArgc; i = i + 2)
    {
      char * lArgFlag = aArgv[i];
      char * lArgValue = aArgv[i + 1];

      if(!strcmp("-p", lArgFlag))
      {
        mResolverPort = atoi(lArgValue);
      }
      else if(!strcmp("-ip", lArgFlag))
      {
        mResolverIP = lArgValue;
      }
    }

    // If the resolver IP was not set on the command line, attempt to find the IP
    // in /etc/resolv.conf
    if(mResolverIP.empty())
    {
        parseResolver(mResolverIP);
    }

    // DEBUG!!!!!
    if(mResolverPort == -1)
    {
        std::cout << "DEBUG!! Defaulting to port 53..." << std::endl;
        mResolverPort = 53;
    }
}

int ConfigManager::getResolverPort() const
{
    return mResolverPort;
}

const std::string & ConfigManager::getResolverIPString() const
{
    return mResolverIP;
}

in_addr_t ConfigManager::getResolverIPInetAddr() const
{
    return inet_addr(getResolverIPString().c_str());
}

void ConfigManager::parseResolver(std::string & aResolverIP)
{
    std::ifstream lFileStream;
    lFileStream.open(RESOLVER_CONF_FILE, std::ios::in);
    if(lFileStream.is_open())
    {
        while(!lFileStream.eof())
        {
            std::string lLine;
            getline(lFileStream, lLine);
            std::vector<std::string> lStrings = StringUtilities::SplitString(lLine, " ");

            if(lStrings.size() >= 2 && lStrings[0] == "nameserver")
            {
                mResolverIP = lStrings[1];
            }
        }
    }
    else
    {
        throw ParseException(RESOLVER_CONF_FILE, "Could not open.");
    }
    lFileStream.close();

    if(mResolverIP.empty())
    {
        throw ParseException("Could not determine nameserver.");
    }
}
