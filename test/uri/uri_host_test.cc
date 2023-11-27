#include <gtest/gtest.h>
#include <sourcemeta/jsontoolkit/uri.h>

TEST(URI_host, https_example_url) {
  const sourcemeta::jsontoolkit::URI uri{"https://example.com/foo"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "example.com");
}

TEST(URI_host, https_example_url_with_port) {
  const sourcemeta::jsontoolkit::URI uri{"https://example.com:8000/foo"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "example.com");
}

TEST(URI_host, relative_url) {
  const sourcemeta::jsontoolkit::URI uri{"../foo"};
  EXPECT_FALSE(uri.host().has_value());
}
