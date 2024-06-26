![JSON Toolkit](./assets/banner.png)

JSON Toolkit is a swiss-army knife for [JSON](https://www.json.org) programming
in modern C++. It comes with a built-in parser and growing support for
standards such as [JSON Schema](http://json-schema.org), [JSON
Pointer](https://www.rfc-editor.org/rfc/rfc6901),
[JSONL](https://jsonlines.org), and more.

Example

```cpp
#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonpointer.h>
#include <sstream>
#include <iostream>

int main()  {
    // Creating JSON using parse
    sourcemeta::jsontoolkit::JSON document =
        sourcemeta::jsontoolkit::parse(R"JSON({
            "name": "John Doe",
            "age": 20,
            "address": "zxy"
        })JSON");

    // JSON pointer for the name property
    const sourcemeta::jsontoolkit::Pointer name_pointer{"name"};
    // A new JSON document for the name
    const sourcemeta::jsontoolkit::JSON name_value{"Johnny Doe"};
    // Updating the value of the name using JSON pointer
    sourcemeta::jsontoolkit::set(document, name_pointer, name_value);
    sourcemeta::jsontoolkit::prettify(document, std::cout);
    std::cout<<"\n";

    // The above program will print the following to standard output:
    //output :
    //    {
    //       "address": "zxy_with_bar",
    //       "age": 21,
    //       "name": "xyz_with_foo"
    //    }
}
```

Documentation
-------------

Refer to the project website for documentation:
[https://jsontoolkit.sourcemeta.com](https://jsontoolkit.sourcemeta.com).

Commercial Use
--------------

This project is governed by the [AGPL-3.0](./LICENSE) copyleft license and our
[Contribution Agreement](https://www.sourcemeta.com/contributing/). To ensure
the sustainability of the project, you can freely make use of this software as
long the outcome is distributed under the same license. Otherwise, you must
obtain a [commercial license](./LICENSE-COMMERCIAL) that removes such
restrictions. Read more about our licensing approach
[here](https://www.sourcemeta.com/licensing/).


