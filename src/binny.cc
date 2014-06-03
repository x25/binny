#include <node.h>
#include <nan.h>

using v8::FunctionTemplate;
using v8::Handle;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Array;

NAN_METHOD(Unpack) {
  NanScope();

  if (args.Length() < 1 || !node::Buffer::HasInstance(args[0])) {
    return NanThrowError("First argument needs to be a buffer");
  }

  Local<Object> bufferObj = args[0]->ToObject();
  size_t inputLength = node::Buffer::Length(bufferObj);

  Local<Array> result = NanNew<Array>();

  if (!inputLength) {
    NanReturnValue(result);
  }

  char *inputData = node::Buffer::Data(bufferObj);

  if ((unsigned char)inputData[0] != 0xB1 || inputLength < 3) {
    return NanThrowError("Bad header");
  }

  uint16_t numBlocks = ntohs(*((uint16_t *)(inputData + 1)));

  unsigned int offset = 3;

  for (unsigned int i = 0; i < numBlocks; i++) {
    uint16_t blockLen = ntohs(*((uint16_t *)(inputData + offset)));

    if (offset + 2 + blockLen > inputLength) {
      return NanThrowError("Bad data");
    }

    result->Set(i, NanNew<String>(inputData + offset + 2, blockLen));

    offset += 2 + blockLen;
  }

  NanReturnValue(result);
}

NAN_METHOD(Pack) {
  NanScope();

  if (args.Length() < 1 || !args[0]->IsArray()) {
    return NanThrowError("First argument needs to be a array");
  }

  size_t outputLen = 3;

  Handle<Array> input = Handle<Array>::Cast(args[0]);
  for (unsigned int i = 0; i < input->Length(); i++) {
    if (!input->Get(i)->IsString()) {
      return NanThrowError("Array must contain only strings");
    }

    Local<String> str = input->Get(i)->ToString();

    outputLen += 2 + str->Length();
  }

  Local<Object> result = NanNewBufferHandle(outputLen);
  char *outputData = node::Buffer::Data(result);

  outputData[0] = 0xB1;

  *((uint16_t *)(outputData + 1)) = htons(input->Length());

  unsigned int offset = 3;

  for (unsigned int i = 0; i < input->Length(); i++) {
    Local<String> str = input->Get(i)->ToString();

    size_t count;
    char* val = NanCString(str, &count);

    *((uint16_t *)(outputData + offset)) = htons(str->Length());
    offset += 2;
    memcpy(outputData + offset, val, str->Length());
    offset += str->Length();
  }

  NanReturnValue(result);
}

void Init(Handle<Object> exports) {
  exports->Set(
    NanNew<String>("pack"),
    NanNew<FunctionTemplate>(Pack)->GetFunction()
  );

  exports->Set(
    NanNew<String>("unpack"),
    NanNew<FunctionTemplate>(Unpack)->GetFunction()
  );
}

NODE_MODULE(binny, Init)
