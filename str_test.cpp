/*
 * Copyright (C) 2019 Miloš Stojanović
 *
 * SPDX-License-Identifier: MIT
 */

#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

#include "str.h"

int main()
{
	// Default constructor
	{
		mc::str s;

		assert(s.begin() == nullptr);
		assert(s.end() == nullptr);
		assert(s.len() == 0);
		assert(s.cap() == 0);
	}

	// Constructor with char * as an argument
	{
		const char *c_str = "1234";

		mc::str s(c_str);

		assert(std::memcmp(s.begin(), c_str, std::strlen(c_str)+1) == 0);
	}

	// Constructor with char as an argument
	{
		const char c = '1';

		mc::str s(c);

		assert(std::memcmp(s.begin(), &c, 1) == 0);
	}

	// Copy constructor
	{
		mc::str s1("1234");

		mc::str s(s1);

		assert(std::memcmp(s.begin(), s1.begin(), s1.len()+1) == 0);
	}

	// Move constructor
	{
		mc::str s1("1234");
		mc::str s2(s1);

		mc::str s(std::move(s1));

		assert(s1.begin() == nullptr);
		assert(s1.end() == nullptr);
		assert(std::memcmp(s.begin(), s2.begin(), s2.len()+1) == 0);
	}

	// Copy assignment,
	// cap()
	{
		mc::str s;
		// self assignment
		s = s;
		assert(s.cap() == 0);
		// expand cap to fit the new str
		mc::str s1("1234");
		size_t old_cap1 = s.cap();

		s = s1;

		assert(s.cap() > old_cap1);
		assert(s.cap() >= s1.cap());
		assert(std::memcmp(s.begin(), s1.begin(), s1.len()+1) == 0);
		// expand cap
		mc::str s2("123456");
		size_t old_cap2 = s.cap();

		s = s2;

		assert(s.cap() > old_cap2);
		assert(s.cap() >= s2.cap());
		assert(std::memcmp(s.begin(), s2.begin(), s2.len()+1) == 0);
		// don't change cap
		mc::str s3("12");
		size_t old_cap3 = s.cap();

		s = s3;

		assert(s.cap() == old_cap3);
		assert(s.cap() >= s3.cap());
		assert(std::memcmp(s.begin(), s3.begin(), s3.len()+1) == 0);
	}

	// Move assignment
	{
		mc::str s;
		// self assignment
		s = std::move(s);

		mc::str s1("1234");
		mc::str s2(s1);

		s = std::move(s1);

		assert(s1.begin() == nullptr);
		assert(s1.end() == nullptr);
		assert(std::memcmp(s.begin(), s2.begin(), s2.len()+1) == 0);
	}

	// reserve()
	{
		mc::str s;
		assert(s.cap() == 0);

		size_t new_cap = 4;
		s.reserve(new_cap);
		assert(s.cap() >= new_cap);
	}

	// len()/size()
	{
		const char *c_str = "1234";
		mc::str s(c_str);

		assert(s.len() == std::strlen(c_str));
		assert(s.size() == s.len());
	}

	// begin(), end()
	{
		mc::str s("1234");
		const char *c = s.begin();

		for (size_t i = 0; i < s.len()+1; i++) {
			assert(*c++ == s[i]);
		}
		assert(--c == s.end());
		assert((size_t)(s.end() - s.begin()) == s.len());
	}

	// Array subscript operators
	{
		// non-const
		{
			const char *c_str = "1234";
			mc::str s(c_str);
			size_t i;
			for (i = 0; i < std::strlen(c_str)+1; i++) {
				assert(s[i] == c_str[i]);
			}
			assert(s[i] == c_str[MIN(i, std::strlen(c_str))]);
			assert(&s[i] == s.end());
		}
		// const
		{
			const char *c_str = "1234";
			const mc::str s(c_str);
			size_t i;
			for (i = 0; i < std::strlen(c_str)+1; i++) {
				assert(s[i] == c_str[i]);
			}
			assert(s[i] == c_str[MIN(i, std::strlen(c_str))]);
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

	// overwrite_at()
	{
		mc::str s;

		s.overwrite_at("1234", 0);
		assert(s == "1234");

		s.overwrite_at("56", 2);
		assert(s == "1256");

		s.overwrite_at("78", 4);
		assert(s == "125678");

		s.overwrite_at("90", 100);
		assert(s == "12567890");
	}

	// append()/operator+=()
	{
		// append()
		{
			mc::str s;
			mc::str s1("56");
			mc::str s2("78");

			s.append("1234");
			assert(s == "1234");

			s.append(s1).append(s2);
			assert(s == "12345678");
		}
		// operator+=()
		{
			mc::str s;
			mc::str s1("56");
			mc::str s2("78");

			s += "1234";
			assert(s == "1234");

			s += s1 += s2;
			assert(s == "12345678");
		}
	}

	// operator+()
	{
		mc::str s;
		mc::str s1("12");
		mc::str s2("34");

		s = s1 + s2;
		assert(s == "1234");
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

	std::cout << "[\e[32m PASS \e[39m]" << '\n';
}
