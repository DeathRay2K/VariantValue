#include "VariantValue.h"

VariantValue VariantValue::sort(const VariantValue& set)
{
	if (set.type == TYPE_VARIANT_MAP)
	{
		std::vector<VariantValue> sortMid;
		VariantValue sortOut;
		for (auto& i : set.vMap)
		{
			sortMid.push_back(i.second);
			auto& last(sortMid.back());
			if (!last.has("key"))
			{
				last["key"] = i.first;
			}
		}
		std::sort(sortMid.begin(), sortMid.end());
		for (auto& i : sortMid)
		{
			sortOut.insert(i);
		}
		return sortOut;
	}
	else
	{
		return set;
	}
}
VariantValue VariantValue::sort(const VariantValue& set, std::function<bool(const VariantValue& a, const VariantValue& b)> comparison)
{
	if (set.type == TYPE_VARIANT_MAP)
	{
		std::vector<VariantValue> sortMid;
		VariantValue sortOut;
		for (auto& i : set.vMap)
		{
			sortMid.push_back(i.second);
			auto& last(sortMid.back());
			if (!last.has("key"))
			{
				last["key"] = i.first;
			}
		}
		std::sort(sortMid.begin(), sortMid.end(), comparison);
		for (auto& i : sortMid)
		{
			sortOut.insert(i);
		}
		return sortOut;
	}
	else
	{
		return set;
	}
}

VariantValue VariantValue::parse_recursive_variant(const std::string& definition, int offset, int& next)
{
	int tokEnd = offset;
	VariantValue output;
	if (definition[offset] == '\'')
	{
		//String token found
		tokEnd = definition.find('\'', offset + 1);
		output = definition.substr(offset + 1, tokEnd - offset - 1);
	}
	else if (definition.find_first_of("1234567890.-+", offset) == offset)
	{
		tokEnd = definition.find_first_of(" \r\n\t,]:", offset + 1) - 1;
		std::string::size_type numEnd;
		if (definition.find('.', offset) < tokEnd)
		{
			output = std::stof(definition.substr(offset), &numEnd);
		}
		else
		{
			output = std::stoi(definition.substr(offset), &numEnd);
		}
		tokEnd = numEnd + offset - 1;
	}
	else if (definition.find("false", offset) == offset)
	{
		tokEnd = offset + 4;
		output = false;
	}
	else if (definition.find("true", offset) == offset)
	{
		tokEnd = offset + 3;
		output = true;
	}
	else if (definition.find("undefined", offset) == offset)
	{
		tokEnd = offset + 8;
	}
	else if (definition.find('[', offset) == offset)
	{
		bool inMap = true;
		int mapNext = definition.find_first_not_of(" \r\n\t", offset + 1);
		VariantValue token;
		while (inMap)
		{
			token = parse_recursive_variant(definition, mapNext, mapNext);
			if (definition.find(":", mapNext) == mapNext)
			{
				mapNext = definition.find_first_not_of(" \r\n\t", mapNext + 1);
				switch (token.type)
				{
				case TYPE_STRING:
					output[token.asString()] = parse_recursive_variant(definition, mapNext, mapNext);
					break;
				case TYPE_INT:
					output[token.asInt()] = parse_recursive_variant(definition, mapNext, mapNext);
					break;
				default:
					output[output.next_index()] = parse_recursive_variant(definition, mapNext, mapNext);
					break;
				}
				if (definition[mapNext] == ',')
				{
					mapNext = definition.find_first_not_of(" \r\n\t", mapNext + 1);
				}
			}
			else
			{
				output[output.next_index()] = token;
				mapNext = definition.find_first_not_of(" \r\n\t,", mapNext);
			}
			if (definition[mapNext] == ']' || mapNext == definition.npos)
			{
				inMap = false;
				tokEnd = mapNext;
			}
		}
	}
	next = definition.find_first_not_of(" \r\n\t", tokEnd + 1);
	return output;
}

