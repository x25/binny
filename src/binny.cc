#define BUILDING_NODE_EXTENSION

#include <node.h>
#include <node_buffer.h>
#include <string.h>

using namespace v8;

Handle<Value> Unpack(const Arguments& args) {
  HandleScope scope;

  if (args.Length() < 1 || !node::Buffer::HasInstance(args[0])) {
    ThrowException(Exception::TypeError(String::New("First argument needs to be a buffer")));
    return scope.Close(Undefined());
  }

  Local<Object> bufferObj = args[0]->ToObject();
  size_t inputLength = node::Buffer::Length(bufferObj);

  Local<Array> result = Array::New();

  if (!inputLength) {
    return scope.Close(result);
  }

  char *inputData = node::Buffer::Data(bufferObj);

  if ((unsigned char)inputData[0] != 0xB1 || inputLength < 3) {
    ThrowException(Exception::TypeError(String::New("Bad header")));
    return scope.Close(Undefined());
  }

  uint16_t numBlocks = ntohs(*((uint16_t *)(inputData + 1)));

  unsigned int offset = 3;

  for (unsigned int i = 0; i < numBlocks; i++) {
    uint16_t blockLen = ntohs(*((uint16_t *)(inputData + offset)));

    if (offset + 2 + blockLen > inputLength) {
      ThrowException(Exception::TypeError(String::New("Bad data")));
      return scope.Close(Undefined());
    }

    result->Set(i, String::New(inputData + offset + 2, blockLen));

    offset += 2 + blockLen;
  }

  return scope.Close(result);
}

Handle<Value> Pack(const Arguments& args) {
  HandleScope scope;

  if (args.Length() < 1 || !args[0]->IsArray()) {
    ThrowException(Exception::TypeError(String::New("First argument needs to be a array")));
    return scope.Close(Undefined());
  }

  size_t outputLen = 3;

  Handle<Array> input = Handle<Array>::Cast(args[0]);
  for (unsigned int i = 0; i < input->Length(); i++) {
    if (!input->Get(i)->IsString()) {
      ThrowException(Exception::TypeError(String::New("Array must contain only strings")));
      return scope.Close(Undefined());
    }

    Local<String> str = input->Get(i)->ToString();

    outputLen += 2 + str->Length();
  }

  node::Buffer *slowBuffer = node::Buffer::New(outputLen);
  char *outputData = node::Buffer::Data(slowBuffer);

  outputData[0] = 0xB1;

  *((uint16_t *)(outputData + 1)) = htons(input->Length());

  unsigned int offset = 3;

  for (unsigned int i = 0; i < input->Length(); i++) {
    Local<String> str = input->Get(i)->ToString();
    String::AsciiValue val(str);

    *((uint16_t *)(outputData + offset)) = htons(str->Length());
    offset += 2;
    memcpy(outputData + offset, *val, str->Length());
    offset += str->Length();
  }

  Local<Object> global = Context::GetCurrent()->Global();
  Local<Value> bv = global->Get(String::NewSymbol("Buffer"));

  assert(bv->IsFunction());

  Local<Function> bc = Local<Function>::Cast(bv);
  Handle<Value> cArgs[3] = {
    slowBuffer->handle_,
    Integer::New(outputLen),
    Integer::New(0)
  };

  Local<Object> fastBuffer = bc->NewInstance(3, cArgs);
  return scope.Close(fastBuffer);
}

void Init(Handle<Object> exports) {
  exports->Set(
    String::NewSymbol("pack"),
    FunctionTemplate::New(Pack)->GetFunction()
  );

  exports->Set(
    String::NewSymbol("unpack"),
    FunctionTemplate::New(Unpack)->GetFunction()
  );
}

NODE_MODULE(binny, Init)
