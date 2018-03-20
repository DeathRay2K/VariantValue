#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <iterator>
#include <algorithm>
class VariantValue
{
	//Global method helpers
	static std::string str_replace(const std::string& search, const std::string& replace, std::string subject)
	{
		std::string::size_type current = 0;
		while ((current = subject.find(search, current)) != std::string::npos)
		{
			subject.replace(current, search.size(), replace);
			current += replace.size();
		}
		return subject;
	}
	static VariantValue parse_recursive_variant(const std::string& definition, int offset, int& next);
	static VariantValue parse_recursive_json(const std::string& definition, int offset, int& next);
	static VariantValue parse_recursive_json_test(const std::string& definition, int offset, int& next);



public:
	//Global definitions
	enum TypeName
	{
		TYPE_UNDEFINED,
		TYPE_BOOL,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_STRING,
		TYPE_VARIANT_MAP,
	};
	enum FormatName
	{
		FORMAT_VARIANT,
		FORMAT_JSON,
		FORMAT_TEST
	};
	typedef std::map<VariantValue, VariantValue> variMap;
	typedef variMap::iterator iterator;
	typedef variMap::const_iterator const_iterator;

	static VariantValue parse(const std::string& definition, FormatName format = FORMAT_VARIANT);
	static VariantValue sort(const VariantValue& set);
	static VariantValue sort(const VariantValue& set, std::function<bool(const VariantValue& a, const VariantValue& b)> comparison);

private:
	std::string print_recursive_variant() const;
	std::string print_recursive_variant_pretty(unsigned tabs = 0) const;
	std::string print_recursive_json() const;
	std::string print_recursive_json_pretty(unsigned tabs = 0) const;
	int next_index() const
	{
		int high = 0;
		while (vMap.count(high) != 0)
		{
			++high;
		}
		return high;
	}
	bool ordered_array() const
	{
		bool numeric = true;
		int check = 0;
		for (auto& it = vMap.cbegin(); numeric && it != vMap.cend(); ++it, ++check)
		{
			if (it->first.type != TYPE_INT || (it->first.type == TYPE_INT && it->first.vInt != check))
			{
				numeric = false;
			}
		}
		return numeric;
	}
	void cleanup(TypeName t)
	{
		if (t != TYPE_VARIANT_MAP)
		{
			vMap.clear();
		}
		if (t != TYPE_STRING)
		{
			vString.clear();
		}
	}
	iterator broadSearch(const VariantValue& key);
	const_iterator broadSearch(const VariantValue& key) const;
	
	//Safe (No-Exception) string-number conversions
	template<typename T>
	std::pair<bool, T> toNumeric(const std::string& num) const
	{
		T result;
		try
		{
			if (std::is_same<T, int>::value)		result = std::stoi(num);
			else if (std::is_same<T, float>::value)	result = std::stof(num);
			else if (std::is_same<T, long>::value)	result = std::stol(num);
			return{ true, result };
		}
		catch (...)
		{
			return{ false, static_cast<T>(0) };
		}
	}

	bool tryNumericConversion();
	
public:
	mutable TypeName type;
	//Data components
	union
	{
		bool vBool;
		int vInt;
		float vFloat;
	};
	variMap vMap;
	std::string vString;

	//Accessors
	std::string asString() const;
	bool asBool() const;
	int asInt() const;
	float asFloat() const;
	variMap asMap() const;

	//Type tests
	bool isBool() const { return type == TYPE_BOOL; }
	bool isInt() const { return type == TYPE_INT; }
	bool isFloat() const { return type == TYPE_FLOAT; }
	bool isNumeric() const { return type == TYPE_INT || type == TYPE_FLOAT; }
	bool isMap() const { return type == TYPE_VARIANT_MAP; }
	bool isString() const { return type == TYPE_STRING; }

	//Utility functions
	VariantValue select(std::function<bool(const VariantValue&)> selector);
	std::string print(FormatName format = FORMAT_VARIANT, bool prettify = false) const;
	unsigned size() const;
	unsigned count(const VariantValue& needle) const;
	void clear();
	bool has(const VariantValue& index) const;
	int compareValue(VariantValue other) const;

	// Constructors
	VariantValue(void);
	VariantValue(const std::initializer_list<VariantValue>& initList);
	VariantValue(const VariantValue& other);
	VariantValue(const std::string& value);
	VariantValue(const char* value);
	VariantValue(char value);
	VariantValue(bool value);
	VariantValue(int value);
	VariantValue(float value);
	VariantValue(const variMap& value);
	//~VariantValue();

	//Insertion
	bool insert(const VariantValue& value);
	bool insert(const std::pair<const VariantValue&, const VariantValue&>& value);
	bool insert( VariantValue key, const VariantValue& value);

	bool push(VariantValue key, const VariantValue& value);

	//C String output
	const char* c_str() const
	{
		if (type == TYPE_STRING)
		{
			return vString.c_str();
		}
		else
		{
			return nullptr;
		}
	}

	void cast(TypeName newType);

	//Iteration
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;

	variMap::size_type erase(const VariantValue& key);
	VariantValue& erase(std::string::size_type offset, std::string::size_type length);
	VariantValue& erase(std::string::size_type offset);

	//operator bool() const { return asBool(); }
	//operator int() const { return asInt(); }
	//operator std::string() const { return asString(); }
	//operator variMap() const { return asMap(); }
	//operator float() const { return asFloat(); }

	VariantValue& operator=(const VariantValue& other);

	//Accessors
	VariantValue at(const VariantValue& key) const;
	VariantValue& operator[](const std::string& key);
	const VariantValue& operator[](const std::string& key) const;
	VariantValue& operator[](const int key);
	const VariantValue& operator[](const int key) const;

	VariantValue operator*=(const VariantValue& rhs);
	VariantValue operator/=(const VariantValue& rhs);
	VariantValue operator+=(const VariantValue& rhs);
	VariantValue operator-=(const VariantValue& rhs);

	VariantValue& operator++();
	VariantValue& operator--();
	VariantValue operator++(int);
	VariantValue operator--(int);

	friend std::ostream& operator<<(std::ostream& os, const VariantValue& value);
	friend std::istream& operator>>(std::istream& is, VariantValue& value);

	friend bool operator== (const VariantValue &v1, const VariantValue &v2);
	friend bool operator!= (const VariantValue &v1, const VariantValue &v2);
	friend bool operator> (const VariantValue &v1, const VariantValue &v2);
	friend bool operator<= (const VariantValue &v1, const VariantValue &v2);
	friend bool operator< (const VariantValue &v1, const VariantValue &v2);
	friend bool operator>= (const VariantValue &v1, const VariantValue &v2);

	friend VariantValue operator*(VariantValue lhs, const VariantValue& rhs);
	friend VariantValue operator/(VariantValue lhs, const VariantValue& rhs);
	friend VariantValue operator+(VariantValue lhs, const VariantValue& rhs);
	friend VariantValue operator-(VariantValue lhs, const VariantValue& rhs);

};

