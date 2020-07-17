/**********************************************************************************
 * MIT License                                                                    *
 *                                                                                *
 * Copyright (c) 2020 Marco "X39" Silipo                                          *
 *                                                                                *
 * Permission is hereby granted, free of charge, to any person obtaining a copy   *
 * of this software and associated documentation files (the "Software"), to deal  *
 * in the Software without restriction, including without limitation the rights   *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
 * copies of the Software, and to permit persons to whom the Software is          *
 * furnished to do so, subject to the following conditions:                       *
 *                                                                                *
 * The above copyright notice and this permission notice shall be included in all *
 * copies or substantial portions of the Software.                                *
 *                                                                                *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  *
 * SOFTWARE.                                                                      *
 **********************************************************************************/

#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include <algorithm>
#include <variant>
#include <initializer_list>

namespace sqf
{
	class value
	{
	private:
		enum class value_type
		{
			Nil,
			Array,
			Boolean,
			Scalar,
			String
		};
		value_type m_type;
		std::variant<std::vector<sqf::value>, std::string, bool, float> m_variant;
	public:

		value() : m_type(value_type::Nil) {}
		value(double scalarD) : m_type(value_type::Scalar)
		{
			m_variant = (float)scalarD;
		}
		value(float scalar) : m_type(value_type::Scalar), m_variant(scalar) {}
		value(int scalar) : m_type(value_type::Scalar), m_variant((float)scalar) {}
		value(bool boolean) : m_type(value_type::Boolean), m_variant(boolean) {}
		value(const char* c_str) : m_type(value_type::String), m_variant(std::string(c_str)) {}
		value(std::string string) : m_type(value_type::String), m_variant(string) {}
		value(std::vector<value> array) : m_type(value_type::Array), m_variant(array) {}
		value(std::initializer_list<value> initializer) : m_type(value_type::Array), m_variant(std::vector(initializer.begin(), initializer.end())) {}

		template<typename T> inline T as() { static_assert(false, "Invalid input type"); }
		template<> inline float as() { if (m_type != value_type::Scalar) { m_variant = float{}; } return std::get<float>(m_variant); }
		template<> inline bool as() { if (m_type != value_type::Boolean) { m_variant = bool{}; } return std::get<bool>(m_variant); }
		template<> inline std::string as() { if (m_type != value_type::String) { m_variant = std::string{}; } return std::get<std::string>(m_variant); }
		template<> inline std::vector<value>& as() { if (m_type != value_type::Array) { m_variant = std::vector<sqf::value>{}; } return std::get<std::vector<sqf::value>>(m_variant); }

		template<typename T> inline T as() const { static_assert(false, "Invalid input type"); }
		template<> inline float as() const { if (m_type == value_type::Scalar) { return std::get<float>(m_variant); } return 0; }
		template<> inline bool as() const { if (m_type == value_type::Boolean) { return std::get<bool>(m_variant); } return false; }
		template<> inline std::string as() const { if (m_type == value_type::String) { return std::get<std::string>(m_variant); } return {}; }
		template<> inline std::vector<value> as() const { if (m_type == value_type::Array) { return std::get<std::vector<sqf::value>>(m_variant); } return {}; }

