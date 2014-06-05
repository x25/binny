#include <node.h>
#include <nan.h>
#include <vector>

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

  std::vector<size_t> arrLen(input->Length(), 0);
  std::vector<char*> arrStr(input->Length(), 0);

  for (unsigned int i = 0; i < input->Length(); i++) {
    if (!input->Get(i)->IsString()) {
      if (i > 0) {
        for (unsigned int j = i; j > 0; j--) {
          delete arrStr[i];
        }
      }

      return NanThrowError("Array must contain only strings");
    }

    arrStr[i] = NanCString(input->Get(i), &arrLen[i]);
    outputLen += sizeof(uint16_t) + arrLen[i];
  }

  Local<Object> result = NanNewBufferHandle(outputLen); //slowBuffer!

  char *outputData = node::Buffer::Data(result);

  outputData[0] = 0xB1;

  *((uint16_t *)(outputData + 1)) = htons(input->Length());

  unsigned int offset = sizeof(char) + sizeof(uint16_t);

  for (unsigned int i = 0; i < input->Length(); i++) {
    *((uint16_t *)(outputData + offset)) = htons(arrLen[i]);

    memcpy(outputData + offset + sizeof(uint16_t), arrStr[i], arrLen[i]);
    offset += sizeof(uint16_t) + arrLen[i];

    delete arrStr[i];
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
