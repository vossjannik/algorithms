/* Author: Jannik Voss
 *
 * Example Usage:
 *     auto buf = unchecked_serialport_streambuf(L"COM3");  // open serial port
 *     auto s = std::basic_iostream<byte>(&buf);  // pass buffer to an io-stream
 *     s << byte('E') << std::flush;  // send 'E' over the wire
 *
 * Description:
 *     this file contains a generic streambuffer, which is used to perform serialport communication.
 *     the streambuffers are designed to be fully compatible with the standart template library (STL)
 */

#pragma once

#include <string>
#include <vector>
#include <streambuf>
#include <functional>
#include <Windows.h>
#include <assert.h>
#include <array>
#include <thread>
#include <iostream>

typedef unsigned char byte; // 1 byte = 8 bits*/


#define SERIALPORT_DEFAULT_BAUDRATE 19200 // default baudrate for serialport communication

// nulltype is used as a placeholder-type to indicate that a
// template parameter is not used in a specific scenario
class nulltype {}; // placeholder type


/**
 * POLICY CLASSES
 *
 * these policy classes encapsulate certain tasks and represent
 * building blocks for the stream-buffer classes defined below.
 */

template<typename T, typename Traits = std::char_traits<T>>
	struct p_winapi_file_writer
	{ // function object: provide write function for file-handles
	void operator () (HANDLE serial_port, const T* buffer, std::size_t num)
		{
		assert(buffer!=nullptr); // make sure that the buffer exists
		std::size_t num_send = 0; // number of bytes which where already send
		while(num_send < num) // all bytes send?
			{ // send one byte at a time
			DWORD num_written = 0; // output variable for WriteFile
			if(WriteFile(serial_port, &buffer[num_send], num, &num_written, NULL) != 0) // try to write a few bytes
				num_send += num_written; // update status
			else // WriteFile failed
				throw new std::runtime_error("unable to send data");
			}
		}
	};

template<template<class,class> typename Reader, typename T, typename Traits = std::char_traits<T>>
	struct p_winapi_file_writer_with_echo
	{ // function object: provide write function for file-handles
	void operator () (HANDLE serial_port, const T* buffer, std::size_t num)
		{
		assert(buffer!=nullptr); // make sure that the buffer exists
		std::size_t num_send = 0; // number of bytes which where already send
		while(num_send < num) // all bytes send?
			{ // send one byte at a time
			DWORD num_written = 0; // output variable for WriteFile
			if(WriteFile(serial_port, &buffer[num_send], 1/*send only one at a time*/, &num_written, NULL) == FALSE) // try to write a few bytes
				throw new std::runtime_error("unable to send data"); // WriteFile failed

			// wait for echo
			T required_echo = (buffer[num_send]&0b10000000)>0 ? ~(buffer[num_send]) : buffer[num_send];
			Reader<T,Traits> r;
			auto start_ts = std::chrono::steady_clock::now();
			while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_ts) < std::chrono::milliseconds(1000))
				{ // "infinite loop". timeout after 1 second
				DWORD num_read;
				T read;
				if(r(serial_port, &read, 1, &num_read) && num_read==1 && read == required_echo)
					{ // received valid echo
					num_send += num_written; // update number of bytes send
					break;
					}
				}
			}
		}
	};

template<typename T, typename Traits = std::char_traits<T>>
	struct p_winapi_file_reader
	{ // function object: provide read function for file-handles
	bool operator () (HANDLE serial_port, T* buffer, std::size_t num, DWORD* num_read)
		{
		// receive new data and store it in the buffer
		return (ReadFile(serial_port, buffer, num, num_read, NULL) != FALSE);
		}
	};

template<typename T, typename Traits = std::char_traits<T>>
	struct p_winapi_file_deleter
	{ // function object: provide custom deleter for file-handles
	void operator () (HANDLE* serial_port)
		{
		if(*serial_port != INVALID_HANDLE_VALUE)
			{ // if handle is valid
		   CloseHandle(*serial_port); // close the handle
			*serial_port = INVALID_HANDLE_VALUE; // invalidate handle
			}
		}
	};

template<typename T, typename Traits = std::char_traits<T>>
	struct p_serialport_initializer
	{ // provide initializer for serial ports
	// please notice that this policy isn't a function object!
	// instead it has the ability to store data and thus should be stored as a member of the streambuffer
	p_serialport_initializer(HANDLE* serial_port, LPCWSTR port_name, std::size_t baud_rate=SERIALPORT_DEFAULT_BAUDRATE)
		{
		// create serialport handle
		*serial_port = CreateFile(port_name, // connect to port 3
										 GENERIC_READ | GENERIC_WRITE, // open for read and write access
										 0,
										 NULL,
										 OPEN_EXISTING, // validate existance
										 0,
										 NULL);
		if(*serial_port == INVALID_HANDLE_VALUE) // failure?
			 throw new std::runtime_error("invalid handle");

		DCB config; // configuration struct
		if(GetCommState(*serial_port, &config)) // retrive current configuration
			{
			config.BaudRate = baud_rate;		// 19200 signals per second
			config.ByteSize = sizeof(T)*8;	// 8bits per byte
			config.fBinary = TRUE;
			config.fParity = TRUE;
			config.Parity = NOPARITY;		// disable parity
			config.StopBits = ONESTOPBIT; // use one stop bit
			}
		else // failure
			throw new std::runtime_error("unable to retrieve configuration");

		if(!SetCommState(*serial_port, &config)) // set configuration
			throw new std::runtime_error("unable to change the configuration");

		COMMTIMEOUTS timeouts; // timeouts struct
		if(GetCommTimeouts(*serial_port, &timeouts)) // retrieve current timeouts
			{
			// set timeouts for read and write operations
			// after one second it can be assumed that there is a problem with the connection
			timeouts.ReadIntervalTimeout         = 1000;
			timeouts.ReadTotalTimeoutMultiplier  = 1000;
			timeouts.ReadTotalTimeoutConstant    = 1000;
			timeouts.WriteTotalTimeoutConstant   = 1000;
			timeouts.WriteTotalTimeoutMultiplier = 1000;
			}
		else // failure
			throw new std::runtime_error("unable to retrieve timeouts");

		if(!SetCommTimeouts(*serial_port, &timeouts)) // set timeouts
			throw new std::runtime_error("unable to change timeouts");
		}
	};

