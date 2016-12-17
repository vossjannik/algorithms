// algorithms_sort.h standart header - by jannik voss

/*
HEADER FILE INFORMATION:

language: C++11 not required
source: no source file needed

algorithms (quick overview):
    bubble_sort
    selection_sort
    quick_sort
    insertion_sort
    shell_sort
    comb_sort
    heap_sort

functions:
	template<class Fd_iter, class Less 
		= std::less<std::iterator_traits<Fd_iter>::value_type>>
	void bubble_sort(Fd_iter first, Fd_iter last, Less cmp = Less())

	template<class Fd_iter, class Less 
		= std::less<std::iterator_traits<Fd_iter>::value_type>>
	void selection_sort(Fd_iter first, Fd_iter last, Less cmp = Less())

	template<class Bd_iter, class Less 
		= std::less<std::iterator_traits<Bd_iter>::value_type>>
	void quick_sort(Bd_iter first, Bd_iter last, Less cmp=Less())

	template<class Bd_iter, class Less 
		= std::less<std::iterator_traits<Bd_iter>::value_type>>
	void insertion_sort(Bd_iter first, Bd_iter last, Less cmp = Less())

	template<class Bd_iter, class Less 
		= std::less<std::iterator_traits<Bd_iter>::value_type>>
	void shell_sort(Bd_iter first, Bd_iter last, Less cmp = Less())

	template<class Fd_iter, class Less 
		= std::less<std::iterator_traits<Fd_iter>::value_type>>
	void comb_sort(Fd_iter first, Fd_iter last, Less cmp = Less())

	template<class Rda_iter, class Less 
		= std::less<std::iterator_traits<Rda_iter>::value_type>>
	void heap_sort(Rda_iter first, Rda_iter last, Less cmp = Less())
*/

#pragma once

#include <algorithm>
#include <iterator>
#include <functional>


namespace algo{

/*
TEMPLATE FUNCTION bubble_sort

order [_First, _Last) using a bubble sort algorithm.
further information: http://en.wikipedia.org/wiki/Bubble_sort

you can optionally use a binary function object class whose call returns 
whether the its first argument compares less than the second.

requirements: Fd_iter is a forward-iterator type
complexity:  ...

assertions: (assert(x))
	Fd_iter is an STL-like forward iterator (std::forward_iterator_tag)
*/

	// TEMPLATE FUNCTION Bubble_sort0  (simple version)
template<class Fd_iter, class Less>
	void Bubble_sort0(Fd_iter first, Fd_iter last, Less cmp)
	{   // order [_First, _Last)
	typedef std::is_base_of<std::forward_iterator_tag,typename Fd_iter::iterator_category> ass_ty;
	assert(typename ass_ty::value);

	for(auto i = first; i != last; std::advance(i,1))
		for(auto j = first; j < i; std::advance(j,1))
			if(cmp(*i, *j)) // i<j
				std::iter_swap(i, j);
	}

	// TEMPLATE FUNCTION Bubble_sort1  (optimized version)
template<class Fd_iter, class Less>
	void Bubble_sort1(Fd_iter first, Fd_iter last, Less cmp)
	{   // order [_First, _Last)
	typedef std::is_base_of<std::forward_iterator_tag,typename Fd_iter::iterator_category> ass_ty;
	assert(typename ass_ty::value);
		
	// left (first+1) and right (last-1) element
	const auto r = std::next(first,std::distance(first, last)-1);
	const auto l = std::next(first);

	Fd_iter i;  // the loop variable
	do  {
		auto newi = l;
		for(auto j=first; j != r; std::advance(j,1))
			{
			const auto nj = std::next(j);  // next element
			if(cmp(*nj,*j))  // nj<j
				{
				std::iter_swap(j, nj);
				newi = nj;
				}
			}
		i = newi;  // update the loop variable
		} while(i!=l);
	}

