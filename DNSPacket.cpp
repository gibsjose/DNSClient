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

void Record::DecodeName(const char * name) {

    unsigned int len = strlen(name);

    const char * p = name;
    char * tmp = (char *)malloc(len - 1);
    memset(tmp, 0, len -1);

    while(*p != (char)0) {
        strncat(tmp, (p + 1), *p);

        p += (*p + 1);

        if(*p != (char)0) {
            strncat(tmp, ".", 1);
        }
    }

    //Encode to set display name
    this->EncodeName(std::string(tmp));

    //Free tmp
    free(tmp);
}

std::string Record::DecodeType(const unsigned short recordType) {
    if(recordType == TYPE_A) {
        return "A";
    } else if(recordType == TYPE_CNAME) {
        return "CNAME";
    } else {
        return std::to_string(recordType);
    }
}

std::string Record::DecodeClass(const unsigned short recordClass) {
    if(recordClass == CLASS_IN) {
        return "Internet (IN)";
    } else {
        return std::to_string(recordClass);
    }
}

void Record::Print(void) {
    std::cout << "\t\tRaw Name --> " << rawName << std::endl;
    std::cout << "\t\tEncoded Name --> " << displayName << std::endl;
    std::cout << "\t\tType --> " << DecodeType(recordType) << std::endl;
    std::cout << "\t\tClass --> " << DecodeClass(recordClass) << std::endl;
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

    unsigned short recordType = SWAP16(this->recordType);
    unsigned short recordClass = SWAP16(this->recordClass);

    memcpy(p, &(recordType), sizeof(this->recordType));
    p += sizeof(this->recordType);

    memcpy(p, &(recordClass), sizeof(this->recordClass));

    return data;
}

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

DNSPacket::DNSPacket(const std::string & name) {
    srand(time(NULL));

    //Create a random ID
    id = (unsigned short)rand();

    //Set the flags (enable RD)
    flags = RD_FLAG;

    //Set Counts
    qdcount = 1;
    ancount = 0;
    nscount = 0;
    arcount = 0;

    data = NULL;

    QuestionRecord question;

    //Build the question section
    question.SetRawName(name);
    question.SetType(TYPE_A);
    question.SetClass(CLASS_IN);

    //Build the length-encoded name
    question.EncodeName();

    //Add the question
    questions.push_back(question);
}

