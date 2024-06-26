#include <gtest/gtest.h>

#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonpointer.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>

#include "jsonschema_test_utils.h"

#define EXPECT_FRAME_STATIC_2019_09(frame, reference, root_id,                 \
                                    expected_pointer, expected_base,           \
                                    expected_relative_pointer)                 \
  EXPECT_FRAME_STATIC(frame, reference, root_id, expected_pointer,             \
                      "https://json-schema.org/draft/2019-09/schema",          \
                      expected_base, expected_relative_pointer);

TEST(JSONSchema_frame_2019_09, empty_schema) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame frame;
  sourcemeta::jsontoolkit::ReferenceMap references;
  sourcemeta::jsontoolkit::frame(document, frame, references,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  EXPECT_EQ(frame.size(), 3);
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema",
                              "https://www.sourcemeta.com/schema", "",
                              "https://www.sourcemeta.com/schema", "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema#/$id",
                              "https://www.sourcemeta.com/schema", "/$id",
                              "https://www.sourcemeta.com/schema", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame,
                              "https://www.sourcemeta.com/schema#/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema");

  // References

  EXPECT_TRUE(references.empty());
}

TEST(JSONSchema_frame_2019_09, one_level_applicators_without_identifiers) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": { "type": "string" },
    "properties": {
      "foo": { "type": "number" }
    }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame frame;
  sourcemeta::jsontoolkit::ReferenceMap references;
  sourcemeta::jsontoolkit::frame(document, frame, references,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  EXPECT_EQ(frame.size(), 8);
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema",
                              "https://www.sourcemeta.com/schema", "",
                              "https://www.sourcemeta.com/schema", "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema#/$id",
                              "https://www.sourcemeta.com/schema", "/$id",
                              "https://www.sourcemeta.com/schema", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame,
                              "https://www.sourcemeta.com/schema#/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema#/items",
                              "https://www.sourcemeta.com/schema", "/items",
                              "https://www.sourcemeta.com/schema", "/items");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/schema", "/items/type");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type");

  // References

  EXPECT_TRUE(references.empty());
}

TEST(JSONSchema_frame_2019_09, one_level_applicators_with_identifiers) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/test/qux",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": { "$id": "../foo", "type": "string" },
    "properties": {
      "foo": { "$anchor": "test", "type": "number" }
    }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame frame;
  sourcemeta::jsontoolkit::ReferenceMap references;
  sourcemeta::jsontoolkit::frame(document, frame, references,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  EXPECT_EQ(frame.size(), 14);

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/test/qux",
                              "https://www.sourcemeta.com/test/qux", "",
                              "https://www.sourcemeta.com/test/qux", "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/foo",
                              "https://www.sourcemeta.com/test/qux", "/items",
                              "https://www.sourcemeta.com/foo", "");

  // Anchors

  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/test/qux#test",
      "https://www.sourcemeta.com/test/qux", "/properties/foo",
      "https://www.sourcemeta.com/test/qux", "/properties/foo");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/test/qux#/$id",
                              "https://www.sourcemeta.com/test/qux", "/$id",
                              "https://www.sourcemeta.com/test/qux", "/$id");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/test/qux#/$schema",
      "https://www.sourcemeta.com/test/qux", "/$schema",
      "https://www.sourcemeta.com/test/qux", "/$schema");
  EXPECT_FRAME_STATIC_2019_09(frame,
                              "https://www.sourcemeta.com/test/qux#/items",
                              "https://www.sourcemeta.com/test/qux", "/items",
                              "https://www.sourcemeta.com/foo", "");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/test/qux#/items/$id",
      "https://www.sourcemeta.com/test/qux", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/test/qux#/items/type",
      "https://www.sourcemeta.com/test/qux", "/items/type",
      "https://www.sourcemeta.com/foo", "/type");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/test/qux#/properties",
      "https://www.sourcemeta.com/test/qux", "/properties",
      "https://www.sourcemeta.com/test/qux", "/properties");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/test/qux#/properties/foo",
      "https://www.sourcemeta.com/test/qux", "/properties/foo",
      "https://www.sourcemeta.com/test/qux", "/properties/foo");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/test/qux#/properties/foo/$anchor",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/$anchor",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/$anchor");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/test/qux#/properties/foo/type",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/type",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/type");

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/foo#/$id",
                              "https://www.sourcemeta.com/test/qux",
                              "/items/$id", "https://www.sourcemeta.com/foo",
                              "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/foo#/type",
                              "https://www.sourcemeta.com/test/qux",
                              "/items/type", "https://www.sourcemeta.com/foo",
                              "/type");

  // References

  EXPECT_TRUE(references.empty());
}

