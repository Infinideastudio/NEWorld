#ifndef BLOCKTYPE_H_
#define BLOCKTYPE_H_

#include <string>

class BlockType
{
private:
    const std::string name;
    const bool solid;
    const bool translucent;
    const bool opaque;
    const int explodePower;
    const int hardness;

public:
    BlockType(const char* name_, bool solid_, bool translucent_, bool opaque_, int explodePower_, int hardness_) :
        name(name_), solid(solid_), translucent(translucent_), opaque(opaque_), explodePower(explodePower_), hardness(hardness_) {}

    // Internal block name
    std::string getName() const { return name; }
    // Is solid block
    bool isSolid() const { return solid; }
    // Transparency determines how it will rendered
    bool isTranslucent() const { return translucent; }
    // Opaque means it blocks light
    bool isOpaque() const { return opaque; }
    // Explode power, if it isn't a explosive set this to 0
    int getExplodePower() const { return explodePower; }
    // Hardness
    int getHardness() const { return hardness; }

};

#endif // !BLOCKTYPE_H_