var PromiseTestFunctions = {
  nbdl_test_js_promise_test1: function() {
    var resolve = Module.NBDL_DETAIL_JS_GET(arguments[0]);
    var reject  = Module.NBDL_DETAIL_JS_GET(arguments[1]);
    var input0  = Module.NBDL_DETAIL_JS_GET(arguments[2]);
    var input1  = Module.NBDL_DETAIL_JS_GET(arguments[3]);
    var input2  = Module.NBDL_DETAIL_JS_GET(arguments[4]);
    var input3  = Module.NBDL_DETAIL_JS_GET(arguments[5]);
    var input4  = Module.NBDL_DETAIL_JS_GET(arguments[6]);
    var input5  = Module.NBDL_DETAIL_JS_GET(arguments[7]);
    var input6  = Module.NBDL_DETAIL_JS_GET(arguments[8]);
    var input7  = Module.NBDL_DETAIL_JS_GET(arguments[9]);

    resolve(input0 + input1 + input2 + input3 + input4 + input5 + input6 + input7);
  }
};

mergeInto(LibraryManager.library, PromiseTestFunctions);
