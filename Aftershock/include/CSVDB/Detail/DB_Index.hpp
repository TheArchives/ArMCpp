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

#ifndef DB_INDEX_H_INCLUDED
#define DB_INDEX_H_INCLUDED

#include "DB_Base.hpp"
#include "DB_Row.hpp"
#include <map>


template<size_t KeyColumn, class TupleType, class KeyType, class RowType>
class DBIndex: public DBIndex_Base {
    public:

    typedef std::map<KeyType,size_t> indexType;
    typedef typename indexType::iterator indexIterator;

    DBIndex(const DB_Base::RowList& r): rows(r){}
    DBIndex(const DBIndex&) = delete;
    DBIndex& operator=(const DBIndex&) = delete;

    // [re]enerate full index
    void generateIndex(const bool& regenerate = false){
        if(index.size() == rows.size() && !regenerate) return;
        index.clear();
        size_t RowID =0;
        for(auto it=rows.begin(),itend=rows.end();it!=itend;++it,++RowID){
            auto& k = std::get<KeyColumn>( getRow(it).data );
            if(index.find(k) == index.end()) index[ k ] = RowID;
            else std::cout << "Duplicate key at Row " << RowID << ", Key value: " << k << std::endl;
        }
    }

    bool hasIndex(const KeyType& key){
        return index.find(key) != index.end();
    }

    // Add an index
    void addIndex(const KeyType& key, const size_t& RowID){
        if(index.find(key) != index.end()) throw DuplicateIndex();
        else index[ key ] = RowID;
    }

    // Update an index
    void updateIndex(const KeyType& oldKey, const KeyType& newKey){
        indexIterator it = getIndexAndCheck(oldKey);
        index[newKey] = it->second;
        index.erase(it);
    }

    // Remove an index;
    void removeIndex(const KeyType* key){
        index.erase(getIndexAndCheck(key));
    }

    // Get index from key
    indexIterator getIndex(const KeyType& key){
        return index.find(key);
    }

    // Get an index and check it
    indexIterator getIndexAndCheck(const KeyType& key){
        indexIterator it = getIndex(key);
        if(it==index.end()) throw InvalidIndex();
        return it;
    }

    // Get the RowID at an index
    indexIterator getRowID(const KeyType& key){
        return getIndexAndCheck(key)->second;
    }

    // Get a key from a index
    const KeyType& getKey(const size_t& RowID){
        for(auto it=index.begin(),itend=index.end();it!=itend;++it)
            if(it->second == RowID)
                return it->first;
        throw InvalidRowID();
    }

    protected:
    const RowType& getRow(DB_Base::RowList::const_iterator& it){
        return dynamic_cast<const RowType&>(**it);
    }
    indexType index;
    const DB_Base::RowList& rows;
};


#endif // DB_INDEX_H_INCLUDED
