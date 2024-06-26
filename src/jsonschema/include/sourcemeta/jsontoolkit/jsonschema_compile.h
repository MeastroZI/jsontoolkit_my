#ifndef SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_COMPILE_H_
#define SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_COMPILE_H_

#if defined(__EMSCRIPTEN__) || defined(__Unikraft__)
#define SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
#else
#include "jsonschema_export.h"
#endif

#include <sourcemeta/jsontoolkit/jsonschema_reference.h>
#include <sourcemeta/jsontoolkit/jsonschema_resolver.h>
#include <sourcemeta/jsontoolkit/jsonschema_walker.h>

#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonpointer.h>
#include <sourcemeta/jsontoolkit/uri.h>

#include <functional> // std::function
#include <map>        // std::map
#include <optional>   // std::optional, std::nullopt
#include <regex>      // std::regex
#include <string>     // std::string
#include <utility>    // std::move, std::pair
#include <variant>    // std::variant
#include <vector>     // std::vector

namespace sourcemeta::jsontoolkit {

/// @ingroup jsonschema
/// Represents a type of compiler step target
enum class SchemaCompilerTargetType {
  /// An static instance literal
  Instance,

  /// A pointer to the property name of an instance location
  TemplateProperty,

  /// A pointer to an instance
  TemplateInstance
};

/// @ingroup jsonschema
/// Represents a generic compiler step target
using SchemaCompilerTarget = std::pair<SchemaCompilerTargetType, Pointer>;

/// @ingroup jsonschema
/// Represents a compiler step empty value
struct SchemaCompilerValueNone {};

/// @ingroup jsonschema
/// Represents a compiler step JSON value
using SchemaCompilerValueJSON = JSON;

/// @ingroup jsonschema
/// Represents a compiler step string value
using SchemaCompilerValueString = JSON::String;

/// @ingroup jsonschema
/// Represents a compiler step JSON type value
using SchemaCompilerValueType = JSON::Type;

/// @ingroup jsonschema
/// Represents a compiler step ECMA regular expression value. We store both the
/// original string and the regular expression as standard regular expressions
/// do not keep a copy of their original value (which we need for serialization
/// purposes)
using SchemaCompilerValueRegex = std::pair<std::regex, std::string>;

/// @ingroup jsonschema
/// Represents a value in a compiler step
template <typename T>
using SchemaCompilerValue = std::variant<T, SchemaCompilerTarget>;

/// @ingroup jsonschema
/// Represents a compiler assertion step that always fails
struct SchemaCompilerAssertionFail;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks if an object defines a
/// given property
struct SchemaCompilerAssertionDefines;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks if a document is of the
/// given type
struct SchemaCompilerAssertionType;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a string against an ECMA
/// regular expression
struct SchemaCompilerAssertionRegex;

/// @ingroup jsonschema
/// Represents a compiler step that emits a public annotation
struct SchemaCompilerAnnotationPublic;

/// @ingroup jsonschema
/// Represents a compiler step that emits a private annotation
struct SchemaCompilerAnnotationPrivate;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents a disjunction
struct SchemaCompilerLogicalOr;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents a conjunction
struct SchemaCompilerLogicalAnd;

/// @ingroup jsonschema
/// Represents a compiler step that loops over object properties
struct SchemaCompilerLoopProperties;

/// @ingroup jsonschema
/// Represents a compiler step that consists of a mark to jump to
struct SchemaCompilerControlLabel;

/// @ingroup jsonschema
/// Represents a schema compilation result that can be evaluated
using SchemaCompilerTemplate = std::vector<std::variant<
    SchemaCompilerAssertionFail, SchemaCompilerAssertionDefines,
    SchemaCompilerAssertionType, SchemaCompilerAssertionRegex,
    SchemaCompilerAnnotationPublic, SchemaCompilerAnnotationPrivate,
    SchemaCompilerLogicalOr, SchemaCompilerLogicalAnd,
    SchemaCompilerLoopProperties, SchemaCompilerControlLabel>>;

#if !defined(DOXYGEN)
#define DEFINE_STEP_WITH_VALUE(category, name, type)                           \
  struct SchemaCompiler##category##name {                                      \
    const SchemaCompilerTarget target;                                         \
    const Pointer evaluation_path;                                             \
    const std::string keyword_location;                                        \
    const SchemaCompilerValue<type> value;                                     \
    const SchemaCompilerTemplate condition;                                    \
  };

