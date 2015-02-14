#ifndef DNSPACKET_H
#define DNSPACKET_H

#include <string>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>

#if defined (__APPLE__)
    #include <netinet/in.h>
    #include <libkern/OSByteOrder.h>
#else
    #include <byteswap.h>
#endif

#include "StringUtilities.h"

const short RD_FLAG = (short)(1 << 8);      //RD (Recursion desired) bit set
const short CLASS_IN = 1;                   //IN = Internet class
const short TYPE_A = 1;                     //A = IPv4 Addresses

class Record {
public:
    Record(void) {
        name = NULL;
        displayName.clear();
        rawName.clear();
        recordType = TYPE_A;
        recordClass = CLASS_IN;
        data = NULL;
    }

    ~Record(void) {
        if(name != NULL)
            free(name);

        if(data != NULL)
            free(data);
    }

    std::string & GetRawName(void) { return rawName; }
    std::string & GetDisplayName(void) { return displayName; }
    char * GetName(void) { return name; }
    short GetType(void) { return recordType; }
    short GetClass(void) { return recordClass; }

    void SetRawName(const std::string & rawName) { this->rawName = rawName; }
    void SetType(const short recordType) { this->recordType = recordType; }
    void SetClass(const short recordClass) { this->recordClass = recordClass; }

    void EncodeName(void);
    void EncodeName(const std::string &);

    void Print(void);

    size_t Size(void);

    char * GetData(void);

protected:
    char * name;           //(00000011)www(00000110)google(00000011)com(00000000)
    std::string displayName;    //3www6google3com0
    std::string rawName;        //www.google.com
    short recordType;           //TYPE_A (IPv4)
    short recordClass;          //CLASS_IN (Internet)
    char * data;                //Serialized byte data
};

class ExtendedRecord : public Record {
public:
    ExtendedRecord(void) {
        ttl = 0;
        rdlength = 0;
        rdata.clear();
    }

    long GetTTL(void) { return ttl; }
    short GetRecordDataLength(void) { return rdlength; }
    std::string & GetRecordData(void) { return rdata; }

    void SetTTL(const long ttl) { this->ttl = ttl; }
    void SetRecordDataLength(const short rdlength) { this->rdlength = rdlength; }
    void SetRecordData(const std::string & rdata) { this->rdata = rdata; }

    void Print(void);

    size_t Size(void);

    char * GetData(void);

protected:
    long ttl;
    short rdlength;
    std::string rdata;
};

class QuestionRecord : public Record {};
class AnswerRecord : public ExtendedRecord {};
class NameServerRecord : public ExtendedRecord {};
class AdditionalRecord : public ExtendedRecord {};

class DNSPacket {
public:
    DNSPacket(const std::string &);             //Construct a request packet with the domain name (rawName)
    DNSPacket(const std::string &, const char *, const size_t);      //Construct a response packet with the raw data (data)
    ~DNSPacket(void) { free(data); }            //Destructor (free data)
    void Print(void);                           //Print the packet data
    char * GetData(void);                       //Convert the packet into a byte array for transmission
    size_t Size(void);                          //Size of the packet in bytes

    //Getters...
    short GetID(void) { return id; }
    short GetFlags(void) { return flags; }
    short GetQuestionCount(void) { return qdcount; }
    short GetAnswerCount(void) { return ancount; }
    short GetNameServerCount(void) { return nscount; }
    short GetAdditionalRecordCount(void) { return arcount; }
    std::vector<QuestionRecord> & GetQuestionSection(void) { return questions; }
    std::vector<AnswerRecord> & GetAnswerSection(void) { return answers; }
    std::vector<NameServerRecord> & GetNameServerSection(void) { return nameServers; }
    std::vector<AdditionalRecord> & GetAdditionalSection(void) { return additionals; }

private:
    short id;
    short flags;
    short qdcount;
    short ancount;
    short nscount;
    short arcount;
    std::vector<QuestionRecord> questions;
    std::vector<AnswerRecord> answers;
    std::vector<NameServerRecord> nameServers;
    std::vector<AdditionalRecord> additionals;

    char * data;
};

#endif//DNSPACKET_H
