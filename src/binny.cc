#include <node.h>
#include <nan.h>

using namespace v8;

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

  unsigned int offset = sizeof(char) + sizeof(uint16_t);

  for (unsigned int i = 0; i < numBlocks; i++) {
    uint16_t blockLen = ntohs(*((uint16_t *)(inputData + offset)));

    if (offset + sizeof(uint16_t) + blockLen > inputLength) {
      return NanThrowError("Bad data");
    }

    result->Set(i, NanNew<String>(inputData + offset + sizeof(uint16_t), blockLen));

    offset += sizeof(uint16_t) + blockLen;
  }

  NanReturnValue(result);
}

NAN_METHOD(Pack) {
  NanScope();

  if (args.Length() < 1 || !args[0]->IsArray()) {
    return NanThrowError("First argument needs to be a array");
  }

  size_t outputLen = sizeof(char) + sizeof(uint16_t);

  Handle<Array> input = Handle<Array>::Cast(args[0]);
  for (unsigned int i = 0; i < input->Length(); i++) {
    if (!input->Get(i)->IsString()) {
      return NanThrowError("Array must contain only strings");
    }

    size_t len;
    char* val = NanCString(input->Get(i), &len);
    outputLen += sizeof(uint16_t) + len;
    delete val;
  }

  Local<Object> result = NanNewBufferHandle(outputLen); //slowBuffer!

  char *outputData = node::Buffer::Data(result);

  outputData[0] = 0xB1;

  *((uint16_t *)(outputData + 1)) = htons(input->Length());

  unsigned int offset = sizeof(char) + sizeof(uint16_t);

  for (unsigned int i = 0; i < input->Length(); i++) {
    size_t len;
    char* val = NanCString(input->Get(i), &len);

    *((uint16_t *)(outputData + offset)) = htons(len);

    memcpy(outputData + offset + sizeof(uint16_t), val, len);
    offset += sizeof(uint16_t) + len;

    delete val;
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
