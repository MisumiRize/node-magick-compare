#include "compare.h"

CompareWorker::CompareWorker(NanCallback *callback, Image *image, Image *compare)
    : NanAsyncWorker(callback), _image(*image), _compare(*compare) {}

CompareWorker::~CompareWorker() {}

void CompareWorker::SetSupress(bool supress) {
    _supress = supress;
}

void CompareWorker::Execute() {
    try {
        _result = _image.compare(_compare);
    } catch (...) {
        if (_supress) {
            SetErrorMessage("unhandled error");
        } else {
            throw;
        }
    }

    if (!_result) {
        _image.composite(_compare, Magick::ForgetGravity, Magick::DifferenceCompositeOp);
        _image.write(&_diff);
    }
}

void CompareWorker::HandleOKCallback() {
    NanScope();

    Local<Value> argv[3];
    argv[0] = NanNull();
    argv[1] = NanNew<Boolean>(_result);
    if (_result) {
        argv[2] = NanNull();
    } else {
        argv[2] = NanBufferUse(static_cast<char*>(const_cast<void*>(_diff.data())), _diff.length());
    }

    callback->Call(3, argv);
}

NAN_METHOD(Compare) {
    NanScope();

    if (args.Length() != 4) {
        return NanThrowError("compare() requires 4 arguments");
    }
    if (!args[0]->IsObject()) {
        return NanThrowError("compare()'s 1st argument should be an object");
    }
    if (!args[1]->IsObject()) {
        return NanThrowError("compare()'s 2nd argument should be an object");
    }
    if (!args[2]->IsObject()) {
        return NanThrowError("compare()'s 3rd argument should be an object");
    }
    if (!args[3]->IsFunction()) {
        return NanThrowError("compare()'s 4th argument should be a function");
    }

    Local<Object> options = Local<Object>::Cast(args[2]);
    bool supress = options->Get(NanNew<String>("supress"))->BooleanValue();

    Local<Object> imageSrcData = Local<Object>::Cast(args[0]);
    Blob imageSrcBlob(Data(imageSrcData), Length(imageSrcData));
    Image image;
    try {
        image.read(imageSrcBlob);
    } catch (...) {
        if (supress) {
            return NanThrowError("unhandled error");
        }
        throw;
    }

    Local<Object> compareSrcData = Local<Object>::Cast(args[1]);
    Blob compareSrcBlob(Data(compareSrcData), Length(compareSrcData));
    Image compare;
    try {
        compare.read(compareSrcBlob);
    } catch (...) {
        if (supress) {
            return NanThrowError("unhandled error");
        }
        throw;
    }

    NanCallback *callback = new NanCallback(args[3].As<Function>());
    CompareWorker *worker = new CompareWorker(callback, &image, &compare);
    worker->SetSupress(supress);
    NanAsyncQueueWorker(worker);

    NanReturnUndefined();
}

void init(Handle<Object> exports) {
    exports->Set(NanNew<String>("compare"),
        NanNew<FunctionTemplate>(Compare)->GetFunction());
}

NODE_MODULE(addon, init)
