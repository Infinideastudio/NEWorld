#include "Renderer.h"
#include <fstream>
#include <algorithm>
#include "Temp/String.h"
#include "Frustum.h"
#include "BufferBuilder.h"

namespace Renderer {

    /*
    好纠结啊好纠结，“高级”渲染模式里的所有数据要不要都用VertexAttribArray啊。。。
    然而我还是比较懒。。。所以除了【附加】的顶点属性之外，其他属性（比如颜色、纹理坐标）都保留原来的算了。。。

    说到为啥要用【附加】的顶点属性。。。这是由于Shadow Map的精度问题。。。
    有的时候背光面的外圈会有亮光。。。很难看。。。所以要用Shader把背光面弄暗。。。
    于是如何让shader知道这个面朝哪里呢？懒得用NormalArray的我就用了一个附加的顶点属性。。。
    0.0f表示前面(z+)，1.0f表示后面(z-)，2.0f表示右面(x+)，3.0f表示左面(x-)，4.0f表示上面(y+)，5.0f表示下面(y-)

        你没有看错。。。这些值。。。全都是

            浮！
                点！
                    型！
                        的！！！！！！！

    坑爹的GLSL不支持整型作为顶点属性。。。只好用浮点型代替了(╯‵□′)╯︵┻━┻
    然后为了解决浮点数的精度问题，我在shader里写了个四舍五入取整。。。
    不说了。。。

    等等我还没有签名呢。。。
    --qiaozhanrong

    ====================================================
    留言板：

    1楼. qiaozhanrong: 自己抢个沙发先
    2楼. Null: 这就是你在源码里写这么一长串的理由？23333333333
    3楼. qiaozhanrong: 无聊啊233333333333

    4楼. [请输入姓名]: [请输入回复内容]

    [回复]
    ====================================================
    */

    bool AdvancedRender;
    int ShadowRes = 4096;
    int MaxShadowDist = 4;
    int shadowdist;
    float sunlightXrot, sunlightYrot;
    std::vector<Pipeline> pipelines;
    int ActivePipeline;
    int index = 0, size = 0;
    unsigned int ShadowFBO, DepthTexture;

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

    struct IndirectBaseVertex {
        uint32_t count;
        uint32_t instanceCount;
        uint32_t firstIndex;
        uint32_t baseVertex;
        uint32_t baseInstance;
    };

    void RenderBufferDirect(VBOID buffer, vtxCount vtxs) {
        pipelines[ActivePipeline]->BindVertexBuffer(1, buffer, 0);
        pipelines[ActivePipeline]->DrawIndexed(vtxs + vtxs/2, 0);
        /*glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetDefaultQuadIndex());
        BatchStart(tc, cc, ac);
        IndirectBaseVertex command = {static_cast<uint32_t>(vtxs + vtxs / 2), 1, 0, 0, 0};
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &command, 1, 0);*/
    }

    static temp::string LoadFile(const std::string &path) {
        auto ss = temp::ostringstream{};
        std::ifstream input_file(path);
        if (!input_file.is_open()) {
            throw std::runtime_error("No such file:" + path);
        }
        ss << input_file.rdbuf();
        return ss.str();
    }

    Pipeline BuildPipeline(
            const std::string& vshPath, const std::string& fshPath,
            int tc, int cc, int ac, bool useAc, const std::vector<std::string>& defines = {}
    ) noexcept {
        constexpr int sof = sizeof(float);
        PipelineBuilder builder{Topology::TriangleList};
        builder.SetBinding(1, (tc + cc + ac + 3) * sof, 0);
        if (ac && useAc) builder.AddAttribute(DataType::Float32, 1, 1, ac, 0);
        if (tc) builder.AddAttribute(DataType::Float32, 1, 2, tc, ac * sof);
        if (cc) builder.AddAttribute(DataType::Float32, 1, 3, cc, (ac + tc) * sof);
        builder.AddAttribute(DataType::Float32, 1, 4, 3, (ac + tc + cc) * sof);
        const auto sourceVsh = LoadFile(vshPath);
        const auto sourceFsh = LoadFile(fshPath);
        builder.SetShader(ShaderType::Vertex, Compile(ShaderType::Vertex, sourceVsh, defines));
        builder.SetShader(ShaderType::Fragment, Compile(ShaderType::Fragment, sourceFsh, defines));
        auto result = builder.Build();
        result->BindIndexBuffer(GetDefaultQuadIndex(), IndexType::U32);
        return result;
    }

