//
// Created by adrian on 16/06/17.
//

#ifndef INC_HASH_TABLE_CHAINNING_HPP
#define INC_HASH_TABLE_CHAINNING_HPP

#include <stdint.h>
#include <sdsl/int_vector.hpp>
#include <list>

/**
    * Hash table with open addressing and double hashing
    */
namespace hash_table {

    template<class t_hash = uint64_t, class t_value = uint64_t>
    class hash_table_chainning {

    public:
        typedef uint64_t size_type;
        //typedef chainning_list_element<t_value> key_value_type;
        typedef t_hash hash_type;
        typedef t_value value_type;
        typedef std::list<std::pair<value_type, bool>> value_list_type;
        typedef std::pair<hash_type, value_list_type> hash_list_element_type;
        typedef std::list<hash_list_element_type> hash_list_type;
        typedef std::vector<hash_list_type> table_type;
        typedef typename table_type::iterator iterator_table_type;
        typedef typename value_list_type::iterator iterator_value_type;
        typedef typename hash_list_type::iterator iterator_hash_type;

    private:
        size_type m_table_size;
        size_type m_used;
        table_type m_table;

        size_type h(const hash_type pre_hash) {
            return (pre_hash % m_table_size);
        }

        void copy(const hash_table_chainning &p) {
            m_table_size = p.m_table_size;
            m_used = p.m_used;
            m_table = p.m_table;
        }


        size_type nearest_prime(size_type n){
            size_type position;
            size_type index;
            for (position = n; ; position++){
                //checks if those values from 2 to $\sqrt{m}$ can be factors of $m$
                for (index = 2; index <= (size_type) sqrt((double_t) position) && position % index != 0; index++) ;
                // No factors in that range, therefore a prime number was found
                if (position % index != 0){
                    break;
                }
            }
            return position;
        }

        inline bool check_collision(table_type &table, const hash_type hash, iterator_table_type &it_table,
                                    iterator_hash_type &it_hash){
            it_table = table.begin() + h(hash);
            it_hash = it_table->begin();
            auto i = 0;
            while(it_hash != it_table->end()){
                ++i;
                if(it_hash->first == hash){
                    //if(i > 5)std::cout << "check_collision: " << i << std::endl;
                    return true;
                }
                ++it_hash;
            }
            //if(i > 5)std::cout << "check_collision: " << i << std::endl;
            return false;
        }

        void resize() {
            m_table_size = nearest_prime(m_table_size * 2);
            std::cout << "resize: " << m_table_size << std::endl;
            m_used = 0;
            iterator_table_type aux_it_table;
            iterator_hash_type aux_it_hash;
            table_type aux_table(m_table_size, hash_list_type());
            auto it_table = m_table.begin();
            while (it_table != m_table.end()){
                auto it_hash = it_table->begin();
                while(it_hash != it_table->end()){
                    auto hash = it_hash->first;
                    check_collision(aux_table, hash, aux_it_table, aux_it_hash);
                    ++m_used;
                    aux_it_table->emplace_back(std::move(*it_hash));
                    ++it_hash;
                }
                ++it_table;
            }
            //m_table.clear();
            m_table = std::move(aux_table);
        }

    public:
        hash_table_chainning() = default;

        explicit hash_table_chainning(size_type size) {
            m_table_size = nearest_prime(size);
            m_used = 0;
            m_table = table_type(m_table_size, hash_list_type());
        }

        bool hash_collision(const hash_type hash, iterator_table_type &it_table, iterator_hash_type &it_hash){
            if(m_used > m_table_size * 0.7){
                resize();
            }
            //std::cout << "hash: " << hash << std::endl;
            return check_collision(m_table, hash, it_table, it_hash);
        }


        void insert_no_hash_collision(const iterator_table_type &it_table, const hash_type &hash, value_type &value){
            ++m_used;
            //std::cout << "list_hashes: " << it_table->size() << std::endl;
            value_list_type value_list = {{value, false}};
            it_table->emplace_back(hash, value_list);
        }

        void insert_hash_collision(const iterator_hash_type &it_hash, value_type &value){
            // std::cout << "list_values: " << it_hash->second.size() << std::endl;
            ++m_used;
            it_hash->second.emplace_back(value, false);
        }

        void remove_value(const iterator_table_type &it_table, iterator_hash_type &it_hash, iterator_value_type &it) {
            it = it_hash->second.erase(it);
            if(it_hash->second.empty()){
                it_hash = it_table->erase(it_hash);
                --m_used;
            }
        }

        void remove_marked(){
            auto it_table = m_table.begin();
            while(it_table != m_table.end()){
                auto it_hash = it_table->begin();
                while(it_hash != it_table->end()){
                    auto it = it_hash->second.begin();
                    //Delete those marked elements
                    while(it != it_hash->second.end()){
                        if(it->second){
                            it = it_hash->second.erase(it);
                        }else{
                            ++it;
                        }
                    }
                    //There is no elements with the current hash
                    if(it_hash->second.empty()){
                        it_hash = it_table->erase(it_hash);
                        --m_used;
                    }else{
                        ++it_hash;
                    }
                }
                ++it_table;
            }
        }

        bool remove_value(const hash_type &hash, const value_type &value){
            auto it_table = m_table.begin() + h(hash);
            auto it_hash = it_table->begin();
            while(it_hash != it_table->end()){
                if(it_hash->first == hash){
                    auto it = it_hash->second.begin();
                    while(it != it_hash->second.end()){
                        if(it->first == value){
                            remove_value(it_table, it_hash, it);
                            return true;
                        }
                        ++it;
                    }
                    return false;
                }
                ++it_hash;
            }
            return false;
        }


        //! Copy constructor
        hash_table_chainning(const hash_table_chainning &p) {
            copy(p);
        }

        //! Move constructor
        hash_table_chainning(hash_table_chainning &&p) {
            *this = std::move(p);
        }

        //! Assignment move operation
        hash_table_chainning &operator=(hash_table_chainning &&p) {
            if (this != &p) {
                m_used = std::move(p.m_used);
                m_table_size = std::move(p.m_table_size);
                m_table = std::move(p.m_table);
            }
            return *this;
        }

        //! Assignment operator
        hash_table_chainning &operator=(const hash_table_chainning &p) {
            if (this != &p) {
                copy(p);
            }
            return *this;
        }

        //! Swap method
        /*! Swaps the content of the two data structure.
        *  You have to use set_vector to adjust the supported bit_vector.
        *  \param bp_support Object which is swapped.
        */
        void swap(hash_table_chainning &p) {
            std::swap(m_used, p.m_used);
            std::swap(m_table_size, p.m_table_size);
            std::swap(m_table, p.m_table);
        }

        void print(){
            std::cout << "Hash Table Double Chainning" << std::endl;
            for(size_type i = 0; i < m_table.size();i++){
                std::cout << "Entry: " << i << std::endl;
                std::cout << "-----------------------------" << std::endl;
                auto it_h = m_table[i].begin();
                while(it_h != m_table[i].end()){
                    std::cout << "Hash: " << it_h->first << std::endl;
                    auto it_v = it_h->second.begin();
                    std::cout << "Values: ";
                    while(it_v != it_h->second.end()){
                        std::cout << "<" << it_v->first << ", " << it_v->second << ">" << ", ";
                        ++it_v;
                    }
                    std::cout << std::endl;
                    ++it_h;
                }
                std::cout << "-----------------------------" << std::endl;
            }
        }


    };
}

#endif //BLOCK_TREES_2D_HASH_TABLE_CHAINNING_HPP
