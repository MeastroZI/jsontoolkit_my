#include <gtest/gtest.h>
#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>

static auto EXPECT_SCHEMA(std::string_view identifier) -> void {
  const std::optional<sourcemeta::jsontoolkit::JSON> result{
      sourcemeta::jsontoolkit::official_resolver(identifier).get()};
  EXPECT_TRUE(result.has_value());
  const sourcemeta::jsontoolkit::JSON &document{result.value()};
  EXPECT_TRUE(sourcemeta::jsontoolkit::is_schema(document));
  std::optional<std::string> id{sourcemeta::jsontoolkit::id(document)};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), identifier);
}

TEST(JSONSchema, default_resolver_jsonschema_2020_12) {
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/applicator");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/content");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/core");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/format-annotation");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/format-assertion");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/hyper-schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/meta-data");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/unevaluated");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/validation");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/links");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/output/schema");
}

TEST(JSONSchema, default_resolver_jsonschema_2019_09) {
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/applicator");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/content");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/core");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/format");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/hyper-schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/meta-data");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/validation");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/output/schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/links");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/output/hyper-schema");
}

TEST(JSONSchema, default_resolver_jsonschema_draft7) {
  EXPECT_SCHEMA("http://json-schema.org/draft-07/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/links#");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/hyper-schema-output");
}

TEST(JSONSchema, default_resolver_jsonschema_draft6) {
  EXPECT_SCHEMA("http://json-schema.org/draft-06/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-06/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-06/links#");
}

TEST(JSONSchema, default_resolver_jsonschema_draft4) {
  EXPECT_SCHEMA("http://json-schema.org/draft-04/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-04/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-04/links#");
}

TEST(JSONSchema, default_resolver_jsonschema_draft3) {
  EXPECT_SCHEMA("http://json-schema.org/draft-03/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-03/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-03/links#");
  EXPECT_SCHEMA("http://json-schema.org/draft-03/json-ref#");
}

TEST(JSONSchema, default_resolver_jsonschema_draft2) {
  EXPECT_SCHEMA("http://json-schema.org/draft-02/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-02/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-02/links#");
  EXPECT_SCHEMA("http://json-schema.org/draft-02/json-ref#");
}

TEST(JSONSchema, default_resolver_jsonschema_draft1) {
  EXPECT_SCHEMA("http://json-schema.org/draft-01/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-01/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-01/links#");
  EXPECT_SCHEMA("http://json-schema.org/draft-01/json-ref#");
}

TEST(JSONSchema, default_resolver_jsonschema_draft0) {
  EXPECT_SCHEMA("http://json-schema.org/draft-00/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-00/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-00/links#");
  EXPECT_SCHEMA("http://json-schema.org/draft-00/json-ref#");
}

TEST(JSONSchema, default_resolver_idempotency) {
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema, default_resolver_invalid) {
  const std::optional<sourcemeta::jsontoolkit::JSON> result{
      sourcemeta::jsontoolkit::official_resolver("https://example.com/foobar")
          .get()};
  EXPECT_FALSE(result.has_value());
}
