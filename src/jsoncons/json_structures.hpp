// Copyright 2013 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_JSON_STRUCTURES_HPP
#define JSONCONS_JSON_STRUCTURES_HPP

#include <string>
#include <vector>
#include <exception>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <utility>
#include <initializer_list>
#include "jsoncons/json_text_traits.hpp"

namespace jsoncons {

template <class Json, class Allocator>
class json_array
{
public:
    typedef Allocator allocator_type;
    typedef Json value_type;
    typedef typename std::allocator_traits<Allocator>:: template rebind_alloc<Json> vector_allocator_type;
    typedef typename std::vector<Json,Allocator>::reference reference;
    typedef typename std::vector<Json,Allocator>::const_reference const_reference;
    typedef typename std::vector<Json,Allocator>::iterator iterator;
    typedef typename std::vector<Json,Allocator>::const_iterator const_iterator;

    json_array()
        : elements_()
    {
    }

    explicit json_array(const Allocator& allocator)
        : elements_(allocator)
    {
    }

    explicit json_array(size_t n, const Allocator& allocator = Allocator())
        : elements_(n,Json(),allocator)
    {
    }

    explicit json_array(size_t n, const Json& value, const Allocator& allocator = Allocator())
        : elements_(n,value,allocator)
    {
    }

    template <class InputIterator>
    json_array(InputIterator begin, InputIterator end, const Allocator& allocator = Allocator())
        : elements_(begin,end,allocator)
    {
    }

    json_array(const json_array& val)
        : elements_(val.elements_)
    {
    }

    json_array(const json_array& val, const Allocator& allocator)
        : elements_(val.elements_,allocator)
    {
    }
    json_array(json_array&& val)
        : elements_(std::move(val.elements_))
    {
    }
    json_array(json_array&& val, const Allocator& allocator)
        : elements_(std::move(val.elements_),allocator)
    {
    }

    json_array(std::initializer_list<Json> init, 
               const Allocator& allocator = Allocator())
        : elements_(std::move(init),allocator)
    {
    }

    Allocator get_allocator() const
    {
        return elements_.get_allocator();
    }

    void swap(json_array<Json,Allocator>& val)
    {
        elements_.swap(val.elements_);
    }

    size_t size() const {return elements_.size();}

    size_t capacity() const {return elements_.capacity();}

    void clear() {elements_.clear();}

    void shrink_to_fit() 
    {
        for (size_t i = 0; i < elements_.size(); ++i)
        {
            elements_[i].shrink_to_fit();
        }
        elements_.shrink_to_fit();
    }

    void reserve(size_t n) {elements_.reserve(n);}

    void resize(size_t n) {elements_.resize(n);}

    void resize(size_t n, const Json& val) {elements_.resize(n,val);}

    void remove_range(size_t from_index, size_t to_index) 
    {
        JSONCONS_ASSERT(from_index <= to_index);
        JSONCONS_ASSERT(to_index <= elements_.size());
        elements_.erase(elements_.begin()+from_index,elements_.begin()+to_index);
    }

    void erase(iterator first, iterator last) 
    {
        elements_.erase(first,last);
    }

    Json& operator[](size_t i) {return elements_[i];}

    const Json& operator[](size_t i) const {return elements_[i];}

    void push_back(const Json& value)
    {
        elements_.push_back(value);
    }

    void push_back(Json&& value)
    {
        elements_.push_back(std::move(value));
    }

    void add(size_t index, const Json& value)
    {
        auto position = index < elements_.size() ? elements_.begin() + index : elements_.end();
        elements_.insert(position, value);
    }

    void add(size_t index, Json&& value)
    {
        auto it = index < elements_.size() ? elements_.begin() + index : elements_.end();
        elements_.insert(it, std::move(value));
    }

#if defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ < 9
    // work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=54577
    iterator add(const_iterator pos, const Json& value)
    {
        iterator it = elements_.begin() + (pos - elements_.begin());
        return elements_.insert(it, value);
    }

    iterator add(const_iterator pos, Json&& value)
    {
        iterator it = elements_.begin() + (pos - elements_.begin());
        return elements_.insert(it, std::move(value));
    }
#else
    iterator add(const_iterator pos, const Json& value)
    {
        return elements_.insert(pos, value);
    }

    iterator add(const_iterator pos, Json&& value)
    {
        return elements_.insert(pos, std::move(value));
    }
#endif

