> [!NOTE]
> Ryu is working on something new!
> 
> We are archiving the RyuGraph project here: https://github.com/ryugraph/ryu/
> 
> For those using Ryu currently, prior Ryu releases will continue to be usable in the same way without modifications to your code.
> 
> If you are also using extensions, moving forward you have two options:
>   1. we have a new release 0.11.3 that bundles many (but not all) of the extensions, so you can migrate to 0.11.3; or
>   2. you can follow the [instructions here](http://ryugraph.github.io/docs/extensions/#host-your-own-extension-server) to run a local extension server.
> 
> Further, some of our resources are moving from our website to GitHub:
>   - Docs: http://ryugraph.github.io/docs
>   - Blog: http://ryugraph.github.io/blog
> 
> We thank you for being early users of Ryu and making Ryu better over the last few years!


# Ryu
Ryu is an embedded graph database built for query speed and scalability. Ryu is optimized for handling complex analytical workloads 
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

Ryu was initially developed by Ryu Inc. It is available under a permissible license.

## Docs and Blog

To learn more about Ryu, see our [Documentation](https://ryugraph.github.io/docs) and [Blog](https://ryugraph.github.io/blog) page.

## Getting Started

Refer to our [Getting Started](https://ryugraph.github.io/docs/get-started/) page for your first example.

## Extensions
Ryu has an extension framework that users can dynamically load the functionality you need at runtime.
We've developed a list of [official extensions](https://ryugraph.github.io/docs/extensions/#available-extensions) that you can use to extend Ryu's functionality.

Ryu requires you to install the extension before loading and using it.
Note that Ryu no longer provides the official extension server, where you can directly install any official extensions.

If you've upgraded to the latest version v0.11.3, Ryu has pre-installed four commonly used extensions (`algo`, `fts`, `json`, `vector`) for you.
You do not need to manually INSTALL these extensions.

For Ryu versions before v0.11.3, or to install extensions that haven't been pre-installed, you have to set up a local extension server.
The instructions of setting up a local extension server can be found below.

### Host your own extension server

The extension server is based on NGINX and is hosted on [GitHub](https://ghcr.io/ryugraph/extension-repo). You can pull the Docker image and run it in your environment:

```bash
docker pull ghcr.io/ryugraph/extension-repo:latest
docker run -d -p 8080:80 ghcr.io/ryugraph/extension-repo:latest
```

In this example, the extension server will be available at `http://localhost:8080`. You can then install extensions from your server by appending the `FROM` clause to the `INSTALL` command:

```cypher
INSTALL <EXTENSION_NAME> FROM 'http://localhost:8080/';
```

## Build from Source

You can build from source using the instructions provided in the [developer guide](https://ryugraph.github.io/docs/developer-guide).

## License
Ryu is licensed under the [MIT License](LICENSE).
