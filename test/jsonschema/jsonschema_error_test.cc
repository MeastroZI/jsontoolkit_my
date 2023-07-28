#include <gtest/gtest.h>
#include <sourcemeta/jsontoolkit/jsonschema/error.h>

#include <exception>   // std::exception
#include <string>      // std::string
#include <type_traits> // std::is_base_of_v

TEST(jsonschema, schema_error_throw) {
  static_assert(
      std::is_base_of_v<std::exception, sourcemeta::jsontoolkit::SchemaError>,
      "Must subclass std::exception");
  auto exception{sourcemeta::jsontoolkit::SchemaError("My error")};
  EXPECT_THROW(throw exception, sourcemeta::jsontoolkit::SchemaError);
  EXPECT_EQ(std::string{exception.what()}, "My error");
}

TEST(jsonschema, resolution_error_throw) {
  static_assert(std::is_base_of_v<std::exception,
                                  sourcemeta::jsontoolkit::ResolutionError>,
                "Must subclass std::exception");
  auto exception{sourcemeta::jsontoolkit::ResolutionError(
      "https://sourcemeta.com/test", "My error")};
  EXPECT_THROW(throw exception, sourcemeta::jsontoolkit::ResolutionError);
  EXPECT_EQ(std::string{exception.what()}, "My error");
  EXPECT_EQ(exception.id(), "https://sourcemeta.com/test");
}