TEST(JSONSchema_frame_2019_09, subschema_absolute_identifier) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": {
      "$id": "https://www.sourcemeta.com/foo",
      "type": "string"
     }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame frame;
  sourcemeta::jsontoolkit::ReferenceMap references;
  sourcemeta::jsontoolkit::frame(document, frame, references,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  EXPECT_EQ(frame.size(), 9);
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema",
                              "https://www.sourcemeta.com/schema", "",
                              "https://www.sourcemeta.com/schema", "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/foo",
                              "https://www.sourcemeta.com/schema", "/items",
                              "https://www.sourcemeta.com/foo", "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema#/$id",
                              "https://www.sourcemeta.com/schema", "/$id",
                              "https://www.sourcemeta.com/schema", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame,
                              "https://www.sourcemeta.com/schema#/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema#/items",
                              "https://www.sourcemeta.com/schema", "/items",
                              "https://www.sourcemeta.com/foo", "");
  EXPECT_FRAME_STATIC_2019_09(frame,
                              "https://www.sourcemeta.com/schema#/items/$id",
                              "https://www.sourcemeta.com/schema", "/items/$id",
                              "https://www.sourcemeta.com/foo", "/$id");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/foo", "/type");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/foo#/$id",
                              "https://www.sourcemeta.com/schema", "/items/$id",
                              "https://www.sourcemeta.com/foo", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/foo#/type",
                              "https://www.sourcemeta.com/schema",
                              "/items/type", "https://www.sourcemeta.com/foo",
                              "/type");

  // References

  EXPECT_TRUE(references.empty());
}

TEST(JSONSchema_frame_2019_09, nested_schemas) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": {
        "$id": "https://www.sourcemeta.com/foo",
        "$anchor": "test",
        "items": {
          "$id": "qux"
        }
      },
      "bar": {
        "$id": "https://www.sourcemeta.com/bar"
      },
      "baz": {
        "$id": "baz",
        "items": {
          "$anchor": "extra"
        }
      }
    }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame frame;
  sourcemeta::jsontoolkit::ReferenceMap references;
  sourcemeta::jsontoolkit::frame(document, frame, references,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  EXPECT_EQ(frame.size(), 30);

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema",
                              "https://www.sourcemeta.com/schema", "",
                              "https://www.sourcemeta.com/schema", "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/foo",
                              "https://www.sourcemeta.com/schema",
                              "/properties/foo",
                              "https://www.sourcemeta.com/foo", "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/foo#test",
                              "https://www.sourcemeta.com/schema",
                              "/properties/foo",
                              "https://www.sourcemeta.com/foo", "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/bar",
                              "https://www.sourcemeta.com/schema",
                              "/properties/bar",
                              "https://www.sourcemeta.com/bar", "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/baz",
                              "https://www.sourcemeta.com/schema",
                              "/properties/baz",
                              "https://www.sourcemeta.com/baz", "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/baz#extra",
                              "https://www.sourcemeta.com/schema",
                              "/properties/baz/items",
                              "https://www.sourcemeta.com/baz", "/items");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/qux",
                              "https://www.sourcemeta.com/schema",
                              "/properties/foo/items",
                              "https://www.sourcemeta.com/qux", "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema#/$id",
                              "https://www.sourcemeta.com/schema", "/$id",
                              "https://www.sourcemeta.com/schema", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame,
                              "https://www.sourcemeta.com/schema#/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties");

  // foo
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/foo", "");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/$id",
      "https://www.sourcemeta.com/foo", "/$id");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$anchor",
      "https://www.sourcemeta.com/foo", "/$anchor");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/items",
      "https://www.sourcemeta.com/schema", "/properties/foo/items",
      "https://www.sourcemeta.com/qux", "");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/items/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/items/$id",
      "https://www.sourcemeta.com/qux", "/$id");

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/foo#/$id",
                              "https://www.sourcemeta.com/schema",
                              "/properties/foo/$id",
                              "https://www.sourcemeta.com/foo", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/foo#/$anchor",
                              "https://www.sourcemeta.com/schema",
                              "/properties/foo/$anchor",
                              "https://www.sourcemeta.com/foo", "/$anchor");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/foo#/items",
                              "https://www.sourcemeta.com/schema",
                              "/properties/foo/items",
                              "https://www.sourcemeta.com/qux", "");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/foo#/items/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/items/$id",
      "https://www.sourcemeta.com/qux", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/qux#/$id",
                              "https://www.sourcemeta.com/schema",
                              "/properties/foo/items/$id",
                              "https://www.sourcemeta.com/qux", "/$id");

  // bar
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$id",
      "https://www.sourcemeta.com/schema", "/properties/bar/$id",
      "https://www.sourcemeta.com/bar", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/bar#/$id",
                              "https://www.sourcemeta.com/schema",
                              "/properties/bar/$id",
                              "https://www.sourcemeta.com/bar", "/$id");

  // baz
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz",
      "https://www.sourcemeta.com/baz", "");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/$id",
      "https://www.sourcemeta.com/schema", "/properties/baz/$id",
      "https://www.sourcemeta.com/baz", "/$id");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/items",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "https://www.sourcemeta.com/baz", "/items");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/items/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/baz/items/$anchor",
      "https://www.sourcemeta.com/baz", "/items/$anchor");

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/baz#/$id",
                              "https://www.sourcemeta.com/schema",
                              "/properties/baz/$id",
                              "https://www.sourcemeta.com/baz", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/baz#/items",
                              "https://www.sourcemeta.com/schema",
                              "/properties/baz/items",
                              "https://www.sourcemeta.com/baz", "/items");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/baz#/items/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/baz/items/$anchor",
      "https://www.sourcemeta.com/baz", "/items/$anchor");

  // References

  EXPECT_TRUE(references.empty());
}

