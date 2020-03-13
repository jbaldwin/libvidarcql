#pragma once

#include "priam/Blob.hpp"
#include "priam/CppDriver.hpp"
#include "priam/Decimal.hpp"
#include "priam/Duration.hpp"

#include <cstddef>
#include <ctime>
#include <string>

namespace priam {

class Row;
class ResultList;
class Map;
class Set;
class Tuple;

/**
 * @param type Converts this CassValueType into a std::string representation.
 * @return The string representation.
 */
auto to_string(CassValueType type) -> const std::string&;

/**
 * Encapsulates a column's Value returned from a query.
 *
 * Note the following data type conversions from C* to C++ here:
 * https://datastax.github.io/cpp-driver/topics/basics/
 */
class Value {
    friend Row; ///< For private constructor, Rows can create Values.
    friend ResultList; ///< For private constructor, Lists can create Values.
    friend Map; ///< For private constructor, Maps can create Values.
    friend Set; ///< For private constructor, Sets can create Values.
    friend Tuple; ///< For private constructor, Tuples can create Values.
public:
    Value(const Value&) = delete;
    Value(Value&&) = delete;
    auto operator=(const Value&) -> Value& = delete;
    auto operator=(Value &&) -> Value&& = delete;

    /**
     * Unfortunately the CassValueType does not support 'null'.
     * @return True if the value is null.
     */
    auto IsNull() const -> bool;

    /**
     * @return The data type of this Value.
     */
    auto GetDataType() const -> CassValueType;

    // TODO: implement
    //auto GetCustom() const -> void*;

    /**
     * @return Cassandra column data type 'ascii' into std::string.
     */
    auto GetASCII() const -> std::string;

    /**
     * @return Cassandra column data type 'bigint' into int64_t.
     */
    auto GetBigInt() const -> int64_t;

    /**
     * @return Cassandra column data type 'blob' into Blob.
     */
    auto GetBlob() const -> Blob;

    /**
     * @return Cassandra column data type 'boolean' into bool.
     */
    auto GetBoolean() const -> bool;

    /**
     * @return Cassandra column data type 'counter' into int64_t.
     */
    auto GetCounter() const -> int64_t;

    /**
     * @return Cassandra column data type 'decimal' into Decimal.
     */
    auto GetDecimal() const -> Decimal;

    /**
     * @return Cassandra column data type 'double' into double.
     */
    auto GetDouble() const -> double;

    /**
     * @return Cassandra column data type 'float' into float.
     */
    auto GetFloat() const -> float;

    /**
     * @return Cassandra column data type 'int' into int32_t.
     */
    auto GetInt() const -> int32_t;

    /**
     * @return Cassandra column data type 'text' into std::string.
     */
    auto GetText() const -> std::string;

    /**
     * @return Cassandra column data type 'timestamp' into std::time_t.
     */
    auto GetTimestamp() const -> std::time_t;

    /**
     * @return Cassandra column data type 'timestamp' into a date formatted timestamp.
     */
    auto GetTimestampAsDateFormatted() const -> std::string;

    /**
     * @return Cassandra column data type 'uuid' into std::string uuid.
     */
    auto GetUUID() const -> std::string;

    /**
     * @return Cassandra column data type 'varchar' into std::string.
     */
    auto GetVarChar() const -> std::string;

    /**
     * @return Cassandra column data type 'varint' into Blob.  The blob contains the variable integer raw information.
     */
    auto GetVarInt() const -> Blob;

    /**
     * @return Cassandra column data type 'time uuid' into std::string.
     */
    auto GetTimeUUID() const -> std::string;

    /**
     * @return Cassandra column data type 'inet' into std::string.
     */
    auto GetINet() const -> std::string;

    /**
     * @see http://datastax.github.io/cpp-driver/topics/basics/date_and_time/
     * @return Cassandra data type 'date' into uint32_t.
     */
    auto GetDate() const -> uint32_t;

    /**
     * @see http://datastax.github.io/cpp-driver/topics/basics/date_and_time/
     * @return Cassandra data type 'time' into int64_t.
     */
    auto GetTime() const -> int64_t;

    /**
     * @return Cassandra data type 'smallint' into int16_t.
     */
    auto GetSmallInt() const -> int16_t;

    /**
     * @return Cassandra data type 'tinyint' into int8_t.
     */
    auto GetTinyInt() const -> int8_t;

    /**
     * @return Cassandra data type 'duration' into Duration.
     */
    auto GetDuration() const -> Duration;

    /**
     * @return Cassandra data type 'list' into priam::List.
     */
    auto GetList() const -> priam::ResultList;

    /**
     * @return Cassandra data type 'map' into priam::Map.
     */
    auto GetMap() const -> priam::Map;

    /**
     * @return Cassandra data type 'set' into priam::Set.
     */
    auto GetSet() const -> priam::Set;

    // TODO: implement
    //auto GetUDT() const -> priam::UDT;

    /**
     * @return Cassandra data type 'tuple' into priam::Tuple.
     */
    auto GetTuple() const -> priam::Tuple;

private:
    const CassValue* m_cass_value { nullptr }; ///< The underlying cassandra value for this column/value, this object does not need to be free'ed.

    /**
     * Creates a column/value out of the underlying cassandra column/value.
     * @param cass_column Pointer to the cassandra driver value for this column.
     */
    explicit Value(
        const CassValue* cass_column);
};

} // namespace priam