	// TEMPLATE FUNCTION bubble_sort
template<class Fd_iter, class Less 
		= std::less<std::iterator_traits<Fd_iter>::value_type>>
	void bubble_sort(Fd_iter first, Fd_iter last, Less cmp = Less())
	{   // order [_First, _Last)
	Bubble_sort1(first, last, cmp);
	}


/*
TEMPLATE FUNCTION selection_sort

order [_First, _Last) using a selection sort algorithm.
further information: http://en.wikipedia.org/wiki/Selection_sort

you can optionally use a binary function object class whose call returns 
whether the its first argument compares less than the second.

requirements: Fd_iter is a forward-iterator type
complexity: ...

assertions: (assert(x))
	Fd_iter is an STL-like forward iterator (std::forward_iterator_tag)
*/

	// TEMPLATE FUNCTION selection_sort
template<class Fd_iter, class Less 
		= std::less<std::iterator_traits<Fd_iter>::value_type>>
	void selection_sort(Fd_iter first, Fd_iter last, Less cmp = Less())
	{   // order [_First, _Last)
	typedef std::is_base_of<std::forward_iterator_tag,typename Fd_iter::iterator_category> ass_ty;
	assert(typename ass_ty::value);

	for(auto i=first; i<last; std::advance(i,1))
		{
		auto min = std::min_element(i,last,cmp);
		std::iter_swap(i,min);
		}
	}


/*
TEMPLATE FUNCTION quick_sort

order [_First, _Last) using a quick sort algorithm.
further information: http://en.wikipedia.org/wiki/Quick_sort

you can optionally use a binary function object class whose call returns 
whether the its first argument compares less than the second.

requirements: Bd_iter is a bidirectional-iterator type
complexity:  ...

assertions: (assert(x))
	Bd_iter is an STL-like bidirectional iterator (std::bidirectional_iterator_tag)
*/

	// TEMPLATE FUNCTION quick_sort
template<class Bd_iter, class Less 
		= std::less<std::iterator_traits<Bd_iter>::value_type>>
	void quick_sort(Bd_iter first, Bd_iter last, Less cmp=Less())
	{   // order [_First, _Last)
	typedef std::is_base_of<std::bidirectional_iterator_tag,typename Bd_iter::iterator_category> ass_ty;
	assert(typename ass_ty::value);

	auto dist = std::distance(first,last);
	if(dist < 2)
		return;

	// select pivot in the middle of the sequence
	auto pivot = first;
	std::advance(pivot, dist/2);
	auto val = *pivot;
	
	// select left (first) and right (last-1) element
	auto l = first;
	auto r = std::prev(last);
	
	std::iter_swap(pivot, r);   // swap the pivot element with the last element

	// split the sequence into two parts
	// ->values lower than the pivot element go to the left part
	// ->values highter than the pivot element go to the right part
	auto pivot_pos = l;
	while(l != r)
		{
		if(cmp(*l,val))
			{
			std::iter_swap(l, pivot_pos);
			std::advance(pivot_pos, 1);
			}
		std::advance(l, 1);
		}

	std::iter_swap(pivot_pos, r);   // swap back the pivot element

	// recursively call the function for both parts
	quick_sort(first, pivot_pos, cmp);
	quick_sort(std::next(pivot_pos), last, cmp);
	}


/*
TEMPLATE FUNCTION insertion_sort

order [_First, _Last) using an insertion sort algorithm.
further information: http://en.wikipedia.org/wiki/Insertion_sort

you can optionally use a binary function object class whose call returns 
whether the its first argument compares less than the second.

requirements: Bd_iter is a bidirectional-iterator type
complexity:  ...

assertions: (assert(x))
	Bd_iter is an STL-like bidirectional iterator (std::bidirectional_iterator_tag)
*/

	// TEMPLATE FUNCTION insertion_sort
template<class Bd_iter, class Less 
		= std::less<std::iterator_traits<Bd_iter>::value_type>>
	void insertion_sort(Bd_iter first, Bd_iter last, Less cmp = Less())
	{   // order [_First, _Last)
	typedef std::is_base_of<std::bidirectional_iterator_tag,typename Bd_iter::iterator_category> ass_ty;
	assert(typename ass_ty::value);

	for(auto i = std::next(first); i != last; std::advance(i,1))
		for(auto j = i; j != first && cmp(*j,*std::prev(j)); std::advance(j,-1))
			std::iter_swap(std::prev(j), j);
	}

/*
TEMPLATE FUNCTION shell_sort

order [_First, _Last) using a shell sort algorithm.
shell sort is a modification of insertion sort.
further information: http://en.wikipedia.org/wiki/Shellsort

you can optionally use a binary function object class whose call returns 
whether the its first argument compares less than the second.

requirements: Bd_iter is a bidirectional-iterator type
complexity: ...
gaps: 701,301,132,57,23,10,4,1

assertions: (assert(x))
	Bd_iter is an STL-like bidirectional iterator (std::bidirectional_iterator_tag)
*/

