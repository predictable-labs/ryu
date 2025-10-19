const { assert } = require("chai");

describe("Get version", function () {
  it("should get the version of the library", function () {
    assert.isString(ryu.VERSION);
    assert.notEqual(ryu.VERSION, "");
  });

  it("should get the storage version of the library", function () {
    assert.isNumber(ryu.STORAGE_VERSION);
    assert.isAtLeast(ryu.STORAGE_VERSION, 1);
  });
});
