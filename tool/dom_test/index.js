var fs      = require('fs');
var process = require('process');
var jsdom   = require('jsdom');

global.initDom = initDom;
global.initDomEquality = initDomEquality;
global.checkDomEquals = checkDomEquals;
initDom('')

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

/**************************/

function initDom(body)
{
  global.document = jsdom.jsdom('<!doctype html><html><body>' + body + '</body></html>');
  global.window   = global.document.defaultView;

  for (var prop in global.window)
  {
    global[prop] = global.window[prop];
  }
}

function initDomEquality(expected)
{
  initDom("<section>" + expected + "</section><section></section>");
  var test_sections = global.document.getElementsByTagName("section");
  global.test_sections = test_sections;
  global.test_target = test_sections[1];
}

function checkDomEquals()
{
  var s = global.test_sections;

  return s[0].outerHTML == s[1].outerHTML;

  // Note that `isEqualNode` was returning false in the case of multiple textNodes
  //return s[0].isEqualNode(s[1]);
}
