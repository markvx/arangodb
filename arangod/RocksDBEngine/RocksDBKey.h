////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2017 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Daniel H. Larkin
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGO_ROCKSDB_ROCKSDB_KEY_H
#define ARANGO_ROCKSDB_ROCKSDB_KEY_H 1

#include "Basics/Common.h"
#include "Basics/StringRef.h"
#include "RocksDBEngine/RocksDBTypes.h"
#include "VocBase/vocbase.h"

#include <rocksdb/slice.h>

#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>

namespace arangodb {

class RocksDBKey {
 public:
  RocksDBKey() = delete;
  RocksDBKey(rocksdb::Slice slice)
      : _type(static_cast<RocksDBEntryType>(slice.data()[0])),
        _buffer(slice.data(), slice.size()){};

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified database key
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey Database(TRI_voc_tick_t databaseId);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified collection key
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey Collection(TRI_voc_tick_t databaseId,
                               TRI_voc_cid_t collectionId);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified document key
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey Document(uint64_t objectId, TRI_voc_rid_t revisionId);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified key for an entry in a primary index
  ///
  /// The indexId is an object ID generated by the engine, rather than the
  /// actual index ID.
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey PrimaryIndexValue(uint64_t indexId,
                                      arangodb::StringRef const& primaryKey);