    iterator begin() {return elements_.begin();}

    iterator end() {return elements_.end();}

    const_iterator begin() const {return elements_.begin();}

    const_iterator end() const {return elements_.end();}

    bool operator==(const json_array<Json,Allocator>& rhs) const
    {
        if (size() != rhs.size())
        {
            return false;
        }
        for (size_t i = 0; i < size(); ++i)
        {
            if (elements_[i] != rhs.elements_[i])
            {
                return false;
            }
        }
        return true;
    }
private:
    json_array& operator=(const json_array<Json,Allocator>&);
    std::vector<Json,Allocator> elements_;
};

template <class ValueT,class CharT>
class member_lt_string
{
    size_t length_;
public:
    member_lt_string(size_t length)
        : length_(length)
    {
    }

    bool operator()(const ValueT& a, const CharT* b) const
    {
        size_t len = std::min JSONCONS_NO_MACRO_EXP(a.name().length(),length_);
        int result = std::char_traits<CharT>::compare(a.name().data(),b,len);
        if (result != 0)
        {
            return result < 0;
        }

        return a.name().length() < length_;
    }
};

template <class StringT,class CharT>
bool name_le_string(const StringT& a, const CharT* b, size_t length)
{
    size_t min_len = std::min JSONCONS_NO_MACRO_EXP(a.length(),length);
    int result = std::char_traits<CharT>::compare(a.data(),b, min_len);
    if (result != 0)
    {
        return result < 0;
    }

    return a.length() <= length;
}

template <class StringT,class CharT>
bool name_eq_string(const StringT& a, const CharT* b, size_t length)
{
    return a.length() == length && std::char_traits<CharT>::compare(a.data(),b,length) == 0;
}

template <class ValueT>
class member_lt_member
{
    typedef typename ValueT::char_type char_type;
public:
    bool operator()(const ValueT& a, const ValueT& b) const
    {
        if (a.name().length() == b.name().length())
        {
            return std::char_traits<char_type>::compare(a.name().data(),b.name().data(),a.name().length()) < 0;
        }

        size_t len = std::min JSONCONS_NO_MACRO_EXP(a.name().length(),b.name().length());
        int result = std::char_traits<char_type>::compare(a.name().data(),b.name().data(),len);
        if (result != 0)
        {
            return result < 0;
        }

        return a.name().length() < b.name().length();
    }
};

template <class ValueT,class CharT>
class equals_pred
{
    const CharT* b_;
    size_t length_;
public:
    equals_pred(const CharT* b, size_t length)
        : b_(b), length_(length)
    {
    }

    bool operator()(const ValueT& a) const
    {
        size_t len = std::min JSONCONS_NO_MACRO_EXP(a.name().length(),length_);
        int result = std::char_traits<CharT>::compare(a.name().data(),b_,len);

        return result == 0 && a.name().length() == length_;
    }
};

template <class StringT, class ValueT>
class name_value_pair
{
public:
    typedef StringT string_type;
    typedef typename StringT::value_type char_type;

    name_value_pair()
    {
    }
    name_value_pair(const string_type& name)
        : name_(name)
    {
    }
    name_value_pair(string_type&& name)
        : name_(std::move(name))
    {
    }

    name_value_pair(const string_type& name, const ValueT& val)
        : name_(name), value_(val)
    {
    }
    name_value_pair(string_type&& name, const ValueT& val)
        : name_(std::move(name)), value_(val)
    {
    }
    name_value_pair(const string_type& name, ValueT&& val)
        : name_(name), value_(std::move(val))
    {
    }
    name_value_pair(string_type&& name, ValueT&& val)
        : name_(std::move(name)), value_(std::move(val))
    {
    }
    name_value_pair(const name_value_pair& member)
        : name_(member.name_), value_(member.value_)
    {
    }
    name_value_pair(name_value_pair&& member)
        : name_(std::move(member.name_)), value_(std::move(member.value_))
    {
    }

    const string_type& name() const
    {
        return name_;
    }

    ValueT& value()
    {
        return value_;
    }

    const ValueT& value() const
    {
        return value_;
    }

    void value(const ValueT& value)
    {
        value_ = value;
    }

    void value(ValueT&& value)
    {
        value_ = std::move(value);
    }

    void swap(name_value_pair& member)
    {
        name_.swap(member.name_);
        value_.swap(member.value_);
    }

