#include <node.h>
#include <nan.h>
#include <vector>

using namespace v8;

#define BINNY_V1 0xB1
#define BINNY_V1_NUM_SIZE sizeof(uint16_t)
#define BINNY_V1_HEADER_SIZE sizeof(char) + BINNY_V1_NUM_SIZE
#define BINNY_V1_MAX_BLOCKS (uint16_t)-1
#define BINNY_V1_BLOCK_MAX_LEN (uint16_t)-1

NAN_METHOD(Unpack) {
  NanScope();

  if (args.Length() < 1 || !node::Buffer::HasInstance(args[0])) {
    return NanThrowError("First argument needs to be a buffer");
  }

  Handle<Object> inputObj = Handle<Object>::Cast(args[0]);
  size_t inputLength = node::Buffer::Length(inputObj);

  if (inputLength < BINNY_V1_HEADER_SIZE) {
    return NanThrowError("Invalid header size");
  }

  char *inputRawData = node::Buffer::Data(inputObj);

  if ((unsigned char)inputRawData[0] != BINNY_V1) {
    return NanThrowError("Unsupported blob version");
  }

  uint16_t numBlocks = ntohs(*((uint16_t *)(inputRawData + 1)));

  Local<Array> result = NanNew<Array>();

  unsigned int offset = BINNY_V1_HEADER_SIZE;

  for (unsigned int i = 0; i < numBlocks; i++) {
    uint16_t blockLen = ntohs(*((uint16_t *)(inputRawData + offset)));

    if (offset + BINNY_V1_NUM_SIZE + blockLen > inputLength) {
      return NanThrowError("Malformed blob data");
    }

    result->Set(i, NanNew<String>(inputRawData + offset + BINNY_V1_NUM_SIZE, blockLen));

    offset += BINNY_V1_NUM_SIZE + blockLen;
  }

  NanReturnValue(result);
}

NAN_METHOD(Pack) {
  NanScope();

  if (args.Length() < 1 || !args[0]->IsArray()) {
    return NanThrowError("First argument needs to be an array");
  }

  size_t outputLen = BINNY_V1_HEADER_SIZE;

  Handle<Array> input = Handle<Array>::Cast(args[0]);

  if (input->Length() > BINNY_V1_MAX_BLOCKS) {
    return NanThrowError("Too many elements");
  }

  std::vector<char*> arrBlock(input->Length(), 0);
  std::vector<size_t> arrBlockLen(input->Length(), 0);

  for (unsigned int i = 0; i < input->Length(); i++) {
    if (!input->Get(i)->IsString()) {
      while (i-- >= 1) {
        delete arrBlock[i];
      }

      return NanThrowError("Input array must contain only strings");
    }

    arrBlock[i] = NanCString(input->Get(i), &arrBlockLen[i]);

    if (arrBlockLen[i] > BINNY_V1_BLOCK_MAX_LEN) {
      while (i + 1 > 0) {
        delete arrBlock[i--];
      }

      return NanThrowError("Too big string");
    }

    outputLen += BINNY_V1_NUM_SIZE + arrBlockLen[i];
  }

  Local<Object> result = NanNewBufferHandle(outputLen); //slowBuffer in node <=0.10!

  char *outputData = node::Buffer::Data(result);

  outputData[0] = BINNY_V1;

  *((uint16_t *)(outputData + 1)) = htons(input->Length());

  unsigned int offset = BINNY_V1_HEADER_SIZE;

  for (unsigned int i = 0; i < input->Length(); i++) {
    *((uint16_t *)(outputData + offset)) = htons(arrBlockLen[i]);

    memcpy(outputData + offset + BINNY_V1_NUM_SIZE, arrBlock[i], arrBlockLen[i]);
    offset += BINNY_V1_NUM_SIZE + arrBlockLen[i];

    delete arrBlock[i];
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