  static RocksDBKey PrimaryIndexValue(uint64_t indexId, char const* primaryKey);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified key for an entry in an edge index
  ///
  /// The indexId is an object ID generated by the engine, rather than the
  /// actual index ID. The edge index should provide two such object IDs, one
  /// for the `_to` sub-index and one for the `_from` sub-index.
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey EdgeIndexValue(uint64_t indexId,
                                   arangodb::StringRef const& vertexId,
                                   arangodb::StringRef const& primaryKey);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified key for an entry in a user-defined,
  /// non-unique index
  ///
  /// The indexId is an object ID generated by the engine, rather than the
  /// actual index ID.
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey IndexValue(uint64_t indexId,
                               arangodb::StringRef const& primaryKey,
                               VPackSlice const& indexValues);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified key for an entry in a unique user-defined
  /// index
  ///
  /// The indexId is an object ID generated by the engine, rather than the
  /// actual index ID.
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey UniqueIndexValue(uint64_t indexId,
                                     VPackSlice const& indexValues);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified key for the fulltext index
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey FulltextIndexValue(uint64_t indexId,
                                       arangodb::StringRef const& word,
                                       arangodb::StringRef const& primaryKey);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified key for a geoIndexValue
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey GeoIndexValue(uint64_t indexId, bool isSlot, uint64_t offset);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified key for a view
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey View(TRI_voc_tick_t databaseId, TRI_voc_cid_t viewId);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified key for a settings value
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey SettingsValue();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified key for a counter value
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey CounterValue(uint64_t objectId);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a fully-specified key for a replication applier config
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBKey ReplicationApplierConfig(TRI_voc_tick_t databaseId);

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Extracts the type from a key
  ///
  /// May be called on any valid key (in our keyspace)
  //////////////////////////////////////////////////////////////////////////////
  static RocksDBEntryType type(RocksDBKey const&);
  static RocksDBEntryType type(rocksdb::Slice const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Extracts the object id
  ///
  /// May be called on any valid key (in our keyspace)
  //////////////////////////////////////////////////////////////////////////////
  static uint64_t counterObjectId(rocksdb::Slice const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Extracts the databaseId from a key
  ///
  /// May be called only on the following key types: Database, Collection,
  /// View. Other types will throw.
  //////////////////////////////////////////////////////////////////////////////
  static TRI_voc_tick_t databaseId(RocksDBKey const&);
  static TRI_voc_tick_t databaseId(rocksdb::Slice const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Extracts the collectionId from a key
  ///
  /// May be called only on the the following key types: Collection.
  /// Other types will throw.
  //////////////////////////////////////////////////////////////////////////////
  static TRI_voc_cid_t collectionId(RocksDBKey const&);
  static TRI_voc_cid_t collectionId(rocksdb::Slice const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Extracts the objectId from a key
  ///
  /// May be called only on the the following key types: Document, 
  /// all kinds of index entries. Other types will throw.
  //////////////////////////////////////////////////////////////////////////////
  static uint64_t objectId(RocksDBKey const&);
  static uint64_t objectId(rocksdb::Slice const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Extracts the viewId from a key
  ///
  /// May be called only on View keys. Other types will throw.
  //////////////////////////////////////////////////////////////////////////////
  static TRI_voc_cid_t viewId(RocksDBKey const&);
  static TRI_voc_cid_t viewId(rocksdb::Slice const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Extracts the revisionId from a key
  ///
  /// May be called only on Document keys. Other types will throw.
  //////////////////////////////////////////////////////////////////////////////
  static TRI_voc_rid_t revisionId(RocksDBKey const&);
  static TRI_voc_rid_t revisionId(rocksdb::Slice const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Extracts the primary key (`_key`) from a key
  ///
  /// May be called only on the following key types: PrimaryIndexValue,
  /// EdgeIndexValue, IndexValue, FulltextIndexValue. Other types will throw.
  //////////////////////////////////////////////////////////////////////////////
  static StringRef primaryKey(RocksDBKey const&);
  static StringRef primaryKey(rocksdb::Slice const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Extracts the vertex ID from a key
  ///
  /// May be called only on EdgeIndexValue keys. Other types will throw.
  //////////////////////////////////////////////////////////////////////////////
  static std::string vertexId(RocksDBKey const&);
  static std::string vertexId(rocksdb::Slice const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Extracts the indexed VelocyPack values from a key
  ///
  /// May be called only on IndexValue and UniqueIndexValue keys. Other types
  /// will throw. Returns only a slice. The value must be copied if one wishes
  /// to retain the values after the underlying string goes out of scope.
  //////////////////////////////////////////////////////////////////////////////
  static VPackSlice indexedVPack(RocksDBKey const&);
  static VPackSlice indexedVPack(rocksdb::Slice const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Extracts the geo pot offset 
  ///
  /// May be called only on GeoIndexValues
  //////////////////////////////////////////////////////////////////////////////
  std::pair<bool, uint32_t> geoValues(rocksdb::Slice const& slice);

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns a reference to the full, constructed key
  //////////////////////////////////////////////////////////////////////////////
  std::string const& string() const;

 private:
  explicit RocksDBKey(RocksDBEntryType type);
  RocksDBKey(RocksDBEntryType type, uint64_t first);
  RocksDBKey(RocksDBEntryType type, uint64_t first, uint64_t second);
  RocksDBKey(RocksDBEntryType type, uint64_t first, VPackSlice const& slice);
  RocksDBKey(RocksDBEntryType type, uint64_t first,
             arangodb::StringRef const& docKey, VPackSlice const& indexData);
  RocksDBKey(RocksDBEntryType type, uint64_t first,
             arangodb::StringRef const& second);
  RocksDBKey(RocksDBEntryType type, uint64_t first, std::string const& second,
             std::string const& third);
  RocksDBKey(RocksDBEntryType type, uint64_t first, arangodb::StringRef const& second,
             arangodb::StringRef const& third);
  RocksDBKey(RocksDBEntryType type, uint64_t objectId, uint32_t index, bool isSlot);

 private:
  static RocksDBEntryType type(char const* data, size_t size);
  static TRI_voc_tick_t databaseId(char const* data, size_t size);
  static TRI_voc_cid_t collectionId(char const* data, size_t size);
  static TRI_voc_cid_t objectId(char const* data, size_t size);
  static TRI_voc_cid_t viewId(char const* data, size_t size);
  static TRI_voc_rid_t revisionId(char const* data, size_t size);
  static StringRef primaryKey(char const* data, size_t size);
  static std::string vertexId(char const* data, size_t size);
  static VPackSlice indexedVPack(char const* data, size_t size);

 private:
  static const char _stringSeparator;
  RocksDBEntryType const _type;
  std::string _buffer;
};

}  // namespace arangodb

#endif
