#include "priam/Map.h"
#include "priam/Value.h"

namespace priam {

template <typename Functor>
auto Map::ForEachKeyValue(
    Functor&& key_value_callback) -> void
{
    CassIteratorPtr cass_iterator_ptr { cass_iterator_from_map(m_cass_value) };

    while (cass_iterator_next(cass_iterator_ptr.get())) {
        const CassValue* cass_value_key = cass_iterator_get_map_key(cass_iterator_ptr.get());
        const CassValue* cass_value_value = cass_iterator_get_map_value(cass_iterator_ptr.get());
        if (cass_value_key != nullptr && cass_value_value != nullptr) {
            const priam::Value key { cass_value_key };
            const priam::Value value { cass_value_value };
            key_value_callback(key, value);
        }
    }
}

} // namespace priam
