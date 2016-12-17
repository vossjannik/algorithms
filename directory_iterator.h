// directory_iterator.h standart header - by jannik voss

/*
HEADER FILE INFORMATION:

source: no source file needed

class:
	directory_iterator
*/

#pragma once

#include <string>
#include <memory>
#include <Windows.h>

namespace filesys{

/*
TEMPLATE CLASS directory_iterator

PLATFORM: Windows (filesystem: wchar_t)

iterate through all files in a directory
-> notice: the current version of the directory_iterator does not support backward iteration
-> usage:   for(auto it=directory_iterator(L"C:/"); it != directory_iterator(); ++it)
				std::wcout << it->c_str() << std::endl;
-> TODO: functions: 'equals()' and 'swap()'; optimize the comparison operator
*/

	// TEMPLATE CLASS Basic_directory_iterator
template<class T>
	class Basic_directory_iterator
	{
public:
	typedef std::input_iterator_tag iterator_category;
	typedef T value_type;
	typedef std::ptrdiff_t		 difference_type;
	typedef value_type*			 pointer;
	typedef value_type&			 reference;
	typedef const value_type* 	 const_pointer;
	typedef const value_type&    const_reference;
	typedef typename value_type::size_type	 size_type;

	//static Basic_directory_iterator end()
	//	{   // get end-iterator
	//	return Basic_directory_iterator();
	//	}

	~Basic_directory_iterator()
		{   // destruct
		Clear();
		}

	Basic_directory_iterator()
		: MyFind_data()
		, MyHfile()
		, MyCurrent()
		{   // end-iterator
		}

	Basic_directory_iterator(const Basic_directory_iterator& o)
		: MyFind_data(o.MyFind_data)
		, MyHfile(o.MyHfile)
		, MyCurrent(o.MyCurrent)
		{
		}

	Basic_directory_iterator(Basic_directory_iterator&& o)
		: MyFind_data(o.MyFind_data)
		, MyHfile(o.MyHfile)
		, MyCurrent(o.MyCurrent)
		{
		o.clear();
		}

	Basic_directory_iterator(const value_type& p)
		: MyFind_data()
		, MyHfile( FindFirstFile( (p+L"*").c_str() , &MyFind_data ), &FindClose )
		, MyCurrent(value_type(MyHfile.get() != INVALID_HANDLE_VALUE? MyFind_data.cFileName: TEXT("") ))
		{   // construct from specified value
		}

	Basic_directory_iterator& operator ++ ()
		{   // pre-increment
		Forward();
		return (*this);
		}

	Basic_directory_iterator operator ++(int)
		{   // increment
		Basic_directory_iterator tmp = *this;
		Forward();
		return tmp;
		}

	Basic_directory_iterator& operator += (/*int*/ size_type num)
		{   // step Forward
		std::advance(*this, num);
		return (*this);
		}

	Basic_directory_iterator operator + (int num) const
		{   // step Forward
		Basic_directory_iterator tmp = *this;
		return (tmp += num);
		}


	#if defined (_MSC_VER) && (_MSC_VER < 1300)
		#pragma warning(disable:4284) // infix notation problem when using iterator operator ->
	#endif

	const value_type& operator * () const
		{   // reference to the current filepath
		return (MyCurrent);
		}

	const value_type* operator ->() const
		{   // pointer to the current filepath
		return (&MyCurrent);
		}

	value_type get() const
		{   // copy the current filepath
		return (MyCurrent);
		}

	void swap(Basic_directory_iterator& o)
		{   // exchange internals
		std::swap(MyFind_data, o.MyFind_data);
		std::swap(MyHfile, o.MyHfile);
		std::swap(MyCurrent, o.MyCurrent);
		}

	bool equals(const Basic_directory_iterator& o) const
		{   // test for equality
		return (MyCurrent == o.MyCurrent); // unsafe (two different iterators pointing to one file)
		}

private:
	void Forward()
		{   // step Forward to the next file
		if(FindNextFile(MyHfile.get(), &MyFind_data))
			MyCurrent.assign(value_type(MyFind_data.cFileName));
		else
			MyCurrent.clear();
		}

	void Clear()
		{   // make end-iterator
		MyFind_data = WIN32_FIND_DATA();
		MyHfile = shared_handle();
		MyCurrent.clear();
		}

	typedef std::shared_ptr< std::remove_pointer< HANDLE >::type > shared_handle;

	WIN32_FIND_DATA MyFind_data;
	shared_handle MyHfile;
	value_type MyCurrent; // MyCurrent filepath
	};


template<class T>
	bool operator == (const Basic_directory_iterator<T>& l, const Basic_directory_iterator<T>& r)
	{   // test for equality
	return (l.equals(r));
	}

template<class T>
	bool operator != (const Basic_directory_iterator<T>& l, const Basic_directory_iterator<T>& r)
	{   // test for inequality
	return !(l == r);
	}

template<class T>
	void swap(Basic_directory_iterator<T>& l, Basic_directory_iterator<T>& r)
	{   // exchange internals
	l.swap(r);
	}


	typedef Basic_directory_iterator<std::basic_string<wchar_t>>
		directory_iterator;

};//end: namespace
