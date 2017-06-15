# Utilities
A few useful algorithms and classes implemented in C++.

## Serial Port Streambuffer
Header only. C++11 required.

Platform: Microsoft Windows

File: serialport_streambuf.h
```
auto buf = unchecked_serialport_streambuf(L"COM3");  // open serial port
auto s = std::basic_iostream<byte>(&buf);  // pass buffer to an io-stream
s << byte('E') << std::flush;  // send 'E' over the wire
```
 
This file contains a generic streambuffer, which is used to perform serialport communication.
The streambuffers are designed to be fully compatible with the standart template library (STL).

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
