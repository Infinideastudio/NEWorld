#pragma once

#include <string>
#include <utility>
#include <string_view>

struct IType {
    virtual ~IType() noexcept = default;
    [[nodiscard]] virtual int GetSize() const noexcept = 0;
    [[nodiscard]] virtual bool IsFixedSize() const noexcept = 0;
    [[nodiscard]] virtual const char* GetName() const noexcept = 0;
    [[nodiscard]] virtual std::string GetSizeCall(const std::string& argName) const = 0;
    [[nodiscard]] virtual std::string GetSerializerCall(const std::string& argName) const = 0;
    [[nodiscard]] virtual std::string GetDeserializerCall(const std::string& argName) const = 0;
};

struct BuiltIn : IType {
    explicit BuiltIn(std::string name)
            :Name(std::move(name)) { }
    [[nodiscard]] const char* GetName() const noexcept override { return Name.c_str(); }
    [[nodiscard]] std::string GetSerializerCall(const std::string& argName) const override {
        return "writer."+Name+'('+argName+");";
    }
    [[nodiscard]] std::string GetDeserializerCall(const std::string& argName) const override {
        return argName+" = reader."+Name+"();";
    }
private:
    std::string Name;
};

struct BuiltInScalars : BuiltIn {
    BuiltInScalars(const char* string, int i) noexcept
            :BuiltIn(string), Size(i) { }
    [[nodiscard]] std::string GetSizeCall(const std::string& argName) const override {
        return "sizeof("+argName+')';
    }
    [[nodiscard]] int GetSize() const noexcept override { return Size; }
    [[nodiscard]] bool IsFixedSize() const noexcept override { return true; }
private:
    int Size{};
};

struct BuiltInVariadic : BuiltIn {
    explicit BuiltInVariadic(const std::string& name)
            :BuiltIn(name) { }
    [[nodiscard]] std::string GetSizeCall(const std::string& argName) const override {
        return "PacketWriter::"+std::string(GetName())+"Size("+argName+")";
    }
    [[nodiscard]] int GetSize() const noexcept override { return 0; }
    [[nodiscard]] bool IsFixedSize() const noexcept override { return false; }
};

struct BuiltInArray : BuiltInVariadic {
    explicit BuiltInArray(const char* string) noexcept
            :BuiltInVariadic(std::string(string)+"Array") { }
};

struct BuiltUnboundedArray : BuiltInVariadic {
    explicit BuiltUnboundedArray(const char* string) noexcept
            :BuiltInVariadic(std::string(string)+"UnboundedArray") { }
};

struct CustomType : IType {
    explicit CustomType(const std::string& mangled) noexcept
            :NamespacedName(mangled.substr(1)) {
        for (auto& x : NamespacedName) { if (x=='@') x = ':'; }
    }
    [[nodiscard]] int GetSize() const noexcept override { return 0; }
    [[nodiscard]] bool IsFixedSize() const noexcept override { return false; }
    [[nodiscard]] const char* GetName() const noexcept override { return NamespacedName.c_str(); }
    [[nodiscard]] std::string GetSizeCall(const std::string& argName) const override {
        return argName+".SerializedSize()";
    }
    [[nodiscard]] std::string GetSerializerCall(const std::string& argName) const override {
        return argName+".Serialize(writer);";
    }
    [[nodiscard]] std::string GetDeserializerCall(const std::string& argName) const override {
        return argName+".Deserialize(reader);";
    }
private:
    std::string NamespacedName;
};

IType* GetOrAdd(const std::string& name);

std::string Headers();