    name_value_pair& operator=(const name_value_pair& member)
    {
        if (this != & member)
        {
            name_ = member.name_;
            value_ = member.value_;
        }
        return *this;
    }

    name_value_pair& operator=(name_value_pair&& member)
    {
        if (this != &member)
        {
            name_.swap(member.name_);
            value_.swap(member.value_);
        }
        return *this;
    }

    void shrink_to_fit() 
    {
        name_.shrink_to_fit();
        value_.shrink_to_fit();
    }
private:
    string_type name_;
    ValueT value_;
};

template <class IteratorT,class NonConstIteratorT>
class json_object_iterator
{
public:
    typedef IteratorT iterator;
    typedef typename std::iterator_traits<IteratorT>::value_type value_type;
    typedef typename std::iterator_traits<IteratorT>::difference_type difference_type;
    typedef typename std::iterator_traits<IteratorT>::pointer pointer;
    typedef typename std::iterator_traits<IteratorT>::reference reference;
    typedef std::bidirectional_iterator_tag iterator_category;

    json_object_iterator()
    {
    }

    json_object_iterator(iterator it)
        : it_(it)
    {
    }

    json_object_iterator(const json_object_iterator<NonConstIteratorT,NonConstIteratorT>& it)
        : it_(it.it_)
    {
    }

    json_object_iterator& operator=(json_object_iterator rhs)
    {
        swap(*this,rhs);
        return *this;
    }

    json_object_iterator& operator++()
    {
        ++it_;
        return *this;
    }

    json_object_iterator operator++(int) // postfix increment
    {
        json_object_iterator temp(*this);
        ++it_;
        return temp;
    }

    json_object_iterator& operator--()
    {
        --it_;
        return *this;
    }

    json_object_iterator operator--(int)
    {
        json_object_iterator temp(*this);
        --it_;
        return temp;
    }

    reference operator*() const
    {
        return *it_;
    }

    pointer operator->() const
    {
        return &(*it_);
    }

    friend bool operator==(const json_object_iterator& it1, const json_object_iterator& it2)
    {
        return it1.it_ == it2.it_;
    }
    friend bool operator!=(const json_object_iterator& it1, const json_object_iterator& it2)
    {
        return !(it1.it_ == it2.it_);
    }
    friend void swap(json_object_iterator& lhs, json_object_iterator& rhs)
    {
        using std::swap;
        swap(lhs.it_,rhs.it_);
    }

    iterator get()
    {
        return it_;
    }

//private:
    IteratorT it_;
};

template <class StringT,class Json,bool IsSorted,class Allocator>
class json_object
{
};

template <class StringT,class Json,class Allocator>
class json_object<StringT,Json,true,Allocator>
{
public:
    typedef Allocator allocator_type;
    typedef typename Json::char_type char_type;
    typedef StringT string_type;
    typedef name_value_pair<StringT,Json> value_type;
    typedef typename std::vector<value_type, allocator_type>::iterator base_iterator;
    typedef typename std::vector<value_type, allocator_type>::const_iterator const_base_iterator;

    typedef json_object_iterator<base_iterator,base_iterator> iterator;
    typedef json_object_iterator<const_base_iterator,base_iterator> const_iterator;
private:
    std::vector<value_type,allocator_type> members_;
public:
    json_object()
        : members_()
    {
    }
    json_object(const allocator_type& allocator)
        : members_(allocator)
    {
    }

    json_object(const json_object& val)
        : members_(val.members_)
    {
    }

    json_object(json_object&& val)
        : members_(std::move(val.members_))
    {
    }

    json_object(const json_object& val, const allocator_type& allocator) :
        members_(val.members_,allocator)
    {
    }

    json_object(json_object&& val,const allocator_type& allocator) :
        members_(std::move(val.members_),allocator)
    {
    }

    json_object(std::initializer_list<typename Json::array> init, 
               const Allocator& allocator = Allocator())
    {
        (void)allocator;
        for (const auto& element : init)
        {
            if (element.size() != 2 || !element[0].is_string())
            {
                JSONCONS_THROW_EXCEPTION(std::runtime_error, "Cannot create object from initializer list");
                break;
            }
        }
        for (auto& element : init)
        {
            set(element[0].as_string(), std::move(element[1]));
        }
    }

    Allocator get_allocator() const
    {
        return members_.get_allocator();
    }