VariantValue VariantValue::parse_recursive_json(const std::string& definition, int offset, int& next)
{
	int tokEnd = offset;
	VariantValue output;
	if (definition[offset] == '"')
	{
		//String token found
		int ignore = 1;
		do
		{
			tokEnd = definition.find('"', offset + ignore);
			++ignore;
		} while (definition[tokEnd - 1] == '\\' && definition[tokEnd - 2] != '\\');


		output = str_replace("\\\\", "\\",
			str_replace("\\n", "\n",
				str_replace("\\/", "/",
					str_replace("\\b", "\b",
						str_replace("\\f", "\f",
							str_replace("\\r", "\r",
								str_replace("\\t", "\t",
									str_replace("\\\"", "\"",
									definition.substr(offset + 1, tokEnd - offset - 1)
								))))))));
	}
	else if (definition.find_first_of("1234567890.-+", offset) == offset)
	{
		tokEnd = definition.find_first_of(" \r\n\t,]}:", offset + 1) - 1;
		std::string::size_type numEnd;
		if (definition.find_first_of(".eE", offset) < tokEnd)
		{
			output = std::stof(definition.substr(offset), &numEnd);
		}
		else
		{
			output = std::stoi(definition.substr(offset), &numEnd);
		}
		tokEnd = numEnd + offset - 1;
	}
	else if (definition.find("false", offset) == offset)
	{
		tokEnd = offset + 4;
		output = false;
	}
	else if (definition.find("true", offset) == offset)
	{
		tokEnd = offset + 3;
		output = true;
	}
	else if (definition.find("null", offset) == offset)
	{
		tokEnd = offset + 3;
	}
	else if (definition.find_first_of("{[", offset) == offset)
	{
		output.type = TYPE_VARIANT_MAP;
		bool inMap = true;
		int mapNext = definition.find_first_not_of(" \r\n\t", offset + 1);
		VariantValue token;
		while (inMap)
		{
			if (definition.find_first_of("]}", mapNext) != mapNext)
			{
				token = parse_recursive_json(definition, mapNext, mapNext);
				if (mapNext != definition.npos)
				{
					if (definition.find(":", mapNext) == mapNext)
					{
						mapNext = definition.find_first_not_of(" \r\n\t", mapNext + 1);
						switch (token.type)
						{
						case TYPE_STRING:
							output[token.asString()] = parse_recursive_json(definition, mapNext, mapNext);
							break;
						case TYPE_INT:
							output[token.asInt()] = parse_recursive_json(definition, mapNext, mapNext);
							break;
						default:
							output[output.next_index()] = parse_recursive_json(definition, mapNext, mapNext);
							break;
						}
						if (mapNext != definition.npos && definition.find(",", mapNext) == mapNext)
						{
							mapNext = definition.find_first_not_of(" \r\n\t", mapNext + 1);
						}
					}
					else
					{
						output[output.next_index()] = token;
						mapNext = definition.find_first_not_of(" \r\n\t,", mapNext);
					}
				}
			}
			if (mapNext == definition.npos || definition.find_first_of("]}", mapNext) == mapNext)
			{
				inMap = false;
				tokEnd = mapNext;
			}
		}
	}
	if (tokEnd != -1)
	{
		next = definition.find_first_not_of(" \r\n\t", tokEnd + 1);
	}
	else
	{
		next = definition.npos;
	}
	
	return output;
}

VariantValue VariantValue::parse_recursive_json_test(const std::string& definition, int offset, int& next)
{
	int tokEnd = offset;
	VariantValue output;
	std::string accumulator;
	if (definition[offset] == '"')
	{
		//String token found
		int ignore = 1;
		do
		{
			tokEnd = definition.find('"', offset + ignore);
			++ignore;
		} while (definition[tokEnd - 1] == '\\' && definition[tokEnd - 2] != '\\');


		output = str_replace("\\\\", "\\",
			str_replace("\\n", "\n",
				str_replace("\\/", "/",
					str_replace("\\b", "\b",
						str_replace("\\f", "\f",
							str_replace("\\r", "\r",
								str_replace("\\t", "\t",
									definition.substr(offset + 1, tokEnd - offset - 1)
								)))))));
	}
	else if (definition.find_first_of("1234567890.-+eE", offset) == offset)
	{
		tokEnd = definition.find_first_of(" \r\n\t,]}:", offset + 1) - 1;
		std::string::size_type numEnd;
		if (definition.find_first_of(".eE", offset) < tokEnd)
		{
			output = std::stof(definition.substr(offset), &numEnd);
		}
		else
		{
			output = std::stoi(definition.substr(offset), &numEnd);
		}
		tokEnd = numEnd + offset - 1;
	}
	else if (definition.find("false", offset) == offset)
	{
		tokEnd = offset + 5;
		output = false;
	}
	else if (definition.find("true", offset) == offset)
	{
		tokEnd = offset + 4;
		output = true;
	}
	else if (definition.find("null", offset) == offset)
	{
		tokEnd = offset + 4;
	}
	else if (definition.find_first_of("{[", offset) == offset)
	{
		bool inMap = true;
		int mapNext = definition.find_first_not_of(" \r\n\t", offset + 1);
		VariantValue token;
		while (inMap)
		{
			token = parse_recursive_json(definition, mapNext, mapNext);
			if (definition.find(":", mapNext) == mapNext)
			{
				mapNext = definition.find_first_not_of(" \r\n\t", mapNext + 1);
				switch (token.type)
				{
				case TYPE_STRING:
					output[token.asString()] = parse_recursive_json(definition, mapNext, mapNext);
					break;
				case TYPE_INT:
					output[token.asInt()] = parse_recursive_json(definition, mapNext, mapNext);
					break;
				default:
					output[output.next_index()] = parse_recursive_json(definition, mapNext, mapNext);
					break;
				}
				if (definition[mapNext] == ',')
				{
					mapNext = definition.find_first_not_of(" \r\n\t", mapNext + 1);
				}
			}
			else
			{
				output[output.next_index()] = token;
				mapNext = definition.find_first_not_of(" \r\n\t,", mapNext);
			}
			if (definition[mapNext] == ']' || definition[mapNext] == '}' || mapNext == definition.npos)
			{
				inMap = false;
				tokEnd = mapNext;
			}
		}
	}
	next = definition.find_first_not_of(" \r\n\t", tokEnd + 1);
	return output;
}

VariantValue VariantValue::parse(const std::string& definition, FormatName format)
{
	int offset = definition.find_first_not_of(" \r\n\t");
	if (offset != definition.npos)
	{
		int next;
		if (format == FORMAT_VARIANT)
		{
			return parse_recursive_variant(definition, offset, next);
		}
		else if (format == FORMAT_JSON)
		{
			return parse_recursive_json(definition, offset, next);
		}
		else if (format == FORMAT_TEST)
		{
			return parse_recursive_json_test(definition, offset, next);
		}
	}
	return{};
}