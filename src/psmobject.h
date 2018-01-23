#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <node.h>
#include <string>
#include <node_object_wrap.h>

class PsmObject : public node::ObjectWrap {
public:
	static void Init(v8::Isolate* isolate);
	static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
	explicit PsmObject(std::string);
	~PsmObject();

	static void GetInitFlag(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void CardParse(const v8::FunctionCallbackInfo<v8::Value>& args);
	static v8::Persistent<v8::Function> constructor;
};

#endif