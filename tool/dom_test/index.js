var fs      = require('fs');
var process = require('process');
var jsdom   = require('jsdom');

global.document = jsdom.jsdom('<!doctype html<html><body></body></html>');
global.window   = global.document.defaultView;

for (var prop in global.window)
{
  global[prop] = global.window[prop];
}

var Module = {
  ENVIRONMENT: 'WEB', // or 'NODE' maybe
  "arguments": []
};

// "include" the test js file
eval(fs.readFileSync(process.argv[2], 'utf8'));

// emscripten set exit status
if (typeof EXITSTATUS == 'undefined')
  EXITSTATUS = 1
process.exit(EXITSTATUS);