/**
 * THE INTERESTING STUFF
 *
 * this section includes the generic streambuffer class
 */

template<typename T, typename Traits, // character type and character traits
			typename Handle,			// type of the handle
			typename Writer,			// writing policy
			typename Reader,			// reading policy
			typename Deleter,			// deleting policy
			typename Initializer>   // initalizing policy
	class basic_serialport_streambuf
	: public std::basic_streambuf<T, Traits>
	{
public:
	// notice: this streambuffer class doesn't use an input buffer and thus support putback

	typedef T char_type;
	typedef Traits traits_type;
	typedef typename Traits::int_type int_type;
	typedef std::streampos pos_type;
	typedef std::streamoff off_type;

	// disable copy construction and copy assignment
	basic_serialport_streambuf(const basic_serialport_streambuf&) = delete;
	basic_serialport_streambuf& operator = (const basic_serialport_streambuf&) = delete;

	explicit basic_serialport_streambuf(
			LPCWSTR port_name, std::size_t baud_rate=SERIALPORT_DEFAULT_BAUDRATE, std::size_t buf_size = 128)
		: buffer_(buf_size+1)
		, init_(&serial_port_, port_name, baud_rate)
		{ // construct from parameters (create serial port RAII)
		auto base = &buffer_.front();
		// adjust pointers
		// do not use the last element. this makes the overflow function easier
		this->setp(base, base + buffer_.size() -1);

		auto inbase = &readbuf_.front() + readbuf_.size(); // retrieve pointer to the end of the input buffer
		this->setg(inbase, inbase,  inbase); // don't use an input buffer
		}

	~basic_serialport_streambuf()
		{ // destruct
		Deleter d;
		d(&serial_port_); // pass handle to the deleter
		}

protected:
	int_type overflow(int_type ch) override
		{ // called if there are no empty slots in the buffer
		if(ch != traits_type::eof())
			{ // valid character?
			assert(std::less_equal<char_type*>()(this->pptr(), this->epptr()) && "logic error: incorrect pointers");
			// store ch in the buffer
			*(this->pptr()) = static_cast<char_type>(ch);
			this->pbump(1);
			return flush_buffer() ? ch : traits_type::eof(); // success
			}
		return traits_type::eof(); // error
		}

	int sync() override
		{ // flush buffer
		// -1 indicates error
		return flush_buffer() ? 0/*success*/ : -1/*error*/;
		}

	int_type underflow() override
		{
		DWORD num_read = 0; // number of bytes read
		Reader r;
		std::lock_guard<decltype(m_)> l(m_); // lock mutex
		if(r(serial_port_, &readbuf_[0], 1, &num_read) && num_read > 0) // use the reader policy to read new data
			return static_cast<int_type>(readbuf_[0]); // success
		return Traits::eof(); // failure
		}

	std::streamsize showmanyc() override
		{
		return 0; // no information available
		}

	int_type uflow() override
		{
		return readbuf_[0]; // return stored input
		}

private:
	bool flush_buffer()
		{ // send buffered data to the
		std::ptrdiff_t num = pptr() - pbase(); // number of elements in the buffer
		std::ptrdiff_t num_send = 0; // number of bytes send
		try{
			Writer w;
			std::lock_guard<decltype(m_)> l(m_); // lock mutex
			w(serial_port_, &buffer_[num_send], num); // use the writer policy to send the data
		}catch(...){
			return false; // write failed. data remains in buffer
		}
      this->pbump(-num); // the buffer is now empty again
		return true;
		}

	Handle serial_port_;
	std::vector<char_type> buffer_; // output buffer

	// input buffer.
	// notice: the streambuffer doesn't fully support input buffers.
	//			  this only exists to make the implementation of 'uflow' simpler
	std::array<char_type, 1> readbuf_;

	Initializer init_; // notice!! Initializer may hold data

	mutable std::mutex m_; // control reads and writes
	};


// default stream-buffer: use the serialport to connect to the hardware
typedef basic_serialport_streambuf<byte, std::char_traits<byte>, HANDLE,
		p_winapi_file_writer_with_echo<p_winapi_file_reader, byte>,	// writing policy
		p_winapi_file_reader<byte>,                                 // reading policy
		p_winapi_file_deleter<byte>,                                // deleting policy
		p_serialport_initializer<byte>>                             // initalizing policy
	serialport_streambuf;


// default stream-buffer: use the serialport to connect to the hardware
typedef basic_serialport_streambuf<byte, std::char_traits<byte>, HANDLE,
		p_winapi_file_writer<byte>,         // writing policy
		p_winapi_file_reader<byte>,         // reading policy
		p_winapi_file_deleter<byte>,        // deleting policy
		p_serialport_initializer<byte>>     // initalizing policy
	unchecked_serialport_streambuf;