    iterator begin()
    {
        //return members_.begin();
        return iterator(members_.begin());
    }

    iterator end()
    {
        //return members_.end();
        return iterator(members_.end());
    }

    const_iterator begin() const
    {
        //return iterator(members.data());
        return const_iterator(members_.begin());
    }

    const_iterator end() const
    {
        return const_iterator(members_.end());
    }

    void swap(json_object& val)
    {
        members_.swap(val.members_);
    }

    size_t size() const {return members_.size();}

    size_t capacity() const {return members_.capacity();}

    void clear() {members_.clear();}

    void shrink_to_fit() 
    {
        for (size_t i = 0; i < members_.size(); ++i)
        {
            members_[i].shrink_to_fit();
        }
        members_.shrink_to_fit();
    }

    void reserve(size_t n) {members_.reserve(n);}

    Json& at(size_t) 
    {
        JSONCONS_THROW_EXCEPTION(std::runtime_error,"Index on non-array value not supported");
    }

    const Json& at(size_t) const 
    {
        JSONCONS_THROW_EXCEPTION(std::runtime_error,"Index on non-array value not supported");
    }

    iterator find(const char_type* name, size_t length)
    {
        member_lt_string<value_type,char_type> comp(length);
        auto it = std::lower_bound(members_.begin(),members_.end(), name, comp);
        auto result = (it != members_.end() && name_eq_string(it->name(),name,length)) ? it : members_.end();
        return iterator(result);
    }

    const_iterator find(const char_type* name, size_t length) const
    {
        member_lt_string<value_type,char_type> comp(length);
        auto it = std::lower_bound(members_.begin(),members_.end(), name, comp);
        auto result = (it != members_.end() && name_eq_string(it->name(),name,length)) ? it : members_.end();
        return const_iterator(result);
    }

    void erase(iterator first, iterator last) 
    {
        members_.erase(first.get(),last.get());
    }

    void erase(const char_type* name, size_t length) 
    {
        member_lt_string<value_type,char_type> comp(length);
        auto it = std::lower_bound(members_.begin(),members_.end(), name, comp);
        if (it != members_.end() && name_eq_string(it->name(),name,length))
        {
            members_.erase(it);
        }
    }

    template<class InputIt, class UnaryPredicate>
    void insert(InputIt first, InputIt last, UnaryPredicate pred)
    {
        size_t count = std::distance(first,last);
        size_t pos = members_.size();
        members_.resize(pos+count);
        auto d = members_.begin()+pos;
        for (auto s = first; s != last; ++s, ++d)
        {
            *d = pred(*s);
        }
        std::sort(members_.begin(),members_.end(),member_lt_member<value_type>());
    }

    void set(const char_type* s, size_t length, const Json& value)
    {
        auto it = std::lower_bound(members_.begin(),members_.end(),s,member_lt_string<value_type,char_type>(length));
        if (it == members_.end())
        {
            members_.push_back(value_type(string_type(s,length),value));
        }
        else if (name_eq_string(it->name(),s,length))
        {
            it->value(value);
        }
        else
        {
            members_.insert(it,value_type(string_type(s,length),value));
        }
    }

    void set(const char_type* s, size_t length, Json&& value)
    {
        auto it = std::lower_bound(members_.begin(),members_.end(),s,member_lt_string<value_type,char_type>(length));
        if (it == members_.end())
        {
            members_.push_back(value_type(string_type(s,length),std::move(value)));
        }
        else if (name_eq_string(it->name(),s,length))
        {
            it->value(std::move(value));
        }
        else
        {
            members_.insert(it,value_type(string_type(s,length),std::move(value)));
        }
    }

    void set(string_type&& name, const Json& value)
    {
        auto it = std::lower_bound(members_.begin(),members_.end(),name.data() ,member_lt_string<value_type,char_type>(name.length()));
        if (it == members_.end())
        {
            members_.push_back(value_type(std::move(name), value));
        }
        else if (it->name() == name)
        {
            it->value(value);
        }
        else
        {
            members_.insert(it,value_type(std::move(name),value));
        }
    }

    void set(const string_type& name, const Json& value)
    {
        set(name.data(),name.length(),value);
    }

    void set(const string_type& name, Json&& value)
    {
        set(name.data(),name.length(),std::move(value));
    }

