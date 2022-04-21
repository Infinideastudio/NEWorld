#pragma once

#include <string_view>
#include "stdinclude.h"
#include <kls/temp/STL.h>

namespace Renderer {
    enum class DataType {
        Int32, Float16, Float32, Float64
    };

    enum class ShaderType {
        Vertex, Geometry, Fragment, Compute
    };

    enum class IndexType {
        U8, U16, U32
    };

    namespace Internal {
        struct AttributeSpec {
            DataType type;
            int binding, location, width, offset;

            constexpr AttributeSpec(DataType type, int binding, int location, int width, int offset) noexcept:
                    type(type), binding(binding), location(location), width(width), offset(offset) {}
        };

        class Object {
        };
    }

    using GLShader = std::shared_ptr<Internal::Object>;

    GLShader Compile(ShaderType type, std::string_view program, const std::vector<std::string> &defines);
    GLShader CompileFile(ShaderType type, const std::string &path, const std::vector<std::string> &defines);

    struct IPipeline {
        virtual void Use() = 0;

        virtual void BindVertexBuffer(int bind, GLuint buffer, int offset = 0) = 0;

        virtual void BindIndexBuffer(GLuint buffer, Renderer::IndexType type) = 0;

        virtual void Draw(int count, int first = 0, int instance = 1) = 0;

        virtual void DrawIndexed(int count, int first = 0, int instance = 1) = 0;

        virtual void SetUniform(GLint loc, float value) = 0;

        virtual void SetUniform(GLint loc, int value) = 0;

        virtual void SetUniform(GLint loc, float v0, float v1, float v2, float v3) = 0;

        virtual void SetUniform(GLint loc, float *value) = 0;
    };

    enum class Topology {
        PointList, LineList, LineStrip, TriangleList, TriangleStrip, TriangleFan, Quad
    };

    using Pipeline = std::shared_ptr<IPipeline>;

    class PipelineBuilder {
    public:
        explicit PipelineBuilder(Topology topology) noexcept: mTopology(topology) {}

        void SetBinding(int location, int stride, int divisor) noexcept {
            mBindings.insert_or_assign(location, std::pair(stride, divisor));
        }

        bool AddAttribute(DataType type, int binding, int location, int width, int offset) noexcept {
            const auto binds = mBindings.find(binding);
            if (binds == mBindings.end()) return false; // 没有对应binding记录
            const auto stride = binds->second.first;
            switch (type) {
                case DataType::Float16:
                    if (offset + width * 2 > stride) return false; //超出stride长度
                    break;
                case DataType::Int32:
                case DataType::Float32:
                    if (offset + width * 4 > stride) return false; //超出stride长度
                    break;
                case DataType::Float64:
                    if (offset + width * 8 > stride) return false; //超出stride长度
                    break;
                default:
                    return false; //未定义的数据类别
            }
            mSpecs.emplace_back(type, binding, location, width, offset);
            return true;
        }

        void SetShader(ShaderType type, GLShader stage) noexcept {
            mStages.insert_or_assign(type, std::move(stage));
        }

        Pipeline Build();

    private:
        Topology mTopology;
        kls::temp::vector<Internal::AttributeSpec> mSpecs;
        kls::temp::unordered_map<ShaderType, GLShader> mStages;
        kls::temp::unordered_map<int, std::pair<int, int>> mBindings;
    };

    GLuint GetDefaultQuadIndex();
}