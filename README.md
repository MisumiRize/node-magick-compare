magick-compare [![Build Status](https://travis-ci.org/MisumiRize/node-magick-compare.svg?branch=master)](https://travis-ci.org/MisumiRize/node-magick-compare)
===================

Compare image using ImageMagick's Magick++

## Installation

Install with npm:

```
$ npm install magick-compare
```

## Example

### Basic

```javascript
var compare = require('compare')
    , fs = require('fs')
    ;

fs.readFile('./foo.png', function(_, base) {
    fs.readFile('./bar.png', function(_, compareWith) {
        compare(base, compareWith, function(err, isSame, diff) {
            console.log(isSame);
            console.log(diff);
        });
    });
});
```
