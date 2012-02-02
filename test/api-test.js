var hogan = require('../'),
    assert = require('assert');

suite('API test', function() {
  test('only string', function() {
    assert.equal(hogan.compile('just a string').render(), 'just a string');
  });

  test('prop and string', function() {
    assert.equal(hogan.compile('just {{a}} string').render({
      a: 'not a'
    }), 'just not a string');
  });

  test('if', function() {
    var t = hogan.compile('just {{#prop}}a{{^prop}}not a{{/prop}} string');

    assert.equal(t.render({ prop: true }), 'just a string');
    assert.equal(t.render({ prop: false }), 'just not a string');
    assert.equal(t.render({ prop: [1, 2, 3] }), 'just aaa string');
  });
});