#define DEFINE_STEP_APPLICATOR(category, name)                                 \
  struct SchemaCompiler##category##name {                                      \
    const SchemaCompilerTarget target;                                         \
    const Pointer evaluation_path;                                             \
    const std::string keyword_location;                                        \
    const SchemaCompilerTemplate children;                                     \
    const SchemaCompilerTemplate condition;                                    \
  };

#define DEFINE_CONTROL(name)                                                   \
  struct SchemaCompilerControl##name {                                         \
    const std::size_t id;                                                      \
    const SchemaCompilerTemplate children;                                     \
  };

DEFINE_STEP_WITH_VALUE(Assertion, Fail, SchemaCompilerValueNone)
DEFINE_STEP_WITH_VALUE(Assertion, Defines, SchemaCompilerValueString)
DEFINE_STEP_WITH_VALUE(Assertion, Type, SchemaCompilerValueType)
DEFINE_STEP_WITH_VALUE(Assertion, Regex, SchemaCompilerValueRegex)
DEFINE_STEP_WITH_VALUE(Annotation, Public, SchemaCompilerValueJSON)
DEFINE_STEP_WITH_VALUE(Annotation, Private, SchemaCompilerValueJSON)
DEFINE_STEP_APPLICATOR(Logical, Or)
DEFINE_STEP_APPLICATOR(Logical, And)
DEFINE_STEP_APPLICATOR(Loop, Properties)
DEFINE_CONTROL(Label)

#undef DEFINE_STEP_WITH_VALUE
#undef DEFINE_STEP_APPLICATOR
#undef DEFINE_CONTROL
#endif

#if !defined(DOXYGEN)
struct SchemaCompilerContext;
#endif

/// @ingroup jsonschema
/// A compiler is represented as a function that maps a keyword compiler context
/// into a compiler template. You can provide your own to implement your own
/// keywords
using SchemaCompiler =
    std::function<SchemaCompilerTemplate(const SchemaCompilerContext &)>;

/// @ingroup jsonschema
/// The compiler context is the information you have at your disposal to
/// implement a keyword
struct SchemaCompilerContext {
  /// The schema keyword
  const std::string keyword;
  /// The current subschema
  const JSON &schema;
  /// The schema vocabularies in use
  const std::map<std::string, bool> &vocabularies;
  /// The value of the keyword
  const JSON &value;
  /// The root schema resource
  const JSON &root;
  /// The schema base URI
  const URI base;
  /// The schema location relative to the base URI
  const Pointer relative_pointer;
  /// The schema evaluation path
  const Pointer evaluation_path;
  /// The instance location that the keyword must be evaluated to
  const Pointer instance_location;
  /// The reference frame of the entire schema
  const ReferenceFrame &frame;
  /// The references of the entire schema
  const ReferenceMap &references;
  /// The schema walker in use
  const SchemaWalker walker;
  /// The schema resolver in use
  const SchemaResolver resolver;
  /// The schema compiler in use
  const SchemaCompiler compiler;
  /// The default dialect of the schema
  const std::optional<std::string> &default_dialect;
};

// TODO: Give these functions better names

/// @ingroup jsonschema
/// Helper function to instantiate a value-oriented step
template <typename Step, typename ValueType>
auto make(const SchemaCompilerContext &context, ValueType &&type,
          SchemaCompilerTemplate &&condition,
          const SchemaCompilerTargetType target_type =
              SchemaCompilerTargetType::Instance,
          const std::optional<Pointer> &target_location = std::nullopt)
    -> Step {
  return {{target_type, target_location.value_or(context.instance_location)},
          context.evaluation_path,
          to_uri(context.relative_pointer, context.base).recompose(),
          std::move(type),
          std::move(condition)};
}

