#include "compare.h"

CompareWorker::CompareWorker(NanCallback *callback, Image *image, Image *compare)
    : NanAsyncWorker(callback), _image(*image), _compare(*compare) {}

CompareWorker::~CompareWorker() {}

void CompareWorker::Execute() {
    _result = _image.compare(_compare);

    if (!_result) {
        _image.composite(_compare, Magick::ForgetGravity, Magick::DifferenceCompositeOp);
        _image.write(&_diff);
    }
}

void CompareWorker::HandleOKCallback() {
    NanScope();

    Local<Value> argv[2];
    argv[0] = NanNew<Boolean>(_result);
    if (_result) {
        argv[1] = NanNull();
    } else {
        argv[1] = NanBufferUse(static_cast<char*>(const_cast<void*>(_diff.data())), _diff.length());
    }

    callback->Call(2, argv);
}

NAN_METHOD(Compare) {
    NanScope();

    if (args.Length() != 3) {
        return NanThrowError("compare() requires 3 arguments");
    }
    if (!args[0]->IsObject()) {
        return NanThrowError("compare()'s 1st argument should be an object");
    }
    if (!args[1]->IsObject()) {
        return NanThrowError("compare()'s 2nd argument should be an object");
    }
    if (!args[2]->IsFunction()) {
        return NanThrowError("compare()'s 3rd argument should be a function");
    }

    Local<Object> imageSrcData = Local<Object>::Cast(args[0]);
    Blob imageSrcBlob(Data(imageSrcData), Length(imageSrcData));
    Image image;
    image.read(imageSrcBlob);

    Local<Object> compareSrcData = Local<Object>::Cast(args[1]);
    Blob compareSrcBlob(Data(compareSrcData), Length(compareSrcData));
    Image compare;
    compare.read(compareSrcBlob);

    NanCallback *callback = new NanCallback(args[2].As<Function>());

    NanAsyncQueueWorker(new CompareWorker(callback, &image, &compare));

    NanReturnUndefined();
}

void init(Handle<Object> exports) {
    exports->Set(NanNew<String>("compare"),
        NanNew<FunctionTemplate>(Compare)->GetFunction());
}

NODE_MODULE(addon, init)
