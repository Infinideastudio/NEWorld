#include <iostream>
#include <stdint.h>
#include <string.h>
#include <typeinfo>
#include <utility>

#include "NNN.h"


namespace NNN
{

bool Magic::operator != (const Magic &src) const
{
    return (data[0] != src.data[0])
           || (data[1] != src.data[1])
           || (data[2] != src.data[2])
           || (data[3] != src.data[3]);
}

bool Version::operator > (const Version &src) const
{
    return (data[0] > src.data[0])
           || ((data[0] == src.data[0]) && (data[1] > src.data[1]))
           || ((data[1] == src.data[1]) && (data[2] > src.data[2]))
           || ((data[2] == src.data[2]) && (data[3] > src.data[3]));
}

bool NodePackage::read(std::istream& in, Information& info)
{
    uint8_t flags;
    in.read((char*)&flags, 1);
    if (!in)
        return false;

    info.depth++;

    std::map<std::string, Node*> data_;
    while(true)
    {
        uint8_t rtype;
        in.read((char*)&rtype, 1);
        if (!in)
        {
            cleanMap(data_);
            info.depth--;
            return false;
        }

        if (rtype == 0x00)
            break;

        Node* node;

        switch (rtype)
        {
        case Package:
            node = (Node*)new NodePackage();
            break;
        case String:
            node = (Node*)new NodeString();
            break;
        case U8:
            node = (Node*)new NodeU8(0);
            break;
        case U16:
            node = (Node*)new NodeU16(0);
            break;
        case U32:
            node = (Node*)new NodeU32(0);
            break;
        case U64:
            node = (Node*)new NodeU64(0);
            break;
        case S8:
            node = (Node*)new NodeS8(0);
            break;
        case S16:
            node = (Node*)new NodeS16(0);
            break;
        case S32:
            node = (Node*)new NodeS32(0);
            break;
        case S64:
            node = (Node*)new NodeS64(0);
            break;
        case F32:
            node = (Node*)new NodeF32(0);
            break;
        case F64:
            node = (Node*)new NodeF64(0);
            break;
        case VS32:
            node = (Node*)new ValueNode<std::vector<int>>();
            break;
        default:
            cleanMap(data_);
            info.depth--;
            return false;
        }

        uint16_t namelen;
        in.read((char*)&namelen, 2);
        if (!in)
        {
            cleanMap(data_);
            info.depth--;
            return false;
        }

        std::string name;
        if (namelen != 0)
        {
            char* namedata = new char[namelen];
            in.read(namedata, namelen);
            if (!in)
            {
                delete[] namedata;
                cleanMap(data_);
                info.depth--;
                return false;
            }
            name.append(namedata, namelen);
            delete[] namedata;
        }

        if (!node->read(in, info))
        {
            cleanMap(data_);
            info.depth--;
            return false;
        }

        if (!data_.insert(std::pair<std::string, Node*>(name, node)).second)
        {
            cleanMap(data_);
            info.depth--;
        }

    }

    info.depth--;
    data = data_;
    return true;
}

bool NodePackage::write(std::ostream& out, Information& info)
{

    uint8_t flags = 0x00;
    out.write((char*)&flags, 1);
    if (!out) return false;

    info.depth++;

    for (std::map<std::string, Node*>::iterator it = data.begin(); it != data.end(); ++it)
    {
        if (it->second == NULL)
        {
            info.depth--;
            return false;
        }

        uint8_t wtype;
        const std::type_info& typeinfo = typeid(*(it->second));
        if (typeinfo == typeid(NodePackage)) wtype = Package;
        else if (typeinfo == typeid(NodeString)) wtype = String;
        else if (typeinfo == typeid(NodeU8)) wtype = U8;
        else if (typeinfo == typeid(NodeU16)) wtype = U16;
        else if (typeinfo == typeid(NodeU32)) wtype = U32;
        else if (typeinfo == typeid(NodeU64)) wtype = U64;
        else if (typeinfo == typeid(NodeS8)) wtype = S8;
        else if (typeinfo == typeid(NodeS16)) wtype = S16;
        else if (typeinfo == typeid(NodeS32)) wtype = S32;
        else if (typeinfo == typeid(NodeS64)) wtype = S64;
        else if (typeinfo == typeid(NodeF32)) wtype = F32;
        else if (typeinfo == typeid(NodeF64)) wtype = F64;
        else if (typeinfo == typeid(ValueNode<std::vector<int32_t>>)) wtype = VS32;
        else
        {
            info.depth--;
            return false;
        }

        out.write((char*)&wtype, 1);
        if (!out)
        {
            info.depth--;
            return false;
        }

        if (it->first.length() > 0xFFFF)
        {
            info.depth--;
            return false;
        }

        uint16_t namelen = (uint16_t)it->first.length();
        ByteOrder::swapEndiannessU16(&namelen, 1);
        out.write((char*)&namelen, 2);
        if (!out)
        {
            info.depth--;
            return false;
        }

        out.write(it->first.data(), it->first.length());
        if (!out)
        {
            info.depth--;
            return false;
        }

        if (!it->second->write(out, info))
        {
            info.depth--;
            return false;
        }

    }

    uint8_t end = 0x00;
    out.write((char*)&end, 1);
    if (!out)
    {
        info.depth--;
        return false;
    }

    info.depth--;
    return true;
}

bool ValueNode<std::string>::read(std::istream& in, Information&)
{
    uint8_t flags;
    in.read((char*)&flags, 1);
    if (!in)
        return false;

    std::string data_;

    uint16_t slen;
    in.read((char*)&slen, sizeof(uint16_t));
    if (!in)
        return false;

    if (slen != 0)
    {
        char* sdata = new char[slen];
        in.read(sdata, slen);
        if (!in)
        {
            delete[] sdata;
            return false;
        }
        data_ = std::string(sdata, slen);
        delete[] sdata;
    }

    data = data_;
    return true;
}

bool ValueNode<std::string>::write(std::ostream& out, Information&)
{

    uint8_t flags = 0x00;
    out.write((char*)&flags, 1);
    if(!out)
        return false;

    uint16_t slen = (uint16_t)data.length();
    ByteOrder::swapEndiannessU16(&slen, 1);
    out.write((char*)&slen, 2);
    if (!out)
        return false;

    out.write(data.data(), data.length());
    if (!out)
        return false;

    return true;
}

bool read(std::istream& in, NodePackage& package)
{

    Magic magic;
    in.read((char*)magic.data, 4);
    if (!in)
        return false;
    if (magic != MAGIC)
        return false;

    Version version;
    in.read((char*)version.data, 4);
    if (!in)
        return false;
    if (version > VERSION)
        return false;

    Information info;
    info.version = version;
    info.depth = 0;
    if (!package.read(in, info))
        return false;

    return true;
}

bool write(std::ostream& out, NodePackage& package)
{
    out.write((char*)MAGIC.data, 4);
    if (!out)
        return false;

    out.write((char*)VERSION.data, 4);
    if (!out)
        return false;

    Information info;
    info.version = VERSION;
    info.depth = 0;
    if (!package.write(out, info))
        return false;

    return true;
}

std::vector<std::string> split(std::string str, std::string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    str += pattern;//扩展字符串以方便操作

    for (size_t i = 0, size = str.size(); i < size; i++)
    {
        pos = str.find(pattern, i);
        if (pos < size)
        {
            std::string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

NodeType getType(Node* n)
{
    NodeType type = Unknown;
    const std::type_info& typeinfo = typeid(*n);
    if (typeinfo == typeid(NodePackage)) type = Package;
    else if (typeinfo == typeid(NodeString)) type = String;
    else if (typeinfo == typeid(NodeU8)) type = U8;
    else if (typeinfo == typeid(NodeU16)) type = U16;
    else if (typeinfo == typeid(NodeU32)) type = U32;
    else if (typeinfo == typeid(NodeU64)) type = U64;
    else if (typeinfo == typeid(NodeS8)) type = S8;
    else if (typeinfo == typeid(NodeS16)) type = S16;
    else if (typeinfo == typeid(NodeS32)) type = S32;
    else if (typeinfo == typeid(NodeS64)) type = S64;
    else if (typeinfo == typeid(NodeF32)) type = F32;
    else if (typeinfo == typeid(NodeF64)) type = F64;
    else if (typeinfo == typeid(ValueNode<std::vector<int32_t>>)) type = VS32;
    return type;
}

}