TEST(JSONSchema_frame_2019_09, id_override) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": { "$id": "schema" }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame frame;
  sourcemeta::jsontoolkit::ReferenceMap references;
  EXPECT_THROW(sourcemeta::jsontoolkit::frame(
                   document, frame, references,
                   sourcemeta::jsontoolkit::default_schema_walker,
                   sourcemeta::jsontoolkit::official_resolver)
                   .wait(),
               sourcemeta::jsontoolkit::SchemaError);
}

TEST(JSONSchema_frame_2019_09, static_anchor_override) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$anchor": "foo",
    "items": { "$anchor": "foo" }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame frame;
  sourcemeta::jsontoolkit::ReferenceMap references;
  EXPECT_THROW(sourcemeta::jsontoolkit::frame(
                   document, frame, references,
                   sourcemeta::jsontoolkit::default_schema_walker,
                   sourcemeta::jsontoolkit::official_resolver)
                   .wait(),
               sourcemeta::jsontoolkit::SchemaError);
}

TEST(JSONSchema_frame_2019_09, explicit_argument_id_same) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame frame;
  sourcemeta::jsontoolkit::ReferenceMap references;
  sourcemeta::jsontoolkit::frame(document, frame, references,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver,
                                 "https://json-schema.org/draft/2019-09/schema",
                                 "https://www.sourcemeta.com/schema")
      .wait();

  EXPECT_EQ(frame.size(), 3);
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema",
                              "https://www.sourcemeta.com/schema", "",
                              "https://www.sourcemeta.com/schema", "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema#/$id",
                              "https://www.sourcemeta.com/schema", "/$id",
                              "https://www.sourcemeta.com/schema", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame,
                              "https://www.sourcemeta.com/schema#/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema");

  // References

  EXPECT_TRUE(references.empty());
}

TEST(JSONSchema_frame_2019_09, anchor_top_level) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$anchor": "foo"
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame frame;
  sourcemeta::jsontoolkit::ReferenceMap references;
  sourcemeta::jsontoolkit::frame(document, frame, references,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  EXPECT_EQ(frame.size(), 5);

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema",
                              "https://www.sourcemeta.com/schema", "",
                              "https://www.sourcemeta.com/schema", "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema#/$id",
                              "https://www.sourcemeta.com/schema", "/$id",
                              "https://www.sourcemeta.com/schema", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame,
                              "https://www.sourcemeta.com/schema#/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema");
  EXPECT_FRAME_STATIC_2019_09(frame,
                              "https://www.sourcemeta.com/schema#/$anchor",
                              "https://www.sourcemeta.com/schema", "/$anchor",
                              "https://www.sourcemeta.com/schema", "/$anchor");

  // Anchors

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema#foo",
                              "https://www.sourcemeta.com/schema", "",
                              "https://www.sourcemeta.com/schema", "");

  // References

  EXPECT_TRUE(references.empty());
}

