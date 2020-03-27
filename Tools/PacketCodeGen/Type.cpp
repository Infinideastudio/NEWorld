#include "Type.h"
#include <map>
#include <vector>
#include <sstream>

static std::map<std::string, std::shared_ptr<IType>> Types;

int InitBuiltin() {
    // Add Builtin
    Types = std::map<std::string, std::shared_ptr<IType>>({
            {"T", std::make_unique<BuiltInScalars>("Bool", 1)},
            {"B", std::make_unique<BuiltInScalars>("Byte", 1)},
            {"S", std::make_unique<BuiltInScalars>("Short", 2)},
            {"I", std::make_unique<BuiltInScalars>("Int", 4)},
            {"L", std::make_unique<BuiltInScalars>("Long", 8)},
            {"UB", std::make_unique<BuiltInScalars>("UByte", 1)},
            {"US", std::make_unique<BuiltInScalars>("UShort", 2)},
            {"UI", std::make_unique<BuiltInScalars>("UInt", 4)},
            {"UL", std::make_unique<BuiltInScalars>("ULong", 8)},
            {"F", std::make_unique<BuiltInScalars>("Float", 4)},
            {"D", std::make_unique<BuiltInScalars>("Double", 8)},
            {"V", std::make_unique<BuiltInVariadic>("VarInt")},
            {"s", std::make_unique<BuiltInVariadic>("String")},
            {"X", std::make_unique<BuiltInVariadic>("UUID")},

            {"[T", std::make_unique<BuiltInArray>("Bool")},
            {"[B", std::make_unique<BuiltInArray>("Byte")},
            {"[S", std::make_unique<BuiltInArray>("Short")},
            {"[I", std::make_unique<BuiltInArray>("Int")},
            {"[L", std::make_unique<BuiltInArray>("Long")},
            {"[UB", std::make_unique<BuiltInArray>("UByte")},
            {"[US", std::make_unique<BuiltInArray>("UShort")},
            {"[UI", std::make_unique<BuiltInArray>("UInt")},
            {"[UL", std::make_unique<BuiltInArray>("ULong")},
            {"[F", std::make_unique<BuiltInArray>("Float")},
            {"[D", std::make_unique<BuiltInArray>("Double")},
            {"[V", std::make_unique<BuiltInArray>("VarInt")},
            {"[s", std::make_unique<BuiltInArray>("String")},
            {"[X", std::make_unique<BuiltInArray>("UUID")},

            {"{T", std::make_unique<BuiltUnboundedArray>("Bool")},
            {"{B", std::make_unique<BuiltUnboundedArray>("Byte")},
            {"{S", std::make_unique<BuiltUnboundedArray>("Short")},
            {"{I", std::make_unique<BuiltUnboundedArray>("Int")},
            {"{L", std::make_unique<BuiltUnboundedArray>("Long")},
            {"{UB", std::make_unique<BuiltUnboundedArray>("UByte")},
            {"{US", std::make_unique<BuiltUnboundedArray>("UShort")},
            {"{UI", std::make_unique<BuiltUnboundedArray>("UInt")},
            {"{UL", std::make_unique<BuiltUnboundedArray>("ULong")},
            {"{F", std::make_unique<BuiltUnboundedArray>("Float")},
            {"{D", std::make_unique<BuiltUnboundedArray>("Double")},
            {"{V", std::make_unique<BuiltUnboundedArray>("VarInt")},
            {"{s", std::make_unique<BuiltUnboundedArray>("String")},
            {"{X", std::make_unique<BuiltUnboundedArray>("UUID")},
    });
    return 0;
}

static std::vector<std::string> Imports = { // NOLINT
        "Network/ProtocolBase.h"
};

static std::string ToPath(const std::string& mangled) {
    std::string ret {};
    for (auto x : mangled.substr(1)) {
        if (x != '@') ret.push_back(x);
        else {
            if (ret.back() != '/') { ret.push_back('/'); }
        }
    }
    ret += ".h";
    return ret;
}

IType* GetOrAdd(const std::string& name) {
    static int i = InitBuiltin();
    auto it = Types.find(name);
    if (it == Types.end()) {
        Imports.push_back(ToPath(name));
        it = Types.insert_or_assign(name, std::shared_ptr<IType>(new CustomType(name))).first;
    }
    return it->second.get();
}

std::string Headers() {
    std::stringstream ss {};
    ss << "#pragma once" << std::endl << std::endl;
    for (auto& x : Imports) {
        ss << "#include \"" << x << '"' << std::endl;
    }
    ss << std::endl;
    return ss.str();
}