	// TEMPLATE FUNCTION shell_sort
template<class Bd_iter, class Less 
		= std::less<std::iterator_traits<Bd_iter>::value_type>>
	void shell_sort(Bd_iter first, Bd_iter last, Less cmp = Less())
	{   // order [_First, _Last)
	typedef std::is_base_of<std::bidirectional_iterator_tag,typename Bd_iter::iterator_category> ass_ty;
	assert(typename ass_ty::value);
		
	typedef typename std::iterator_traits<Bd_iter>::difference_type diff_ty;
	static const diff_ty gaps[8] = {701,301,132,57,23,10,4,1};

	auto n = std::distance(first,last);
	for(diff_ty g:gaps)
		{
		for(auto i=g; i<n; ++i)
			{
			auto t=*std::next(first,i);
			diff_ty j=i;
			for(; j>=g && cmp(t,*std::next(first,j-g)); j-=g)
				*std::next(first,j) = *std::next(first,j-g);
			*std::next(first,j) = t;
			}
		}
	}

/*
TEMPLATE FUNCTION comb_sort

order [_First, _Last) using a comb sort algorithm.
comb sort is a modification of bubble sort.
further information: http://en.wikipedia.org/wiki/Comb_sort

you can optionally use a binary function object class whose call returns 
whether the its first argument compares less than the second.

requirements: Fd_iter is a forward-iterator type
complexity:  ...

assertions: (assert(x))
	Fd_iter is an STL-like forward iterator (std::forward_iterator_tag)
*/
	// TEMPLATE FUNCTION comb_sort
template<class Fd_iter, class Less 
		= std::less<std::iterator_traits<Fd_iter>::value_type>>
	void comb_sort(Fd_iter first, Fd_iter last, Less cmp = Less())
	{   // order [_First, _Last)
	typedef std::is_base_of<std::forward_iterator_tag,typename Fd_iter::iterator_category> ass_ty;
	assert(typename ass_ty::value);
		
	typedef typename std::iterator_traits<Fd_iter>::difference_type diff_ty;
	
	const diff_ty n = std::distance(first,last);
	diff_ty step=n;
	 
	bool swapped;
	do  {
		swapped = false;  // reset the loop variable
		if(step > 1) step = diff_ty(step/1.3); // shrink by factor 1.3 
		
		for(Fd_iter i=first, j=std::next(i,step); 
			i<std::next(first,n-step); 
			std::advance(i,1), std::advance(j,1)
		   ){
			if(cmp(*j,*i))
				{
				std::iter_swap(i, j);
				swapped = true;
				}
			}
		} while(swapped || step > 1);
	}


/*
TEMPLATE FUNCTION heap_sort

order [_First, _Last) using a heap sort algorithm.
further information: http://en.wikipedia.org/wiki/Heapsort

you can optionally use a binary function object class whose call returns 
whether the its first argument compares less than the second.

requirements: Rda_iter is a random-access-iterator type
complexity:  ...

assertions: (assert(x))
	Rd_iter is an STL-like random access iterator (std::random_access_iterator_tag)
*/
	// TEMPLATE FUNCTION heap_sort
template<class Rda_iter, class Less 
		= std::less<std::iterator_traits<Rda_iter>::value_type>>
	void heap_sort(Rda_iter first, Rda_iter last, Less cmp = Less())
	{   // order [_First, _Last)
	typedef std::is_base_of<std::random_access_iterator_tag,typename Rda_iter::iterator_category> ass_ty;
	assert(typename ass_ty::value);

	std::make_heap(first, last, cmp);
	std::sort_heap(first, last, cmp);
	}

};//end: namespace