    void set(string_type&& name, Json&& value)
    {
        auto it = std::lower_bound(members_.begin(),members_.end(),name.data() ,member_lt_string<value_type,char_type>(name.length()));
        if (it == members_.end())
        {
            members_.push_back(value_type(std::move(name), std::move(value)));
        }
        else if (it->name() == name)
        {
            it->value(std::move(value));
        }
        else
        {
            members_.insert(it,value_type(std::move(name),std::move(value)));
        }
    }

    iterator set(iterator hint, const char_type* name, const Json& value)
    {
        return set(hint, name, std::char_traits<char_type>::length(name), value);
    }

    iterator set(iterator hint, const char_type* name, Json&& value)
    {
        return set(hint, name, std::char_traits<char_type>::length(name), std::move(value));
    }

    iterator set(iterator hint, const char_type* s, size_t length, const Json& value)
    {
        base_iterator it;
        if (hint.get() != members_.end() && name_le_string(hint.get()->name(), s, length))
        {
            it = std::lower_bound(hint.get(),members_.end(),s,member_lt_string<value_type,char_type>(length));
        }
        else
        {
            it = std::lower_bound(members_.begin(),members_.end(),s, member_lt_string<value_type,char_type>(length));
        }

        if (it == members_.end())
        {
            members_.push_back(value_type(string_type(s, length), value));
            it = members_.begin() + (members_.size() - 1);
        }
        else if (name_eq_string(it->name(),s,length))
        {
            it->value(value);
        }
        else
        {
           it = members_.insert(it,value_type(string_type(s,length),value));
        }
        return iterator(it);
    }

    iterator set(iterator hint, const char_type* s, size_t length, Json&& value)
    {
        base_iterator it;
        if (hint.get() != members_.end() && name_le_string(hint.get()->name(), s, length))
        {
            it = std::lower_bound(hint.get(),members_.end(),s,member_lt_string<value_type,char_type>(length));
        }
        else
        {
            it = std::lower_bound(members_.begin(),members_.end(),s, member_lt_string<value_type,char_type>(length));
        }

        if (it == members_.end())
        {
            members_.push_back(value_type(string_type(s, length), std::move(value)));
            it = members_.begin() + (members_.size() - 1);
        }
        else if (name_eq_string(it->name(),s,length))
        {
            it->value(std::move(value));
        }
        else
        {
           it = members_.insert(it,value_type(string_type(s,length),std::move(value)));
        }
        return iterator(it);
    }

    iterator set(iterator hint, const string_type& name, const Json& value)
    {
        return set(hint,name.data(),name.length(),value);
    }

    iterator set(iterator hint, string_type&& name, const Json& value)
    {
        base_iterator it;
        if (hint.get() != members_.end() && hint.get()->name() <= name)
        {
            it = std::lower_bound(hint.get(),members_.end(),name.data() ,member_lt_string<value_type,char_type>(name.length()));
        }
        else
        {
            it = std::lower_bound(members_.begin(),members_.end(),name.data() ,member_lt_string<value_type,char_type>(name.length()));
        }

        if (it == members_.end())
        {
            members_.push_back(value_type(std::move(name), value));
            it = members_.begin() + (members_.size() - 1);
        }
        else if (it->name() == name)
        {
            it->value(value);
        }
        else
        {
            it = members_.insert(it,value_type(std::move(name),value));
        }
        return iterator(it);
    }

    iterator set(iterator hint, const string_type& name, Json&& value)
    {
        return set(hint,name.data(),name.length(),std::move(value));
    }

    iterator set(iterator hint, string_type&& name, Json&& value)
    {
        typename std::vector<value_type,allocator_type>::iterator it;
        if (hint.get() != members_.end() && hint.get()->name() <= name)
        {
            it = std::lower_bound(hint.get(),members_.end(),name.data() ,member_lt_string<value_type,char_type>(name.length()));
        }
        else
        {
            it = std::lower_bound(members_.begin(),members_.end(),name.data() ,member_lt_string<value_type,char_type>(name.length()));
        }

        if (it == members_.end())
        {
            members_.push_back(value_type(std::move(name), std::move(value)));
            it = members_.begin() + (members_.size() - 1);
        }
        else if (it->name() == name)
        {
            it->value(std::move(value));
        }
        else
        {
            it = members_.insert(it,value_type(std::move(name),std::move(value)));
        }
        return iterator(it);
    }

