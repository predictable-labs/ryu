# Ryu
Ryu, a fork of Kuzu, is an embedded graph database built for query speed and scalability. Ryu is optimized for handling complex analytical workloads 
on very large databases and provides a set of retrieval features, such as a full text search and vector indices. Our core feature set includes:

- Flexible Property Graph Data Model and Cypher query language
- Embeddable, serverless integration into applications
- Native full text search and vector index
- Columnar disk-based storage
- Columnar sparse row-based (CSR) adjacency list/join indices
- Vectorized and factorized query processor
- Novel and very fast join algorithms
- Multi-core query parallelism
- Serializable ACID transactions
- Wasm (WebAssembly) bindings for fast, secure execution in the browser

Ryu or RyuGraph is a fork of [Kuzu](https://github.com/kuzudb/kuzu), which was originally developed by Kuzu Inc. Following Kuzu's archival in October 2025, Ryu continues the development and evolution of this embedded graph database technology. Ryu is maintained by Predictable Labs, Inc. and is available under a permissible license, ensuring the continued advancement of this innovative graph database solution.

## Docs and Blog

To learn more about Ryu, see our [Documentation](https://ryugraph.io/docs) and [Blog](https://ryugraph.io/blog) page.

## Getting Started

Refer to our [Getting Started](https://ryugraph.io/docs/get-started/) page for your first example.

## Extensions
Ryu has an extension framework that users can dynamically load the functionality you need at runtime.
We've developed a list of [official extensions](https://ryugraph.io/docs/extensions/#available-extensions) that you can use to extend Ryu's functionality.

Ryu requires you to install the extension before loading and using it.
Note that Ryu no longer provides the official extension server, where you can directly install any official extensions.

If you've upgraded to the latest version v0.11.3, Ryu has pre-installed four commonly used extensions (`algo`, `fts`, `json`, `vector`) for you.
You do not need to manually INSTALL these extensions.

For Ryu versions before v0.11.3, or to install extensions that haven't been pre-installed, you have to set up a local extension server.
The instructions of setting up a local extension server can be found below.

### Host your own extension server

The extension server is based on NGINX and is hosted on [GitHub Container Registry](https://ghcr.io/predictable-labs/extension-repo). You can pull the Docker image and run it in your environment:

```bash
docker pull ghcr.io/predictable-labs/extension-repo:latest
docker run -d -p 8080:80 ghcr.io/predictable-labs/extension-repo:latest
```

In this example, the extension server will be available at `http://localhost:8080`. You can then install extensions from your server by appending the `FROM` clause to the `INSTALL` command:

```cypher
INSTALL <EXTENSION_NAME> FROM 'http://localhost:8080/';
```

## Build from Source

You can build from source using the instructions provided in the [developer guide](https://ryugraph.io/docs/developer-guide).

## License
Ryu is licensed under the [MIT License](LICENSE).