/// @ingroup jsonschema
/// Helper function to instantiate an applicator step
template <typename Step>
auto make(const SchemaCompilerContext &context,
          SchemaCompilerTemplate &&children,
          SchemaCompilerTemplate &&condition) -> Step {
  return {{SchemaCompilerTargetType::Instance, context.instance_location},
          context.evaluation_path,
          to_uri(context.relative_pointer, context.base).recompose(),
          std::move(children),
          std::move(condition)};
}

/// @ingroup jsonschema
/// Helper function to instantiate a control step
template <typename Step>
auto make(const std::size_t id, SchemaCompilerTemplate &&children) -> Step {
  return {id, std::move(children)};
}

/// @ingroup jsonschema
/// Represents the mode of evalution
enum class SchemaCompilerEvaluationMode {
  /// Attempt to get to a boolean result as fast as possible, ignoring
  /// everything that is not strictly required (like collecting most
  /// annotations)
  Fast,
  /// Perform a full schema evaluation
  Exhaustive
};

/// @ingroup jsonschema
/// A callback of this type is invoked after evaluating any keyword. The first
/// argument is whether the evaluation was successful or not, the second
/// argument is the actual step that was evaluated, and the third one is the
/// annotation result, if any (otherwise null). A callback can be used to
/// implement arbitrary output formats
using SchemaCompilerEvaluationCallback = std::function<void(
    bool, const SchemaCompilerTemplate::value_type &, const JSON &)>;

// TODO: Support standard output formats. Maybe through pre-made evaluation
// callbacks?

/// @ingroup jsonschema
///
/// This function evaluates a schema compiler template in validation mode,
/// returning a boolean without error information. For example:
///
/// ```cpp
/// #include <sourcemeta/jsontoolkit/json.h>
/// #include <sourcemeta/jsontoolkit/jsonschema.h>
/// #include <cassert>
///
/// const sourcemeta::jsontoolkit::JSON schema =
///     sourcemeta::jsontoolkit::parse(R"JSON({
///   "$schema": "https://json-schema.org/draft/2020-12/schema",
///   "type": "string"
/// })JSON");
///
/// const auto schema_template{sourcemeta::jsontoolkit::compile(
///     schema, sourcemeta::jsontoolkit::default_schema_walker,
///     sourcemeta::jsontoolkit::official_resolver,
///     sourcemeta::jsontoolkit::default_schema_compiler)};
///
/// const sourcemeta::jsontoolkit::JSON instance{"foo bar"};
/// const auto result{sourcemeta::jsontoolkit::evaluate(
///   schema_template, instance)};
/// assert(result);
/// ```
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
evaluate(const SchemaCompilerTemplate &steps, const JSON &instance) -> bool;

/// @ingroup jsonschema
///
/// This function evaluates a schema compiler template, executing the given
/// callback at every step of the way. For example:
///
/// ```cpp
/// #include <sourcemeta/jsontoolkit/json.h>
/// #include <sourcemeta/jsontoolkit/jsonschema.h>
/// #include <cassert>
/// #include <iostream>
///
/// const sourcemeta::jsontoolkit::JSON schema =
///     sourcemeta::jsontoolkit::parse(R"JSON({
///   "$schema": "https://json-schema.org/draft/2020-12/schema",
///   "type": "string"
/// })JSON");
///
/// const auto schema_template{sourcemeta::jsontoolkit::compile(
///     schema, sourcemeta::jsontoolkit::default_schema_walker,
///     sourcemeta::jsontoolkit::official_resolver,
///     sourcemeta::jsontoolkit::default_schema_compiler)};
///
/// static auto callback(bool result,
///   const sourcemeta::jsontoolkit::SchemaCompilerTemplate::value_type &step)
///   -> void {
///   if (result) {
///     std::cout << "Success: ";
///   } else {
///     std::cout << "Failure: ";
///   }
///
///   sourcemeta::jsontoolkit::prettify(
///     sourcemeta::jsontoolkit::to_json({step}), std::cout);
///     std::cout << "\n";
/// }
///
/// const sourcemeta::jsontoolkit::JSON instance{"foo bar"};
/// const auto result{sourcemeta::jsontoolkit::evaluate(
///   schema_template, instance,
///   sourcemeta::jsontoolkit::SchemaCompilerEvaluationMode::Fast,
///   callback)};
///
/// assert(result);
/// ```
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
evaluate(const SchemaCompilerTemplate &steps, const JSON &instance,
         const SchemaCompilerEvaluationMode mode,
         const SchemaCompilerEvaluationCallback &callback) -> bool;

