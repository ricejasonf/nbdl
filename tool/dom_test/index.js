var fs      = require('fs');
var process = require('process');
var jsdom   = require('jsdom');
var ws      = require('ws');

global.initDom = initDom;
global.initDomEquality = initDomEquality;
global.checkDomEquals = checkDomEquals;
global.triggerEvent = triggerEvent;
initDom('')

var Module = {
  ENVIRONMENT: 'NODE',
  "arguments": []
};

// "include" the test js file
eval(fs.readFileSync(process.argv[2], 'utf8'));

// emscripten set exit status
if (!Module.noExitRuntime)
{
  if (typeof EXITSTATUS == 'undefined')
    EXITSTATUS = 1
  process.exit(EXITSTATUS);
}

/**************************/

function initDom(body)
{
  global.document = jsdom.jsdom('<!doctype html><html><body>' + body + '</body></html>', {
    url: 'http://foo.com'
  });
  global.window   = global.document.defaultView;
  global.window.WebSocket = ws;

  var originalPushState = global.window.history.pushState;
  global.window.history.pushState = function(_1, _2, loc)
  {
    originalPushState.call(global.window.history, _1, _2, loc);
    if (global.window.onpopstate)
    {
      global.window.onpopstate({ target: global.window });
    }
  }

  for (var prop in global.window)
  {
    if (!global[prop]) 
    {
      global[prop] = global.window[prop];
    }
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

  if (s[0].outerHTML != s[1].outerHTML)
  {
    console.error('EXPECTED: ', s[0].outerHTML);
    console.error('RESULT:   ', s[1].outerHTML);
    return false;
  }
  else
  {
    return true;
  }

  // Note that `isEqualNode` was returning false in the case of multiple textNodes
  //return s[0].isEqualNode(s[1]);
}

function triggerEvent(event_name, id)
{
  var el = test_target.querySelector('#' + id);
  var event = global.document.createEvent('Event');
  event.initEvent(event_name, true, true);
  el.dispatchEvent(event);
}