DNSPacket::DNSPacket(const char * data, const size_t length) {
    if(length == 0) {
        //EXCEPTION!
        this->data = NULL;
    }

    QuestionRecord question;

    //Copy the data
    this->data = (char *)malloc(length);
    memcpy(this->data, data, length);

    //Parse the raw byte stream
    char * p = this->data;

    memcpy(&(this->id), p, sizeof(this->id));
    p += sizeof(this->id);
    this->id = SWAP16(this->id);

    memcpy(&(this->flags), p, sizeof(this->flags));
    p += sizeof(this->flags);
    this->flags = SWAP16(this->flags);

    memcpy(&(this->qdcount), p, sizeof(this->qdcount));
    p += sizeof(this->qdcount);
    this->qdcount = SWAP16(this->qdcount);

    memcpy(&(this->ancount), p, sizeof(this->ancount));
    p += sizeof(this->ancount);
    this->ancount = SWAP16(this->ancount);

    memcpy(&(this->nscount), p, sizeof(this->nscount));
    p += sizeof(this->nscount);
    this->nscount = SWAP16(this->nscount);

    memcpy(&(this->arcount), p, sizeof(this->arcount));
    p += sizeof(this->arcount);
    this->arcount = SWAP16(this->arcount);

    //Malloc the correct number of bytes to fit the question name
    //  p should be terminated with a 0x00 byte as per DNS specs
    char * name = (char *)malloc(strlen(p) + 1);
    memcpy(name, p, strlen(p) + 1);
    p += strlen(p) + 1;

    //Sets the rawName and displayName of the question and re-encodes it
    question.DecodeName(name);

    short recordType;
    short recordClass;

    memcpy(&recordType, p, sizeof(recordType));
    p += sizeof(recordType);
    recordType = SWAP16(recordType);
    question.SetType(recordType);

    memcpy(&recordClass, p, sizeof(recordClass));
    p += sizeof(recordClass);
    recordClass = SWAP16(recordClass);
    question.SetClass(recordClass);

    //Add the question record
    this->questions.push_back(question);

    //Parse the answer section
    for(int i = 0; i < this->ancount; i++) {
        AnswerRecord answer;

        //Two bytes for the name pointer (should be 0xC___)
        unsigned short nameOffset = 0;
        memcpy(&nameOffset, p, sizeof(nameOffset));
        p += sizeof(nameOffset);
        nameOffset = SWAP16(nameOffset) & ~0xC000;

        const char * answerName = data + nameOffset;

        //Decode the name
        answer.DecodeName(answerName);

        //Copy the type
        unsigned short answerType;
        memcpy(&answerType, p, sizeof(short));
        p += sizeof(short);
        answerType = SWAP16(answerType);
        answer.SetType(answerType);

        //Copy the class
        unsigned short answerClass;
        memcpy(&answerClass, p, sizeof(answerClass));
        p += sizeof(answerClass);
        answerClass = SWAP16(answerClass);
        answer.SetClass(answerClass);

        //Copy the time to live
        uint32_t answerTTL;
        memcpy(&answerTTL, p, sizeof(answerTTL));
        p += sizeof(answerTTL);
        answerTTL = SWAP32(answerTTL);
        answer.SetTTL(answerTTL);

        //Copy the data length
        unsigned short answer_rdlength;
        memcpy(&answer_rdlength, p, sizeof(answer_rdlength));

        p += sizeof(answer_rdlength);
        answer_rdlength = SWAP16(answer_rdlength);
        answer.SetRecordDataLength(answer_rdlength);

        //CNAME Record
        if(answerType == TYPE_CNAME) {
            char * rdata = (char *)calloc(answer_rdlength, sizeof(char));
            memcpy(rdata, p, answer_rdlength);
            answer.SetRecordData(std::string(rdata));
            free(rdata);
        }

        //A Record
        else if(answerType == TYPE_A) {

            //Make sure length is 4 for A records
            if(answer_rdlength != 4) {
                //EXCEPTION! Should always be 4 for an answer of type A (IPv4...)
                std::cout << "Error: Type A Records should always have a length of 4" << std::endl;
            }
            else {
                //Copy the record data
                unsigned char * rdata = (unsigned char *)calloc(answer_rdlength, sizeof(char));
                memcpy(rdata, p, answer_rdlength);
                char str[32];

                sprintf(str, "%u.%u.%u.%u", static_cast<unsigned>(rdata[0]), static_cast<unsigned>(rdata[1]), \
                    static_cast<unsigned>(rdata[2]), static_cast<unsigned>(rdata[3]));

                answer.SetRecordData(std::string(str));
                free(rdata);
            }
        }

        p += answer_rdlength;

        this->answers.push_back(answer);
    }

    //Parse the name server section
    for(int i = 0; i < this->nscount; i++) {
        NameServerRecord nameServer;

        //Two bytes for the name pointer (should be 0xC___)
        unsigned short nameOffset = 0;
        memcpy(&nameOffset, p, sizeof(nameOffset));
        p += sizeof(nameOffset);
        nameOffset = SWAP16(nameOffset) & ~0xC000;

        const char * nameServerName = data + nameOffset;

        //Decode the name
        nameServer.DecodeName(nameServerName);

        //Copy the type
        unsigned short nameServerType;
        memcpy(&nameServerType, p, sizeof(short));
        p += sizeof(short);
        nameServerType = SWAP16(nameServerType);
        nameServer.SetType(nameServerType);

        //Copy the class
        unsigned short nameServerClass;
        memcpy(&nameServerClass, p, sizeof(nameServerClass));
        p += sizeof(nameServerClass);
        nameServerClass = SWAP16(nameServerClass);
        nameServer.SetClass(nameServerClass);

        //Copy the time to live
        uint32_t nameServerTTL;
        memcpy(&nameServerTTL, p, sizeof(nameServerTTL));
        p += sizeof(nameServerTTL);
        nameServerTTL = SWAP32(nameServerTTL);
        nameServer.SetTTL(nameServerTTL);

        //Copy the data length
        unsigned short nameServer_rdlength;
        memcpy(&nameServer_rdlength, p, sizeof(nameServer_rdlength));

        p += sizeof(nameServer_rdlength);
        nameServer_rdlength = SWAP16(nameServer_rdlength);
        nameServer.SetRecordDataLength(nameServer_rdlength);

        char * rdata = (char *)calloc(nameServer_rdlength, sizeof(char));
        memcpy(rdata, p, nameServer_rdlength);
        nameServer.SetRecordData(std::string(rdata));
        free(rdata);

        p += nameServer_rdlength;

        this->nameServers.push_back(nameServer);
    }

    //Parse the name server section
    for(int i = 0; i < this->arcount; i++) {
        AdditionalRecord additional;

        //Two bytes for the name pointer (should be 0xC___)
        unsigned short nameOffset = 0;
        memcpy(&nameOffset, p, sizeof(nameOffset));
        p += sizeof(nameOffset);
        nameOffset = SWAP16(nameOffset) & ~0xC000;

        const char * additionalName = data + nameOffset;

        //Decode the name
        additional.DecodeName(additionalName);

        //Copy the type
        unsigned short additionalType;
        memcpy(&additionalType, p, sizeof(short));
        p += sizeof(short);
        additionalType = SWAP16(additionalType);
        additional.SetType(additionalType);

        //Copy the class
        unsigned short additionalClass;
        memcpy(&additionalClass, p, sizeof(additionalClass));
        p += sizeof(additionalClass);
        additionalClass = SWAP16(additionalClass);
        additional.SetClass(additionalClass);

        //Copy the time to live
        uint32_t additionalTTL;
        memcpy(&additionalTTL, p, sizeof(additionalTTL));
        p += sizeof(additionalTTL);
        additionalTTL = SWAP32(additionalTTL);
        additional.SetTTL(additionalTTL);

        //Copy the data length
        unsigned short additional_rdlength;
        memcpy(&additional_rdlength, p, sizeof(additional_rdlength));

        p += sizeof(additional_rdlength);
        additional_rdlength = SWAP16(additional_rdlength);
        additional.SetRecordDataLength(additional_rdlength);

        char * rdata = (char *)calloc(additional_rdlength, sizeof(char));
        memcpy(rdata, p, additional_rdlength);
        additional.SetRecordData(std::string(rdata));
        free(rdata);

        p += additional_rdlength;

        this->additionals.push_back(additional);
    }
}

