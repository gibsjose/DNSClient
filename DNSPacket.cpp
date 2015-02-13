#include "DNSPacket.h"

void Record::EncodeName(void) {
    std::vector<std::string> tokens = StringUtilities::SplitString(rawName, ".");
    std::ostringstream oss;

    for(int i = 0; i < tokens.size(); i++) {
        std::string & token = tokens.at(i);
        oss << token.size();
        oss << token;
    }
    oss << "0";

    name = oss.str();
}

void Record::EncodeName(const std::string & rawName) {
    this->rawName = rawName;
    this->EncodeName();
}

void Record::Print(void) {
    std::cout << "\t\tRaw Name --> " << rawName << std::endl;
    std::cout << "\t\tEncoded Name --> " << name << std::endl;
    std::cout << "\t\tType --> " << recordType << std::endl;
    std::cout << "\t\tClass --> " << recordClass << std::endl;
}

size_t Record::Size(void) {
    size_t size = 0;

    size += this->name.size();
    size += sizeof(this->recordType);
    size += sizeof(this->recordClass);

    return size;
}

char * Record::GetData(void) {
    //Malloc the required amount of space
    size_t dataLen = this->Size();
    data = (char *)malloc(dataLen);
    char * p = data;

    memcpy(p, this->name.c_str(), this->name.size());
    p += this->name.size();

    memcpy(p, &(this->recordType), sizeof(this->recordType));
    p += sizeof(this->recordType);

    memcpy(p, &(this->recordClass), sizeof(this->recordClass));

    return data;
}

void ExtendedRecord::Print(void) {
    std::cout << "\t\tRaw Name --> " << rawName << std::endl;
    std::cout << "\t\tEncoded Name --> " << name << std::endl;
    std::cout << "\t\tType --> " << recordType << std::endl;
    std::cout << "\t\tClass --> " << recordClass << std::endl;
    std::cout << "\t\tTTL --> " << ttl << std::endl;
    std::cout << "\t\tRecord Data Length --> " << rdlength << std::endl;
    std::cout << "\t\tRecord Data --> " << rdata << std::endl;
}

size_t ExtendedRecord::Size(void) {
    size_t size = 0;

    size += this->name.size();
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

    memcpy(p, this->name.c_str(), this->name.size());
    p += this->name.size();

    memcpy(p, &(this->recordType), sizeof(this->recordType));
    p += sizeof(this->recordType);

    memcpy(p, &(this->recordClass), sizeof(this->recordClass));
    p += sizeof(this->recordClass);

    memcpy(p, &(this->ttl), sizeof(this->ttl));
    p += sizeof(this->ttl);

    memcpy(p, &(this->rdlength), sizeof(this->rdlength));
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
    std::cout << std::hex << "\tFlags --> " << flags << std::endl;
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

    short id = htons(this->id);
    short flags = htons(this->flags);
    short qdcount = htons(this->qdcount);
    short ancount = htons(this->ancount);
    short nscount = htons(this->nscount);
    short arcount = htons(this->arcount);

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