/// @ingroup jsonschema
/// A default compiler that aims to implement every keyword for official JSON
/// Schema dialects.
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT default_schema_compiler(
    const SchemaCompilerContext &) -> SchemaCompilerTemplate;

/// @ingroup jsonschema
///
/// This function compiles an input JSON Schema into a template that can be
/// later evaluated. For example:
///
/// ```cpp
/// #include <sourcemeta/jsontoolkit/json.h>
/// #include <sourcemeta/jsontoolkit/jsonschema.h>
///
/// const sourcemeta::jsontoolkit::JSON schema =
///     sourcemeta::jsontoolkit::parse(R"JSON({
///   "$schema": "https://json-schema.org/draft/2020-12/schema",
///   "type": "string"
/// })JSON");
///
/// const auto schema_template{sourcemeta::jsontoolkit::compile(
///     schema, sourcemeta::jsontoolkit::default_schema_walker,
///     sourcemeta::jsontoolkit::official_resolver,
///     sourcemeta::jsontoolkit::default_schema_compiler)};
///
/// // Evaluate or encode
/// ```
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
compile(const JSON &schema, const SchemaWalker &walker,
        const SchemaResolver &resolver, const SchemaCompiler &compiler,
        const std::optional<std::string> &default_dialect = std::nullopt)
    -> SchemaCompilerTemplate;

/// @ingroup jsonschema
///
/// This function compiles a single subschema into a compiler template as
/// determined by the given pointer. If a URI is given, the compiler will
/// attempt to jump to that corresponding frame entry. Otherwise, it will
/// navigate within the current keyword. This function is not meant to be used
/// directly, but instead as a building block for supporting applicators on
/// compiler functions.
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
compile(const SchemaCompilerContext &context, const Pointer &schema_suffix,
        const Pointer &instance_suffix = empty_pointer,
        const std::optional<std::string> &uri = std::nullopt)
    -> SchemaCompilerTemplate;

/// @ingroup jsonschema
///
/// This function converts a compiler template into JSON. Convenient for storing
/// it or sending it over the wire. For example:
///
/// ```cpp
/// #include <sourcemeta/jsontoolkit/json.h>
/// #include <sourcemeta/jsontoolkit/jsonschema.h>
/// #include <iostream>
///
/// const sourcemeta::jsontoolkit::JSON schema =
///     sourcemeta::jsontoolkit::parse(R"JSON({
///   "$schema": "https://json-schema.org/draft/2020-12/schema",
///   "type": "string"
/// })JSON");
///
/// const auto schema_template{sourcemeta::jsontoolkit::compile(
///     schema, sourcemeta::jsontoolkit::default_schema_walker,
///     sourcemeta::jsontoolkit::official_resolver,
///     sourcemeta::jsontoolkit::default_schema_compiler)};
///
/// const sourcemeta::jsontoolkit::JSON result{
///     sourcemeta::jsontoolkit::to_json(schema_template)};
///
/// sourcemeta::jsontoolkit::prettify(result, std::cout);
/// std::cout << "\n";
/// ```
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
to_json(const SchemaCompilerTemplate &steps) -> JSON;

} // namespace sourcemeta::jsontoolkit

#endif