TEST(JSONSchema_frame_2019_09, explicit_argument_id_different) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": {
      "$anchor": "foo"
    },
    "properties": {
      "one": {
        "$id": "test",
        "$anchor": "bar"
      },
      "two": {
        "$id": "https://www.test.com",
        "$anchor": "baz"
      }
    }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame frame;
  sourcemeta::jsontoolkit::ReferenceMap references;
  sourcemeta::jsontoolkit::frame(document, frame, references,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver,
                                 "https://json-schema.org/draft/2019-09/schema",
                                 "https://www.example.com")
      .wait();

  EXPECT_EQ(frame.size(), 38);

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema",
                              "https://www.sourcemeta.com/schema", "",
                              "https://www.sourcemeta.com/schema", "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/test",
                              "https://www.sourcemeta.com/schema",
                              "/properties/one",
                              "https://www.sourcemeta.com/test", "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.example.com",
                              "https://www.sourcemeta.com/schema", "",
                              "https://www.sourcemeta.com/schema", "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.example.com/test",
                              "https://www.sourcemeta.com/schema",
                              "/properties/one", "https://www.example.com/test",
                              "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.test.com",
                              "https://www.sourcemeta.com/schema",
                              "/properties/two", "https://www.test.com", "");

  // Anchors

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema#foo",
                              "https://www.sourcemeta.com/schema", "/items",
                              "https://www.sourcemeta.com/schema", "/items");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.example.com#foo",
                              "https://www.sourcemeta.com/schema", "/items",
                              "https://www.example.com", "/items");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/test#bar",
                              "https://www.sourcemeta.com/schema",
                              "/properties/one",
                              "https://www.sourcemeta.com/test", "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.example.com/test#bar",
                              "https://www.sourcemeta.com/schema",
                              "/properties/one", "https://www.example.com/test",
                              "");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.test.com#baz",
                              "https://www.sourcemeta.com/schema",
                              "/properties/two", "https://www.test.com", "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema#/$id",
                              "https://www.sourcemeta.com/schema", "/$id",
                              "https://www.example.com", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame,
                              "https://www.sourcemeta.com/schema#/$schema",
                              "https://www.sourcemeta.com/schema", "/$schema",
                              "https://www.example.com", "/$schema");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/schema#/items",
                              "https://www.sourcemeta.com/schema", "/items",
                              "https://www.example.com", "/items");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/items/$anchor",
      "https://www.sourcemeta.com/schema", "/items/$anchor",
      "https://www.example.com", "/items/$anchor");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.example.com", "/properties");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/one",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.example.com/test", "");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/one/$id",
      "https://www.sourcemeta.com/schema", "/properties/one/$id",
      "https://www.example.com/test", "/$id");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/one/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/one/$anchor",
      "https://www.example.com/test", "/$anchor");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/two",
      "https://www.sourcemeta.com/schema", "/properties/two",
      "https://www.test.com", "");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/two/$id",
      "https://www.sourcemeta.com/schema", "/properties/two/$id",
      "https://www.test.com", "/$id");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.sourcemeta.com/schema#/properties/two/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/two/$anchor",
      "https://www.test.com", "/$anchor");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/test#/$id",
                              "https://www.sourcemeta.com/schema",
                              "/properties/one/$id",
                              "https://www.example.com/test", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.sourcemeta.com/test#/$anchor",
                              "https://www.sourcemeta.com/schema",
                              "/properties/one/$anchor",
                              "https://www.example.com/test", "/$anchor");
  EXPECT_FRAME_STATIC_2019_09(
      frame, "https://www.test.com#/$id", "https://www.sourcemeta.com/schema",
      "/properties/two/$id", "https://www.test.com", "/$id");
  EXPECT_FRAME_STATIC_2019_09(frame, "https://www.test.com#/$anchor",
                              "https://www.sourcemeta.com/schema",
                              "/properties/two/$anchor", "https://www.test.com",
                              "/$anchor");

  // References

  EXPECT_TRUE(references.empty());
}
