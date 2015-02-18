#ifndef DNSPACKET_H
#define DNSPACKET_H

#include <string>
#include <cstring>
#include <cstdlib>
#include <bitset>
#include <arpa/inet.h>

#include "StringUtilities.h"
#include "Record.hpp"
#include "ExtendedRecord.hpp"

class DNSPacket {
public:
    DNSPacket(const std::string &);             //Construct a request packet with the domain name (rawName)
    DNSPacket(const char *, const size_t);      //Construct a response packet with the raw data (data)

    ~DNSPacket(void) {
        if(data != NULL) {
            free(data);
            data = NULL;
        }
    }

    void Print(void);                           //Print the packet data
    char * GetData(void);                       //Convert the packet into a byte array for transmission
    size_t Size(void);                          //Size of the packet in bytes

    //Getters...
    unsigned short GetID(void) { return id; }
    unsigned short GetFlags(void) { return flags; }
    unsigned short GetQuestionCount(void) { return qdcount; }
    unsigned short GetAnswerCount(void) { return ancount; }
    unsigned short GetNameServerCount(void) { return nscount; }
    unsigned short GetAdditionalRecordCount(void) { return arcount; }
    std::vector<QuestionRecord> & GetQuestionSection(void) { return questions; }
    std::vector<AnswerRecord> & GetAnswerSection(void) { return answers; }
    std::vector<NameServerRecord> & GetNameServerSection(void) { return nameServers; }
    std::vector<AdditionalRecord> & GetAdditionalSection(void) { return additionals; }

private:
    unsigned short id;
    unsigned short flags;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
    std::vector<QuestionRecord> questions;
    std::vector<AnswerRecord> answers;
    std::vector<NameServerRecord> nameServers;
    std::vector<AdditionalRecord> additionals;

    char * data;
};

#endif//DNSPACKET_H
