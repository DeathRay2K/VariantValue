# VariantValue
A c++ class for handling JSON-like representations of data with a super intuitive interface.
The idea is to make handling data as effortlessly simple as it is in a dynamically-typed language.

Supports initializer lists:
```c++
VariantValue data{"this", "will", "be", "an", "array", {"of", "mixed", "items"}, 123, 24.7}};

VariantValue object{
  {"paired", "entries"},
  {"will", "be"},
  {"combined", "into"},
  {"keyed", "values"}
};
```

Supports C++11 range-based for:
```c++
for(const auto& v : data)
{
  // v == "this", v == "will", etc...
}
```

The operators you expect just work:
```c++
VariantValue num(0);
num++;
// num == 1
num *= 10;
// num == 10
```

Get native types with automatic conversions:
```c++
VariantValue num("10");
int value = num.asInt();
// value = 10
```

Read and write JSON with ease:
```json
{
  "item":
  {
    "values":
      [
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10
      ],
    "properties":
      {
        "first": 1,
        "second": 2,
        "third": 3
      },
    "children":
      [
        {
          "test": "data",
          "this": "exists"
        },
        {
          "test": "another",
          "this": "alsoExists"
        }
      ]
  }
}
```
```c++
VariantValue parsedData(VariantValue::parse(jsonText, VariantValue::FORMAT_JSON));
// parsedData["item"]["properties"]["second"] == 2
// parsedData["item"]["values"][0] == 1
// parsedData["item"]["children"][1]["test"] == "another"

std::cout << parsedData.print(VariantValue::FORMAT_JSON, true)
// Prints pretty JSON
// Leave out the second parameter, or set to false for minified JSON
```
