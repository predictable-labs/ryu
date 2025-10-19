/**
 * This file is a customized loader for the ryujs.node native module.
 * It is used to load the native module with the correct flags on Linux so that
 * extension loading works correctly.
 * @module ryu_native
 * @private
 */

const process = require("process");
const constants = require("constants");
const join = require("path").join;

const ryuNativeModule = { exports: {} };
const modulePath = join(__dirname, "ryujs.node");
if (process.platform === "linux") {
  process.dlopen(
    ryuNativeModule,
    modulePath,
    constants.RTLD_LAZY | constants.RTLD_GLOBAL
  );
} else {
  process.dlopen(ryuNativeModule, modulePath);
}

module.exports = ryuNativeModule.exports;
