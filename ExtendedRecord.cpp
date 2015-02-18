#include "ExtendedRecord.hpp"

void ExtendedRecord::Print(void) {
    std::cout << "\t\tRaw Name --> " << rawName << std::endl;
    std::cout << "\t\tEncoded Name --> " << displayName << std::endl;
    std::cout << "\t\tType --> " << DecodeType(recordType) << std::endl;
    std::cout << "\t\tClass --> " << DecodeClass(recordClass) << std::endl;
    std::cout << "\t\tTTL --> " << ttl << std::endl;
    std::cout << "\t\tRecord Data Length --> " << rdlength << std::endl;
    std::cout << "\t\tRecord Data --> " << rdata << std::endl;
}

size_t ExtendedRecord::Size(void) {
    size_t size = 0;

    size += strlen(this->name) + 1;
    size += sizeof(this->recordType);
    size += sizeof(this->recordClass);
    size += sizeof(this->ttl);
    size += sizeof(this->rdlength);
    size += this->rdata.size();

    return size;
}

char * ExtendedRecord::GetData(void) {
    //Malloc the required amount of space
    size_t dataLen = this->Size();
    data = (char *)malloc(dataLen);
    char * p = data;

    memcpy(p, this->name, strlen(this->name) + 1);
    p += strlen(this->name) + 1;

    unsigned short recordType = SWAP16(this->recordType);
    unsigned short recordClass = SWAP16(this->recordClass);
    uint32_t ttl = SWAP16(this->ttl);
    unsigned short rdlength = SWAP16(this->rdlength);

    memcpy(p, &(recordType), sizeof(this->recordType));
    p += sizeof(this->recordType);

    memcpy(p, &(recordClass), sizeof(this->recordClass));
    p += sizeof(this->recordClass);

    memcpy(p, &(ttl), sizeof(this->ttl));
    p += sizeof(this->ttl);

    memcpy(p, &(rdlength), sizeof(this->rdlength));
    p += sizeof(this->rdlength);

    memcpy(p, this->rdata.c_str(), this->rdata.size());

    return data;
}

std::string ExtendedRecord::getIPFromBytes(unsigned char * aBytes, unsigned short aNumBytes)
{
    char * str = (char *)malloc(sizeof(char) * 32);
    sprintf(str, "%u.%u.%u.%u", static_cast<unsigned>(aBytes[0]), static_cast<unsigned>(aBytes[1]), \
        static_cast<unsigned>(aBytes[2]), static_cast<unsigned>(aBytes[3]));
    std::string lStr(str);
    free(str);
    return lStr;
}
