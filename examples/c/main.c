#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "ryu.h"

int main() {
    ryu_database db;
    ryu_connection conn;
    ryu_database_init("" /* fill db path */, ryu_default_system_config(), &db);
    ryu_connection_init(&db, &conn);

    // Create schema.
    ryu_query_result result;
    ryu_connection_query(
        &conn, "CREATE NODE TABLE Person(name STRING, age INT64, PRIMARY KEY(name));", &result);
    ryu_query_result_destroy(&result);
    // Create nodes.
    ryu_connection_query(&conn, "CREATE (:Person {name: 'Alice', age: 25});", &result);
    ryu_query_result_destroy(&result);
    ryu_connection_query(&conn, "CREATE (:Person {name: 'Bob', age: 30});", &result);
    ryu_query_result_destroy(&result);

    // Execute a simple query.
    ryu_connection_query(&conn, "MATCH (a:Person) RETURN a.name AS NAME, a.age AS AGE;", &result);

    // Fetch each value.
    ryu_flat_tuple tuple;
    ryu_value value;
    while (ryu_query_result_has_next(&result)) {
        ryu_query_result_get_next(&result, &tuple);

        ryu_flat_tuple_get_value(&tuple, 0, &value);
        char* name;
        ryu_value_get_string(&value, &name);

        ryu_flat_tuple_get_value(&tuple, 1, &value);
        int64_t age;
        ryu_value_get_int64(&value, &age);

        printf("name: %s, age: %" PRIi64 " \n", name, age);
        ryu_destroy_string(name);
    }
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&tuple);

    // Print query result.
    char* result_string = ryu_query_result_to_string(&result);
    printf("%s", result_string);
    ryu_destroy_string(result_string);

    ryu_query_result_destroy(&result);
    ryu_connection_destroy(&conn);
    ryu_database_destroy(&db);
    return 0;
}
