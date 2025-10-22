libneo4j-omni: libneo4j-client for all Neo4j versions
=====================================================

About
-----

neo4j-client is a command shell (CLI) for Neo4j written by Chris
Leishman ([@cleishm](https://github.com/cleishm)).  It supports secure
connections to Neo4j server, sending of statements (including
multiline statements), persistent command history, and rendering of
results to tables or CSV.

neo4j-client utilizes the [Bolt Network Protocol](https://neo4j.com/docs/bolt/current/bolt/), and will work with any server that supports Bolt.
 
This version of neo4j-client is maintained by
[@majensen](https://github.com/majensen), and attempts to stay current
with the latest version of Bolt and the
[Neo4j server](https://neo4j.com). At some time, these improvements
may fold in to the orginal repo at
[https://github.com/cleishm/libneo4j-client](https://github.com/cleishm/libneo4j-client).

The libneo4j-client API is [browseable here](https://majensen.github.io/libneo4j-omni/neo4j-client-doxy.html#).

Features
--------

libneo4j-omni includes all features of
[libneo4j-client](https://github.com/cleishm/libneo4j-client), plus
the following:

- Support for Neo4j versions 3, 4, and 5, including
  - Transactions
  - Database selection
  - Element IDs on graph entities for Neo4j 5+
- Most [Structure-based data
  types](https://neo4j.com/docs/bolt/current/bolt/structure-semantics/)
  returned by the Neo4j server along with Node, Relationship, and
  Path, including
  - Date, Time, LocalTime, DateTime, LocalDateTime, Duration, Point2D, Point3D
- Meaningful rendering of supported data types in the REPL client
- Hack the version negotiation handshake with `--support`, e.g. 
  `neo4j-client --support 6.0,5.7-5.0 <host>`

Requirements
------------

neo4j-client is known to work on GNU/Linux, Mac OS X and FreeBSD. It
supports Neo4j through version 5.


Getting Started
---------------


neo4j-client Usage
------------------

Example interactive usage:

```console
$ neo4j-client -u neo4j localhost
The authenticity of host 'localhost:7687' could not be established.
TLS certificate fingerprint is ded0fd2e893cd0b579f47f7798e10cb68dfa2fd3bc9b3c973157da81bab451d74f9452ba99a9c5f66dadb8a360959e5ebd8abb2d7c81230841e60531a96d268.
Would you like to trust this host (NO/yes/once)? yes
Password: *****
neo4j> :help
Enter commands or cypher statements at the prompt.

Commands always begin with a colon (:) and conclude at the end of the line,
for example `:help`. Statements do not begin with a colon (:), may span
multiple lines, are terminated with a semi-colon (;) and will be sent to
the Neo4j server for evaluation.

Available commands:
:quit                  Exit the shell
:connect '<url>'       Connect to the specified URL
:connect host [port]   Connect to the specified host (and optional port)
:disconnect            Disconnect the client from the server
:export                Display currently exported parameters
:export name=val ...   Export parameters for queries
:unexport name ...     Unexport parameters for queries
:begin [timeout(ms)] [mode(r|w)]
                       Begin an explicit transaction (v3.0+)
:commit                Commit an open transaction (v3.0+)
:rollback              Rollback an open transaction (v3.0+)
:reset                 Reset the session with the server
:set                   Display current option values
:set option=value ...  Set shell options
:unset option ...      Unset shell options
:status                Show the client connection status
:help                  Show usage information
:dbname [name]         View or set database for queries (v4.0+)
:format (table|csv)    Set the output format
:width (<n>|auto)      Set the number of columns in the table output

For more information, see the neo4j-client(1) manpage.
neo4j>
neo4j> :status
Connected to 'neo4j://neo4j@localhost:7687'
neo4j>
neo4j> MATCH (n:Person) RETURN n LIMIT 3;
+----------------------------------------------------------------------------+
| n                                                                          |
+----------------------------------------------------------------------------+
| (:Person{born:1964,name:"Keanu Reeves"})                                   |
| (:Person{born:1967,name:"Carrie-Anne Moss"})                               |
| (:Person{born:1961,name:"Laurence Fishburne"})                             |
+----------------------------------------------------------------------------+
neo4j>
neo4j> :set
 echo=off           // echo non-interactive commands before rendering results
 insecure=no        // do not attempt to establish secure connections
 format=table       // set the output format (`table` or `csv`).
 outfile=           // redirect output to a file
 username="neo4j"   // the default username for connections
 width=auto         // the width to render tables (`auto` for term width)
neo4j>
neo4j> :quit
$
```

Example non-interactive usage:

```console
$ echo "MATCH (n:Person) RETURN n.name AS name, n.born AS born LIMIT 3" | \
        neo4j-client -u neo4j -P localhost > result.csv
Password: *****
$
$ cat result.csv
"name","born"
"Keanu Reeves",1964
"Carrie-Anne Moss",1967
"Laurence Fishburne",1961
$
```

Evaluating source files, e.g.:

```console
$ cat query.cyp
:set echo
:export name='Emil'

// Create a person node if it doesn't exist
begin;
MERGE (:Person {name: {name}});
commit;

// return the total number of people
MATCH (n:Person)
RETURN count(n);
$
$ neo4j-client -u neo4j -p pass -o result.out -i query.cyp
$ cat result.out
+:export name='Emil'
+begin;
+MERGE (:Person {name: {name}});
Nodes created: 1
Properties set: 1
Labels added: 1
+commit;
+MATCH (n:Person)
 RETURN count(n);
"count(n)"
137
$
```


libneo4j-client
---------------

libneo4j-client is a client library for Neo4j, written in C, and intended as a
foundation on which basic tools and drivers for various languages may be built.
libneo4j-client takes care of all the detail of establishing a session with a
Neo4j server, sending statements for evaluation, and retrieving results.

libneo4j-client provides a single C header file, `neo4j-client.h`, for
inclusion in source code using the libneo4j-client API. 

libneo4j-client can be included in your project by linking the library at
compile time, typically using the linking flags
`-lneo4j-client -lssl -lcrypto -lm`.  Alternatively, libneo4j-client ships with
a [pkg-config]( https://wiki.freedesktop.org/www/Software/pkg-config/)
description file, enabling you to obtain the required flags using
`pkg-config --libs libneo4j-client`.


## Example


```C
#include <neo4j-client.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    neo4j_client_init();

    /* use NEO4J_INSECURE when connecting to disable TLS */
    neo4j_connection_t *connection =
            neo4j_connect("neo4j://user:pass@localhost:7687", NULL, NEO4J_INSECURE);
    if (connection == NULL)
    {
        neo4j_perror(stderr, errno, "Connection failed");
        return EXIT_FAILURE;
    }

    neo4j_result_stream_t *results =
            neo4j_run(connection, "RETURN 'hello world'", neo4j_null);
    if (results == NULL)
    {
        neo4j_perror(stderr, errno, "Failed to run statement");
        return EXIT_FAILURE;
    }

    neo4j_result_t *result = neo4j_fetch_next(results);
    if (result == NULL)
    {
        neo4j_perror(stderr, errno, "Failed to fetch result");
        return EXIT_FAILURE;
    }

    neo4j_value_t value = neo4j_result_field(result, 0);
    char buf[128];
    printf("%s\n", neo4j_tostring(value, buf, sizeof(buf)));

    neo4j_close_results(results);
    neo4j_close(connection);
    neo4j_client_cleanup();
    return EXIT_SUCCESS;
}
```

## API

See [the API documentation](https://majensen.github.io/libneo4j-omni/neo4j-client-doxy.html#) on [GitHub Pages](https://majensen.github.io/libneo4j-omni).

Building
--------

Please [download the latest release](
https://github.com/majensen/libneo4j-omni/releases), unpack and then:

```console
$ ./configure
$ make clean check
$ sudo make install
```

libneo4j-client requires OpenSSL, although this can be disabled by invoking
configure with `--without-tls`.

neo4j-client also requires some dependencies to build, including
[libedit](http://thrysoee.dk/editline/) and
[libcypher-parser](https://github.com/cleishm/libcypher-parser). If these are not available,
just the library can be built (without neo4j-client), by invoking configure
with `--disable-tools`.

Building from the GitHub repository requires a few extra steps. Firstly, some
additional tooling is required, including autoconf, automake and libtool.
Assuming these are available, to checkout from GitHub and build:

```console
$ git clone https://github.com/majensen/libneo4j-omni.git
$ cd libneo4j-omni
$ ./autogen.sh
$ ./configure
$ make clean check
$ sudo make install
```

If you encounter warnings or errors during the build, please report them at
<https://github.com/majensen/libneo4j-omni/issues>. If you wish to proceed
despite warnings, please invoke configure with the `--disable-werror`.

NOTE: Recent versions of Mac OS X ship without the OpenSSL header files, and
autoconf doesn't pick this up (yet). If you used the homebrew install method,
this will resolve the issue. If you're using Mac OS X, want to build manually
instead of using homebrew, and you get a build failure related to missing
openssl headers, try the following:

```console
$ brew install openssl
$ ./configure --with-libs=/usr/local/opt/openssl
$ make clean check
$ sudo make install
```

More detail about this workaround can be found via `brew info openssl`.


Support
-------

Please raise your issue on
[GitHub](https://github.com/majensen/libneo4j-omni). Include details
of how to reproduce the bug.

The [libneo4j-client FAQ](https://github.com/cleishm/libneo4j-client/wiki/FAQ)
should be applicable to libneo4j-omni, too.


Contributing
------------

See [/CONTRIBUTING.md](https://github.com/majensen/libneo4j-omni/blob/main/CONTRIBUTING.md)

License
-------

neo4j-client as libneo4j-client is licensed under the [Apache License, Version 2.0](
http://www.apache.org/licenses/LICENSE-2.0).

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.  See the License for the
specific language governing permissions and limitations under the License.

Copyright
---------

libneo4j-omni is (c) 2016-2021 Chris Leishman, with additions (c)
2021-2023 Mark A. Jensen. Use git-blame to determine attribution.
