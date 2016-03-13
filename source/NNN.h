#pragma once

#include <iostream>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include <stdexcept>
#include "ByteOrder.h"

namespace NNN
{

enum NodeType { Unknown, Package, String, U8, U16, U32, U64, S8, S16, S32, S64, F32, F64, VS32 };

struct Magic
{
    uint8_t data[4];
    bool operator != (const Magic &src) const;
};

struct Version
{
    uint8_t data[4];
    bool operator > (const Version &src) const;
};

struct Information
{
    Version version;
    uintptr_t depth;
};

std::vector<std::string> split(std::string str, std::string pattern);

class Node
{
protected:
    NodeType type;

public:

    virtual ~Node() {};

    virtual Node* clone() = 0;

    virtual bool read(std::istream& in, Information& info) = 0;
    virtual bool write(std::ostream& out, Information& info) = 0;
    virtual void print(std::ostream& out) const = 0;

    virtual NodeType getType() = 0;
};

NodeType getType(Node* n);

template <typename DataType>
class ValueNode : public Node
{
private:
    DataType data;

public:
    ValueNode()
    {
        type = NNN::getType(this);
    }
    ValueNode(const DataType data_) : data(data_)
    {
        type = NNN::getType(this);
    }
    virtual ~ValueNode() {}

    Node* clone()
    {
        return new ValueNode(*this);
    }

    bool read(std::istream& in, Information&)
    {
        uint8_t flags;
        in.read((char*)&flags, 1);
        if (!in)
            return false;

        DataType data_;
        in.read((char*)&data_, sizeof(DataType));
        if (!in)
            return false;
        data = data_;
        return true;
    }

    bool write(std::ostream& out, Information&)
    {
        uint8_t flags = 0x00;
        out.write((char*)&flags, 1);
        if (!out)
            return false;

        out.write((char*)&data, sizeof(DataType));
        if (!out)
            return false;
        return true;
    }

    virtual void print(std::ostream& out) const
    {
        out << data;
    }

    inline virtual NodeType getType()
    {
        return type;
    }

    inline DataType* get()
    {
        return data;
    }

    inline const DataType* get() const
    {
        return data;
    }

    inline void set(const DataType& data_)
    {
        data = data_;
    }
};

template <>
class ValueNode<std::string> : public Node
{
private:

    std::string data;

public:

    ValueNode() {};
    ValueNode(std::string data_)
    {
        data = data_;
    }
    virtual ~ValueNode() {};

    Node* clone()
    {
        return new ValueNode(*this);
    }

    bool read(std::istream& in, Information& info);
    bool write(std::ostream& out, Information& info);
    virtual void print(std::ostream& out) const
    {
        out << data;
    }

    inline virtual NodeType getType()
    {
        return NodeType::String;
    }

    inline std::string& getData()
    {
        return data;
    }

    inline const std::string& getData() const
    {
        return data;
    }

    void set(std::string data_)
    {
        data = data_;
    };

};

template <typename ArrayType>
class ValueNode<std::vector<ArrayType> > : public Node
{
private:
    typedef std::vector<ArrayType> Vector;

    Vector data;

public:

    ValueNode()
    {
        type = NNN::getType(this);
    }
    ValueNode(Vector data_)
    {
        data = data_;
        type = NNN::getType(this);
    }
    virtual ~ValueNode() {}

    Node* clone()
    {
        return new ValueNode(*this);
    }

    bool read(std::istream& in, Information&)
    {

        uint8_t flags;
        in.read((char*)&flags, 1);
        if (!in) return false;

        Vector data_;

        uint16_t slen;
        in.read((char*)&slen, sizeof(uint16_t));
        if (!in) return false;

        if (slen != 0)
        {
            char* sdata = new char[slen];
            in.read(sdata, slen);
            if (!in||slen%sizeof(ArrayType)!=0)
            {
                delete[] sdata;
                return false;
            }

            int elemCount = slen / sizeof(ArrayType);
            data_ = Vector(elemCount);
            ArrayType* p = (ArrayType*)sdata;
            for (int i = 0; i < elemCount; i++)
            {
                data_[i]=*(p+i);
            }
            delete[] sdata;
        }

        data = data_;

        return true;

    }

    bool write(std::ostream& out, Information&)
    {

        uint8_t flags = 0x0D;
        out.write((char*)&flags, 1);
        if (!out) return false;

        uint16_t slen = (uint16_t)data.size()*sizeof(ArrayType);
        ByteOrder::swapEndiannessU16(&slen, 1);
        out.write((char*)&slen, sizeof(uint16_t));
        if (!out) return false;

        out.write((const char*)&data[0], data.size()* sizeof(ArrayType));
        if (!out) return false;

        return true;

    }

    virtual void print(std::ostream& out) const
    {
        for (Vector::const_iterator iter = data.begin(); iter != data.end(); ++iter)
            out << *iter << " ";
    }

