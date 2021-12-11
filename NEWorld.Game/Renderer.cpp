#include "Renderer.h"
#include "Shader.h"
#include <algorithm>
#include "Frustum.h"

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

    int Vertexes, Texcoordc, Colorc, Attribc;
    float *VertexArray = nullptr;
    float *VA = nullptr;
    float TexCoords[3], Colors[4], Attribs;
    //unsigned int Buffers[3];
    bool AdvancedRender;
    int ShadowRes = 4096;
    int MaxShadowDist = 4;
    int shadowdist;
    float sunlightXrot, sunlightYrot;
    std::vector<Shader> shaders;
    int ActiveShader;
    int index = 0, size = 0;
    unsigned int ShadowFBO, DepthTexture;
    unsigned int ShaderAttribLoc = 0;

    void Init(int tc, int cc, int ac) {
        Texcoordc = tc;
        Colorc = cc;
        Attribc = ac;
        if (VertexArray == nullptr) VertexArray = new float[ArraySize];
        index = 0;
        VA = VertexArray;
        Vertexes = 0;
        size = (tc + cc + +ac + 3) * 4;
    }

    void Vertex3f(float x, float y, float z) {
        if ((Vertexes + 1) * (Texcoordc + Colorc + 3) > ArraySize) return;
        if (Attribc != 0) VertexArray[index++] = Attribs;
        if (Texcoordc != 0) memcpy(VertexArray + index, TexCoords, Texcoordc * sizeof(float));
        index += Texcoordc;
        if (Colorc != 0) memcpy(VertexArray + index, Colors, Colorc * sizeof(float));
        index += Colorc;
        VertexArray[index++] = x;
        VertexArray[index++] = y;
        VertexArray[index++] = z;
        Vertexes++;
    }

    void TexCoord2f(float x, float y) {
        TexCoords[0] = x;
        TexCoords[1] = y;
    }

    void TexCoord3f(float x, float y, float z) {
        TexCoords[0] = x;
        TexCoords[1] = y;
        TexCoords[2] = z;
    }

    void Color3f(float r, float g, float b) {
        Colors[0] = r;
        Colors[1] = g;
        Colors[2] = b;
    }

    void Color4f(float r, float g, float b, float a) {
        Colors[0] = r;
        Colors[1] = g;
        Colors[2] = b;
        Colors[3] = a;
    }

    void Attrib1f(float a) { Attribs = a; }

    void Flush(VBOID &buffer, vtxCount &vtxs) {

        //上次才知道原来Flush还有冲厕所的意思QAQ
        //OpenGL有个函数glFlush()，翻译过来就是GL冲厕所() ←_←

        vtxs = Vertexes;
        if (Vertexes != 0) {
            if (buffer == 0) glGenBuffers(1, &buffer);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER,
                            Vertexes * ((Texcoordc + Colorc + Attribc + 3) * sizeof(float)),
                            VertexArray, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    void BatchStart(int tc, int cc, int ac) noexcept {
        auto cnt = tc + cc + 3;
        if (!AdvancedRender || ac == 0) {
            if (tc != 0) {
                if (cc != 0) {
                    glTexCoordPointer(tc, GL_FLOAT, cnt * sizeof(float), static_cast<float *>(0));
                    glColorPointer(cc, GL_FLOAT, cnt * sizeof(float), (float *) (tc * sizeof(float)));
                    glVertexPointer(3, GL_FLOAT, cnt * sizeof(float), (float *) ((tc + cc) * sizeof(float)));
                } else {
                    glTexCoordPointer(tc, GL_FLOAT, cnt * sizeof(float), static_cast<float *>(0));
                    glVertexPointer(3, GL_FLOAT, cnt * sizeof(float), (float *) (tc * sizeof(float)));
                }
            } else {
                if (cc != 0) {
                    glColorPointer(cc, GL_FLOAT, cnt * sizeof(float), static_cast<float *>(0));
                    glVertexPointer(3, GL_FLOAT, cnt * sizeof(float), (float *) (cc * sizeof(float)));
                } else glVertexPointer(3, GL_FLOAT, cnt * sizeof(float), static_cast<float *>(0));
            }
        } else {
            cnt += ac;
            glVertexAttribPointer(ShaderAttribLoc, ac, GL_FLOAT, GL_FALSE, cnt * sizeof(float), static_cast<float *>(0));
            glTexCoordPointer(tc, GL_FLOAT, cnt * sizeof(float), (float *) (ac * sizeof(float)));
            glColorPointer(cc, GL_FLOAT, cnt * sizeof(float), (float *) ((ac + tc) * sizeof(float)));
            glVertexPointer(3, GL_FLOAT, cnt * sizeof(float), (float *) ((ac + tc + cc) * sizeof(float)));
        }
    }

    void RenderBuffer(VBOID buffer, vtxCount vtxs) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        //这个框是不是很装逼2333 --qiaozhanrong
        //====================================================================================================//
        /**/                                                                                                /**/
        /**/                                                                                                /**/
        /**/                                glDrawArrays(GL_QUADS, 0, vtxs);                                /**/
        /**/                                                                                                /**/
        /**/                                                                                                /**/
        //====================================================================================================//
    }

    void RenderBufferDirect(VBOID buffer, vtxCount vtxs, int tc, int cc, int ac) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        BatchStart(tc, cc, ac);
        //这个框是不是很装逼2333 --qiaozhanrong
        //====================================================================================================//
        /**/                                                                                                /**/
        /**/                                                                                                /**/
        /**/                                glDrawArrays(GL_QUADS, 0, vtxs);                                /**/
        /**/                                                                                                /**/
        /**/                                                                                                /**/
        //====================================================================================================//
    }

    void initShaders() {
        ShaderAttribLoc = 1;
        std::set<std::string> defines;
        sunlightXrot = 30.0f;
        sunlightYrot = 60.0f;
        shadowdist = std::min(MaxShadowDist, viewdistance);
        shaders = {
                Shader("./Assets/Shaders/Main.vsh", "./Assets/Shaders/Main.fsh", true),
                Shader("./Assets/Shaders/Shadow.vsh", "./Assets/Shaders/Shadow.fsh", false),
                Shader("./Assets/Shaders/Depth.vsh", "./Assets/Shaders/Depth.fsh", false, defines)
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

        shaders[MainShader].bind();
        shaders[MainShader].setUniform("Tex", 0);
        shaders[MainShader].setUniform("DepthTex", 1);
        shaders[MainShader].setUniform("SkyColor", skycolorR, skycolorG, skycolorB, 1.0f);
        Shader::unbind();
    }

    void destroyShaders() {
        for (auto & shader : shaders)
            shader.release();
        shaders.clear();
        glDeleteTextures(1, &DepthTexture);
        glDeleteFramebuffersEXT(1, &ShadowFBO);
    }

    void EnableShaders() {
        shadowdist = std::min(MaxShadowDist, viewdistance);

        //Enable shader
        auto& shader = shaders[MainShader];
        bindShader(MainShader);

        //Calc matrix
        const auto scale = 16.0f * sqrt(3.0f);
        const auto length = shadowdist * scale;
        Frustum frus;
        frus.LoadIdentity();
        frus.SetOrtho(-length, length, -length, length, -length, length);
        frus.MultRotate(sunlightXrot, 1.0f, 0.0f, 0.0f);
        frus.MultRotate(sunlightYrot, 0.0f, 1.0f, 0.0f);

        //Set uniform
        shader.setUniform("renderdist", viewdistance * 16.0f);
        shader.setUniform("Depth_proj", frus.getProjMatrix());
        shader.setUniform("Depth_modl", frus.getModlMatrix());

        //Enable arrays for additional vertex attributes
        glEnableVertexAttribArray(ShaderAttribLoc);
    }

    void DisableShaders() {
        //Disable shader
        Shader::unbind();

        //Disable arrays for additional vertex attributes
        glDisableVertexAttribArray(ShaderAttribLoc);
    }

    void StartShadowPass() {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ShadowFBO);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        bindShader(ShadowShader);
        glViewport(0, 0, ShadowRes, ShadowRes);
    }

    void EndShadowPass() {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
        Shader::unbind();
        glViewport(0, 0, windowwidth, windowheight);
    }

}
