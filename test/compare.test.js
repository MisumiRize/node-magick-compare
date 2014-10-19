var compare = require('../')
    , test = require('tap').test
    , fs = require('fs')
    , path = require('path')
    , base = fs.readFileSync(path.join(__dirname, 'base.png'))
    ;

test('comparing with same image', function(t) {
    var compareWith = fs.readFileSync(path.join(__dirname, 'base.png'));
    compare(base, compareWith, function(err, isEqual, diff) {
        t.equal(err, null, 'err is null');
        t.equal(isEqual, true, 'is equal');
        t.equal(diff, null, 'diff is null');
        t.end();
    });
});

test('comparing with different image', function(t) {
    var compareWith = fs.readFileSync(path.join(__dirname, 'differ.png'));
    compare(base, compareWith, function(err, isEqual, diff) {
        t.equal(err, null, 'err is null');
        t.equal(isEqual, false, 'is not equal');
        t.equal(Buffer.isBuffer(diff), true, 'diff is buffer');
        t.end();
    });
});
