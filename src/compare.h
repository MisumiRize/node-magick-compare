#include <Magick++.h>
#include <node.h>
#include <nan.h>

using v8::Value;
using v8::String;
using v8::Boolean;
using v8::Object;
using v8::Local;
using v8::Handle;
using v8::Function;
using v8::FunctionTemplate;
using node::Buffer::Data;
using node::Buffer::Length;
using Magick::Image;
using Magick::Blob;

class CompareWorker : public NanAsyncWorker {
 public:
    CompareWorker(NanCallback *callback, Image *image, Image *compare);
    ~CompareWorker();
    void Execute();
    void HandleOKCallback();

 private:
    Image _image;
    Image _compare;
    bool _result;
    Blob _diff;
};
