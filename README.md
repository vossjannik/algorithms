# Algorithms
A few useful algorithms implemented in C++.

## Directory iterator
Header only. C++11 not required.

Platform: Microsoft Windows

File: directory_iterator.h
```
for(auto it=directory_iterator(L"C:/"); it != directory_iterator(); ++it)
    std::wcout << it->c_str() << std::endl;
```

## Supported sort algorithms
Header only. C++11 not required.

File: algorithms_sort.h
  - bubble_sort
  - selection_sort
  - quick_sort
  - insertion_sort
  - shell_sort
  - comb_sort
  - heap_sort

## Supported crypto algorithms

### aes256
Header only. C++11 not required.

File: aes256_cipher.h

Rudimentary implementation by a non-professional.
