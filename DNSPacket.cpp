#include "DNSPacket.h"

void Record::EncodeName(void) {
    std::vector<std::string> tokens = StringUtilities::SplitString(rawName, ".");
    std::ostringstream oss;

    //Allocate bytes for name (calloc to clear)
    name = (char *)calloc(rawName.size() + 1, sizeof(char));
    char * p = name;

    for(int i = 0; i < tokens.size(); i++) {
        std::string & token = tokens.at(i);
        oss << token.size();
        oss << token;

        //Copy the size of the token (as an unsigned char)
        unsigned char l = (unsigned char)token.size();
        memcpy(p, &l, sizeof(char));
        p += sizeof(char);

        //Copy the actual token (minus the NULL byte)
        memcpy(p, token.c_str(), token.size());
        p += token.size();
    }

    oss << "0";

    //Terminate with a '0'
    unsigned char l = 0;
    memcpy(p, &l, sizeof(char));

    displayName = oss.str();
}

void Record::EncodeName(const std::string & rawName) {
    this->rawName = rawName;
    this->EncodeName();
}

void Record::Print(void) {
    std::cout << "\t\tRaw Name --> " << rawName << std::endl;
    std::cout << "\t\tEncoded Name --> " << displayName << std::endl;
    std::cout << "\t\tType --> " << recordType << std::endl;
    std::cout << "\t\tClass --> " << recordClass << std::endl;
}

size_t Record::Size(void) {
    size_t size = 0;

    size += strlen(this->name) + 1;
    size += sizeof(this->recordType);
    size += sizeof(this->recordClass);

    return size;
}

char * Record::GetData(void) {
    //Malloc the required amount of space
    size_t dataLen = this->Size();
    data = (char *)malloc(dataLen);
    char * p = data;

    memcpy(p, this->name, strlen(this->name) + 1);
    p += strlen(this->name) + 1;

#if defined (__APPLE__)
    short recordType = _OSSwapInt16(this->recordType);
    short recordClass = _OSSwapInt16(this->recordClass);
#else
    short recordType = __bswap_16(this->recordType);
    short recordClass = __bswap_16(this->recordClass);
#endif

    memcpy(p, &(recordType), sizeof(this->recordType));
    p += sizeof(this->recordType);

    memcpy(p, &(recordClass), sizeof(this->recordClass));

    return data;
}

