/*
 * Copyright (C) 2019 Miloš Stojanović
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef MC_STR_H
#define MC_STR_H

#include <cstdlib>
#include <cstring>
#include <iostream>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

namespace mc {

class str {
private:
	char *start_ptr = nullptr;
	char *end_ptr = nullptr;
	char *cap_ptr = nullptr;
public:
	str() = default;
	str(const char *c_str)
	{
		if (c_str) {
			const size_t len = std::strlen(c_str);
			start_ptr = (char*) std::malloc(len + 1);
			if (start_ptr) {
				strcpy(start_ptr, c_str);
				cap_ptr = start_ptr + len;
				end_ptr = cap_ptr;
			}
		}
	}
	// Copy constructor
	str(const str& str_2) : str(str_2.start_ptr) {}
	// Move constructor
	str(str&& str_2)
	{
		start_ptr = str_2.start_ptr;
		end_ptr = str_2.end_ptr;
		cap_ptr = str_2.cap_ptr;
		str_2.start_ptr = nullptr;
		str_2.end_ptr = nullptr;
		str_2.cap_ptr = nullptr;
	}

	~str()
	{
		std::free(start_ptr);
		start_ptr = nullptr;
		end_ptr = nullptr;
		cap_ptr = nullptr;
	}

	// Copy assignment
	str& operator=(const str& str_2)
	{
		if (this != &str_2) {
			const size_t cap = this->cap();
			const size_t len_2 = str_2.len();
			if (len_2 > cap) {
				std::free(start_ptr);
				end_ptr = nullptr;
				const size_t new_len = MAX(len_2, 2*cap);
				start_ptr = (char*) std::malloc(new_len + 1);
				cap_ptr = start_ptr + new_len;
			}

			if (start_ptr) {
				std::strcpy(start_ptr, str_2.start_ptr);
				end_ptr = start_ptr + len_2;
			}
		}
		return *this;
	}

	// Move assignment
	str& operator=(str&& str_2)
	{
		if (this != &str_2) {
			start_ptr = str_2.start_ptr;
			end_ptr = str_2.end_ptr;
			cap_ptr = str_2.cap_ptr;
			str_2.start_ptr = nullptr;
			str_2.end_ptr = nullptr;
			str_2.cap_ptr = nullptr;
		}
		return *this;
	}

	size_t len() const { return end_ptr - start_ptr; }
	size_t size() const { return len(); }

	size_t cap() const { return cap_ptr - start_ptr; }

	char *begin() const { return start_ptr; }
	char *end() const { return end_ptr; }

	// Constrained array subscript operators
	char& operator[](size_t index)
	{
		return start_ptr[MIN(index, len())];
	}
	const char& operator[](size_t index) const
	{
		return start_ptr[MIN(index, len())];
	}

	// Relational operators
	friend bool operator<=(const str& left_str, const str& right_str)
	{
		const size_t left_str_len = left_str.len();
		const size_t right_str_len = right_str.len();
		const size_t shorter_len = MIN(left_str_len, right_str_len);
		for (size_t i = 0; i < shorter_len; i++) {
			if (left_str[i] < right_str[i]) {
				return true;
			} else if (left_str[i] > right_str[i]) {
				return false;
			}
		}
		return left_str_len <= right_str_len;
	}
	friend bool operator>(const str& left_str, const str& right_str)
	{
		return !(left_str <= right_str);
	}
	friend bool operator<(const str& left_str, const str& right_str)
	{
		return right_str > left_str;
	}
	friend bool operator>=(const str& left_str, const str& right_str)
	{
		return !(left_str < right_str);
	}
	friend bool operator==(const str& left_str, const str& right_str)
	{
		const size_t left_len = left_str.len();
		const size_t right_len = right_str.len();
		if (left_len != right_len) {
			return false;
		}

		for (size_t i = 0; i < left_len; i++) {
			if (left_str[i] != right_str[i]) {
				return false;
			}
		}
		return true;
	}
	friend bool operator!=(const str& left_str, const str& right_str)
	{
		return !(left_str == right_str);
	}
};

std::ostream& operator<<(std::ostream& os, const str& str)
{
	os.write(str.begin(), str.len());
	return os;
}

std::istream& operator>>(std::istream& is, str& str)
{
	// TODO
	return is;
}

} // namespace mc

#endif
