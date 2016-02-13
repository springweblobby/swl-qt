#ifndef _MARSHAL_H
#define _MARSHAL_H

// This provides callNative() which is used to automatically marshal between
// native code and V8.

#include <cef_v8.h>

namespace Marshal {

class NotEnoughArgumentsException : public std::exception {};
class TooManyArgumentsException : public std::exception {};
class TypeMismatchException : public std::exception {};

template<class T>
static T fromV8(CefRefPtr<CefV8Value>);
template<>
int fromV8<int>(CefRefPtr<CefV8Value> v8_val) {
    if (!v8_val->IsInt())
        throw TypeMismatchException();
    return v8_val->GetIntValue();
}
template<>
unsigned int fromV8<unsigned int>(CefRefPtr<CefV8Value> v8_val) {
    if (!v8_val->IsUInt())
        throw TypeMismatchException();
    return v8_val->GetUIntValue();
}
template<>
bool fromV8<bool>(CefRefPtr<CefV8Value> v8_val) {
    if (!v8_val->IsBool())
        throw TypeMismatchException();
    return v8_val->GetBoolValue();
}
template<>
std::string fromV8<std::string>(CefRefPtr<CefV8Value> v8_val) {
    if (!v8_val->IsString())
        throw TypeMismatchException();
    return v8_val->GetStringValue().ToString();
}

template<class T>
static CefRefPtr<CefV8Value> toV8(const T&);
template<>
CefRefPtr<CefV8Value> toV8(const int& n) {
    return CefV8Value::CreateInt(n);
}
template<>
CefRefPtr<CefV8Value> toV8(const unsigned int& n) {
    return CefV8Value::CreateUInt(n);
}
template<>
CefRefPtr<CefV8Value> toV8(const bool& a) {
    return CefV8Value::CreateBool(a);
}
template<>
CefRefPtr<CefV8Value> toV8(const std::string& str) {
    return CefV8Value::CreateString(str);
}

template<class T, class Ret, typename... Args>
static std::function<Ret(Args...)> bind(Ret (T::*func)(Args...), T* obj) {
    return [=](Args... args){
        return (obj->*func)(args...);
    };
}

// Doing this with variadic templates proved complicated.
template<class Ret>
static Ret callWithArgs(std::function<Ret()> func, const CefV8ValueList& v8_args) {
    if (!v8_args.empty())
        throw TooManyArgumentsException();
    return func();
}
template<class Ret, class Arg1>
static Ret callWithArgs(std::function<Ret(Arg1)> func, const CefV8ValueList& v8_args) {
    if (v8_args.size() < 1)
        throw NotEnoughArgumentsException();
    if (v8_args.size() > 1)
        throw TooManyArgumentsException();
    return func(fromV8<Arg1>(v8_args[0]));
}
template<class Ret, class Arg1, class Arg2>
static Ret callWithArgs(std::function<Ret(Arg1, Arg2)> func, const CefV8ValueList& v8_args) {
    if (v8_args.size() < 2)
        throw NotEnoughArgumentsException();
    if (v8_args.size() > 2)
        throw TooManyArgumentsException();
    return func(fromV8<Arg1>(v8_args[0]), fromV8<Arg2>(v8_args[1]));
}
template<class Ret, class Arg1, class Arg2, class Arg3>
static Ret callWithArgs(std::function<Ret(Arg1, Arg2, Arg3)> func, const CefV8ValueList& v8_args) {
    if (v8_args.size() < 3)
        throw NotEnoughArgumentsException();
    if (v8_args.size() > 3)
        throw TooManyArgumentsException();
    return func(fromV8<Arg1>(v8_args[0]), fromV8<Arg2>(v8_args[1]), fromV8<Arg3>(v8_args[2]));
}
template<class Ret, class Arg1, class Arg2, class Arg3, class Arg4>
static Ret callWithArgs(std::function<Ret(Arg1, Arg2, Arg3, Arg4)> func, const CefV8ValueList& v8_args) {
    if (v8_args.size() < 4)
        throw NotEnoughArgumentsException();
    if (v8_args.size() > 4)
        throw TooManyArgumentsException();
    return func(fromV8<Arg1>(v8_args[0]), fromV8<Arg2>(v8_args[1]), fromV8<Arg3>(v8_args[2]), fromV8<Arg4>(v8_args[3]));
}
template<class Ret, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
static Ret callWithArgs(std::function<Ret(Arg1, Arg2, Arg3, Arg4, Arg5)> func, const CefV8ValueList& v8_args) {
    if (v8_args.size() < 5)
        throw NotEnoughArgumentsException();
    if (v8_args.size() > 5)
        throw TooManyArgumentsException();
    return func(fromV8<Arg1>(v8_args[0]), fromV8<Arg2>(v8_args[1]), fromV8<Arg3>(v8_args[2]), fromV8<Arg4>(v8_args[3]), fromV8<Arg5>(v8_args[4]));
}
template<class Ret, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
static Ret callWithArgs(std::function<Ret(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)> func, const CefV8ValueList& v8_args) {
    if (v8_args.size() < 6)
        throw NotEnoughArgumentsException();
    if (v8_args.size() > 6)
        throw TooManyArgumentsException();
    return func(fromV8<Arg1>(v8_args[0]), fromV8<Arg2>(v8_args[1]), fromV8<Arg3>(v8_args[2]), fromV8<Arg4>(v8_args[3]), fromV8<Arg5>(v8_args[4]), fromV8<Arg6>(v8_args[5]));
}
template<class Ret, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
static Ret callWithArgs(std::function<Ret(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)> func, const CefV8ValueList& v8_args) {
    if (v8_args.size() < 7)
        throw NotEnoughArgumentsException();
    if (v8_args.size() > 7)
        throw TooManyArgumentsException();
    return func(fromV8<Arg1>(v8_args[0]), fromV8<Arg2>(v8_args[1]), fromV8<Arg3>(v8_args[2]), fromV8<Arg4>(v8_args[3]), fromV8<Arg5>(v8_args[4]), fromV8<Arg6>(v8_args[5]), fromV8<Arg5>(v8_args[6]));
}
template<class Ret, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8>
static Ret callWithArgs(std::function<Ret(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)> func, const CefV8ValueList& v8_args) {
    if (v8_args.size() < 8)
        throw NotEnoughArgumentsException();
    if (v8_args.size() > 8)
        throw TooManyArgumentsException();
    return func(fromV8<Arg1>(v8_args[0]), fromV8<Arg2>(v8_args[1]), fromV8<Arg3>(v8_args[2]), fromV8<Arg4>(v8_args[3]), fromV8<Arg5>(v8_args[4]), fromV8<Arg6>(v8_args[5]), fromV8<Arg5>(v8_args[6]), fromV8<Arg6>(v8_args[7]));
}
// 8 arguments ought to be enough for anyone

template<class T, class Ret, typename... Args>
static CefRefPtr<CefV8Value> callNative(Ret (T::*func)(Args...), T* obj, const CefV8ValueList& v8_args) {
    Ret retval = callWithArgs(bind(func, obj), v8_args);
    return toV8(retval);
}
template<class T, typename... Args>
static CefRefPtr<CefV8Value> callNative(void (T::*func)(Args...), T* obj, const CefV8ValueList& v8_args) {
    callWithArgs(bind(func, obj), v8_args);
    return CefV8Value::CreateUndefined();
}

}

#endif // _MARSHAL_H