    void initShaders() {
        std::vector<std::string> defines {};
        sunlightXrot = 30.0f;
        sunlightYrot = 60.0f;
        shadowdist = std::min(MaxShadowDist, viewdistance);
        pipelines = {
                BuildPipeline("./Assets/Shaders/Main.vsh", "./Assets/Shaders/Main.fsh", 2, 3, 1, true),
                BuildPipeline("./Assets/Shaders/Shadow.vsh", "./Assets/Shaders/Shadow.fsh", 0,0,0, false),
                BuildPipeline("./Assets/Shaders/Depth.vsh", "./Assets/Shaders/Depth.fsh", 0, 0, 0, false, defines),
                BuildPipeline("./Assets/Shaders/Simple.vsh", "./Assets/Shaders/Simple.fsh", 2, 3, 1, false)
        };

        glGenTextures(1, &DepthTexture);
        glBindTexture(GL_TEXTURE_2D, DepthTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, ShadowRes, ShadowRes, 0, GL_DEPTH_COMPONENT,
                     GL_UNSIGNED_INT, NULL);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DepthTexture);
        glActiveTexture(GL_TEXTURE0);

        glGenFramebuffersEXT(1, &ShadowFBO);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ShadowFBO);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, DepthTexture, 0);
        if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT) {
            DebugError("Frame buffer creation error!");
        }
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        pipelines[MainShader]->SetUniform(0, 0);
        pipelines[MainShader]->SetUniform(1, 1);
        pipelines[MainShader]->SetUniform(5, skycolorR, skycolorG, skycolorB, 1.0f);
        pipelines[SimpleShader]->SetUniform(0, 0);
    }

    void destroyShaders() {
        glDeleteTextures(1, &DepthTexture);
        glDeleteFramebuffersEXT(1, &ShadowFBO);
    }

    void BindPipeline(int shaderID) {
        pipelines[shaderID]->Use();
        ActivePipeline = shaderID;
    }

    void EnableAdvancedShaders() {
        shadowdist = std::min(MaxShadowDist, viewdistance);
        //Enable pipeline
        auto &pipeline = pipelines[MainShader];
        BindPipeline(MainShader);

        //Calc matrix
        const auto scale = 16.0f * sqrt(3.0f);
        const auto length = shadowdist * scale;
        Frustum frus;
        frus.LoadIdentity();
        frus.SetOrtho(-length, length, -length, length, -length, length);
        frus.MultRotate(sunlightXrot, 1.0f, 0.0f, 0.0f);
        frus.MultRotate(sunlightYrot, 0.0f, 1.0f, 0.0f);

        //Set uniform
        pipeline->SetUniform(6, viewdistance * 16.0f);
        pipeline->SetUniform(2, frus.getProjMatrix());
        pipeline->SetUniform(3, frus.getModlMatrix());
    }

    void EnableSimpleShaders() {
        BindPipeline(SimpleShader);
    }

    void EnableShaders() {
        if (AdvancedRender) EnableAdvancedShaders(); else EnableSimpleShaders();
    }

    void DisableShaders() {
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void StartShadowPass() {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ShadowFBO);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        BindPipeline(ShadowShader);
        glViewport(0, 0, ShadowRes, ShadowRes);
    }

    void EndShadowPass() {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
        glBindVertexArray(0);
        glViewport(0, 0, windowwidth, windowheight);
    }
}
