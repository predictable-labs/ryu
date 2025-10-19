#!/usr/bin/env python3

import timeit
import sys

import ryu

query = sys.stdin.read()
print(query)

db = ryu.Database(":memory:")
con = ryu.Connection(db)
ver = con.execute("call DB_Version() return *;").get_next()[0]
db = ryu.Database(f"mydb-{ver}", read_only=True)
con = ryu.Connection(db)
for try_num in range(3):
    start = timeit.default_timer()
    results = con.execute(query.replace('\\', '\\\\'))
    end = timeit.default_timer()
    print(end - start)
