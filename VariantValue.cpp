#include "VariantValue.h"

VariantValue::iterator VariantValue::broadSearch(const VariantValue& key)
{
	VariantValue actor(key);
	iterator search = vMap.find(actor);
	if (search == vMap.end())
	{
		if (key.type == TYPE_STRING)
		{
			actor.cast(TYPE_INT);
			if (actor.asString() == key.asString())
			{
				search = vMap.find(actor);
			}
		}
		else if (key.type == TYPE_INT)
		{
			actor.cast(TYPE_STRING);
			if (actor.asInt() == key.asInt())
			{
				search = vMap.find(actor);
			}
		}
	}
	return search;
}

VariantValue::const_iterator VariantValue::broadSearch(const VariantValue& key) const
{
	VariantValue actor(key);
	const_iterator search = vMap.find(actor);
	if (search == vMap.end())
	{
		if (key.type == TYPE_STRING)
		{
			actor.cast(TYPE_INT);
			if (actor.asString() == key.asString())
			{
				search = vMap.find(actor);
			}
		}
		else if (key.type == TYPE_INT)
		{
			actor.cast(TYPE_STRING);
			if (actor.asInt() == key.asInt())
			{
				search = vMap.find(actor);
			}
		}
	}
	return search;
}

bool VariantValue::tryNumericConversion()
{
	switch (type)
	{
	case TYPE_STRING:
		{
			auto testInt = toNumeric<int>(vString);
			if (testInt.first)
			{
				type = TYPE_INT;
				vInt = testInt.second;
				cleanup(type);
				return true;
			}
			else
			{
				auto testFloat = toNumeric<float>(vString);
				if (testFloat.first)
				{
					type = TYPE_FLOAT;
					vFloat = testFloat.second;
					vString.clear();
					cleanup(type);
					return true;
				}
			}
		}
		break;
	case TYPE_UNDEFINED:
		{
			type = TYPE_INT;
			vInt = 0;
			return true;
		}
		break;
	}
	return false;
}

std::string VariantValue::asString() const
{
	switch (type)
	{
		case TYPE_STRING:
			return vString;
		case TYPE_BOOL:
			return vBool ? "true" : "false";
		case TYPE_INT:
			return std::to_string(vInt);
		case TYPE_FLOAT:
			return std::to_string(vFloat);
		case TYPE_VARIANT_MAP:
			return "std::map<VariantValue, VariantValue>";
		default:
			return "";
	}
	return "";
}

bool VariantValue::asBool() const
{
	switch (type)
	{
		case TYPE_BOOL:
			return vBool;
		case TYPE_STRING:
			return !(vString == "" || vString == "0");
		case TYPE_INT:
			return vInt != 0;
		case TYPE_FLOAT:
			return static_cast<int>(vFloat) != 0;
		case TYPE_VARIANT_MAP:
			return vMap.size() != 0;
		default:
			return false;
	}
}

int VariantValue::asInt() const
{
	switch (type)
	{
		case TYPE_INT:
			return vInt;
		case TYPE_BOOL:
			return vBool ? 1 : 0;
		case TYPE_STRING:
			{
				auto result = toNumeric<int>(vString);
				if (result.first)
				{
					return result.second;
				}
				else
					return 0;
			}
		case TYPE_FLOAT:
			return static_cast<int>(vFloat);
		case TYPE_VARIANT_MAP:
			return 0;
		default:
			return 0;
	}
}

float VariantValue::asFloat() const
{
	switch (type)
	{
		case TYPE_FLOAT:
			return vFloat;
		case TYPE_BOOL:
			return vBool ? 1 : 0;
		case TYPE_STRING:
			{
				auto result = toNumeric<float>(vString);
				if (result.first)
				{
					return result.second;
				}
				else
					return 0;
			}
		case TYPE_INT:
			return static_cast<float>(vInt);
		case TYPE_VARIANT_MAP:
			return 0.f;
		default:
			return 0.f;
	}
}

