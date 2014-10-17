var compare = require('./build/Release/compare').compare
    , _ = require('lodash')
    ;

DEFAULT_OPTIONS = {
    supress: true
};

module.exports = function(base, compareWith, options, callback) {
    if (typeof options === 'function') {
        callback = options;
        options = {};
    }
    compare(base, compareWith, _.merge(DEFAULT_OPTIONS, options), callback);
};
