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
    std::string tmp;

    //3www6google3com0

    //Decode into www.google.com

    //Encode to set display name
    this->EncodeName(tmp);
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

    QuestionRecord question;

    //Build the question section
    question.SetRawName(name);
    question.SetType(TYPE_A);
    question.SetClass(CLASS_IN);

    //Build the length-encoded name
    question.EncodeName();

    //Add the question
    questions.push_back(question);

    //Encode the names for the empty sections as well in order to malloc 'name'
    // AnswerRecord answer;
    // answer.EncodeName();
    // answers.push_back(answer);
    //
    // NameServerRecord nameServer;
    // nameServer.EncodeName();
    // nameServers.push_back(nameServer);
    //
    // AdditionalRecord additionalRecord;
    // additionalRecord.EncodeName();
    // additionals.push_back(additionalRecord);
}

DNSPacket::DNSPacket(const char * data, const size_t length) {
    if(length == 0) {
        //EXCEPTION!
        this->data = NULL;
    }

    QuestionRecord question;

    std::cout << "length = " << length << std::endl;

    //Copy the data
    this->data = (char *)malloc(length);
    memcpy(this->data, data, length);

    //Parse the raw byte stream
    char * p = this->data;

    // for(int i = 0; i < (length - 1); i += 2) {
    //     short t = 0;
    //     t = (p[i + 1] | (p[i] << 8));
    //
    //     std::bitset<16> x(t);
    //     std::cout << "p[" << i << "] = " << +t << "\t(" << x << ")"<< std::endl;
    // }

    memcpy(&(this->id), p, sizeof(this->id));
    p += sizeof(this->id);
    this->id = _OSSwapInt16(this->id);

    std::cerr << "this->id = " << this->id << std::endl;

    memcpy(&(this->flags), p, sizeof(this->flags));
    p += sizeof(this->flags);
    this->flags = _OSSwapInt16(this->flags);

    std::cerr << "this->flags = " << (unsigned short)this->flags << std::endl;

    memcpy(&(this->qdcount), p, sizeof(this->qdcount));
    p += sizeof(this->qdcount);
    this->qdcount = _OSSwapInt16(this->qdcount);

    std::cerr << "this->qdcount = " << (unsigned short)this->qdcount << std::endl;

    memcpy(&(this->ancount), p, sizeof(this->ancount));
    p += sizeof(this->ancount);
    this->ancount = _OSSwapInt16(this->ancount);

    std::cerr << "this->ancount = " << (unsigned short)this->ancount << std::endl;

    memcpy(&(this->nscount), p, sizeof(this->nscount));
    p += sizeof(this->nscount);
    this->nscount = _OSSwapInt16(this->nscount);

    std::cerr << "this->nscount = " << (unsigned short)this->nscount << std::endl;

    memcpy(&(this->arcount), p, sizeof(this->arcount));
    p += sizeof(this->arcount);
    this->arcount = _OSSwapInt16(this->arcount);

    std::cerr << "this->arcount = " << (unsigned short)this->arcount << std::endl;

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
    recordType = _OSSwapInt16(recordType);
    question.SetType(recordType);

    memcpy(&recordClass, p, sizeof(recordClass));
    p += sizeof(recordClass);
    recordClass = _OSSwapInt16(recordClass);
    question.SetClass(recordClass);

    //Add the question record
    this->questions.push_back(question);

    //Parse the answer section
    for(int i = 0; i < this->ancount; i++) {
        AnswerRecord answer;

        //Skip 2 bytes for the name (should be 0xC00C)
        //Parse the name...
        p += 2;

        //Malloc the name
        answer.SetRawName(std::string());
        answer.EncodeName();

        //Copy the type
        short aType;
        memcpy(&aType, p, sizeof(short));
        p += sizeof(short);
        aType = _OSSwapInt16(aType);
        answer.SetType(aType);

        //Copy the class
        short aClass;
        memcpy(&aClass, p, sizeof(aClass));
        p += sizeof(aClass);
        aClass = _OSSwapInt16(aClass);
        answer.SetClass(aClass);

        //Copy the time to live
        long aTTL;
        memcpy(&aTTL, p, sizeof(aTTL));
        p += sizeof(aTTL);
        aTTL = _OSSwapInt16(aTTL);
        answer.SetTTL(aTTL);

        //Copy the data length
        short aRdlength;
        memcpy(&aRdlength, p, sizeof(aRdlength));
        p += sizeof(aRdlength);
        aRdlength = _OSSwapInt16(aRdlength);
        answer.SetRecordDataLength(aRdlength);

        if((aType == TYPE_A) && (aRdlength < 4)) {
            //EXCEPTION! Should always be 4 for an answer of type A (IPv4...)
            answer.SetRecordData(std::string());
        } else {
            //Copy the record data

            std::cerr << "aRdlength = " << aRdlength << std::endl;

            char * rdata = (char *)malloc(aRdlength);
            memcpy(rdata, p, aRdlength);
            p += aRdlength;

            std::ostringstream oss;
            oss << rdata[0] << "." << rdata[1] << "." << rdata[2] << "." << rdata[3];
            answer.SetRecordData(oss.str());
        }

        this->answers.push_back(answer);
    }
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
    for(int i = 0; i < questions.size(); i++) {
        std::cout << "\tQuestion " << i << std::endl;
        questions.at(i).Print();
    }

    std::cout << "\tAnswer Section" << std::endl;
    for(int i = 0; i < answers.size(); i++) {
        std::cout << "\tAnswer " << i << std::endl;
        answers.at(i).Print();
    }

    std::cout << "\tName Server Section" << std::endl;
    for(int i = 0; i < nameServers.size(); i++) {
        std::cout << "\tName Server " << i << std::endl;
        nameServers.at(i).Print();
    }

    std::cout << "\tAdditional Record Section" << std::endl;
    for(int i = 0; i < additionals.size(); i++) {
        std::cout << "\tAdditonal Record " << i << std::endl;
        additionals.at(i).Print();
    }
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