VariantValue::variMap VariantValue::asMap() const
{
	//variMap temp;
	switch (type)
	{
		case TYPE_VARIANT_MAP:
			return vMap;
		default:
			return{};
	}
}

VariantValue VariantValue::select(std::function<bool(const VariantValue&)> selector)
{
	VariantValue output;
	if (type == TYPE_VARIANT_MAP)
	{
		for (auto& i : vMap)
		{
			if (selector(i.second))
			{
				output.insert(i);
			}
		}
	}
	return output;
}

std::string VariantValue::print(FormatName format, bool prettify) const
{
	if (format == FORMAT_VARIANT)
	{
		if (prettify)
		{
			return print_recursive_variant_pretty();
		}
		else
		{
			return print_recursive_variant();
		}
		
	}
	else if (format == FORMAT_JSON)
	{
		if (prettify)
		{
			return print_recursive_json_pretty();
		}
		else
		{
			return print_recursive_json();
		}
	}
	else
		return "";
}
unsigned VariantValue::size() const
{
	switch (type)
	{
		case TYPE_STRING:
			return vString.size();
		case TYPE_VARIANT_MAP:
			return vMap.size();
		default:
			return 0;
	}
}
unsigned VariantValue::count(const VariantValue& needle) const
{
	
	switch (type)
	{
		case TYPE_STRING:
			{
				unsigned total = 0;
				int offset = 0; bool done = false;
				while (!done)
				{
					offset = vString.find(needle.asString(), offset);
					if (offset != vString.npos)
					{
						++total;
						offset += needle.asString().size();
					}
					else
					{
						done = true;
					}
				};
				return total;
			}
		case TYPE_VARIANT_MAP:
			return vMap.count(needle);
		default:
			return 0;
	}
}
void VariantValue::clear()
{
	switch (type)
	{
	case TYPE_STRING:
	{
		vString.clear();
	}
	case TYPE_VARIANT_MAP:
		vMap.clear();
	}
}

std::string VariantValue::print_recursive_variant() const
{
	std::string output;
	switch (type)
	{
	case TYPE_UNDEFINED:
		return "undefined";
	case TYPE_VARIANT_MAP:
		output += '[';
		for (auto& it : vMap)
		{
			output += it.first.print_recursive_variant();
			output += ':';
			output += it.second.print_recursive_variant();
		}
		output += ']';
		return output;
	case TYPE_STRING:
		output.reserve(size() + 2);
		output = '\'';
		output += asString();
		output += '\'';
		return output;
	default:
		return asString();
	}
}

std::string VariantValue::print_recursive_variant_pretty(unsigned tabs) const
{
	std::string output;
	switch (type)
	{
		case TYPE_UNDEFINED:
			return "undefined";
		case TYPE_VARIANT_MAP:
			if (tabs != 0)
			{
				output += '\n';
				output.append(tabs, '\t');
			}
			output += "[\n";
			for (auto& it : vMap)
			{
				output.append(tabs, '\t');
				output += it.first.print_recursive_variant_pretty(tabs + 1);
				output += " : ";
				output += it.second.print_recursive_variant_pretty(tabs + 1);
				output += '\n';
			}
			if (tabs != 0)
			{
				output.append(tabs, '\t');
			}
			output += ']';
			return output;
		case TYPE_STRING:
			output.reserve(size() + 2);
			output = '\'';
			output += asString();
			output += '\'';
			return output;
		default:
			return asString();
	}
}

