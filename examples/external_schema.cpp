/**
 * @file
 *
 * @brief Demonstrates validation against a schema loaded from a file.
 *
 */

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <rapidjson/document.h>

#include <valijson/adapters/rapidjson_adapter.hpp>
#include <valijson/utils/rapidjson_utils.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validation_results.hpp>
#include <valijson/validator.hpp>

using std::cerr;
using std::endl;

using valijson::Schema;
using valijson::SchemaParser;
using valijson::Validator;
using valijson::ValidationResults;
using valijson::adapters::RapidJsonAdapter;

int main(int argc, char *argv[])
{
    // Load the document containing the schema
    rapidjson::Document schemaDocument;
    if (!valijson::utils::loadDocument(argv[1], schemaDocument)) {
        cerr << "Failed to load schema document." << endl;
        return 1;
    }

    // Load the document that is to be validated
    rapidjson::Document targetDocument;
    if (!valijson::utils::loadDocument(argv[2], targetDocument)) {
        cerr << "Failed to load target document." << endl;
        return 1;
    }

    // Parse the json schema into an internal schema format
    Schema schema;
    SchemaParser parser;
    RapidJsonAdapter schemaDocumentAdapter(schemaDocument);
    try {
        parser.populateSchema(schemaDocumentAdapter, schema);
    } catch (...) {
        cerr << "Failed to parse schema." << endl;
        return 1;
    }

    // Perform validation
    Validator validator(schema);
    validator.setStrict(false);
    ValidationResults results;
    RapidJsonAdapter targetDocumentAdapter(targetDocument);
    if (!validator.validate(targetDocumentAdapter, &results)) {
        std::cerr << "Validation failed." << endl;
        ValidationResults::Error error;
        unsigned int errorNum = 1;
        while (results.popError(error)) {
            cerr << "Error #" << errorNum << std::endl
                 << "  context: " << error.context << endl
                 << "  desc:    " << error.description << endl;
            ++errorNum;
        }
        return 1;
    }

    return 0;
}