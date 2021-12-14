#include <fstream>
#include <iostream>
#include "Pipeline.h"
#include "FunctionsKit.h"
#include "Temp/String.h"
#include "Temp/OrderedAsscociation.h"
#include "Renderer/BufferBuilder.h"

namespace {
    auto CollapseDefines(const std::vector<std::string> &defines) noexcept {
        temp::set<std::string_view> result{};
        for (const auto &s: defines) result.insert(s);
        return result;
    }

    void PrintDebug(GLuint program) {
        GLint uniform_count = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);
        if (uniform_count != 0) {
            GLint max_name_len = 0;
            GLsizei length = 0;
            GLsizei count = 0;
            GLenum type = GL_NONE;
            glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);
            auto uName = std::make_unique<char[]>(max_name_len);
            for (GLint i = 0; i < uniform_count; ++i) {
                glGetActiveUniform(program, i, max_name_len, &length, &count, &type, uName.get());
                std::cout << uName.get() << ":"
                          << glGetUniformLocation(program, uName.get()) << " count:" << count << std::endl;
            }
        }
    }

    temp::string Preprocess(std::string_view text, temp::set<std::string_view> defines) {
        temp::istringstream input{temp::string(text)};
        temp::stringstream output{};
        temp::istringstream line{};
        temp::string cur, var, macro;
        while (!input.eof()) {
            std::getline(input, cur);
            std::string_view lineView{cur};
            if (cur.empty()) continue;
            if (beginWith(lineView, "#")) { //处理NEWorld预处理器标志
                line.str(cur);
                line >> macro;
                if (macro == "##NEWORLD_SHADER_DEFINES") {
                    line >> var >> macro;
                    if (defines.find(var) != defines.end()) cur = "#define " + macro;
                    else continue;
                }
            }
            output << cur << '\n';
        }
        return output.str();
    }

    GLenum MapShaderTypes(Renderer::ShaderType type) noexcept {
        switch (type) {
            case Renderer::ShaderType::Vertex:
                return GL_VERTEX_SHADER;
            case Renderer::ShaderType::Geometry:
                return GL_GEOMETRY_SHADER;
            case Renderer::ShaderType::Fragment:
                return GL_FRAGMENT_SHADER;
            case Renderer::ShaderType::Compute:
                return GL_COMPUTE_SHADER;
        }
        return GL_INVALID_ENUM;
    }

    void CheckErrorShader(GLuint res, const std::string &eMsg) {
        auto st = GL_TRUE;
        glGetShaderiv(res, GL_COMPILE_STATUS, &st);
        if (st == GL_FALSE) DebugWarning(eMsg); else return;
        int logLength, charsWritten;
        glGetShaderiv(res, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 1) {
            const auto log = temp::make_unique<GLchar[]>(logLength);
            glGetShaderInfoLog(res, logLength, &charsWritten, log.get());
            throw std::runtime_error(log.get());
        }
    }

    void CheckErrorProgram(GLuint res, const std::string &eMsg) {
        auto st = GL_TRUE;
        glGetProgramiv(res, GL_LINK_STATUS, &st);
        if (st == GL_FALSE) DebugWarning(eMsg); else return;
        int logLength, charsWritten;
        glGetProgramiv(res, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 1) {
            const auto log = temp::make_unique<GLchar[]>(logLength);
            glGetProgramInfoLog(res, logLength, &charsWritten, log.get());
            throw std::runtime_error(log.get());
        }
    }

    Renderer::Internal::Object *ToFakePtr(GLuint hdc) noexcept {
        return reinterpret_cast<Renderer::Internal::Object *>(static_cast<uintptr_t>(hdc));
    }

    GLuint FromFakePtr(Renderer::Internal::Object *ptr) noexcept {
        return static_cast<GLuint>(reinterpret_cast<uintptr_t>(ptr));
    }

    using GLObject = Renderer::Internal::Object;

    auto MakeProgram(const temp::unordered_map<Renderer::ShaderType, Renderer::GLShader> &stages) {
        auto program = glCreateProgram();
        auto deleter = [](GLObject *ptr) noexcept {
            if (ptr) {
                glDeleteProgram(FromFakePtr(ptr));
            }
        };
        auto result = std::unique_ptr<GLObject, decltype(deleter)>(ToFakePtr(program), deleter);
        // TODO(implement checks)
        for (auto&&[type, shader]: stages) {
            glAttachShader(program, FromFakePtr(shader.get()));
        }
        glLinkProgram(program);
        //PrintDebug(program);
        CheckErrorProgram(program, "Shader linking error!");
        return result;
    }

    GLenum MapDataType(Renderer::DataType type) {
        switch (type) {
            case Renderer::DataType::Int32:
                return GL_INT;
            case Renderer::DataType::Float16:
                return GL_HALF_FLOAT;
            case Renderer::DataType::Float32:
                return GL_FLOAT;
            case Renderer::DataType::Float64:
                return GL_DOUBLE;
        }
        throw std::runtime_error("Unknown DataType Enum");
    }

    auto MakeVAO(const temp::vector<Renderer::Internal::AttributeSpec> &attributes) {
        GLuint vao;
        glCreateVertexArrays(1, &vao);
        auto deleter = [](GLObject *ptr) noexcept {
            if (ptr) {
                auto vao = FromFakePtr(ptr);
                glDeleteVertexArrays(1, &vao);
            }
        };
        auto result = std::unique_ptr<GLObject, decltype(deleter)>(ToFakePtr(vao), deleter);
        // TODO(implement checks)
        for (auto&&[type, binding, location, width, offset]: attributes) {
            glEnableVertexArrayAttrib(vao, location);
            glVertexArrayAttribFormat(vao, location, width, MapDataType(type), GL_FALSE, offset);
            glVertexArrayAttribBinding(vao, location, binding);
        }
        return result;
    }

    auto ZipBinds(GLuint vao, const temp::unordered_map<int, std::pair<int, int>> &binds) {
        std::vector<int> result{};
        for (auto&&[bind, zip]: binds) {
            if (result.size() <= bind) result.resize(bind + 1, 0);
            result[bind] = zip.first;
            glVertexArrayBindingDivisor(vao, bind, zip.second);
        }
        return result;
    }

    GLenum CastTopology(Renderer::Topology topology) {
        switch (topology) {
            case Renderer::Topology::PointList:
                return GL_POINTS;
            case Renderer::Topology::LineList:
                return GL_LINES;
            case Renderer::Topology::LineStrip:
                return GL_LINE_STRIP;
            case Renderer::Topology::TriangleList:
                return GL_TRIANGLES;
            case Renderer::Topology::TriangleStrip:
                return GL_TRIANGLE_STRIP;
            case Renderer::Topology::TriangleFan:
                return GL_TRIANGLE_FAN;
            case Renderer::Topology::Quad:
                return GL_QUADS;
        }
        throw std::runtime_error("Unknown Topology");
    }

    GLenum CastIndexType(Renderer::IndexType index) {
        switch (index) {
            case Renderer::IndexType::U8:
                return GL_UNSIGNED_BYTE;
            case Renderer::IndexType::U16:
                return GL_UNSIGNED_SHORT;
            case Renderer::IndexType::U32:
                return GL_UNSIGNED_INT;
        }
        throw std::runtime_error("Unknown IndexType");
    }

    class PipelineOGL : public Renderer::IPipeline {
    public:
        PipelineOGL(GLenum mode, std::vector<int> strides, GLuint vao, GLuint program) noexcept:
                mMode(mode), mStrides(std::move(strides)), mVAO(vao), mProgram(program) {}

        ~PipelineOGL() noexcept {
            glDeleteProgram(mProgram);
            glDeleteVertexArrays(1, &mVAO);
        }

        void Use() override {
            glUseProgram(mProgram);
            glBindVertexArray(mVAO);
        }

        void BindVertexBuffer(int bind, GLuint buffer, int offset) override {
            glVertexArrayVertexBuffer(mVAO, bind, buffer, offset, mStrides[bind]);
        }

        void BindIndexBuffer(GLuint buffer, Renderer::IndexType type) override {
            mElement = CastIndexType(type);
            glVertexArrayElementBuffer(mVAO, buffer);
        }

        void Draw(int count, int first, int instance) override {
            if (instance == 1)
                return glDrawArrays(mMode, first, count);
            glDrawArraysInstanced(mMode, first, count, instance);
        }

        void DrawIndexed(int count, int first, int instance) override {
            intptr_t offset = first;
            if (mElement == GL_UNSIGNED_SHORT) offset *= 2;
            if (mElement == GL_UNSIGNED_INT) offset *= 4;
            if (instance == 1) return glDrawElements(mMode, count, mElement, reinterpret_cast<void *>(offset));
            glDrawElementsInstanced(mMode, count, mElement, reinterpret_cast<void *>(offset), instance);
        }

        void SetUniform(GLint loc, float value) override {
            glProgramUniform1f(mProgram, loc, value);
        }

        void SetUniform(GLint loc, int value) override {
            glProgramUniform1i(mProgram, loc, value);
        }

        void SetUniform(GLint loc, float v0, float v1, float v2, float v3) override {
            glProgramUniform4f(mProgram, loc, v0, v1, v2, v3);
        }

        void SetUniform(GLint loc, float *value) override {
            glProgramUniformMatrix4fv(mProgram, loc, 1, GL_FALSE, value);
        }

    private:
        GLenum mMode;
        GLuint mVAO;
        GLuint mProgram;
        GLenum mElement{};
        std::vector<int> mStrides;
    };

    temp::string LoadFile(const std::string &path) {
        auto ss = temp::ostringstream{};
        std::ifstream input_file(path);
        if (!input_file.is_open()) {
            throw std::runtime_error("No such file:" + path);
        }
        ss << input_file.rdbuf();
        return ss.str();
    }
}