std::string VariantValue::print_recursive_json() const
{
	std::string output;
	switch (type)
	{
	case TYPE_UNDEFINED:
		return "null";
	case TYPE_VARIANT_MAP:
	{
		int cur = 0, size = vMap.size();
		//Check if it's numeric
		if (ordered_array())
		{
			output += '[';
			for (auto& it : vMap)
			{
				output += it.second.print_recursive_json();
				++cur;
				if (cur != size)
				{
					output += ',';
				}
			}
			output += ']';
		}
		else
		{
			output += '{';
			for (auto& it : vMap)
			{
				if (it.first.type != TYPE_STRING)
				{
					VariantValue key(it.first.asString());
					output += key.print_recursive_json();
				}
				else
				{
					output += it.first.print_recursive_json();
				}
				output += ':';
				output += it.second.print_recursive_json();
				++cur;
				if (cur != size)
				{
					output += ',';
				}
			}
			output += '}';
		}
	}
		return output;
	case TYPE_STRING:
		{
			std::string curVal(asString());
			curVal = 
				str_replace("\n", "\\n",
					str_replace("/", "\\/",
						str_replace("\b", "\\b",
							str_replace("\f", "\\f",
								str_replace("\r", "\\r",
									str_replace("\t", "\\t",
										str_replace("\"", "\\\"",
											str_replace("\\", "\\\\",
										curVal
									))))))));

			output.reserve(curVal.size() + 2);
			output = '"';
			output += curVal;
			output += '"';
		}
		return output;
	default:
		return asString();
	}
}

std::string VariantValue::print_recursive_json_pretty(unsigned tabs) const
{
	std::string output;
	switch (type)
	{
	case TYPE_UNDEFINED:
		return "null";
	case TYPE_VARIANT_MAP:
	{
		if (tabs != 0)
		{
			output += '\n';
			output.append(tabs, '\t');
		}
		int cur = 0, size = vMap.size();
		//Check if it's numeric
		if (ordered_array())
		{
			output += "[\n";
			for (auto& it : vMap)
			{
				output.append(tabs, '\t');
				output += it.second.print_recursive_json_pretty(tabs + 1);
				++cur;
				if (cur != size)
				{
					output += ',';
				}
				output += '\n';
			}
			if (tabs != 0)
			{
				output.append(tabs, '\t');
			}
			output += ']';
		}
		else
		{
			output += "{\n";
			for (auto& it : vMap)
			{
				output.append(tabs, '\t');
				output += it.first.print_recursive_json_pretty(tabs + 1);
				output += " : ";
				output += it.second.print_recursive_json_pretty(tabs + 1);
				++cur;
				if (cur != size)
				{
					output += ',';
				}
				output += '\n';
			}
			if (tabs != 0)
			{
				output.append(tabs, '\t');
			}
			output += '}';
		}
	}
		return output;
	case TYPE_STRING:
		{
			std::string curVal(asString());
			curVal = str_replace("\\", "\\\\",
				str_replace("\n", "\\n",
					str_replace("/", "\\/",
						str_replace("\b", "\\b",
							str_replace("\f", "\\f",
								str_replace("\r", "\\r",
									str_replace("\t", "\\t",
										curVal
									)))))));

			output.reserve(size() + 2);
			output = '"';
			output += curVal;
			output += '"';
		}
		return output;
	default:
		return asString();
	}
}

VariantValue::VariantValue()
{
	type = TYPE_UNDEFINED;
}

VariantValue::VariantValue(const std::initializer_list<VariantValue>& initList)
{
	type = TYPE_VARIANT_MAP;
	//Two loops through the array
	//First checks if all entries are matched pairs or fewer
	bool matchedSets = true;
	for (auto i = std::begin(initList); matchedSets && i != std::end(initList); ++i)
	{
		matchedSets = ((i->type == TYPE_VARIANT_MAP && i->size() <= 2) || type != TYPE_VARIANT_MAP);
	}
	//Second loop assigns values to keyed indexes or paired indexes
	if (matchedSets)
	{
		for (auto i = std::begin(initList); i != std::end(initList); ++i)
		{
			if (i->type == TYPE_VARIANT_MAP && (*i).ordered_array())
			{
				if (i->size() == 2)
				{
					insert((*i)[0], (*i)[1]);
				}
				else if (i->size() == 1)
				{
					insert((*i)[0]);
				}
			}
			else
			{
				insert(*i);
			}
		}
	}
	else
	{
		for (auto i = std::begin(initList); i != std::end(initList); ++i)
		{
			insert(*i);
		}
	}
}