    bool operator==(const json_object& rhs) const
    {
        if (size() != rhs.size())
        {
            return false;
        }
        for (auto it = members_.begin(); it != members_.end(); ++it)
        {

            auto rhs_it = std::lower_bound(rhs.members_.begin(), rhs.members_.end(), *it, member_lt_member<value_type>());
            // member_lt_member only compares keys, so we need to check the value separately
            if (rhs_it == rhs.members_.end() || rhs_it->name() != it->name() || rhs_it->value() != it->value())
            {
                return false;
            }
        }
        return true;
    }
private:
    json_object& operator=(const json_object&) = delete;
};

// Original order

template <class StringT,class Json,class Allocator>
class json_object<StringT,Json,false,Allocator>
{
public:
    typedef Allocator allocator_type;
    typedef typename Json::char_type char_type;
    typedef StringT string_type;
    typedef name_value_pair<StringT,Json> value_type;
    typedef typename std::vector<value_type, allocator_type>::iterator iterator;
    typedef typename std::vector<value_type, allocator_type>::const_iterator const_iterator;

private:
    std::vector<value_type,allocator_type> members_;
public:
    json_object(const allocator_type& allocator = allocator_type())
        : members_(allocator)
    {
    }

    json_object(const json_object& val)
        : members_(val.members_)
    {
    }

    json_object(json_object&& val)
        : members_(std::move(val.members_))
    {
    }

    json_object(const json_object& val, const allocator_type& allocator) :
        members_(val.members_,allocator)
    {
    }

    json_object(json_object&& val,const allocator_type& allocator) :
        members_(std::move(val.members_),allocator)
    {
    }

    Allocator get_allocator() const
    {
        return members_.get_allocator();
    }

    iterator begin()
    {
        return iterator(members_.begin());
    }

    iterator end()
    {
        return iterator(members_.end());
    }

    const_iterator begin() const
    {
        return const_iterator(members_.begin());
    }

    const_iterator end() const
    {
        return const_iterator(members_.end());
    }

    void swap(json_object& val)
    {
        members_.swap(val.members_);
    }

    size_t size() const {return members_.size();}

    size_t capacity() const {return members_.capacity();}

    void clear() {members_.clear();}

    void shrink_to_fit() 
    {
        for (size_t i = 0; i < members_.size(); ++i)
        {
            members_[i].shrink_to_fit();
        }
        members_.shrink_to_fit();
    }

    void reserve(size_t n) {members_.reserve(n);}

    Json& at(size_t i) 
    {
        if (i >= members_.size())
        {
            JSONCONS_THROW_EXCEPTION(std::out_of_range,"Invalid array subscript");
        }
        return members_[i].value();
    }

    const Json& at(size_t i) const 
    {
        if (i >= members_.size())
        {
            JSONCONS_THROW_EXCEPTION(std::out_of_range,"Invalid array subscript");
        }
        return members_[i].value();
    }

    iterator find(const char_type* name, size_t length)
    {
        equals_pred<value_type,char_type> comp(name, length);
        auto it = std::find_if(members_.begin(),members_.end(), comp);
        return it;
    }

    const_iterator find(const char_type* name, size_t length) const
    {
        equals_pred<value_type,char_type> comp(name, length);
        auto it = std::find_if(members_.begin(),members_.end(), comp);
        return it;
    }

    void erase(iterator first, iterator last) 
    {
        members_.erase(first.get(),last.get());
    }

    void erase(const char_type* name, size_t length) 
    {
        equals_pred<value_type,char_type> comp(name, length);
        auto it = std::find_if(members_.begin(),members_.end(), comp);
        if (it != members_.end())
        {
            members_.erase(it);
        }
    }

    template<class InputIt, class UnaryPredicate>
    void insert(InputIt first, InputIt last, UnaryPredicate pred)
    {
        size_t count = std::distance(first,last);
        size_t pos = members_.size();
        members_.resize(pos+count);
        auto d = members_.begin()+pos;
        for (auto s = first; s != last; ++s, ++d)
        {
            *d = pred(*s);
        }
    }

    void set(const char_type* name, size_t length, const Json& value)
    {
        equals_pred<value_type,char_type> comp(name, length);
        auto it = std::find_if(members_.begin(),members_.end(), comp);

        if (it == members_.end())
        {
            members_.push_back(value_type(string_type(name,length),value));
        }
        else
        {
            it->value(value);
        }
    }

