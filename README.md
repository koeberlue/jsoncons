jsoncons is a C++, header-only library for constructing [JSON](http://www.json.org) and JSON-like
data formats (e.g. CBOR). It supports 

- Parsing JSON-like text or binary data into an unpacked representation
  that defines an interface for accessing and modifying that data.

- Serializing the unpacked representation into different JSON-like text or binary data.

- Converting from JSON-like text or binary data to C++ objects and back.

- Streaming JSON read and write events, somewhat analogously to SAX processing in the XML world. 

It is distributed under the [Boost Software License](http://www.boost.org/users/license.html).

jsoncons uses some features that are new to C++ 11, including [move semantics](http://thbecker.net/articles/rvalue_references/section_02.html) and the [AllocatorAwareContainer](http://en.cppreference.com/w/cpp/concept/AllocatorAwareContainer) concept. It has been tested with MS VC++ 2015, GCC 4.8, GCC 4.9, GCC 6.2.0 and recent versions of clang. Note that `std::regex` isn't fully implemented in GCC 4.8., so `jsoncons_ext/jsonpath` regular expression filters aren't supported for that compiler. 

### Benchmarks

[json_benchmarks](https://github.com/danielaparker/json_benchmarks) provides some measurements about how `jsoncons` compares to other `json` libraries.

- [Performance benchmarks with text and integers](https://github.com/danielaparker/json_benchmarks/blob/master/report/performance.md)

- [Performance benchmarks with text and doubles](https://github.com/danielaparker/json_benchmarks/blob/master/report/performance_fp.md)

- [JSONTestSuite and JSON_checker test suites](https://danielaparker.github.io/json_benchmarks/) 

### Get jsoncons

Download the [latest release](https://github.com/danielaparker/jsoncons/releases) and unpack the zip file. Copy the directory `include/jsoncons` to your `include` directory. If you wish to use extensions, copy `include/jsoncons_ext` as well. 

Or, download the latest code on [master](https://github.com/danielaparker/jsoncons/archive/master.zip).

### How to use it

- For a quick guide, see [jsoncons: a C++ library for json construction](http://danielaparker.github.io/jsoncons). 
- For answers to basic questions, check the [FAQ](doc/FAQ.md).
- For the details, see the [documentation](doc/Home.md). 

As the `jsoncons` library has evolved, names have sometimes changed. To ease transition, jsoncons deprecates the old names but continues to support many of them. See the [deprecated list](doc/ref/deprecated.md) for the status of old names. The deprecated names can be suppressed by defining macro `JSONCONS_NO_DEPRECATED`, which is recommended for new code.

### What's new on master

- The CSV extension now supports multi-valued fields separated by subfield delimiters

- New functions [decode_json](doc/ref/decode_json.md) and [encode_json](doc/ref/encode_json.md) convert JSON 
  formatted strings to C++ objects and back. These functions attempts to 
  perform the conversion by streaming using `json_convert_traits`, and if
  streaming is not supported, fall back to using `json_type_traits`. `decode_json` 
  and `encode_json` will work for all C++ classes that have `json_type_traits` defined.

### Extensions

- [jsonpointer](doc/ref/jsonpointer/jsonpointer.md) implements the IETF standard [JavaScript Object Notation (JSON) Pointer](https://tools.ietf.org/html/rfc6901)
- [jsonpatch](doc/ref/jsonpatch/jsonpatch.md) implements the IETF standard [JavaScript Object Notation (JSON) Patch](https://tools.ietf.org/html/rfc6902)
- [jsonpath](doc/ref/jsonpath/jsonpath.md) implements [Stefan Goessner's JsonPath](http://goessner.net/articles/JsonPath/).  It also supports search and replace using JsonPath expressions.
- [cbor](doc/ref/cbor/cbor.md) implements decode from and encode to the IETF standard [Concise Binary Object Representation (CBOR)](http://cbor.io/). It also supports a set of operations for iterating over and accessing the nested data items of a packed CBOR value.
- [msgpack](doc/ref/msgpack/msgpack.md) implements decode from and encode to the [MessagePack](http://msgpack.org/index.html) binary serialization format.
- [csv](doc/ref/csv/csv.md) implements reading (writing) JSON values from (to) CSV files

Planned new features are listed on the [roadmap](doc/Roadmap.md)

### A simple program using jsoncons

```c++
#include <iostream>
#include <fstream>
#include <jsoncons/json.hpp>

// For convenience
using jsoncons::json;

int main()
{
    json color_spaces = json::array();
    color_spaces.push_back("sRGB");
    color_spaces.push_back("AdobeRGB");
    color_spaces.push_back("ProPhoto RGB");

    json image_sizing; // empty object
    image_sizing["Resize To Fit"] = true; // a boolean 
    image_sizing["Resize Unit"] = "pixels"; // a string
    image_sizing["Resize What"] = "long_edge"; // a string
    image_sizing["Dimension 1"] = 9.84; // a double
    
    json export_settings;

    // create "File Format Options" as an object and put "Color Spaces" in it
    export_settings["File Format Options"]["Color Spaces"] = std::move(color_spaces); 

    export_settings["Image Sizing"] = std::move(image_sizing);

    // Write to stream
    std::ofstream os("export_settings.json");
    os << export_settings;

    // Read from stream
    std::ifstream is("export_settings.json");
    json j = json::parse(is);

    // Pretty print
    std::cout << "(1)\n" << pretty_print(j) << "\n\n";

    // Get reference to object member
    const json& val = j["Image Sizing"];

    // Access member as double
    std::cout << "(2) " << "Dimension 1 = " << val["Dimension 1"].as<double>() << "\n\n";

    // Try access member with default
    std::cout << "(3) " << "Dimension 2 = " << val.get_with_default("Dimension 2",0.0) << "\n";
}
```
Output:
```json
(1)
{
    "File Format Options": {
        "Color Spaces": ["sRGB","AdobeRGB","ProPhoto RGB"]
    },
    "Image Sizing": {
        "Dimension 1": 9.84,
        "Resize To Fit": true,
        "Resize Unit": "pixels",
        "Resize What": "long_edge"
    }
}

(2) Dimension 1 = 9.84

(3) Dimension 2 = 0.0
```

### About jsoncons::basic_json

The jsoncons library provides a `basic_json` class template, which is the generalization of a `json` value for different 
character types, different policies for ordering name-value pairs, etc. A `basic_json` provides an unpacked representation 
of JSON-like string or binary data formats, and defines an interface for accessing and modifying that data.

```c++
typedef basic_json<char,
                   ImplementationPolicy = sorted_policy,
                   Allocator = std::allocator<char>> json;
```
The library includes four instantiations of `basic_json`:

- [json](doc/ref/json.md) constructs a utf8 character json value that sorts name-value members alphabetically

- [ojson](doc/ref/ojson.md) constructs a utf8 character json value that preserves the original name-value insertion order

- [wjson](doc/ref/wjson.md) constructs a wide character json value that sorts name-value members alphabetically

- [wojson](doc/ref/wojson.md) constructs a wide character json value that preserves the original name-value insertion order

#### Convert unpacked json values to standard library types and back

```c++
std::vector<int> v{1, 2, 3, 4};
json j(v);
std::cout << "(1) "<< j << std::endl;
std::deque<int> d = j.as<std::deque<int>>();

std::map<std::string,int> m{{"one",1},{"two",2},{"three",3}};
json j(m);
std::cout << "(2) " << j << std::endl;
std::unordered_map<std::string,int> um = j.as<std::unordered_map<std::string,int>>();
```
Output:
```
(1) [1,2,3,4]

(2) {"one":1,"three":3,"two":2}
```

See [json_type_traits](doc/ref/json_type_traits.md)

#### Convert unpacked `json` values to user defined types and back (also standard library containers of user defined types)

```c++
    struct book
    {
        std::string author;
        std::string title;
        double price;
    };

    namespace jsoncons
    {
        template<class Json>
        struct json_type_traits<Json, book>
        {
            // Implement static functions is, as and to_json 
        };
    }        

    book book1{"Haruki Murakami", "Kafka on the Shore", 25.17};
    book book2{"Charles Bukowski", "Women: A Novel", 12.0};

    std::vector<book> v{book1, book2};

    json j = v;

    std::list<book> l = j.as<std::list<book>>();
```

See [Type Extensibility](doc/Tutorials/Type%20Extensibility.md) for details.

#### Convert JSON formatted text to C++ objects, and back

The functions `decode_json` and `encode_json` convert JSON 
formatted strings to C++ objects and back. These functions attempt to 
perform the conversion by streaming json read and write events with the help of 'json_convert_traits', and if
that is not supported, fall back to using `json_type_traits`. `decode_json` 
and `encode_json` will work for all C++ classes that have `json_type_traits` defined.

```c++
#include <iostream>
#include <map>
#include <tuple>
#include <jsoncons/json.hpp>

using namespace jsoncons;

int main()
{
    typedef std::map<std::string,std::tuple<std::string,std::string,double>> employee_collection;

    employee_collection employees = 
    { 
        {"John Smith",{"Hourly","Software Engineer",10000}},
        {"Jane Doe",{"Commission","Sales",20000}}
    };

    std::string s;
    jsoncons::encode_json(employees, s, jsoncons::indenting::indent);
    std::cout << "(1)\n" << s << std::endl;
    auto employees2 = jsoncons::decode_json<employee_collection>(s);

    std::cout << "\n(2)\n";
    for (const auto& pair : employees2)
    {
        std::cout << pair.first << ": " << std::get<1>(pair.second) << std::endl;
    }
}
```
Output:
```
(1)
{
    "Jane Doe": ["Commission","Sales",20000.0],
    "John Smith": ["Hourly","Software Engineer",10000.0]
}

(2)
Jane Doe: Sales
John Smith: Software Engineer
```

`decode_json` and `encode_json` are supported for many standard library types, and for  
[user defined types](doc/Tutorials/Type%20Extensibility.md)

See [decode_json](doc/ref/decode_json.md) and [encode_json](doc/ref/encode_json.md) 

#### Dump json fragments into a larger document

```c++
#include <jsoncons/json.hpp>

using namespace jsoncons;

int main()
{
    const json some_books = json::parse(R"(
    [
        {
            "title" : "Kafka on the Shore",
            "author" : "Haruki Murakami",
            "price" : 25.17
        },
        {
            "title" : "Women: A Novel",
            "author" : "Charles Bukowski",
            "price" : 12.00
        }
    ]
    )");

    const json more_books = json::parse(R"(
    [
        {
            "title" : "A Wild Sheep Chase: A Novel",
            "author" : "Haruki Murakami",
            "price" : 9.01
        },
        {
            "title" : "Cutter's Way",
            "author" : "Ivan Passer",
            "price" : 8.00
        }
    ]
    )");

    json_serializer serializer(std::cout, jsoncons::indenting::indent); // pretty print
    serializer.begin_json();
    serializer.begin_array();
    for (const auto& book : some_books.array_range())
    {
        book.dump_fragment(serializer);
    }
    for (const auto& book : more_books.array_range())
    {
        book.dump_fragment(serializer);
    }
    serializer.end_array();
    serializer.end_json();
}
```
Output:
```json
[
    {
        "author": "Haruki Murakami",
        "price": 25.17,
        "title": "Kafka on the Shore"
    },
    {
        "author": "Charles Bukowski",
        "price": 12.0,
        "title": "Women: A Novel"
    },
    {
        "author": "Haruki Murakami",
        "price": 9.01,
        "title": "A Wild Sheep Chase: A Novel"
    },
    {
        "author": "Ivan Passer",
        "price": 8.0,
        "title": "Cutter's Way"
    }
]
```

### Extensions

<div id="ext_jsonpointer"/>

#### jsonpointer

Example. Select author from second book

```c++
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

using namespace jsoncons;

int main()
{
    json doc = json::parse(R"(
    [
      { "category": "reference",
        "author": "Nigel Rees",
        "title": "Sayings of the Century",
        "price": 8.95
      },
      { "category": "fiction",
        "author": "Evelyn Waugh",
        "title": "Sword of Honour",
        "price": 12.99
      }
    ]
    )");

    // Using exceptions to report errors
    try
    {
        json result = jsonpointer::get(doc, "/1/author");
        std::cout << "(1) " << result << std::endl;
    }
    catch (const jsonpointer::jsonpointer_error& e)
    {
        std::cout << e.what() << std::endl;
    }

    // Using error codes to report errors
    std::error_code ec;
    json result = jsonpointer::get(doc, "/0/title", ec);

    if (ec)
    {
        std::cout << ec.message() << std::endl;
    }
    else
    {
        std::cout << "(2) " << result << std::endl;
    }
}
```
Output:
```json
(1) "Evelyn Waugh"
(2) "Sayings of the Century"
```

[jsonpointer::get](doc/ref/jsonpointer/get.md) may also be used to query the nested data items of a packed CBOR value.

See [jsonpointer](doc/ref/jsonpointer/jsonpointer.md) for details. 

<div id="ext_jsonpatch"/>

#### jsonpatch

```c++
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpatch/jsonpatch.hpp>

using namespace jsoncons;
using namespace jsoncons::literals;

int main()
{
    // Apply a JSON Patch

    json doc = R"(
        { "foo": "bar"}
    )"_json;

    json doc2 = doc;

    json patch = R"(
        [
            { "op": "add", "path": "/baz", "value": "qux" },
            { "op": "add", "path": "/foo", "value": [ "bar", "baz" ] }
        ]
    )"_json;

    std::error_code ec;
    jsonpatch::apply_patch(doc, patch, ec);

    std::cout << "(1)\n" << pretty_print(doc) << std::endl;

    // Create a JSON Patch from two JSON documents

    auto patch2 = jsonpatch::from_diff(doc2,doc);

    std::cout << "(2)\n" << pretty_print(patch2) << std::endl;

    jsonpatch::apply_patch(doc2, patch2, ec);

    std::cout << "(3)\n" << pretty_print(doc2) << std::endl;
}
```
Output:
```
(1)
{
    "baz": "qux",
    "foo": ["bar","baz"]
}
(2)
[
    {
        "op": "replace",
        "path": "/foo",
        "value": ["bar","baz"]
    },
    {
        "op": "add",
        "path": "/baz",
        "value": "qux"
    }
]
(3)
{
    "baz": "qux",
    "foo": ["bar","baz"]
}
```

See [jsonpatch](doc/ref/jsonpatch/jsonpatch.md) for details.

<div id="ext_jsonpath"/>

#### jsonpath

Example file (booklist.json):
```json
{ "store": {
    "book": [ 
      { "category": "reference",
        "author": "Nigel Rees",
        "title": "Sayings of the Century",
        "price": 8.95
      },
      { "category": "fiction",
        "author": "Evelyn Waugh",
        "title": "Sword of Honour",
        "price": 12.99
      },
      { "category": "fiction",
        "author": "Herman Melville",
        "title": "Moby Dick",
        "isbn": "0-553-21311-3",
        "price": 8.99
      }
    ]
  }
}
```
```c++
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpath/json_query.hpp>

using namespace jsoncons;
using namespace jsoncons::jsonpath;

int main()
{
    std::ifstream is("input/booklist.json");
    json booklist;
    is >> booklist;

    // All books whose author's name starts with Evelyn
    json result1 = json_query(booklist, "$.store.book[?(@.author =~ /Evelyn.*?/)]");
    std::cout << "(1)\n" << pretty_print(result1) << std::endl;

    // Normalized path expressions
    json result2 = json_query(booklist, "$.store.book[?(@.author =~ /Evelyn.*?/)]", 
                              result_type::path);
    std::cout << "(2)\n" << pretty_print(result2) << std::endl;

    // Change the price of "Moby Dick"
    json_replace(booklist,"$.store.book[?(@.isbn == '0-553-21311-3')].price",10.0);
    std::cout << "(3)\n" << pretty_print(booklist) << std::endl;

}
```
Output:
```json
(1)
[
    {
        "author": "Evelyn Waugh",
        "category": "fiction",
        "price": 12.99,
        "title": "Sword of Honour"
    }
]
(2)
[
    "$['store']['book'][1]"
]
(3)
{
    "store": {
        "book": [
            {
                "author": "Nigel Rees",
                "category": "reference",
                "price": 8.95,
                "title": "Sayings of the Century"
            },
            {
                "author": "Evelyn Waugh",
                "category": "fiction",
                "price": 12.99,
                "title": "Sword of Honour"
            },
            {
                "author": "Herman Melville",
                "category": "fiction",
                "isbn": "0-553-21311-3",
                "price": 10.0,
                "title": "Moby Dick"
            }
        ]
    }
}
```

See [jsonpath](doc/ref/jsonpath/jsonpath.md) for details.

<div id="ext_cbor"/>

#### cbor

The `cbor` extension supports decoding a packed CBOR value to an unpacked (json) value and
encoding an unpacked (json) value to a packed CBOR value. It also supports a set of operations 
on a view of a packed CBOR value for iterating over and accessing nested CBOR data items.

This example illustrates encoding a [Reputation Interchange](https://tools.ietf.org/rfc/rfc7071.txt) data object to and from cbor.

```c++
#include <jsoncons/json.hpp>
#include <jsoncons_ext/cbor/cbor.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

using namespace jsoncons;

int main()
{
    ojson j1 = ojson::parse(R"(
    {
       "application": "hiking",
       "reputons": [
       {
           "rater": "HikingAsylum.example.com",
           "assertion": "is-good",
           "rated": "sk",
           "rating": 0.90
         }
       ]
    }
    )");

    // Encoding an unpacked (json) value to a packed CBOR value
    std::vector<uint8_t> data;
    cbor::encode_cbor(j1, data);

    // Decoding a packed CBOR value to an unpacked (json) value
    ojson j2 = cbor::decode_cbor<ojson>(data);
    std::cout << "(1)\n" << pretty_print(j2) << "\n\n";

    // Iterating over and accessing the nested data items of a packed CBOR value
    cbor::cbor_view datav{data};    
    cbor::cbor_view reputons = datav.at("reputons");    

    std::cout << "(2)\n";
    for (auto element : reputons.array_range())
    {
        std::cout << element.at("rated").as_string() << ", ";
        std::cout << element.at("rating").as_double() << "\n";
    }
    std::cout << std::endl;

    // Querying a packed CBOR value for a nested data item with jsonpointer
    std::error_code ec;
    cbor::cbor_view rated = jsonpointer::get(datav, "/reputons/0/rated", ec);
    if (!ec)
    {
        std::cout << "(3) " << rated.as_string() << "\n";
    }
```
Output:
```
(1)
{
    "application": "hiking",
    "reputons": [
        {
            "rater": "HikingAsylum.example.com",
            "assertion": "is-good",
            "rated": "sk",
            "rating": 0.9
        }
    ]
}

(2)
sk, 0.9

(3) sk
```

See [cbor](doc/ref/cbor/cbor.md) for details.

<div id="ext_msgpack"/>

#### msgpack

The `msgpack` extension supports encoding json to and decoding from the [MessagePack](http://msgpack.org/index.html) binary serialization format.

Example file (book.json):
```json
[
    {
        "category": "reference",
        "author": "Nigel Rees",
        "title": "Sayings of the Century",
        "price": 8.95
    },
    {
        "category": "fiction",
        "author": "Evelyn Waugh",
        "title": "Sword of Honour",
        "price": 12.99
    }
]
```
```c++
#include <jsoncons/json.hpp>
#include <jsoncons_ext/msgpack/msgpack.hpp>

using namespace jsoncons;
using namespace jsoncons::msgpack;

int main()
{
    std::ifstream is("input/book.json");
    ojson j1;
    is >> j1;

    // Encode ojson to MessagePack
    std::vector<uint8_t> v;
    encode_msgpack(j1, v);

    // Decode MessagePack to ojson 
    ojson j2 = decode_msgpack<ojson>(v);

    std::cout << pretty_print(j2) << std::endl;

    // or to json (now alphabetically sorted)
    json j3 = decode_msgpack<json>(v);

    // or to wjson (converts from utf8 to wide characters)
    wjson j4 = decode_msgpack<wjson>(v);
}
```
Output:
```json
[
    {
        "category": "reference",
        "author": "Nigel Rees",
        "title": "Sayings of the Century",
        "price": 8.95
    },
    {
        "category": "fiction",
        "author": "Evelyn Waugh",
        "title": "Sword of Honour",
        "price": 12.99
    }
]
```

See [msgpack](doc/ref/msgpack/msgpack.md) for details.

<div id="ext_csv"/>

#### csv
Example file (sales.csv)
```csv
customer_name,has_coupon,phone_number,zip_code,sales_tax_rate,total_amount
"John Roe",true,0272561313,01001,0.05,431.65
"Jane Doe",false,416-272-2561,55416,0.15,480.70
"Joe Bloggs",false,"4162722561","55416",0.15,300.70
"John Smith",FALSE,NULL,22313-1450,0.15,300.70
```

```c++
#include <fstream>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/csv/csv_reader.hpp>

using namespace jsoncons;
using namespace jsoncons::csv;

int main()
{
    csv_serializing_options options;
    params.assume_header(true);

    params.mapping(mapping_type::n_objects);
    std::ifstream is1("input/sales.csv");
    ojson j1 = decode_csv<ojson>(is1,options);
    std::cout << "\n(1)\n"<< pretty_print(j1) << "\n";

    params.mapping(mapping_type::n_rows);
    std::ifstream is2("input/sales.csv");
    ojson j2 = decode_csv<ojson>(is2,options);
    std::cout << "\n(2)\n"<< pretty_print(j2) << "\n";

    params.mapping(mapping_type::m_columns);
    std::ifstream is3("input/sales.csv");
    ojson j3 = decode_csv<ojson>(is3,options);
    std::cout << "\n(3)\n"<< pretty_print(j3) << "\n";
}
```
Output:
```json
(1)
[
    {
        "customer_name": "John Roe",
        "has_coupon": true,
        "phone_number": "0272561313",
        "zip_code": "01001",
        "sales_tax_rate": 0.05,
        "total_amount": 431.65
    },
    {
        "customer_name": "Jane Doe",
        "has_coupon": false,
        "phone_number": "416-272-2561",
        "zip_code": 55416,
        "sales_tax_rate": 0.15,
        "total_amount": 480.7
    },
    {
        "customer_name": "Joe Bloggs",
        "has_coupon": false,
        "phone_number": "4162722561",
        "zip_code": "55416",
        "sales_tax_rate": 0.15,
        "total_amount": 300.7
    },
    {
        "customer_name": "John Smith",
        "has_coupon": false,
        "phone_number": null,
        "zip_code": "22313-1450",
        "sales_tax_rate": 0.15,
        "total_amount": 300.7
    }
]

(2)
[
    ["customer_name","has_coupon","phone_number","zip_code","sales_tax_rate","total_amount"],
    ["John Roe",true,"0272561313","01001",0.05,431.65],
    ["Jane Doe",false,"416-272-2561",55416,0.15,480.7],
    ["Joe Bloggs",false,"4162722561","55416",0.15,300.7],
    ["John Smith",false,null,"22313-1450",0.15,300.7]
]

(3)
{
    "customer_name": ["John Roe","Jane Doe","Joe Bloggs","John Smith"],
    "has_coupon": [true,false,false,false],
    "phone_number": ["0272561313","416-272-2561",4162722561,null],
    "zip_code": ["01001",55416,55416,"22313-1450"],
    "sales_tax_rate": [0.05,0.15,0.15,0.15],
    "total_amount": [431.65,480.7,300.7,300.7]
}
```

See [csv](doc/ref/csv/csv.md) for details.

### Building the test suite and examples with CMake

[CMake](https://cmake.org/) is a cross-platform build tool that generates makefiles and solutions for the compiler environment of your choice. On Windows you can download a [Windows Installer package](https://cmake.org/download/). On Linux it is usually available as a package, e.g., on Ubuntu,
```
sudo apt-get install cmake
```

Instructions for building the test suite with CMake may be found in

    jsoncons/tests/build/cmake/README.txt

Instructions for building the examples with CMake may be found in

    jsoncons/examples/build/cmake/README.txt

### Acknowledgements

Special thanks to our [contributors](https://github.com/danielaparker/jsoncons/blob/master/acknowledgements.txt)