VariantValue::VariantValue(const VariantValue & other)
{
	type = other.type;
	switch (type)
	{
		case TYPE_BOOL:
			vBool = other.vBool;
			break;
		case TYPE_STRING:
			vString = other.vString;
			break;
		case TYPE_INT:
			vInt = other.vInt;
			break;
		case TYPE_FLOAT:
			vFloat = other.vFloat;
			break;
		case TYPE_VARIANT_MAP:
			vMap = other.vMap;
			break;
		default:
			break;
	}
}

VariantValue::VariantValue(const std::string & value)
{
	type = TYPE_STRING;
	vString = value;
	cleanup(type);
}
VariantValue::VariantValue(const char* value)
{
	type = TYPE_STRING;
	vString = value;
	cleanup(type);
}
VariantValue::VariantValue(char value)
{
	type = TYPE_STRING;
	vString = value;
	cleanup(type);
}

VariantValue::VariantValue(bool value)
{
	type = TYPE_BOOL;
	vBool = value;
	cleanup(type);
}

VariantValue::VariantValue(int value)
{
	type = TYPE_INT;
	vInt = value;
	cleanup(type);
}

VariantValue::VariantValue(float value)
{
	type = TYPE_FLOAT;
	vFloat = value;
	cleanup(type);
}

VariantValue::VariantValue(const variMap& value)
{
	type = TYPE_VARIANT_MAP;
	vMap = value;
	cleanup(type);
}

bool VariantValue::insert(const VariantValue& value)
{
	switch (type)
	{
		case TYPE_UNDEFINED:
			type = TYPE_VARIANT_MAP;
		case TYPE_VARIANT_MAP:
			auto check = vMap.insert({next_index(), value });
			return check.second;
	}
	return false;
}
bool VariantValue::insert(const std::pair<const VariantValue&, const VariantValue&>& value)
{
	switch (type)
	{
		case TYPE_UNDEFINED:
			type = TYPE_VARIANT_MAP;
		case TYPE_VARIANT_MAP:
			return insert(value.first, value.second);
	}
	return false;
}
bool VariantValue::insert(VariantValue key, const VariantValue& value)
{
	switch (type)
	{
		case TYPE_UNDEFINED:
			type = TYPE_VARIANT_MAP;
		case TYPE_VARIANT_MAP:
			{
				if (key.type == TYPE_UNDEFINED)
				{
					key = next_index();
				}
				auto check = vMap.insert({ key, value });
				return check.second;
			}
		case TYPE_STRING:
			if (key.type == TYPE_INT)
			{
				vString.insert(key.asInt(), value.asString());
				return true;
			}
	}
	return false;
}

bool VariantValue::push(VariantValue key, const VariantValue& value)
{
	switch (type)
	{
		case TYPE_UNDEFINED:
			type = TYPE_VARIANT_MAP;
		case TYPE_VARIANT_MAP:
		{
			if (key.type == TYPE_UNDEFINED)
			{
				key = 0;
			}
			auto check = vMap.insert({ key, value });
			if (check.second)
			{
				return true;
			}
			else if(key.type == TYPE_INT)
			{
				int open = next_index();
				for (int i = open - 1; i >= key; --i)
				{
					vMap[open] = vMap[i];
					open = i;
				}
				vMap[key] = value;
				return true;
			}
		}
	}
	return false;
}

void VariantValue::cast(TypeName newType)
{
	switch (newType)
	{
	case TYPE_BOOL:
		vBool = asBool();
		break;
	case TYPE_INT:
		vInt = asInt();
		break;
	case TYPE_FLOAT:
		vFloat = asFloat();
		break;
	case TYPE_STRING:
		vString = asString();
		break;
	case TYPE_VARIANT_MAP:
		vMap = asMap();
		break;
	}
	type = newType;
}

