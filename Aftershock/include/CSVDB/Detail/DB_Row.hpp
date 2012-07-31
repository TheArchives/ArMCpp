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

#ifndef DB_ROW_H_INCLUDED
#define DB_ROW_H_INCLUDED

#include "DB_Base.hpp"

#include <string>
#include <tuple>
#include <boost/algorithm/string/replace.hpp>

namespace Row_Tuple_Helpers {

    template<class t>
    constexpr size_t tupleSize(const t&){
        return std::tuple_size<t>::value;
    }

    template<class t, class f, size_t size>
    struct _iterate_tuple {
        static void work(t&a, f& func)
        {
            _iterate_tuple<t,f,size-1>::work(a,func);
            func(std::get<size-1>(a));
        }
    };

    template <class t,class f>
    struct _iterate_tuple<t,f,0>{
        static void work(t&a,f& func){}
    };

    template <class t, class f>
    void tuple_for_each(t &a, f&& func)
    {
        _iterate_tuple<t,f,  tupleSize(a)  >::work(a,func);
    }

    struct OutputTupleToStream{
        std::ostream& s;
        std::string h,b,t;
        size_t i,end;
        OutputTupleToStream(std::ostream& _s, size_t _c, std::string _h = "{", std::string _b = ", ", std::string _t = "}" ):s(_s), h(_h),b(_b),t(_t),i(0),end(_c){}
        template<class T>
        void operator()(const T& a){
            if(i++ == 0) s << h << a << b;
            else if(i == end) s << a << t;
            else s << a << b;
        }
    };

    struct OutputRowToStream{
        std::ostream& s;
        size_t i,end;
        OutputRowToStream(std::ostream& _s, size_t _c):s(_s),i(0),end(_c){}
        template<class T>
        void operator()(const T& a){
            if(++i == end) s << a;
            else s << a << ",";
        }
    };

    template<>
    inline void OutputRowToStream::operator()<std::string>(const std::string& _a){
        const std::string a = boost::algorithm::replace_all_copy(_a, ",", "\\,");
        if(++i == end) s << a;
        else s << a << ',';
    }
    /*
    template<>
    inline void OutputRowToStream::operator()<int8_t>(const int8_t& a){
        if(++i == end) s << (short)a;
        else s << (short)a << ',';
    }
    template<>
    inline void OutputRowToStream::operator()<uint8_t>(const uint8_t& a){
        if(++i == end) s << (short)a;
        else s << (short)a << ',';
    }
    */

    struct InputStreamToRow{
        std::istream& s;
        size_t i,end;
        char d;
        InputStreamToRow(std::istream& _s, size_t _c):s(_s),i(0),end(_c){}
        template<class T>
        void operator()(T& a){
            if(++i == end) s >> a;
            else s >> a >> d;
        }
        void readCommas(std::istream& s, std::string& a){
            getline ( s, a, ',');
            char& c = a[a.size()-1];
            if( c == '\\'){
                std::string t;
                readCommas(s,t);
                c = ',';
                a.append(t);
            }
        }
    };


    template<>
    inline void InputStreamToRow::operator()<std::string>(std::string& a){
        if(i++ == end) getline ( s, a);
        else readCommas(s,a);
        boost::algorithm::replace_all(a, "\\,", ",");
    }

}

template<class...t>
std::ostream& operator<<(std::ostream& s, const std::tuple<t...>& r){
    tuple_for_each(r, Row_Tuple_Helpers::OutputTupleToStream( s, std::tuple_size<std::tuple<t...>>::value));
    return s;
}

template<class...t>
struct TupleRow : RowBase {
    std::tuple<t...> data;

    TupleRow(): data(){}
    TupleRow(const t&... a): data(a...){}

    const std::type_info& dataType()const{
        return typeid(data);
    }
    RowBase* clone() const{
        return new TupleRow(*this);
    }
    std::ostream& operator<<(std::ostream& s) const {
        Row_Tuple_Helpers::tuple_for_each(data, Row_Tuple_Helpers::OutputRowToStream( s, Row_Tuple_Helpers::tupleSize(data)));
        return s;
    }
    std::istream& operator>>(std::istream& s){
        Row_Tuple_Helpers::tuple_for_each(data, Row_Tuple_Helpers::InputStreamToRow( s, Row_Tuple_Helpers::tupleSize(data)));
        return s;
    }
    template<size_t index>
    auto get() -> decltype( std::get<index>(data) ){
        return std::get<index>(data);
    }

};

#endif // DB_ROW_H_INCLUDED
