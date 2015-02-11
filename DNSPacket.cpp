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

void ExtendedRecord::Print(void) {
    std::cout << "\t\tRaw Name --> " << rawName << std::endl;
    std::cout << "\t\tEncoded Name --> " << name << std::endl;
    std::cout << "\t\tType --> " << recordType << std::endl;
    std::cout << "\t\tClass --> " << recordClass << std::endl;
    std::cout << "\t\tTTL --> " << ttl << std::endl;
    std::cout << "\t\tRecord Data Length --> " << rdlength << std::endl;
    std::cout << "\t\tRecord Data --> " << rdata << std::endl;
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

    //Build the question section
    question.SetRawName(name);
    question.SetType(TYPE_A);
    question.SetClass(CLASS_IN);

    //Build the length-encoded name
    question.EncodeName();
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

char * DNSPacket::GetData(void) {
    return NULL;
}