VariantValue& VariantValue::operator=(const VariantValue & other)
{
	type = other.type;
	switch (type)
	{
		case TYPE_BOOL:
			vBool = other.vBool;
			break;
		case TYPE_INT:
			vInt = other.vInt;
			break;
		case TYPE_FLOAT:
			vFloat = other.vFloat;
			break;
		case TYPE_STRING:
			vString = other.vString;
			break;
		case TYPE_VARIANT_MAP:
			vMap = other.vMap;
			break;
		default:
			break;
	}
	cleanup(type);
	return *this;
}

bool VariantValue::has(const VariantValue & index) const
{
	switch (type)
	{
	case TYPE_VARIANT_MAP:
		return vMap.find(index) != vMap.end();
	default:
		return false;
	}
}

int VariantValue::compareValue(VariantValue other) const
{
	other.cast(type);
	if (*this == other)
	{
		return 0;
	}
	else if (*this < other)
	{
		return -1;
	}
	else if (*this > other)
	{
		return 1;
	}
}

VariantValue VariantValue::at(const VariantValue & key) const
{
	switch (type)
	{
	case TYPE_VARIANT_MAP:
		return vMap.at(key);
	case TYPE_STRING:
		return VariantValue(vString[key.asInt()]);
	default:
		return VariantValue();
	}
}

VariantValue& VariantValue::operator[](const std::string & key)
{
	iterator it;
	switch (type)
	{
	case TYPE_UNDEFINED:
		type = TYPE_VARIANT_MAP;
	case TYPE_VARIANT_MAP:

		//it = broadSearch(key);
		it = vMap.find(key);
		if (it != vMap.end())
		{
			return it->second;
		}
		else
		{
			return vMap[key];
		}
	default:
		return *this;
		break;
	}
}

const VariantValue& VariantValue::operator[](const std::string & key) const
{
	const_iterator it;
	switch (type)
	{
	case TYPE_UNDEFINED:
		type = TYPE_VARIANT_MAP;
	case TYPE_VARIANT_MAP:
		//it = broadSearch(key);
		it = vMap.find(key);
		if (it != vMap.end())
		{
			return it->second;
		}
		else
		{
			return vMap.at(key);
		}
	default:
		return *this;
		break;
	}
}
VariantValue& VariantValue::operator[](int key)
{
	iterator it;
	switch (type)
	{
	case TYPE_UNDEFINED:
		type = TYPE_VARIANT_MAP;
	case TYPE_VARIANT_MAP:
		//Implicit conversion
		//it = broadSearch(key);
		it = vMap.find(key);
		if (it != vMap.end())
		{
			return it->second;
		}
		else
		{
			return vMap[key];
		}
	default:
		return *this;
		break;
	}
}

const VariantValue& VariantValue::operator[](int key) const
{
	const_iterator it;
	switch (type)
	{
	case TYPE_UNDEFINED:
		type = TYPE_VARIANT_MAP;
	case TYPE_VARIANT_MAP:
		//it = broadSearch(key);
		it = vMap.find(key);
		if (it != vMap.end())
		{
			return it->second;
		}
		else
		{
			return vMap.at(key);
		}
	default:
		return *this;
		break;
	}
}
VariantValue::iterator VariantValue::begin()
{
	switch (type)
	{
	case VariantValue::TYPE_VARIANT_MAP:
		return vMap.begin();
	}
}

VariantValue::iterator VariantValue::end()
{
	switch (type)
	{
	case VariantValue::TYPE_VARIANT_MAP:
		return vMap.end();
	}
}
VariantValue::const_iterator VariantValue::begin() const
{
	switch (type)
	{
	case VariantValue::TYPE_VARIANT_MAP:
		return vMap.cbegin();
	}
}