        // Tests two sqf::value's for equality.
        // If they are arrays, comparison is executed deep.
        // Comparison is performed case-sensitive.
        bool equals(const value& other) const
        {
            if (other.m_type != m_type) { return false; }
            switch (m_type)
            {
            case value_type::Nil: return true;
            case value_type::Boolean: return as<bool>() == other.as<bool>();
            case value_type::Scalar: return as<float>() == other.as<float>();
            case value_type::String: return as<std::string>() == other.as<std::string>();
            case value_type::Array:
                auto& a = std::get<std::vector<value>>(m_variant);
                auto& b = std::get<std::vector<value>>(other.m_variant);
                return std::equal(a.begin(), a.end(), b.begin(), b.end());
            }
            return false;
        }
        // Tests two sqf::value's for equality.
        // If they are arrays, comparison is executed deep.
        // Comparison is performed case-invariant.
        bool equals_invariant(const value& other) const
        {
            if (other.m_type != m_type) { return false; }
            switch (m_type)
            {
            case value_type::Nil: return true;
            case value_type::Boolean: return as<bool>() == other.as<bool>();
            case value_type::Scalar: return as<float>() == other.as<float>();
            case value_type::String:
            {
                auto& a = std::get<std::string>(m_variant);
                auto& b = std::get<std::string>(other.m_variant);

                return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char l, char r) { return std::tolower(l) == std::tolower(r); });
            }
            case value_type::Array:
            {
                auto& a = std::get<std::vector<value>>(m_variant);
                auto& b = std::get<std::vector<value>>(other.m_variant);

                return std::equal(a.begin(), a.end(), b.begin(), b.end());
            }
            }
            return false;
        }
        bool operator!=(const std::vector<value>& other) const { return !(*this == other); }
        bool operator==(const std::vector<value>& other) const
        {
            if (m_type != value_type::Array)
            {
                return false;
            }
            auto& a = std::get<std::vector<value>>(m_variant);
            return std::equal(a.begin(), a.end(), other.begin(), other.end());
        }
        bool operator!=(const value& other) const { return !equals(other); }
        bool operator==(const value& other) const { return equals(other); }
        bool operator!=(const std::string& other) const { return !(*this == other); }
        bool operator==(const std::string& other) const { if (m_type != value_type::String) { return false; } return other == as<std::string>(); }
        bool operator!=(const char* other) const { return *this != std::string(other); }
        bool operator==(const char* other) const { return *this == std::string(other); }
        bool operator!=(float other) const { return !(*this == other); }
        bool operator==(float other) const { if (m_type != value_type::Scalar) { return false; } return other == as<float>(); }
        bool operator<=(float other) const { if (m_type != value_type::Scalar) { return false; } return other <= as<float>(); }
        bool operator< (float other) const { if (m_type != value_type::Scalar) { return false; } return other <  as<float>(); }
        bool operator>=(float other) const { if (m_type != value_type::Scalar) { return false; } return other >= as<float>(); }
        bool operator> (float other) const { if (m_type != value_type::Scalar) { return false; } return other >  as<float>(); }
        bool operator==(double other) const { if (m_type != value_type::Scalar) { return false; } return ((float)other) == as<float>(); }
        bool operator<=(double other) const { if (m_type != value_type::Scalar) { return false; } return ((float)other) <= as<float>(); }
        bool operator< (double other) const { if (m_type != value_type::Scalar) { return false; } return ((float)other) <  as<float>(); }
        bool operator>=(double other) const { if (m_type != value_type::Scalar) { return false; } return ((float)other) >= as<float>(); }
        bool operator> (double other) const { if (m_type != value_type::Scalar) { return false; } return ((float)other) >  as<float>(); }
        // bool operator!=(bool other) const { return !(*this == other); }
        // bool operator==(bool other) const { if (m_type != value_type::Boolean) { return false; } return other == as<bool>(); }
        bool operator&&(bool other) const { if (m_type != value_type::Boolean) { return false; } return other && as<bool>(); }
        bool operator||(bool other) const { if (m_type != value_type::Boolean) { return false; } return other || as<bool>(); }

		explicit operator float() { return as<float>(); }
		explicit operator bool() { return as<bool>(); }
		explicit operator std::string() { return as<std::string>(); }

		explicit operator float() const { return as<float>(); }
		explicit operator bool() const { return as<bool>(); }
		explicit operator std::string() const { return as<std::string>(); }
		explicit operator std::vector<value>() const { return as<std::vector<value>>(); }

        // Checks if this sqf::value is an array
		bool is_array() const { return m_type == value_type::Array; }
        // Checks if this sqf::value is a number
		bool is_scalar() const { return m_type == value_type::Scalar; }
        // Checks if this sqf::value is a boolean
		bool is_boolean() const { return m_type == value_type::Boolean; }
        // Checks if this sqf::value is a string
		bool is_string() const { return m_type == value_type::String; }
        // Checks if this sqf::value is nil
		bool is_nil() const { return m_type == value_type::Nil; }

        // Parses SQF-Value-String into a valid sqf::value
        static value parse(std::string_view view)
        {
            auto begin = view.begin();
            auto end = view.end();
            auto r = parse_(view, begin, end);
            return r;
        }

        // Transforms value into valid SQF-Value-String
        std::string to_string() const
        {
            switch (m_type)
            {
            case value_type::Nil:
                return "nil";
            case value_type::Array:
            { // ToDo: Replace string-stream with something more efficient
                std::stringstream sstream;
                sstream << '[';
                bool flag = false;
                for (auto& it : std::get<std::vector<value>>(m_variant))
                {
                    if (flag)
                    {
                        sstream << ',';
                    }
                    sstream << it.to_string();
                    flag = true;
                }
                sstream << ']';
                return sstream.str();
            }
            case value_type::Boolean:
                return std::get<bool>(m_variant) ? "true" : "false";
            case value_type::Scalar:
            { // ToDo: Replace string-stream with something more efficient
                std::stringstream sstream;
                sstream << std::get<float>(m_variant);
                return sstream.str();
            }
            case value_type::String:
            {
                size_t quotes = std::count(std::get<std::string>(m_variant).begin(), std::get<std::string>(m_variant).end(), '"');
                std::string out;
                out.reserve(std::get<std::string>(m_variant).size() + quotes + 2);
                out.append("\"");
                for (auto c : std::get<std::string>(m_variant))
                {
                    out.append(&c, &c + 1);
                    if (c == '"')
                    {
                        out.append("\"");
                    }
                }
                out.append("\"");
                return { out };
            }
            default:
                return {};
            }
        }
    private:
        static value parse_(std::string_view& view, std::string_view::const_iterator& begin, std::string_view::const_iterator& end)
        {
        parse_start:
            switch (*begin)
            {
            case '[':
                return parse_array(view, begin, end);
            case '"':
                return parse_string(begin, end);
            case 't':
            case 'f':
                return parse_boolean(begin, end);
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '-':
            case '+':
            case '.':
                return parse_scalar(view, begin, end);
            default:
                ++begin;
                if (begin != end) { goto parse_start; }
                break;
            }
        }
        static value parse_array(std::string_view& view, std::string_view::const_iterator& begin, std::string_view::const_iterator& end)
        {
            ++begin; // Skip initial [
            std::vector<value> values;
        parse_start:
            switch (*begin)
            {
            case ']':
                ++begin;
                return values;
            default:
                values.emplace_back(parse_(view, begin, end));
                if (begin != end) { goto parse_start; }
                break;
            }
        }
        static value parse_string(std::string_view::const_iterator& begin, std::string_view::const_iterator& end)
        {
            // start-char
            char c = *begin;

			// find end
			std::string_view::const_iterator copy;
			size_t quotes = 0;
			for (copy = begin + 1; copy != end; copy++)
			{
				if (*copy == c)
				{
					copy++;
					if (copy != end && *copy == c)
					{
						quotes++;
						continue;
					}
					else
					{
						break;
					}
				}
			}
			// create string
			auto len = copy - begin - 2;
			std::string target;
			target.reserve(len - quotes);
			for (begin++; begin != end; begin++)
			{
				char cur = *begin;
				if (*begin == c)
				{
					begin++;
					if (begin != end && *begin == c)
					{
						target.append(&cur, &cur + 1);
						continue;
					}
					else
					{
						break;
					}
				}
				else
				{
					target.append(&cur, &cur + 1);
				}
			}
			return target;
		}
		static value parse_boolean(std::string_view::const_iterator& begin, std::string_view::const_iterator& end)
		{
			char c = *begin;
			if (c == 't' || c == 'T')
			{ // true
				begin += 4;
				// begin++; /* t */ if (begin == end) { return true; }
				// begin++; /* r */ if (begin == end) { return true; }
				// begin++; /* u */ if (begin == end) { return true; }
				// begin++; /* e */ if (begin == end) { return true; }
				return true;
			}
			else
			{ // false
				begin += 5;
				// begin++; /* f */ if (begin == end) { return false; }
				// begin++; /* a */ if (begin == end) { return false; }
				// begin++; /* l */ if (begin == end) { return false; }
				// begin++; /* s */ if (begin == end) { return false; }
				// begin++; /* e */ if (begin == end) { return false; }
				return false;
			}
		}
		static value parse_scalar(std::string_view& view, std::string_view::const_iterator& begin, std::string_view::const_iterator& end)
		{
			size_t size;
			auto f = std::stof(view.substr(begin - view.begin()).data(), &size);
			begin += size;
			return f;
		}

	};
}