#include "compare.h"

CompareWorker::CompareWorker(NanCallback *callback, Image *base, Image *compareWith)
    : NanAsyncWorker(callback), _base(*base), _compareWith(*compareWith) {}

CompareWorker::~CompareWorker() {}

void CompareWorker::SetSupress(bool supress) {
    _supress = supress;
}

void CompareWorker::Execute() {
    try {
        _result = _base.compare(_compareWith);

        if (!_result) {
            _base.composite(_compareWith, Magick::ForgetGravity, Magick::DifferenceCompositeOp);
            _base.write(&_diff);
        }
    } catch (...) {
        if (_supress) {
            SetErrorMessage("unhandled error while comparing");
        } else {
            throw;
        }
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

    Local<Object> baseSrcData = Local<Object>::Cast(args[0]);
    Blob baseSrcBlob(node::Buffer::Data(baseSrcData), node::Buffer::Length(baseSrcData));
    Image base;
    try {
        base.read(baseSrcBlob);
    } catch (...) {
        if (supress) {
            return NanThrowError("unhandled error while loading base image");
        }
        throw;
    }

    Local<Object> compareWithSrcData = Local<Object>::Cast(args[1]);
    Blob compareWithSrcBlob(node::Buffer::Data(compareWithSrcData), node::Buffer::Length(compareWithSrcData));
    Image compareWith;
    try {
        compareWith.read(compareWithSrcBlob);
    } catch (...) {
        if (supress) {
            return NanThrowError("unhandled error while loading compareWith image");
        }
        throw;
    }

    NanCallback *callback = new NanCallback(args[3].As<Function>());
    CompareWorker *worker = new CompareWorker(callback, &base, &compareWith);
    worker->SetSupress(supress);
    NanAsyncQueueWorker(worker);

    NanReturnUndefined();
}

void init(Handle<Object> exports) {
    exports->Set(NanNew<String>("compare"),
        NanNew<FunctionTemplate>(Compare)->GetFunction());
}

NODE_MODULE(compare, init)
