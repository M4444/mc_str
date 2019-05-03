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

	void free_and_reserve(size_t cap_request, bool free = true)
	{
		const size_t old_cap = cap();
		if (cap_request > old_cap) {
			const size_t old_len = len();
			const size_t new_cap = MAX(cap_request, 2*old_cap);
			if (free) {
				std::free(start_ptr);
				start_ptr = nullptr;
			}
			start_ptr = (char*) std::realloc(start_ptr, new_cap+1);
			if (start_ptr) {
				end_ptr = start_ptr + old_len;
				cap_ptr = start_ptr + new_cap;
			} else {
				end_ptr = nullptr;
				cap_ptr = nullptr;
			}
		}
	}

	void free_and_write(const char *c_str, const size_t c_str_len,
			    const size_t index = 0, bool free = true)
	{
		if (c_str) {
			free_and_reserve(index + c_str_len,
					 free ||
					 (index == 0 && c_str_len >= len()));

			char *index_ptr = start_ptr + MIN(index, len());
			if (free || index + c_str_len > len()) {
				std::strcpy(index_ptr, c_str);
				end_ptr = index_ptr + c_str_len;
			} else {
				std::memcpy(index_ptr, c_str, c_str_len);
			}
		}
	}
public:
	str() = default;
	str(const char *c_str)
	{
		free_and_write(c_str, std::strlen(c_str));
	}
	// Copy constructor
	str(const str& str_2)
	{
		free_and_write(str_2.start_ptr, str_2.len());
	}
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
			free_and_write(str_2.start_ptr, str_2.len());
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

	void reserve(size_t new_cap) {
		free_and_reserve(new_cap, false);
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

	void overwrite_at(const str& str_2, const size_t index)
	{
		free_and_write(str_2.start_ptr, str_2.len(), index, false);
	}

	str& append(const str& str_2)
	{
		overwrite_at(str_2, len());
		return *this;
	}

	str& operator+=(const str& str_2)
	{
		return append(str_2);
	}

	str operator+(const str& str_2)
	{
		return append(str_2);
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
