#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "Parse.h"

class DeserializerGen {
public:
    explicit DeserializerGen(const std::string& ns): mNs(ns) {
        Indent() << "namespace " << ns << " {" << std::endl;
        for (auto x : ns) {
            if (x!=':') mPBase.push_back(x);
            else if (mPBase.back()!='/') mPBase.push_back('/');
        }
        ++mIndent;
    }

    void Print(Signature&& sig) {
        Indent() << "struct " << sig.Name << " final {" << std::endl;
        ++mIndent;
        // Insert the boilerplate
        Indent() << sig.Name << "() noexcept = default;" << std::endl;
        Indent() << sig.Name << "(" << sig.Name << "&&) noexcept = default;" << std::endl;
        Indent() << sig.Name << "& operator = (" << sig.Name << "&&) noexcept = default;" << std::endl;
        Indent() << sig.Name << "(const " << sig.Name << "&) = default;" << std::endl;
        Indent() << sig.Name << "& operator = (const " << sig.Name << "&) = default;" << std::endl << std::endl;
        for (auto& x : sig.Arguments) {
            Indent() << x.T->GetName() << ' ' << x.Name << " {};" << std::endl;
        }
        if (!sig.Arguments.empty()) mOut << std::endl;
        Indent() << "bool Deserialize(PacketReader& reader) noexcept {" << std::endl;
        ++mIndent;
        for (auto&& x : Optimize(GenInstructions(sig))) {
            if (x.isArgument) {
                const auto& arg = *x.isArgument;
                Indent() << arg.T->GetDeserializerCall(arg.Name) << std::endl;
            }
            else {
                if (x.isSizeCheck == -1)
                    Indent() << "if (!reader.Good()) return false;" << std::endl;
                else
                    Indent() << "if (reader.Remains() < " << x.isSizeCheck << ") return false;" << std::endl;
            }
        }
        Indent() << "return true;" << std::endl;
        --mIndent;
        Indent() << '}' << std::endl << std::endl;
        Indent() << "void Process(States::StateBase& state);" << std::endl;
        --mIndent;
        Indent() << "};" << std::endl << std::endl;
        mPackets.insert_or_assign(sig.Id, std::move(sig));
    }

    void Complete(const std::string& base) {
        GenHeader(base);
        GenParse(base);
    }

private:
    struct Instruction {
        int isSizeCheck = 0;
        const Argument* isArgument = nullptr;
    };

    std::ostream& Indent() {
        for (int i = 0; i<mIndent; ++i) mOut << "    ";
        return mOut;
    }

    static std::vector<Instruction> GenInstructions(const Signature& sig) {
        std::vector<Instruction> ret{};
        for (auto& x : sig.Arguments) {
            if (x.T->IsFixedSize()) {
                ret.push_back({x.T->GetSize(), nullptr});
                ret.push_back({0, std::addressof(x)});
            }
            else {
                ret.push_back({0, std::addressof(x)});
                ret.push_back({-1, nullptr});
            }
        }
        return ret;
    }

    static std::vector<Instruction> Optimize(const std::vector<Instruction>& raw) {
        std::vector<Instruction> ret{}, hold{};
        for (int i = 0; i < raw.size(); i += 2) {
            const auto& _1 = raw[i];
            const auto& _2 = raw[i+1];
            if (_1.isSizeCheck == 0) {
                if (!hold.empty()) { for (auto& x : hold) ret.push_back(x); hold.clear(); }
                ret.push_back(_1);
                hold.push_back(_2);
            }
            else {
                if (hold.empty()) hold.push_back(_1);
                else {
                    if (hold[0].isSizeCheck == -1)
                        hold[0].isSizeCheck = _1.isSizeCheck;
                    else
                        hold[0].isSizeCheck += _1.isSizeCheck;
                }
                hold.push_back(_2);
            }
        }
        if (!hold.empty()) for (auto& x : hold) ret.push_back(x);
        return ret;
    }

    void GenParse(const std::string& base) {
        Indent() << "#include \"" << mPBase << ".g.h\"" << std::endl << std::endl;
        Indent() << "namespace " << mNs << " {" << std::endl;
        ++mIndent;
        Indent() << "bool TryHandle(int id, PacketReader& reader, States::StateBase& state) {" << std::endl;
        ++mIndent;
        Indent() << "switch(id) {" << std::endl;
        for (auto& x : mPackets) {
            Indent() << "case " << x.first << ": {" << std::endl;
            ++mIndent;
            Indent() << x.second.Name << " x {};" << std::endl;
            Indent() << "if (x.Deserialize(reader)) { x.Process(state); return true; } else return false;" << std::endl;
            --mIndent;
            Indent() << '}' << std::endl;
        }
        Indent() << "}" << std::endl;
        Indent() << "return false;" << std::endl;
        --mIndent;
        Indent() << "}" << std::endl;
        --mIndent;
        Indent() << '}' << std::endl;
        std::cout << "Writing " << base + '/' +mPBase+ ".g.parse.cpp" << std::endl;
        std::ofstream gH (base + '/' +mPBase+ ".g.parse.cpp");
        gH << mOut.rdbuf();
        mOut.clear();
    }

    void GenHeader(const std::string& base) {
        Indent() << "bool TryHandle(int id, PacketReader& reader, States::StateBase& state);" << std::endl;
        --mIndent;
        Indent() << '}' << std::endl;
        std::cout << "Writing " << base + '/' + mPBase+ ".g.h" << std::endl;
        std::ofstream gH (base + '/' + mPBase+ ".g.h");
        gH << Headers() << mOut.rdbuf();
        mOut.clear();
    }

    int mIndent { 0 };
    std::string mNs;
    std::string mPBase;
    std::stringstream mOut;
    std::map<int, Signature> mPackets;
};

nlohmann::json LoadTree(const std::string& list) {
    nlohmann::json tree;
    std::ifstream input(list);
    input >> tree;
    return tree;
}

void ParseGen(const nlohmann::json& tree, const std::string& base) {
    DeserializerGen gen {tree["Space"]};
    for (auto&& sig : tree["In"]) gen.Print(Parser(std::string(sig)).Get());
    gen.Complete(base);
}

int main(int argc, char** argv) {
    bool error = false;
    std::string lists, base;
    if (argc == 3) {
        lists = argv[1];
        base = argv[2];
    }
    else {
        std::cin >> lists >> base;
    }
    for (auto&& file : std::filesystem::directory_iterator(lists)) {
        if (file.is_regular_file()) {
            if (file.path().extension() == ".json") {
                try {
                    ParseGen(LoadTree(std::filesystem::absolute(file.path())), base);
                }
                catch (std::exception& e) {
                    std::cout << "Error On Generating:" << std::filesystem::absolute(file.path()) << std::endl;
                    std::cout << "Details:" << std::endl << e.what() << std::endl;
                    std::cout << "End Info" << std::endl;
                    error = true;
                }
            }
        }
    }
    return error ? -1 : 0;
}