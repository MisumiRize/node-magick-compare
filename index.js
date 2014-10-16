var compare = require('./build/Release/compare').compare
    , merge = require('merge')
    ;

DEFAULT_OPTIONS = {
    supress: true
};

module.exports = function(base, compareTo, options, callback) {
    compare(base, compareTo, merge(DEFAULT_OPTIONS, options), callback);
};
