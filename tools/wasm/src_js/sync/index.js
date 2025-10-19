/**
 * @file index.js is the root file for the synchronous version of Ryu
 * WebAssembly module. It exports the module's public interface.
 */
"use strict";

const RyuWasm = require("./ryu.js");
const Database = require("./database.js");
const Connection = require("./connection.js");
const PreparedStatement = require("./prepared_statement.js");
const QueryResult = require("./query_result.js");

/**
 * The synchronous version of Ryu WebAssembly module.
 * @module ryu-wasm
 */
module.exports = {
  /**
   * Initialize the Ryu WebAssembly module.
   * @memberof module:ryu-wasm
   * @returns {Promise<void>} a promise that resolves when the module is 
   * initialized. The promise is rejected if the module fails to initialize.
   */
  init: () => {
    return RyuWasm.init();
  },

  /**
   * Get the version of the Ryu WebAssembly module.
   * @memberof module:ryu-wasm
   * @returns {String} the version of the Ryu WebAssembly module.
   */
  getVersion: () => {
    return RyuWasm.getVersion();
  },

  /**
   * Get the storage version of the Ryu WebAssembly module.
   * @memberof module:ryu-wasm
   * @returns {BigInt} the storage version of the Ryu WebAssembly module.
   */
  getStorageVersion: () => {
    return RyuWasm.getStorageVersion();
  },
  
  /**
   * Get the standard emscripten filesystem module (FS). Please refer to the 
   * emscripten documentation for more information.
   * @memberof module:ryu-wasm
   * @returns {Object} the standard emscripten filesystem module (FS).
   */
  getFS: () => {
    return RyuWasm.getFS();
  },

  /**
   * Get the WebAssembly memory. Please refer to the emscripten documentation 
   * for more information.
   * @memberof module:ryu-wasm
   * @returns {Object} the WebAssembly memory object.
   */
  getWasmMemory: () => {
    return RyuWasm.getWasmMemory();
  },

  Database,
  Connection,
  PreparedStatement,
  QueryResult,
};
