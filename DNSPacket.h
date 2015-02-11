#ifndef DNSPACKET_H
#define DNSPACKET_H

#include <string>
#include <cstdlib>

const char RD_FLAG = (1 << 8);
const short CLASS_IN = 1;
const short TYPE_A = 1;

struct QuestionSection_t{
    std::string name;
    std::string rawName;
    short q_type;
    short q_class;

    std::string & GetRawName(void) { return rawName; }
    std::string & GetName(void) { return name; }
    short GetType(void) { return q_type; }
    short GetClass(void) { return q_class; }

    void SetRawName(const std::string & rawName) { this->rawName = rawName; }
    void SetType(const short q_type) { this->q_type = q_type; }
    void SetClass(const short q_class) { this->q_class = q_class; }

    void CreateName(void) {

    }

    void CreateName(const std::string & rawName) {
        this->rawName = rawName;
        this->CreateName();
    }

} QuestionSection_t;

class DNSPacket {

public:
    DNSPacket(const std::string & name) {
        srand(time(NULL));

        //Create a random ID
        id = (short)rand();

        //Set the flags (enable RD)
        flags = RD_FLAG;

        //Set Counts
        qcount = 1;
        ancount = 0;
        nscount = 0;
        arcount = 0;

        //Build the question section
        question.SetRawName(name);
        question.SetType(TYPE_A);
        question.SetClass(CLASS_IN);

        //Build the length-encoded name
        question.CreateName();
    }

private:
    short id;
    short flags;
    short qcount;
    short ancount;
    short nscount;
    short arcount;
    QuestionSection_t question;
};

#endif//DNSPACKET_H