    void set(const char_type* name, size_t length, Json&& value)
    {
        equals_pred<value_type,char_type> comp(name, length);
        auto it = std::find_if(members_.begin(),members_.end(), comp);

        if (it == members_.end())
        {
            members_.push_back(value_type(string_type(name,length),std::move(value)));
        }
        else 
        {
            it->value(std::move(value));
        }
    }

    void set(string_type&& name, const Json& value)
    {
        equals_pred<value_type,char_type> comp(name.data(), name.length());
        auto it = std::find_if(members_.begin(),members_.end(), comp);

        if (it == members_.end())
        {
            members_.push_back(value_type(std::move(name), value));
        }
        else
        {
            it->value(value);
        }
    }

    void set(const string_type& name, const Json& value)
    {
        set(name.data(),name.length(),value);
    }

    void set(const string_type& name, Json&& value)
    {
        set(name.data(),name.length(),std::move(value));
    }

    void set(string_type&& name, Json&& value)
    {
        equals_pred<value_type,char_type> comp(name.data(), name.length());
        auto it = std::find_if(members_.begin(),members_.end(), comp);

        if (it == members_.end())
        {
            members_.push_back(value_type(std::move(name), std::move(value)));
        }
        else
        {
            it->value(std::move(value));
        }
    }

    iterator set(iterator hint, const char_type* name, const Json& value)
    {
        return set(hint, name, std::char_traits<char_type>::length(name), value);
    }

    iterator set(iterator hint, const char_type* name, Json&& value)
    {
        return set(hint, name, std::char_traits<char_type>::length(name), std::move(value));
    }

    iterator set(iterator hint, const char_type* name, size_t length, const Json& value)
    {
        iterator it = hint;

        if (it == members_.end())
        {
            members_.push_back(value_type(string_type(name, length), value));
            it = members_.begin() + (members_.size() - 1);
        }
        else if (name_eq_string(it->name(),name,length))
        {
            it->value(value);
        }
        else
        {
           it = members_.insert(it,value_type(string_type(name,length),value));
        }
        return it;
    }

    iterator set(iterator hint, const char_type* name, size_t length, Json&& value)
    {
        iterator it = hint;

        if (it == members_.end())
        {
            members_.push_back(value_type(string_type(name, length), std::move(value)));
            it = members_.begin() + (members_.size() - 1);
        }
        else if (name_eq_string(it->name(),name,length))
        {
            it->value(std::move(value));
        }
        else
        {
           it = members_.insert(it,value_type(string_type(name,length),std::move(value)));
        }
        return it;
    }

    iterator set(iterator hint, const string_type& name, const Json& value)
    {
        return set(hint,name.data(),name.length(),value);
    }

    iterator set(iterator hint, string_type&& name, const Json& value)
    {
        iterator it = hint;

        if (it == members_.end())
        {
            members_.push_back(value_type(std::move(name), value));
            it = members_.begin() + (members_.size() - 1);
        }
        else if (it->name() == name)
        {
            it->value(value);
        }
        else
        {
            it = members_.insert(it,value_type(std::move(name),value));
        }
        return it;
    }

    iterator set(iterator hint, const string_type& name, Json&& value)
    {
        return set(hint,name.data(),name.length(),std::move(value));
    }

    iterator set(iterator hint, string_type&& name, Json&& value)
    {
        typename std::vector<value_type,allocator_type>::iterator it = hint;

        if (it == members_.end())
        {
            members_.push_back(value_type(std::move(name), std::move(value)));
            it = members_.begin() + (members_.size() - 1);
        }
        else if (it->name() == name)
        {
            it->value(std::move(value));
        }
        else
        {
            it = members_.insert(it,value_type(std::move(name),std::move(value)));
        }
        return it;
    }

    bool operator==(const json_object& rhs) const
    {
        if (size() != rhs.size())
        {
            return false;
        }
        for (auto it = members_.begin(); it != members_.end(); ++it)
        {
            equals_pred<value_type,char_type> comp(it->name().data(), it->name().length());
            auto rhs_it = std::find_if(rhs.members_.begin(),rhs.members_.end(), comp);
            if (rhs_it == rhs.members_.end() || rhs_it->name() != it->name() || rhs_it->value() != it->value())
            {
                return false;
            }
        }
        return true;
    }
private:
    json_object& operator=(const json_object&) = delete;
};



}

#endif
