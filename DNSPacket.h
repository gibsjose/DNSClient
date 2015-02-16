#ifndef DNSPACKET_H
#define DNSPACKET_H

#include <string>
#include <cstring>
#include <cstdlib>
#include <bitset>
#include <arpa/inet.h>

#if defined (__APPLE__)
    #include <netinet/in.h>
    #include <libkern/OSByteOrder.h>
    #define SWAP16(x) (_OSSwapInt16(x))
    #define SWAP32(x) (_OSSwapInt32(x))
#else
    #include <byteswap.h>
    #define SWAP16(x) (__bswap_16(x))
    #define SWAP32(x) (__bswap_32(x))
#endif

#include "StringUtilities.h"

const short RD_FLAG = (short)(1 << 8);      //RD (Recursion desired) bit set
const short CLASS_IN = 1;                   //IN = Internet class
const short TYPE_A = 1;                     //A = IPv4 Addresses
const short TYPE_CNAME = 5;                 //CNAME = Cononical Name (alias)

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
        if(name != NULL) {
            //free(name);
            name = NULL;
        }

        if(data != NULL) {
            free(data);
            data = NULL;
        }
    }

    std::string & GetRawName(void) { return rawName; }
    std::string & GetDisplayName(void) { return displayName; }
    char * GetName(void) { return name; }
    unsigned short GetType(void) { return recordType; }
    unsigned short GetClass(void) { return recordClass; }

    void SetRawName(const std::string & rawName) { this->rawName = rawName; }
    void SetType(const unsigned short recordType) { this->recordType = recordType; }
    void SetClass(const unsigned short recordClass) { this->recordClass = recordClass; }

    void EncodeName(void);
    void EncodeName(const std::string &);

    void DecodeName(const char *);

    std::string DecodeType(const unsigned short);
    std::string DecodeClass(const unsigned short);

    void Print(void);

    size_t Size(void);

    char * GetData(void);

protected:
    char * name;                    //(00000011)www(00000110)google(00000011)com(00000000)
    std::string displayName;        //3www6google3com0
    std::string rawName;            //www.google.com
    unsigned short recordType;      //TYPE_A (IPv4) or TYPE_CNAME (Cannonical Name)
    unsigned short recordClass;     //CLASS_IN (Internet)
    char * data;                    //Serialized byte data
};

class ExtendedRecord : public Record {
public:
    ExtendedRecord(void) {
        ttl = 0;
        rdlength = 0;
        rdata.clear();
    }

    uint32_t GetTTL(void) { return ttl; }
    unsigned short GetRecordDataLength(void) { return rdlength; }
    std::string & GetRecordData(void) { return rdata; }

    void SetTTL(const uint32_t ttl) { this->ttl = ttl; }
    void SetRecordDataLength(const unsigned short rdlength) { this->rdlength = rdlength; }
    void SetRecordData(const std::string & rdata) { this->rdata = rdata; }

    void Print(void);

    size_t Size(void);

    char * GetData(void);

protected:
    uint32_t ttl;
    unsigned short rdlength;
    std::string rdata;
};

class QuestionRecord : public Record {};
class AnswerRecord : public ExtendedRecord {};
class NameServerRecord : public ExtendedRecord {};
class AdditionalRecord : public ExtendedRecord {};

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