VariantValue::const_iterator VariantValue::end() const
{
	switch (type)
	{
	case VariantValue::TYPE_VARIANT_MAP:
		return vMap.cend();
	}
}
VariantValue::const_iterator VariantValue::cbegin() const
{
	switch (type)
	{
	case VariantValue::TYPE_VARIANT_MAP:
		return vMap.cbegin();
	}
}

VariantValue::const_iterator VariantValue::cend() const
{
	switch (type)
	{
	case VariantValue::TYPE_VARIANT_MAP:
		return vMap.cend();
	}
}

VariantValue::variMap::size_type VariantValue::erase(const VariantValue& key)
{
	if (type == TYPE_VARIANT_MAP)
	{
		return vMap.erase(key);
	}
	else
	{
		return 0;
	}
}
VariantValue& VariantValue::erase(std::string::size_type offset, std::string::size_type length)
{
	if (type == TYPE_STRING)
	{
		vString.erase(offset, length);
	}
	return *this;
}
VariantValue& VariantValue::erase(std::string::size_type offset)
{
	if (type == TYPE_STRING)
	{
		vString.erase(offset);
	}
	return *this;
}

//Outside the class definition
std::ostream& operator<<(std::ostream& os, const VariantValue& value)
{
	os << value.asString();
	return os;
}
std::istream& operator >> (std::istream& is, VariantValue& value)
{
	is >> value.vString;
	value.type = VariantValue::TYPE_STRING;
	return is;
}

bool operator== (const VariantValue &v1, const VariantValue &v2)
{
	if (v1.type == v2.type)
	{
		switch (v1.type)
		{
			case VariantValue::TYPE_BOOL:
				return v1.vBool == v2.vBool;
			case VariantValue::TYPE_STRING:
				return v1.vString.compare(v2.vString) == 0;
			case VariantValue::TYPE_INT:
				return v1.vInt == v2.vInt;
			case VariantValue::TYPE_FLOAT:
				return v1.vFloat == v2.vFloat;
			case VariantValue::TYPE_VARIANT_MAP:
				return v1.vMap == v2.vMap;
			case VariantValue::TYPE_UNDEFINED:
				return true;
		}
	}
	else
	{
		//Numeric comparison
		if ((v1.type == VariantValue::TYPE_INT || v1.type == VariantValue::TYPE_FLOAT) &&
			(v2.type == VariantValue::TYPE_INT || v2.type == VariantValue::TYPE_FLOAT))
		{
			return v1.asFloat() == v2.asFloat();
		}
		else
		{
			return false;
		}
	}
}

bool operator!= (const VariantValue &v1, const VariantValue &v2)
{
	return !(v1 == v2);
}

bool operator< (const VariantValue &v1, const VariantValue &v2)
{
	if (v1.type == v2.type)
	{
		switch (v1.type)
		{
		case VariantValue::TYPE_STRING:
			return v1.vString < v2.vString;
		case VariantValue::TYPE_INT:
			return v1.vInt < v2.vInt;
		case VariantValue::TYPE_FLOAT:
			return v1.vFloat < v2.vFloat;
		case VariantValue::TYPE_VARIANT_MAP:
			return v1.vMap < v2.vMap;
		case VariantValue::TYPE_UNDEFINED:
			return true;
		}
	}
	else
	{
		//Numeric comparison
		if ((v1.type == VariantValue::TYPE_INT || v1.type == VariantValue::TYPE_FLOAT))
		{
			if (v2.type == VariantValue::TYPE_INT || v2.type == VariantValue::TYPE_FLOAT)
			{
				return v1.asFloat() < v2.asFloat();
			}
			else if (v2.type == VariantValue::TYPE_STRING)
			{
				VariantValue tempV(v2.asFloat());
				return v1.asFloat() < v2.asFloat();
			}
		}
		else
		{
			return v1.type < v2.type;
		}
	}
}

bool operator<= (const VariantValue &v1, const VariantValue &v2)
{
	return (v1 == v2 || v1 < v2);
}

bool operator> (const VariantValue &v1, const VariantValue &v2)
{
	return !(v1 <= v2);
}