    inline virtual NodeType getType()
    {
        return type;
    }

    inline Vector& getArray()
    {
        return data;
    }
    inline const Vector& getArray() const
    {
        return data;
    }
    void setArray(Vector data_)
    {
        data = data_;
    };

    inline ArrayType& get(size_t index_)
    {
        return data[index_]
    }
    inline const ArrayType& get(size_t index_) const
    {
        return data[index_]
    }
    inline void set(size_t index_, ArrayType data_)
    {
        data[index_] = data_;
    }
    inline void add(ArrayType data_)
    {
        data.push_back(data_);
    }
    inline void remove(size_t index_)
    {
        data.erase(index_);
    }
};

typedef std::map<std::string, Node*> NodeMap;

class NodePackage : public Node
{
private:
    NodeMap data;

    void cleanMap(NodeMap& data_)
    {
        for (NodeMap::iterator it = data_.begin(); it != data_.end(); ++it)
        {
            if (it->second != nullptr) delete it->second;
        }
    }

public:
    NodePackage::NodePackage()
    {
        type = NNN::getType(this);
    }
    NodePackage::NodePackage(const NodeMap& data_)
    {
        data = data_;
        type = NNN::getType(this);
    }
    NodePackage::NodePackage(const NodePackage& src)
    {
        data = src.data;
        for (NodeMap::iterator it = data.begin(); it != data.end(); ++it)
        {
            if (it->second != nullptr) it->second = it->second->clone();
        }
    }
    virtual NodePackage::~NodePackage()
    {
        cleanMap(data);
    }

    inline virtual Node* NodePackage::clone()
    {
        return new NodePackage(*this);
    }

    bool read(std::istream& in, Information& info);
    bool write(std::ostream& out, Information& info);

    inline virtual NodeType getType()
    {
        return NodeType::Package;
    }

    inline NodeMap& getNodeMap()
    {
        return data;
    }

    inline const NodeMap& getNodeMap() const
    {
        return data;
    }

    inline void setNodeMap(const NodeMap& data_)
    {
        data = data_;
    }

    inline void addNode(std::string nodeName, Node* value)
    {
        data.insert(std::make_pair(nodeName, value));
    }

    inline void removeNode(std::string nodeName)
    {
        data.erase(nodeName);
    }

    inline Node* get(std::string path)
    {
        Node* M = this;
        try
        {
            std::vector<std::string> result = split(path, ".");
            for (unsigned int i = 0; i < result.size(); i++)
            {
                Node* n = static_cast<NodePackage*>(M)->data.at(result[i]);
                if (n->getType() == NodeType::Package)
                    M = static_cast<NodePackage*>(n);
                else
                    break;
            }
        }
        catch (std::out_of_range&)
        {
            return nullptr;
        }
        return M;
    }

    virtual void print(std::ostream& out) const
    {
        printStack(out);
    }

    void printStack(std::ostream& out, int stack = 0) const
    {
        for (NodeMap::const_iterator iter = data.begin(); iter != data.end(); iter++)
        {
            out << std::string(stack, '\t');
            out << iter->first;
            if (iter->second->getType() == NodeType::Package)
            {
                NodePackage* v = (NodePackage*)iter->second;
                out << " - Package" << "\n";
                v->printStack(out, stack + 1);
            }
            else
            {
                out << "(" << iter->second->getType() << ") ";
                iter->second->print(out);
                out << "\n";

            }
        }
    }
};

using NodeS8 = ValueNode<char>;
using NodeS16 = ValueNode<short>;
using NodeS32 = ValueNode<int>;
using NodeS64 = ValueNode<long long>;
using NodeU8 = ValueNode<unsigned char>;
using NodeU16 = ValueNode<unsigned short>;
using NodeU32 = ValueNode<unsigned int>;
using NodeU64 = ValueNode<unsigned long long>;
using NodeF32 = ValueNode<float>;
using NodeF64 = ValueNode<double>;

using NodeByte = ValueNode<char>;
using NodeShort = ValueNode<short>;
using NodeInt = ValueNode<int>;
using NodeIntArray = ValueNode<std::vector<int> >;
using NodeLong = ValueNode<long long>;
using NodeUByte = ValueNode<unsigned char>;
using NodeUShort = ValueNode<unsigned short>;
using NodeUInt = ValueNode<unsigned int>;
using NodeULong = ValueNode<unsigned long long>;
using NodeFloat = ValueNode<float>;
using NodeDouble = ValueNode<double>;
using NodeString = ValueNode<std::string>;

bool read(std::istream& in, NodePackage& package);
bool write(std::ostream& out, NodePackage& package);

const Magic MAGIC = {0x4E, 0x4E, 0x4E, 0xFF};
const Version VERSION = {0x00, 0x00, 0x01, 0x00};

}
