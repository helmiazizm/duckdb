//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/catalog/catalog_entry_retriever.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include <functional>
#include "duckdb/common/enums/catalog_type.hpp"
#include "duckdb/common/enums/on_entry_not_found.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/parser/query_error_context.hpp"
#include "duckdb/catalog/catalog_entry/schema_catalog_entry.hpp"
#include "duckdb/catalog/catalog_search_path.hpp"
#include "duckdb/catalog/entry_lookup_info.hpp"

namespace duckdb {

class ClientContext;
class Catalog;
class CatalogEntry;

using catalog_entry_callback_t = std::function<void(CatalogEntry &)>;

// Wraps the Catalog::GetEntry method
class CatalogEntryRetriever {
public:
	explicit CatalogEntryRetriever(ClientContext &context) : context(context) {
	}
	CatalogEntryRetriever(const CatalogEntryRetriever &other) : callback(other.callback), context(other.context) {
	}

public:
	void Inherit(const CatalogEntryRetriever &parent);
	ClientContext &GetContext() {
		return context;
	}

	optional_ptr<CatalogEntry> GetEntry(const string &catalog, const string &schema, const EntryLookupInfo &lookup_info,
	                                    OnEntryNotFound on_entry_not_found = OnEntryNotFound::THROW_EXCEPTION);

	optional_ptr<CatalogEntry> GetEntry(Catalog &catalog, const string &schema, const EntryLookupInfo &lookup_info,
	                                    OnEntryNotFound on_entry_not_found = OnEntryNotFound::THROW_EXCEPTION);

	LogicalType GetType(const string &catalog, const string &schema, const string &name,
	                    OnEntryNotFound on_entry_not_found = OnEntryNotFound::RETURN_NULL);
	LogicalType GetType(Catalog &catalog, const string &schema, const string &name,
	                    OnEntryNotFound on_entry_not_found = OnEntryNotFound::RETURN_NULL);

	optional_ptr<SchemaCatalogEntry> GetSchema(const string &catalog, const EntryLookupInfo &schema_lookup,
	                                           OnEntryNotFound on_entry_not_found = OnEntryNotFound::THROW_EXCEPTION);

	const CatalogSearchPath &GetSearchPath() const;
	void SetSearchPath(vector<CatalogSearchEntry> entries);

	void SetCallback(catalog_entry_callback_t callback);
	catalog_entry_callback_t GetCallback();

	optional_ptr<BoundAtClause> GetAtClause() const;
	void SetAtClause(optional_ptr<BoundAtClause> at_clause);

private:
	optional_ptr<CatalogEntry> ReturnAndCallback(optional_ptr<CatalogEntry> result);

private:
	//! (optional) callback, called on every successful entry retrieval
	catalog_entry_callback_t callback = nullptr;
	ClientContext &context;
	shared_ptr<CatalogSearchPath> search_path;
	optional_ptr<BoundAtClause> at_clause;
};

} // namespace duckdb
