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

#ifndef DB_BASE_H_INCLUDED
#define DB_BASE_H_INCLUDED

#include <list>
#include <exception>
#include <cstddef>
#include <ostream>
#include <istream>
#include <tuple>
#include <typeinfo>

struct RowBase {
    size_t columns;
    virtual const std::type_info& dataType() const = 0;
    template<class...t>
    bool is() const {
        return ( typeid( std::tuple<t...> ) == this->dataType());
    }
    virtual RowBase* clone() const = 0;
    virtual std::ostream& operator<<(std::ostream&) const = 0;
    virtual std::istream& operator>>(std::istream&) = 0;
};

inline std::ostream& operator<<(std::ostream& s, const RowBase& r){
    return r.operator<<(s);
}
inline std::istream& operator>>(std::istream& s, RowBase& r){
    return r.operator>>(s);
}

class DB_Base {
public:
    typedef std::list<RowBase*> RowList;
    virtual ~DB_Base(){
        for(auto it=rows.begin(),itend=rows.end();it!=itend;++it)
            delete *it;
    }


    virtual std::ostream& operator<<(std::ostream& s)const = 0;
    virtual std::istream& operator>>(std::istream& s) = 0;
protected:
    RowList rows;
};

class DBIndex_Base {
    struct InvalidIndex : public std::exception {
        const char* what() const throw(){
            return "No such index";
        }
    };
    struct InvalidRow : public std::exception {
        const char* what() const throw(){
            return "No such row in table";
        }
    };
    struct InvalidRowID : public std::exception {
        const char* what() const throw(){
            return "No such row ID";
        }
    };
    struct DuplicateIndex : public std::exception {
        const char* what() const throw(){
            return "Index already exists";
        }
    };
};

#endif // DB_BASE_H_INCLUDED