bool operator>= (const VariantValue &v1, const VariantValue &v2)
{
	return !(v1 < v2);
}

VariantValue VariantValue::operator*=(const VariantValue & rhs)
{
	switch (type)
	{
		case TYPE_INT:
			vInt *= rhs.asInt();
			break;
		case TYPE_FLOAT:
			vFloat *= rhs.asFloat();
			break;
		case TYPE_STRING:
			if (tryNumericConversion())
			{
				return operator*=(rhs);
			}
	}
	return *this;
}
VariantValue VariantValue::operator/=(const VariantValue & rhs)
{
	switch (type)
	{
		case VariantValue::TYPE_INT:
			vInt /= rhs.asInt();
			break;
		case VariantValue::TYPE_FLOAT:
			vFloat /= rhs.asFloat();
			break;
		case TYPE_STRING:
			if (tryNumericConversion())
			{
				return operator/=(rhs);
			}
	}
	return *this;
}
VariantValue VariantValue::operator+=(const VariantValue & rhs)
{
	switch (type)
	{
		case VariantValue::TYPE_STRING:
			vString += rhs.asString();
			break;
		case VariantValue::TYPE_INT:
			vInt += rhs.asInt();
			break;
		case VariantValue::TYPE_FLOAT:
			vFloat += rhs.asFloat();
			break;
		case VariantValue::TYPE_VARIANT_MAP:
			vMap[next_index()] = rhs;
			break;
		case VariantValue::TYPE_UNDEFINED:
			if (rhs.type != VariantValue::TYPE_UNDEFINED)
			{
				*this = rhs;
			}
	}
	return *this;
}
VariantValue VariantValue::operator-=(const VariantValue & rhs)
{
	switch (type)
	{
		case VariantValue::TYPE_INT:
			vInt -= rhs.asInt();
			break;
		case VariantValue::TYPE_FLOAT:
			vFloat -= rhs.asFloat();
			break;
		case TYPE_STRING:
			if (tryNumericConversion())
			{
				return operator-=(rhs);
			}
	}
	return *this;
}

VariantValue & VariantValue::operator++()
{
	switch (type)
	{
	case VariantValue::TYPE_INT:
		++vInt;
		break;
	case VariantValue::TYPE_FLOAT:
		++vFloat;
		break;
	case VariantValue::TYPE_STRING:
		if (tryNumericConversion())
		{
			return operator++();
		}
	}
	return *this;
}

VariantValue & VariantValue::operator--()
{
	switch (type)
	{
		case VariantValue::TYPE_INT:
			--vInt;
			break;
		case VariantValue::TYPE_FLOAT:
			--vFloat;
			break;
		case VariantValue::TYPE_STRING:
			if (tryNumericConversion())
			{
				return operator--();
			}
	}
	return *this;
}

VariantValue VariantValue::operator++(int)
{
	VariantValue temp;
	switch (type)
	{
		case VariantValue::TYPE_INT:
			temp = vInt;
			break;
		case VariantValue::TYPE_FLOAT:
			temp = vFloat;
			break;
	}
	++(*this);
	return temp;
}

VariantValue VariantValue::operator--(int)
{
	VariantValue temp;
	switch (type)
	{
	case VariantValue::TYPE_INT:
		temp = vInt;
		break;
	case VariantValue::TYPE_FLOAT:
		temp = vFloat;
		break;
	}
	--(*this);
	return temp;
}

VariantValue operator*(VariantValue lhs, const VariantValue & rhs)
{
	lhs *= rhs;
	return lhs;
}
VariantValue operator/(VariantValue lhs, const VariantValue & rhs)
{
	lhs /= rhs;
	return lhs;
}
VariantValue operator+(VariantValue lhs, const VariantValue & rhs)
{
	lhs += rhs;
	return lhs;
}
VariantValue operator-(VariantValue lhs, const VariantValue & rhs)
{
	lhs -= rhs;
	return lhs;
}