namespace Renderer {
    GLShader Compile(ShaderType type, std::string_view program, const std::vector<std::string> &defines) {
        const auto preprocessed = Preprocess(program, CollapseDefines(defines));
        //创建shader
        const auto mode = MapShaderTypes(type);
        if (mode == GL_INVALID_ENUM) throw std::runtime_error("Bad Shader Type Enum");
        const auto res = glCreateShader(mode);
        const auto dataP = preprocessed.c_str();
        const auto dataL = static_cast<int>(preprocessed.size());
        glShaderSource(res, 1, reinterpret_cast<const GLchar *const *>(&dataP), &dataL);
        glCompileShader(res);
        CheckErrorShader(res, "Shader compilation error! Shader: " + std::string(program));
        return {ToFakePtr(res), [](auto ptr) noexcept { glDeleteShader(FromFakePtr(ptr)); }};
    }

    GLShader CompileFile(ShaderType type, const std::string &path, const std::vector<std::string> &defines) {
        const auto source = LoadFile(path);
        return Compile(type, source, defines);
    }

    Pipeline PipelineBuilder::Build() {
        auto program = MakeProgram(mStages);
        auto vao = MakeVAO(mSpecs);
        auto strides = ZipBinds(FromFakePtr(vao.get()), mBindings);
        auto object = std::make_shared<PipelineOGL>(
                CastTopology(mTopology), std::move(strides),
                FromFakePtr(vao.release()), FromFakePtr(program.release())
        );
        return std::static_pointer_cast<IPipeline>(std::move(object));
    }

    GLuint GetDefaultQuadIndex() {
        static auto buffer = []() {
            BufferBuilder<uint32_t> builder{};
            for (auto i = 0; i < 262144; i += 4) builder.put<1>(i, i + 1, i + 2, i, i + 2, i + 3);
            GLuint ibo{0};
            vtxCount count{0};
            builder.flush(ibo, count);
            return ibo;
        }();
        return buffer;
    }
}
