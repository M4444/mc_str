/*
 * Copyright (C) 2019 Miloš Stojanović
 *
 * SPDX-License-Identifier: MIT
 */

#include <array>
#include <cassert>
#include <cstring>
#include <sstream>

#ifndef TEST_MC_STR
#define TEST_MC_STR
#endif
#include "str.h"

int main()
{
	// Default constructor
	{
		mc::str s;

		assert(s.start_ptr == nullptr);
		assert(s.end_ptr == nullptr);
		assert(s.cap_ptr == nullptr);
	}

	// Constructor with const char * as an argument
	{
		const char *c_str = "1234";

		mc::str s(c_str);

		assert(std::memcmp(s.start_ptr, c_str, strlen(c_str)+1) == 0);
	}

	// Copy constructor
	{
		mc::str s1("1234");

		mc::str s(s1);

		assert(std::memcmp(s.start_ptr, s.start_ptr,
				   (s1.end_ptr - s1.start_ptr + 1)) == 0);
	}

	// Move constructor
	{
		mc::str s1("1234");
		mc::str s2;
		s2.start_ptr = s1.start_ptr;
		s2.end_ptr = s1.end_ptr;
		s2.cap_ptr = s1.cap_ptr;

		mc::str s(std::move(s1));

		assert(s1.start_ptr == nullptr);
		assert(s1.end_ptr == nullptr);
		assert(s1.cap_ptr == nullptr);
		assert(std::memcmp(s.start_ptr, s2.start_ptr,
				   (s2.end_ptr - s2.start_ptr + 1)) == 0);
		s2.start_ptr = nullptr;
		s2.end_ptr = nullptr;
		s2.cap_ptr = nullptr;
	}

	// Destructor
	{
		mc::str s("1234");

		s.~str();
	}

	// Copy assignment
	{
		mc::str s;
		// self assignment
		s = s;
		assert((s.cap_ptr - s.start_ptr) == 0);
		// expand cap to fit the new str
		mc::str s1("1234");

		s = s1;

		assert((s.cap_ptr - s.start_ptr) ==
		       (s1.cap_ptr - s1.start_ptr));
		assert(std::memcmp(s.start_ptr, s1.start_ptr,
				   (s1.end_ptr - s1.start_ptr + 1)) == 0);
		// expand cap by doubling it
		mc::str s2("123456");
		size_t old_cap2 = s.cap();

		s = s2;

		assert((size_t)(s.cap_ptr - s.start_ptr) == 2*old_cap2);
		assert(std::memcmp(s.start_ptr, s2.start_ptr,
				   (s2.end_ptr - s2.start_ptr + 1)) == 0);
		// don't change cap
		mc::str s3("12");
		size_t old_cap3 = s.cap();

		s = s3;

		assert((size_t)(s.cap_ptr - s.start_ptr) == old_cap3);
		assert(std::memcmp(s.start_ptr, s3.start_ptr,
				   (s3.end_ptr - s3.start_ptr + 1)) == 0);
	}

	// Move assignment
	{
		mc::str s;
		mc::str s1("1234");
		mc::str s2;
		s2.start_ptr = s1.start_ptr;
		s2.end_ptr = s1.end_ptr;
		s2.cap_ptr = s1.cap_ptr;

		// self assignment
		s = std::move(s);
		s = std::move(s1);

		assert(s1.start_ptr == nullptr);
		assert(s1.end_ptr == nullptr);
		assert(s1.cap_ptr == nullptr);
		assert(std::memcmp(s.start_ptr, s2.start_ptr,
				   (s2.end_ptr - s2.start_ptr + 1)) == 0);
		s2.start_ptr = nullptr;
		s2.end_ptr = nullptr;
		s2.cap_ptr = nullptr;
	}

	// len()/size()
	{
		mc::str s("1234");
		assert(s.len() == (size_t)(s.end_ptr - s.start_ptr));
		assert(s.size() == s.len());
	}

	// cap()
	{
		mc::str s("1234");
		assert(s.cap() == (size_t)(s.cap_ptr - s.start_ptr));
	}

	// begin() and end()
	{
		mc::str s("1234");
		assert(s.begin() == s.start_ptr);
		assert(s.end() == s.end_ptr);
	}

	// Array subscript operators
	{
		// non-const
		{
			const char *c_str = "1234";
			size_t c_str_len = std::strlen(c_str);
			mc::str s(c_str);
			size_t i;
			for (i = 0; i < std::strlen(c_str)+1; i++) {
				assert(s[i] == c_str[i]);
			}
			assert(s[i] == c_str[MIN(i, c_str_len)]);
			assert(&s[i] == s.end());
		}
		// const
		{
			const char *c_str = "1234";
			size_t c_str_len = std::strlen(c_str);
			const mc::str s(c_str);
			size_t i;
			for (i = 0; i < std::strlen(c_str)+1; i++) {
				assert(s[i] == c_str[i]);
			}
			assert(s[i] == c_str[MIN(i, c_str_len)]);
			assert(&s[i] == s.end());
		}
	}
	// Relational operators
	{
		const std::array<mc::str, 8> str_list {
			"A", "Aaa",
			"B", "Bbb",
			"a", "aaa",
			"b", "bbb"
		};
		// A <= A <= Aaa <= Aaa <= B <= B <= Bbb <= Bbb <=
		// a <= a <= aaa <= aaa <= b <= b <= bbb <= bbb
		for (size_t i = 0; i < str_list.size()-1; i++) {
			for (size_t j = i; j < str_list.size(); j++) {
				assert(str_list[i] <= str_list[j]);
			}
		}
		// bbb > b > aaa > a > Bbb > B > Aaa > A
		for (size_t i = str_list.size()-1; i >= 1 ; i--) {
			for (size_t j = i-1; j--;) {
				assert(str_list[i] > str_list[j]);
			}
		}
		// A < Aaa < B < Bbb < a < aaa < b < bbb
		for (size_t i = 0; i < str_list.size()-1; i++) {
			for (size_t j = i+1; j < str_list.size(); j++) {
				assert(str_list[i] < str_list[j]);
			}
		}
		// bbb >= bbb >= b >= b >= aaa >= aaa >= a >= a >=
		// Bbb >= Bbb >= B >= B >= Aaa >= Aaa >= A >= A
		for (size_t i = str_list.size()-1; i >= 1 ; i--) {
			for (size_t j = i; j--;) {
				assert(str_list[i] >= str_list[j]);
			}
		}
		// A == A, Aaa == Aaa, B == B, Bbb == Bbb
		// a == a, aaa == aaa, b == b, bbb == bbb
		for (size_t i = 0; i < str_list.size(); i++) {
			assert(str_list[i] == str_list[i]);
		}
		// A != Aaa != B != Bbb != a != aaa != b != bbb
		for (size_t i = 0; i < str_list.size()-1; i++) {
			for (size_t j = i+1; j < str_list.size(); j++) {
				assert(str_list[i] != str_list[j]);
			}
		}
	}
	// Stream extraction and insertion
	{
		mc::str s = "1234";
		std::stringstream ss;
		std::stringstream ss1;
		int n;
		int n1;

		ss << s;
		ss1 << "1234";

		ss >> n;
		ss1 >> n1;

		assert(n == n1);

		// TODO: extraction
	}
}
