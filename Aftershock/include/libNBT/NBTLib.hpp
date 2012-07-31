/*
Copyright (c) 2011, Adam Guy aka Adam01 ( adam@adam-guy.com )
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ADAM GUY BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef NBTLIB_HPP_INCLUDED
#define NBTLIB_HPP_INCLUDED

#include <string>
#include <iostream>
#include <map>
#include <list>
#include <stdexcept>
#include <memory>
#include <type_traits>
#include <typeinfo>


enum TAG_TYPE :
uint8_t { TAG_END, TAG_BYTE, TAG_SHORT, TAG_INT, TAG_LONG, TAG_FLOAT, TAG_DOUBLE, TAG_BYTE_ARRAY, TAG_STRING, TAG_LIST, TAG_COMPOUND};

extern const char* const TAG_NAME[];

// Thrown when the wrong NBT element is inserted into a list
struct NBTListTypeMissMatch : public std::runtime_error {
    NBTListTypeMissMatch(const TAG_TYPE& t,const TAG_TYPE& t2);
};
// Thrown when TAG_END, or an unknown type is used
struct NBTInvalidType : public std::runtime_error {
    NBTInvalidType(char t): std::runtime_error("Invalid type: " + std::string(0,'0'+t)){}
};

// Forward declarations

struct NBTItem;
struct NBTByte;
struct NBTShort;
struct NBTInt;
struct NBTLong;
struct NBTFloat;
struct NBTDouble;
struct NBTString;
struct NBTList;
template<class t = NBTItem>
struct NBTListIterator;
struct NBTCompound;
struct NBTCompoundModifier;

// Helper functions

NBTByte makeNBT(const int8_t&);
NBTShort makeNBT(const int16_t&);
NBTInt makeNBT(const int32_t&);
NBTLong makeNBT(const int64_t&);
NBTFloat makeNBT(const float&);
NBTDouble makeNBT(const double&);
NBTString makeNBT(const std::string&);
template<class t>
NBTList makeNBT(const std::initializer_list<t>&);
const NBTItem& makeNBT(const NBTItem&);


// Base NBT type interface

struct NBTItem {
    // NBT Type
    virtual TAG_TYPE type() const = 0;

    // Write to stream
    virtual std::ostream& operator<<(std::ostream&) const = 0;
    virtual void write(const NBTString&,std::ostream&) const;

    // Read from stream
    virtual std::istream& operator>>(std::istream&) =0;
    virtual void read(NBTString&,std::istream&);
    virtual NBTString read(std::istream&);

    // Get human readable NBT representation
    virtual std::string string(std::string="")const=0;

    // Duplicate
    virtual NBTItem* clone() const = 0;

    // Virtual destructor
    virtual ~NBTItem() = 0;
};

void Debug_Class(const char*, const void*, const char*);

// Base for integer NBT types
template<class t>
struct NBTIntItem: NBTItem{
protected:
    t i;
    NBTIntItem():i(0){
        Debug_Class("Constructed default NBTIntItem", this, typeid(t).name());
    }
    NBTIntItem(const t& _i): i(_i){
        Debug_Class("Constructed NBTIntItem", this, typeid(t).name());
    }
public:
    ~NBTIntItem(){
        Debug_Class("Destroyed NBTIntItem", this, typeid(t).name());
    }
    // Get access to value
    operator const t&() const {
        return i;
    }
    operator t&() {
        return i;
    }
    // Write
    std::ostream& operator<<(std::ostream& out) const ;
    // Read
    std::istream& operator>>(std::istream&);
};

// NBT Integer structures

struct NBTByte : NBTIntItem<int8_t> {
    NBTByte() = default;
    NBTByte(const int8_t& i);
    TAG_TYPE type() const;
    NBTByte* clone() const;
    std::string string(std::string="")const;
};
struct NBTShort : NBTIntItem<int16_t> {
    NBTShort() = default;
    NBTShort(const int16_t& i);
    TAG_TYPE type() const;
    NBTShort* clone() const;
    std::string string(std::string="")const;
};
struct NBTInt : NBTIntItem<int32_t> {
    NBTInt() = default;
    NBTInt(const int32_t& i);
    TAG_TYPE type() const;
    NBTInt* clone() const;
    std::string string(std::string="")const;
};
struct NBTLong : NBTIntItem<int64_t> {
    NBTLong() = default;
    NBTLong(const int64_t& i);
    TAG_TYPE type() const;
    NBTLong* clone() const;
    std::string string(std::string="")const;
};
struct NBTFloat : NBTIntItem<float> {
    NBTFloat() = default;
    NBTFloat(const float& i);
    TAG_TYPE type() const;
    NBTFloat* clone() const;
    std::string string(std::string)const;
};
struct NBTDouble : NBTIntItem<double> {
    NBTDouble() = default;
    NBTDouble(const double& i);
    TAG_TYPE type() const;
    NBTDouble* clone() const;
    std::string string(std::string="")const;
};

// NBT String

class NBTString : public NBTItem {
    std::string str;
    public:
    NBTString() = default;
    NBTString(const std::string&);
    NBTString(const char*);

    ~NBTString();
    // Tag
    TAG_TYPE type() const;
    // Read
    std::istream& operator>>(std::istream&);
    // Write
    std::ostream& operator<<(std::ostream&) const;
    // Duplicate
    NBTString* clone()const;
    // String
    std::string string(std::string="")const;

    operator const std::string&() const {
        return str;
    }
    operator std::string&(){
        return str;
    }

    bool operator<(const NBTString& a)const{
        return str < a.str;
    }
};



struct NBTByteArray : NBTItem{
    std::shared_ptr<char> data;
    size_t size;

    NBTByteArray();
    NBTByteArray(const std::string&);
    NBTByteArray(const char*,const size_t&);
    NBTByteArray(const size_t&); // Allocate memory

    void reset(size_t); // Reset to hold n bytes (replaces data)

    ~NBTByteArray();

    // Tag
    TAG_TYPE type() const;
    // Read
    std::istream& operator>>(std::istream&);
    // Write
    std::ostream& operator<<(std::ostream&) const;
    // Duplicate structure, not the data
    NBTByteArray* clone()const;
    // Duplicate structure and data
    NBTByteArray copy()const;
    // String
    std::string string(std::string="")const;

    // Deletes the array pointed to by data
    static void DeleteArray(char*);
};


// NBTList

struct NBTList : NBTItem{
    TAG_TYPE listType; // What elements are held
    std::list<NBTItem*> items; // Items (Best using NBTListIterator for access)
    NBTList();
    template<class t>
    NBTList(const std::initializer_list<const t&>&l){
        *this = makeNBT(l);
    }
    // Needs specialised copying
    NBTList(const NBTList&);
    NBTList& operator=(const NBTList&);
    // And a specialised destructor
    ~NBTList();

    // Add an element

    template<class t>
    size_t add(const t&i){
        return add((const NBTItem&)makeNBT(i));
    }
    size_t add(const NBTItem&i);
    template<class t>
    size_t add(const std::initializer_list<t>&l){
        if(l.size()) {
            auto it = l.begin(),itend=l.end();
            listType = makeNBT(*it).type();
            for(;it!=itend;++it) items.push_back( makeNBT(*it).clone() );
        }
        return items.size();
    }

    // Get NBTListIterators

    NBTListIterator<> begin();
    NBTListIterator<> end();

    template<class t>
    NBTListIterator<t> begin(){
        return items.begin();
    }
    template<class t>
    NBTListIterator<t> end(){
        items.end();
    }

    // NBT Interface

    TAG_TYPE type() const;
    NBTList* clone() const;
    // Write
    std::ostream& operator<<(std::ostream&) const;
    // Read
    std::istream& operator>>(std::istream&);

    std::string string(std::string="")const;
};



// NBT List Iterator

template<class NBTType>
struct NBTListIterator {
    typedef std::list<NBTItem*>::iterator iterator;
    iterator it;
    NBTListIterator(iterator _it): it(_it){}
    NBTListIterator operator++(int){
        NBTListIterator t(*this);
        ++it;
        return t;
    }
    NBTListIterator& operator++(){
        ++it;
        return *this;
    }
    bool operator==(const NBTListIterator&rhs){
        return it==rhs.it;
    }
    bool operator!=(const NBTListIterator&rhs){
        return it!=rhs.it;
    }
    NBTType& operator*(){
        return dynamic_cast<NBTType&>(**it);
    }
    NBTType* operator->(){
        return dynamic_cast<NBTType*>(*it);
    }
};

// NBTCompound

struct NBTCompound : NBTItem {
    std::map<NBTString,NBTByte> Bytes;
    std::map<NBTString,NBTShort> Shorts;
    std::map<NBTString,NBTInt> Ints;
    std::map<NBTString,NBTLong> Longs;
    std::map<NBTString,NBTFloat> Floats;
    std::map<NBTString,NBTDouble> Doubles;
    std::map<NBTString,NBTString> Strings;
    std::map<NBTString,NBTByteArray> ByteArrays;
    std::map<NBTString,NBTList> Lists;
    std::map<NBTString,NBTCompound> Compounds;

    TAG_TYPE type() const;
    NBTCompound* clone() const;
    // Write
    std::ostream& operator<<(std::ostream&) const;
    // Read
    std::istream& operator>>(std::istream&);

    NBTCompoundModifier operator[](const std::string&);

    std::string string(std::string="")const;
};

struct NBTCompoundModifier {
    std::string name;
    NBTCompound& c;
    NBTCompoundModifier(std::string,NBTCompound&);

    NBTCompoundModifier& operator=(const NBTItem&);

    NBTCompoundModifier operator[](const std::string);
    NBTCompound* operator->();


    NBTCompoundModifier& operator=(const int8_t&);
    NBTCompoundModifier& operator=(const int16_t&);
    NBTCompoundModifier& operator=(const int32_t&);
    NBTCompoundModifier& operator=(const int64_t&);
    NBTCompoundModifier& operator=(const float&);
    NBTCompoundModifier& operator=(const double&);
    NBTCompoundModifier& operator=(const std::string&);

};

// These fix a few issues
inline std::ostream& operator<<(std::ostream& s,const NBTItem&i){
    return i.operator<<(s);
}
inline std::istream& operator>>(std::istream& s,NBTItem&i){
    return i.operator>>(s);
}

// These functions only swap endianess if the machine is little-endian

const int8_t & NetworkOrder(const int8_t &value);
int16_t & NetworkOrder(int16_t &value);
int32_t & NetworkOrder(int32_t &value);
int64_t & NetworkOrder(int64_t &value);
  float & NetworkOrder(  float &value);
 double & NetworkOrder( double &value);

int16_t NetworkOrder(const int16_t &value);
int32_t NetworkOrder(const int32_t &value);
int64_t NetworkOrder(const int64_t &value);
  float NetworkOrder(const   float &value);
 double NetworkOrder(const  double &value);

// Some template implementations

template<class t>
std::ostream& NBTIntItem<t>::operator<<(std::ostream& out) const{
    t j = NetworkOrder(i);
    return out.write((char*)&j,sizeof(t));
}
template<class t>
std::istream& NBTIntItem<t>::operator>>(std::istream& in){
    in.read((char*)&i,sizeof(t));
    NetworkOrder(i);
    return in;
}

// Some inlines

inline NBTByte makeNBT(const int8_t&i){
    return NBTByte(i);
}
inline NBTShort makeNBT(const int16_t&i){
    return NBTShort(i);
}
inline NBTInt makeNBT(const int32_t&i){
    return NBTInt(i);
}
inline NBTLong makeNBT(const int64_t&i){
    return NBTLong(i);
}
inline NBTFloat makeNBT(const float&i){
    return NBTFloat(i);
}
inline NBTDouble makeNBT(const double&i){
    return NBTDouble(i);
}
inline NBTString makeNBT(const std::string&s){
    return NBTString(s);
}

inline const NBTItem& makeNBT(const NBTItem&i){
    return i;
}


template<class t>
NBTList makeNBT(const std::initializer_list<t>&l){
    NBTList li;
    if(l.size()) {
        auto it = l.begin(),itend=l.end();
        li.listType = makeNBT(*it).type();
        for(;it!=itend;++it) li.items.push_back( makeNBT(*it).clone() );
    }
    return li;
}

inline const int8_t & NetworkOrder(const int8_t &value){
    return value;
    }
#endif // NBTLIB_HPP_INCLUDED