void DNSPacket::Print(void) {
    std::cout << "DNS Packet:" << std::endl;
    std::cout << "<----------------------------------------------------------------------->" << std::endl;
    std::cout << "\tHeaders" << std::endl;
    std::cout << "\t-------" << std::endl;
    std::cout << "\tID --> " << id << std::endl;
    std::cout << "\tFlags --> 0b" << std::bitset<16>(flags) << std::endl;
    std::cout << "\tQuestion Count --> " << qdcount << std::endl;
    std::cout << "\tAnswer Count --> " << ancount << std::endl;
    std::cout << "\tName Server Count --> " << nscount << std::endl;
    std::cout << "\tAdditional Record Count --> " << arcount << std::endl;
    std::cout << std::endl;

    if(qdcount) {
        std::cout << "\tQuestion Section" << std::endl;
        std::cout << "\t----------------" << std::endl;
        for(int i = 0; i < questions.size(); i++) {
            std::cout << "\tQuestion #" << i + 1 << std::endl;
            questions.at(i).Print();
        }
    } else {
        std::cout << "\t[No Questions]" << std::endl;
    }

    std::cout << std::endl;

    if(ancount) {
        std::cout << "\tAnswer Section" << std::endl;
        std::cout << "\t--------------" << std::endl;
        for(int i = 0; i < answers.size(); i++) {
            std::cout << "\tAnswer #" << i + 1<< std::endl;
            answers.at(i).Print();
        }
    } else {
        std::cout << "\t[No Answers]" << std::endl;
    }

    std::cout << std::endl;

    if(nscount) {
        std::cout << "\tName Server Section" << std::endl;
        std::cout << "\t-------------------" << std::endl;
        for(int i = 0; i < nameServers.size(); i++) {
            std::cout << "\tName Server #" << i + 1 << std::endl;
            nameServers.at(i).Print();
        }
    } else {
        std::cout << "\t[No Name Servers]" << std::endl;
    }

    std::cout << std::endl;

    if(arcount) {
        std::cout << "\tAdditional Record Section" << std::endl;
        std::cout << "\t-------------------------" << std::endl;
        for(int i = 0; i < additionals.size(); i++) {
            std::cout << "\tAdditonal Record #" << i + 1<< std::endl;
            additionals.at(i).Print();
        }
    } else {
        std::cout << "\t[No Additional Records]" << std::endl;
    }

    std::cout << "<----------------------------------------------------------------------->" << std::endl << std::endl;
}

size_t DNSPacket::Size(void) {
    size_t size = 0;

    size += sizeof(this->id);
    size += sizeof(this->flags);
    size += sizeof(this->qdcount);
    size += sizeof(this->ancount);
    size += sizeof(this->nscount);
    size += sizeof(this->arcount);

    for(int i = 0; i < qdcount; i++) {
        size += questions.at(i).Size();
    }

    for(int i = 0; i < ancount; i++) {
        size += answers.at(i).Size();
    }

    for(int i = 0; i < nscount; i++) {
        size += nameServers.at(i).Size();
    }

    for(int i = 0; i < arcount; i++) {
        size += additionals.at(i).Size();
    }

    return size;
}

char * DNSPacket::GetData(void) {

    //Malloc the required amount of space
    size_t dataLen = this->Size();

    data = (char *)malloc(dataLen);
    char * p = data;

    unsigned short id = SWAP16(this->id);
    unsigned short flags = SWAP16(this->flags);
    unsigned short qdcount = SWAP16(this->qdcount);
    unsigned short ancount = SWAP16(this->ancount);
    unsigned short nscount = SWAP16(this->nscount);
    unsigned short arcount = SWAP16(this->arcount);

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

    for(int i = 0; i < this->questions.size(); i++) {
        memcpy(p, this->questions.at(i).GetData(), this->questions.at(i).Size());
        p += this->questions.at(i).Size();
    }

    for(int i = 0; i < this->answers.size(); i++) {
        memcpy(p, this->answers.at(i).GetData(), this->answers.at(i).Size());
        p += this->answers.at(i).Size();
    }

    for(int i = 0; i < this->nameServers.size(); i++) {
        memcpy(p, this->nameServers.at(i).GetData(), this->nameServers.at(i).Size());
        p += this->nameServers.at(i).Size();
    }

    for(int i = 0; i < this->additionals.size(); i++) {
        memcpy(p, this->additionals.at(i).GetData(), this->additionals.at(i).Size());
        p += this->additionals.at(i).Size();
    }

    return data;
}
