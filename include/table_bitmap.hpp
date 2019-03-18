/***
BSD 2-Clause License

Copyright (c) 2018, <author name>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/


#ifndef NAME_PROJECT_TABLE_BITMAP_HPP
#define NAME_PROJECT_TABLE_BITMAP_HPP

#include <sdsl/int_vector.hpp>
#include <sdsl/rank_support.hpp>

//This is an example, create your own namespace.
namespace cds {

    template <class t_value>
    class table_bitmap {

    public:

        //Definiton of types, useful when you need to know a type inside a class
        typedef uint64_t size_type;
        typedef t_value value_type;
        typedef sdsl::bit_vector bitmap_type;
        //Typename for obtaining the type inside a class
        typedef typename bitmap_type::rank_1_type rank_type;
        typedef sdsl::int_vector<> data_type;

    private:

        bitmap_type m_bitmap;
        rank_type m_rank_bitmap;
        data_type m_data;


        void copy(const table_bitmap& o)
        {
            m_bitmap = o.m_bitmap;
            m_rank_bitmap = o.m_rank_bitmap;
            m_rank_bitmap.set_vector(&m_bitmap);
            m_data = o.m_data;
        }

    public:

        // If you need access to parts of your class and you don't want to modify their values
        const bitmap_type &bitmap = m_bitmap;
        const data_type &data = m_data;


		//*******************************************************//
        //******************* CONSTRUCTORS **********************//
        //*******************************************************//

        //Default constructor
        table_bitmap() = default;

        //Copy constructor
        table_bitmap(const table_bitmap& o)
        {
            copy(o);
        }

        //Move constructor
        table_bitmap(table_bitmap&& o)
        {
            *this = std::move(o);
        }

        /**
         *  Constructor
         *  Container can be any class.
         *  Depending on the implementation it requires some methods.
         *  In this case we need that the container supports iterators, size, and each element a first and second atrribute.
         */
        template <class Container>
        table_bitmap(const Container &input){
            //1.Computing the maximum key value
            //We need iterators (begin, end) because we are using a for
            value_type max_key = 0;
            //Const: no modification; &var: the value is obtained by reference (no copy)
            for(const auto &element : input){
                //We need a first and second because we are assuming the container contains pairs
                if(element.first > max_key) max_key = element.first;
            }

            //2. Storing the values
            m_bitmap = bitmap_type(max_key+1);
            m_data = data_type(input.size());
            size_type i = 0;
            for(const auto &element : input){
                //We need a first and second because we are assuming the container contains pairs
                m_bitmap[element.first] = 1;
                m_data[i] = element.second;
                ++i;
            }

            //3. Compressing the data
            sdsl::util::bit_compress(m_data);
            //4. Init rank over bitmap
            sdsl::util::init_support(m_rank_bitmap, &m_bitmap);
        }


        //*******************************************************//
        //******************** FUNCTIONS ************************//
        //*******************************************************//

        //Number of elements
        inline size_type size()const
        {
            return m_data.size();
        }

        //Returns if the data structure is empty
        inline bool empty()const
        {
            return m_data.empty();
        }


        //*******************************************************//
        //*************** BASIC OPERATIONS **********************//
        //*******************************************************//

        //Swap method
        void swap(table_bitmap& o)
        {
            m_bitmap.swap(o.m_bitmap);
            //Swap ranks and selects
            sdsl::util::swap_support(m_rank_bitmap, o.m_rank_bitmap,
                               &m_bitmap, &(o.m_bitmap));
            m_data.swap(o.m_data);
        }

        //Assignment Operator
        table_bitmap& operator=(const table_bitmap& o)
        {
            if (this != &o) {
                copy(o);
            }
            return *this;
        }

        //Assignment Move Operator
        table_bitmap& operator=(table_bitmap&& o)
        {
            if (this != &o) {
                //Move elements
                m_bitmap = std::move(o.m_bitmap);
                m_rank_bitmap = std::move(o.m_rank_bitmap);
                m_rank_bitmap.set_vector(&m_bitmap);
                m_data = std::move(o.m_data);
            }
            return *this;
        }

        //Exist
        inline bool exist(size_type i) const{
            return m_bitmap[i];
        }

        //Access operator
        value_type operator[](size_type i) const {
            if(m_bitmap[i]){
                auto n_ones = m_rank_bitmap(i+1);
                auto index = n_ones -1;
                return (value_type) m_data[index];
            }
            return -1;
        }

		//*******************************************************//
        //********************** FILE ***************************//
        //*******************************************************//

        //Serialize to a stream
        size_type serialize(std::ostream& out, sdsl::structure_tree_node* v=nullptr, std::string name="")const
        {
            //Create a node and store the different elements of our structrue in that node
            sdsl::structure_tree_node* child = sdsl::structure_tree::add_child(v, name, sdsl::util::class_name(*this));
            size_type written_bytes = 0;
            written_bytes += m_bitmap.serialize(out, child, "bitmap");
            written_bytes += m_rank_bitmap.serialize(out, child, "rank_bitmap");
            written_bytes += m_data.serialize(out, child, "data");
            sdsl::structure_tree::add_size(child, written_bytes);
            return written_bytes;
        }

        //Load from a stream
        void load(std::istream& in)
        {
            m_bitmap.load(in);
            m_rank_bitmap.load(in, &m_bitmap);
            m_data.load(in);
        }



    };
}

#endif //NAME_PROJECT_TABLE_BITMAP_HPP
