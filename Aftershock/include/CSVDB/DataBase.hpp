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

#ifndef DB_HPP_INCLUDED
#define DB_HPP_INCLUDED

#include "Detail/DB_Base.hpp"
#include "Detail/DB_Row.hpp"
#include "Detail/DB_Index.hpp"

template<size_t KeyColumn, class..._Columns>
class DataBase : public DB_Base {
public:
    typedef std::tuple<_Columns...> TupleType;
    typedef TupleRow<_Columns...> RowType;
    typedef typename std::tuple_element<KeyColumn, TupleType>::type KeyType;
    typedef DBIndex<KeyColumn, TupleType, KeyType, RowType> indexType;

    DataBase(): index(rows){}
    RowType& updateRow(const RowBase& row){
        return getRowFromKey( keyFromRow(row) ) = row;
    }

    RowType& getRowFromID(size_t RowID){
        auto it = rows.begin();
        std::advance(it,RowID);
        return getRow(it);
    }

    RowType& getRowFromKey(const KeyType& key){
        return getRowFromID(index.getIndexAndCheck(key)->second);
    }

    size_t getRowID(const RowBase& row){
        return index.getRowID(keyFromRow(row));
    }

    size_t getRowID2(const RowBase& row){
        size_t d = 0;
        for(auto it=rows.begin(),itend=rows.end();it!=itend;++it,d++)
            if(*it == &row) return d;
        throw DBIndex_Base::InvalidRow();
    }

    void updateRowIndex(const RowBase& row){
        index.updateIndex(index.getKey(getRowID2(row)), keyFromRow(row));
    }

    RowType& operator[](const KeyType& key){
        return getRowFromKey(key);
    }

    const KeyType& keyFromRow(const RowBase& row){
        return std::get<KeyColumn>(dynamic_cast<const RowType&>(row).data);
    }

    template<class...t>
    RowType makeRow(t&&... d){
        return RowType(d...);
    }

    RowType& insertRow(const RowBase& row){
        index.addIndex(keyFromRow(row),rows.size());
        RowType* newRow = (RowType*) row.clone();
        rows.push_back(newRow);
        return *newRow;
    }

    template<class...t>
    RowType& insertRowData(t&&... d){
        return insertRow(RowType(d...));
    }


    std::ostream& operator<<(std::ostream& s) const{
    for(auto it=rows.begin(), itend=rows.end(); it!=itend;++it)
            s<<(**it)<<'\n';
        return s;
    }
    std::istream& operator>>(std::istream& s){
        while(!s.eof()){
            RowType r;
            s>>r;
            char n = s.get();
            if( ! ((n == '\r' && s.get() == '\n') || n == '\n') ) break;
            insertRowNoIndex(r);
        }
        index.generateIndex();
        return s;
    }

    bool hasKey(const KeyType& key){
        return index.hasIndex(key);
    }

protected:

    indexType index;

    void insertRowNoIndex(const RowBase& row){
        rows.push_back((RowType*) row.clone());
    }

    RowType& getRow(RowList::iterator& it){
        return *dynamic_cast<RowType*>(*it);
    }


};


inline std::ostream& operator<<(std::ostream& s, const DB_Base& r){
    return r.operator<<(s);
}
inline std::istream& operator>>(std::istream& s, DB_Base& r){
    return r.operator>>(s);
}

#endif // DB_HPP_INCLUDED
