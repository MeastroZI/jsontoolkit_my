#include <gtest/gtest.h>

#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>

TEST(JSONSchema_compile_template_draft4, allof_type) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [ { "type": "integer" }, { "type": "number" } ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON compiled_schema_json{
      sourcemeta::jsontoolkit::to_json(compiled_schema)};

  const sourcemeta::jsontoolkit::JSON expected{
      sourcemeta::jsontoolkit::parse(R"EOF([
    {
      "category": "logical",
      "type": "and",
      "keywordLocation": "/allOf",
      "absoluteKeywordLocation": "#/allOf",
      "condition": [],
      "children": [
        {
          "category": "assertion",
          "type": "type",
          "condition": [],
          "keywordLocation": "/allOf/0/type",
          "absoluteKeywordLocation": "#/allOf/0/type",
          "target": {
            "category": "target",
            "location": "",
            "type": "instance"
          },
          "value": {
            "category": "value",
            "type": "type",
            "value": "integer"
          }
        },
        {
          "category": "logical",
          "type": "or",
          "condition": [],
          "keywordLocation": "/allOf/1/type",
          "absoluteKeywordLocation": "#/allOf/1/type",
          "children": [
            {
              "category": "assertion",
              "type": "type",
              "condition": [],
              "keywordLocation": "/allOf/1/type",
              "absoluteKeywordLocation": "#/allOf/1/type",
              "target": {
                "category": "target",
                "location": "",
                "type": "instance"
              },
              "value": {
                "category": "value",
                "type": "type",
                "value": "real"
              }
            },
            {
              "category": "assertion",
              "condition": [],
              "keywordLocation": "/allOf/1/type",
              "absoluteKeywordLocation": "#/allOf/1/type",
              "target": {
                "category": "target",
                "location": "",
                "type": "instance"
              },
              "type": "type",
              "value": {
                "category": "value",
                "type": "type",
                "value": "integer"
              }
            }
          ]
        }
      ]
    }
  ])EOF")};

  EXPECT_EQ(compiled_schema_json, expected);
}

TEST(JSONSchema_compile_template_draft4, allof_type_with_id) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://www.example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [
      { "id": "nested", "type": "integer" }
    ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON compiled_schema_json{
      sourcemeta::jsontoolkit::to_json(compiled_schema)};

  const sourcemeta::jsontoolkit::JSON expected{
      sourcemeta::jsontoolkit::parse(R"EOF([
    {
      "category": "logical",
      "type": "and",
      "keywordLocation": "/allOf",
      "absoluteKeywordLocation": "https://www.example.com#/allOf",
      "condition": [],
      "children": [
        {
          "category": "assertion",
          "type": "type",
          "condition": [],
          "keywordLocation": "/allOf/0/type",
          "absoluteKeywordLocation": "https://www.example.com/nested#/type",
          "target": {
            "category": "target",
            "location": "",
            "type": "instance"
          },
          "value": {
            "category": "value",
            "type": "type",
            "value": "integer"
          }
        }
      ]
    }
  ])EOF")};

  EXPECT_EQ(compiled_schema_json, expected);
}