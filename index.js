var compare = require('./build/Release/compare').compare
    , merge = require('merge')
    ;

DEFAULT_OPTIONS = {
    supress: true
};

module.exports = function(base, compareTo, options, callback) {
    if (typeof options === 'function') {
        callback = options;
        options = {};
    }
    compare(base, compareTo, merge(DEFAULT_OPTIONS, options), callback);
};
