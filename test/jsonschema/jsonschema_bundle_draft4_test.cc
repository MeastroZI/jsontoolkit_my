#include <gtest/gtest.h>

#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>

#include <future>      // std::promise, std::future
#include <string>      // std::string
#include <string_view> // std::string_view

static auto test_resolver(std::string_view identifier)
    -> std::future<std::optional<sourcemeta::jsontoolkit::JSON>> {
  std::promise<std::optional<sourcemeta::jsontoolkit::JSON>> promise;
  if (identifier == "https://example.com/foo/bar") {
    promise.set_value(sourcemeta::jsontoolkit::parse(R"JSON({
      "$schema": "http://json-schema.org/draft-04/schema#",
      "id": "https://example.com/foo/bar",
      "$anchor": "baz"
    })JSON"));
  } else if (identifier == "https://www.sourcemeta.com/test-1") {
    promise.set_value(sourcemeta::jsontoolkit::parse(R"JSON({
      "$schema": "http://json-schema.org/draft-04/schema#",
      "id": "https://www.sourcemeta.com/test-1",
      "type": "string"
    })JSON"));
  } else if (identifier == "https://www.sourcemeta.com/test-2") {
    promise.set_value(sourcemeta::jsontoolkit::parse(R"JSON({
      "$schema": "http://json-schema.org/draft-04/schema#",
      "id": "https://www.sourcemeta.com/test-2",
      "allOf": [ { "$ref": "test-3" } ]
    })JSON"));
  } else if (identifier == "https://www.sourcemeta.com/test-3") {
    promise.set_value(sourcemeta::jsontoolkit::parse(R"JSON({
      "$schema": "http://json-schema.org/draft-04/schema#",
      "id": "https://www.sourcemeta.com/test-3",
      "allOf": [ { "$ref": "test-1" } ]
    })JSON"));
  } else if (identifier == "https://www.sourcemeta.com/test-4") {
    promise.set_value(sourcemeta::jsontoolkit::parse(R"JSON({
      "$schema": "http://json-schema.org/draft-04/schema#",
      "id": "https://www.sourcemeta.com/test-4",
      "type": "boolean"
    })JSON"));
  } else {
    promise.set_value(
        sourcemeta::jsontoolkit::official_resolver(identifier).get());
  }

  return promise.get_future();
}

TEST(JSONSchema_bundle_draft4, no_references_no_id) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#"
  })JSON");

  sourcemeta::jsontoolkit::bundle(
      document, sourcemeta::jsontoolkit::default_schema_walker, test_resolver)
      .wait();

  const sourcemeta::jsontoolkit::JSON expected =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft4, const_no_references_no_id) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#"
  })JSON");

  const auto result =
      sourcemeta::jsontoolkit::bundle(
          document, sourcemeta::jsontoolkit::default_schema_walker,
          test_resolver)
          .get();

  const sourcemeta::jsontoolkit::JSON expected =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#"
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_bundle_draft4, simple_with_id) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" },
      "bar": {
        "id": "https://www.sourcemeta.com",
        "allOf": [ { "$ref": "test-2" } ]
      },
      "baz": { "$ref": "https://example.com/foo/bar#baz" },
      "qux": { "$ref": "https://example.com/foo/bar" }
    }
  })JSON");

  sourcemeta::jsontoolkit::bundle(
      document, sourcemeta::jsontoolkit::default_schema_walker, test_resolver)
      .wait();

  const sourcemeta::jsontoolkit::JSON expected =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" },
      "bar": {
        "id": "https://www.sourcemeta.com",
        "allOf": [ { "$ref": "test-2" } ]
      },
      "baz": { "$ref": "https://example.com/foo/bar#baz" },
      "qux": { "$ref": "https://example.com/foo/bar" }
    },
    "definitions": {
      "https://www.sourcemeta.com/test-1": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      },
      "https://www.sourcemeta.com/test-2": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-2",
        "allOf": [ { "$ref": "test-3" } ]
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-1" } ]
      },
      "https://example.com/foo/bar": {
        "id": "https://example.com/foo/bar",
        "$schema": "http://json-schema.org/draft-04/schema#",
        "$anchor": "baz"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft4, simple_without_id) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" },
      "bar": {
        "id": "https://www.sourcemeta.com",
        "allOf": [ { "$ref": "test-2" } ]
      },
      "baz": { "$ref": "https://example.com/foo/bar#baz" },
      "qux": { "$ref": "https://example.com/foo/bar" }
    }
  })JSON");

  sourcemeta::jsontoolkit::bundle(
      document, sourcemeta::jsontoolkit::default_schema_walker, test_resolver)
      .wait();

  const sourcemeta::jsontoolkit::JSON expected =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" },
      "bar": {
        "id": "https://www.sourcemeta.com",
        "allOf": [ { "$ref": "test-2" } ]
      },
      "baz": { "$ref": "https://example.com/foo/bar#baz" },
      "qux": { "$ref": "https://example.com/foo/bar" }
    },
    "definitions": {
      "https://www.sourcemeta.com/test-1": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      },
      "https://www.sourcemeta.com/test-2": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-2",
        "allOf": [ { "$ref": "test-3" } ]
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-1" } ]
      },
      "https://example.com/foo/bar": {
        "id": "https://example.com/foo/bar",
        "$schema": "http://json-schema.org/draft-04/schema#",
        "$anchor": "baz"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft4, schema_not_found) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/xxx" }
    }
  })JSON");

  EXPECT_THROW(sourcemeta::jsontoolkit::bundle(
                   document, sourcemeta::jsontoolkit::default_schema_walker,
                   test_resolver),
               sourcemeta::jsontoolkit::SchemaResolutionError);
}

