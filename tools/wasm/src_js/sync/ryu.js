/**
 * @file ryu.js is the internal wrapper for the WebAssembly module.
 */
const ryu_wasm = require("../ryu/ryu_wasm.js");

class ryu {
  constructor() {
    this._ryu = null;
  }

  async init() {
    this._ryu = await ryu_wasm();
  }

  checkInit() {
    if (!this._ryu) {
      throw new Error("The WebAssembly module is not initialized.");
    }
  }

  getVersion() {
    this.checkInit();
    return this._ryu.getVersion();
  }

  getStorageVersion() {
    this.checkInit();
    return this._ryu.getStorageVersion();
  }

  getFS() {
    this.checkInit();
    return this._ryu.FS;
  }

  getWasmMemory() {
    this.checkInit();
    return this._ryu.wasmMemory;
  }
}

const ryuInstance = new ryu();
module.exports = ryuInstance;