void ExtendedRecord::Print(void) {
    std::cout << "\t\tRaw Name --> " << rawName << std::endl;
    std::cout << "\t\tEncoded Name --> " << displayName << std::endl;
    std::cout << "\t\tType --> " << recordType << std::endl;
    std::cout << "\t\tClass --> " << recordClass << std::endl;
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

#if defined (__APPLE__)
    short recordType = _OSSwapInt16(this->recordType);
    short recordClass = _OSSwapInt16(this->recordClass);
    long ttl = _OSSwapInt16(this->ttl);
    short rdlength = _OSSwapInt16(this->rdlength);
#else
    short recordType = __bswap_16(this->recordType);
    short recordClass = __bswap_16(this->recordClass);
    long ttl = __bswap_16(this->ttl);
    short rdlength = __bswap_16(this->rdlength);
#endif

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

DNSPacket::DNSPacket(const std::string & name) {
    srand(time(NULL));

    //Create a random ID
    id = (short)rand();

    //Set the flags (enable RD)
    flags = RD_FLAG;

    //Set Counts
    qdcount = 1;
    ancount = 0;
    nscount = 0;
    arcount = 0;

    data = NULL;

    //Build the question section
    question.SetRawName(name);
    question.SetType(TYPE_A);
    question.SetClass(CLASS_IN);

    //Build the length-encoded name
    question.EncodeName();

    //Encode the names for the empty sections as well in order to malloc 'name'
    answer.EncodeName();
    nameServer.EncodeName();
    additionalRecord.EncodeName();
}

DNSPacket::DNSPacket(const char * data, const size_t length) {
    if(length == 0) {
        //EXCEPTION!
        this->data = NULL;
    }

    // //Copy the data
    // malloc(this->data, length);
    // memcpy(this->data, data, length);
    //
    // //Parse the raw byte stream
    // char * p = this->data;
    //
    // memcpy(&(this->id), p, sizeof(this->id));
    // p += sizeof(this->id);
    //
    // memcpy(&(this->flags), p, sizeof(this->flags));
    // p += sizeof(this->flags);
    //
    // memcpy(&(this->qdcount), p, sizeof(this->qdcount));
    // p += sizeof(this->qdcount);
    //
    // memcpy(&(this->ancount), p, sizeof(this->ancount));
    // p += sizeof(this->ancount);
    //
    // memcpy(&(this->nscount), p, sizeof(this->nscount));
    // p += sizeof(this->nscount);
    //
    // memcpy(&(this->arcount), p, sizeof(this->arcount));
    // p += sizeof(this->arcount);
}

void DNSPacket::Print(void) {
    std::cout << "DNS Packet:" << std::endl;
    std::cout << "\tID --> " << id << std::endl;
    std::cout << "\tFlags --> " << flags << std::endl;
    std::cout << "\tQuestion Count --> " << qdcount << std::endl;
    std::cout << "\tAnswer Count --> " << ancount << std::endl;
    std::cout << "\tName Server Count --> " << nscount << std::endl;
    std::cout << "\tAdditional Record Count --> " << arcount << std::endl;

    std::cout << "\tQuestion Section" << std::endl;
    question.Print();

    std::cout << "\tAnswer Section" << std::endl;
    answer.Print();

    std::cout << "\tName Server Section" << std::endl;
    nameServer.Print();

    std::cout << "\tAdditional Record Section" << std::endl;
    additionalRecord.Print();
}

size_t DNSPacket::Size(void) {
    size_t size = 0;

    size += sizeof(this->id);
    size += sizeof(this->flags);
    size += sizeof(this->qdcount);
    size += sizeof(this->ancount);
    size += sizeof(this->nscount);
    size += sizeof(this->arcount);
    size += question.Size();
    size += answer.Size();
    size += nameServer.Size();
    size += additionalRecord.Size();

    return size;
}

char * DNSPacket::GetData(void) {

    //Malloc the required amount of space
    size_t dataLen = this->Size();
    std::cout << "dataLen " << dataLen << std::endl;

    data = (char *)malloc(dataLen);
    char * p = data;

#if defined (__APPLE__)
    short id = _OSSwapInt16(this->id);
    short flags = _OSSwapInt16(this->flags);
    short qdcount = _OSSwapInt16(this->qdcount);
    short ancount = _OSSwapInt16(this->ancount);
    short nscount = _OSSwapInt16(this->nscount);
    short arcount = _OSSwapInt16(this->arcount);
#else
    short id = __bswap_16(this->id);
    short flags = __bswap_16(this->flags);
    short qdcount = __bswap_16(this->qdcount);
    short ancount = __bswap_16(this->ancount);
    short nscount = __bswap_16(this->nscount);
    short arcount = __bswap_16(this->arcount);
#endif

    memcpy(p, &(id), sizeof(this->id));
    p += sizeof(this->id);

    memcpy(p, &(flags), sizeof(this->flags));
    p += sizeof(this->flags);

    memcpy(p, &(qdcount), sizeof(this->qdcount));
    p += sizeof(this->qdcount);

    memcpy(p, &(ancount), sizeof(this->ancount));
    p += sizeof(this->ancount);

    memcpy(p, &(nscount), sizeof(this->nscount));
    p += sizeof(this->nscount);

    memcpy(p, &(arcount), sizeof(this->arcount));
    p += sizeof(this->arcount);

    memcpy(p, this->question.GetData(), this->question.Size());
    p += this->question.Size();

    memcpy(p, this->answer.GetData(), this->answer.Size());
    p += this->answer.Size();

    memcpy(p, this->nameServer.GetData(), this->nameServer.Size());
    p += this->nameServer.Size();

    memcpy(p, this->additionalRecord.GetData(), this->additionalRecord.Size());

    return data;
}