TEST(JSONSchema_bundle_draft4, idempotency) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-2" }
    }
  })JSON");

  sourcemeta::jsontoolkit::bundle(
      document, sourcemeta::jsontoolkit::default_schema_walker, test_resolver)
      .wait();
  sourcemeta::jsontoolkit::bundle(
      document, sourcemeta::jsontoolkit::default_schema_walker, test_resolver)
      .wait();
  sourcemeta::jsontoolkit::bundle(
      document, sourcemeta::jsontoolkit::default_schema_walker, test_resolver)
      .wait();

  const sourcemeta::jsontoolkit::JSON expected =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-2" }
    },
    "definitions": {
      "https://www.sourcemeta.com/test-1": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      },
      "https://www.sourcemeta.com/test-2": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-2",
        "allOf": [ { "$ref": "test-3" } ]
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-1" } ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft4, pre_embedded) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-2" }
    },
    "definitions": {
      "already-embedded": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::jsontoolkit::bundle(
      document, sourcemeta::jsontoolkit::default_schema_walker, test_resolver)
      .wait();

  const sourcemeta::jsontoolkit::JSON expected =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-2" }
    },
    "definitions": {
      "already-embedded": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      },
      "https://www.sourcemeta.com/test-2": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-2",
        "allOf": [ { "$ref": "test-3" } ]
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-1" } ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft4, taken_definitions_entry) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-1" },
      "extra": { "$ref": "https://www.sourcemeta.com/test-4" }
    },
    "definitions": {
      "https://www.sourcemeta.com/test-1": { "foo": 1 },
      "https://www.sourcemeta.com/test-4": { "foo": 1 },
      "https://www.sourcemeta.com/test-4/x": { "foo": 1 },
      "https://www.sourcemeta.com/test-4/x/x": { "foo": 1 }
    }
  })JSON");

  sourcemeta::jsontoolkit::bundle(
      document, sourcemeta::jsontoolkit::default_schema_walker, test_resolver)
      .wait();

  const sourcemeta::jsontoolkit::JSON expected =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-1" },
      "extra": { "$ref": "https://www.sourcemeta.com/test-4" }
    },
    "definitions": {
      "https://www.sourcemeta.com/test-1": { "foo": 1 },
      "https://www.sourcemeta.com/test-1/x": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      },
      "https://www.sourcemeta.com/test-4": { "foo": 1 },
      "https://www.sourcemeta.com/test-4/x": { "foo": 1 },
      "https://www.sourcemeta.com/test-4/x/x": { "foo": 1 },
      "https://www.sourcemeta.com/test-4/x/x/x": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-4",
        "type": "boolean"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft4, allof_ref_definitions_type_no_id_no_external) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [ { "$ref": "#/definitions/string" } ],
    "definitions": {
      "string": { "type": "string" }
    }
  })JSON");

  sourcemeta::jsontoolkit::bundle(
      document, sourcemeta::jsontoolkit::default_schema_walker, test_resolver)
      .wait();

  const sourcemeta::jsontoolkit::JSON expected =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [ { "$ref": "#/definitions/string" } ],
    "definitions": {
      "string": { "type": "string" }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}
