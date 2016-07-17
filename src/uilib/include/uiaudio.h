/*
UILib - A Open-Source UI-Library

Copyright(C) 2016 Infinideastudio-UITeam

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and / or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#ifndef UIAUDIO_H_
#define UIAUDIO_H_

#include "uimath.h"
#include <float.h>
#include <map>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>
#include <maximilian.h>
#include <RtAudio.h>

namespace UI
{

    namespace Audio
    {
        class UILIB_API Source;

        class UILIB_API Effect
        {
        public:
            Effect() = default;
            virtual ~Effect() = default;
            virtual double operator()(double input, Source* thisSource);
        };

        class UILIB_API EffectEx
        {
        public:

            EffectEx() = default;
            virtual ~EffectEx() = default;
            virtual double operator()(double input, size_t channel, Source* thisSource);
        };

        class UILIB_API Source
        {
        private:
            Math::Vec3 pos;
            double samplePos;
            bool isPlay, loop;
            std::shared_ptr<maxiSample> sample;
            double sampleValue;
        public:
            size_t refcount;
            double freq;//for User
            double realFreq;//for Effects
            double gain;

            Math::Vec3 move;
            vector<shared_ptr<UI::Audio::Effect>> effects;
            vector<shared_ptr<UI::Audio::EffectEx>> effectsExs;

            Source();
            Source(const Source& source_);
            Source(std::shared_ptr<maxiSample> sample_);
            ~Source();
            bool isStopped();
            void play(bool loop);
            void stop();
            double setPos(double Pos_ = 0.0);
            double operator()(size_t channel);
            void Sample();
            //operator Math::Vec3();
            //operator Math::Vec3&();
            Math::Vec3& getPos() { return pos; }
        };

        class UILIB_API SourceHandler
        {
        public:
            SourceHandler(Source* src);
            ~SourceHandler();
            Source& get() const;
        private:
            Source* msrc;
        };

        class UILIB_API Listener
        {
        private:
            Math::Vec3 pos;
        public:
            Math::Vec3 move;
            std::vector<Math::Vec3> pos2;

            Listener() = default;
            ~Listener() = default;
            //operator Math::Vec3&();
            Math::Vec3& getPos() { return pos; }
            Math::Vec3 operator[](size_t channel);
        };

        class UILIB_API OutEffect
        {
        public:
            OutEffect() = default;
            virtual ~OutEffect() = default;
            virtual double operator()(double input, size_t channel);
        };

        class UILIB_API OutEffectEx
        {
        public:
            OutEffectEx() = default;
            virtual ~OutEffectEx() = default;
            virtual void operator()(double* input, size_t channel);
        };

        UILIB_API extern std::map < std::string, std::function<bool(const std::string&, maxiSample&) >> sourceLoaders;

        UILIB_API extern std::vector <OutEffect*> outEffects;

        UILIB_API extern std::vector <OutEffectEx*> outEffectExs;

        UILIB_API extern Listener thisListener;

        UILIB_API extern double(*speedOfSoundSampler)(const Math::Vec3& pos);

        UILIB_API extern unsigned char SampleNum;

        UILIB_API void init();

        UILIB_API SourceHandler load(const std::string& fileName);

        UILIB_API bool bind(const std::shared_ptr<maxiSample> & sample, SourceHandler& handler);

        UILIB_API void unInit();

        UILIB_API void openStream();

        namespace Effects
        {
            /*
            Balance
            名称:声道平衡
            作用:调节声道音量,或者作为Stereo的替代品.
            */
            class UILIB_API Balance : public OutEffect
            {
            public:
                std::vector<double> factors;
                Balance() = default;
                ~Balance() = default;
                virtual double operator()(double input, size_t channel);
            };

            /*
            Damping
            名称:衰减
            作用:计算声音衰减,不可作为回声的替代品.
            */
            class UILIB_API Damping : public OutEffect
            {
            private:
                std::vector<double> values;
            public:
                std::vector<double> factors;
                Damping() = default;
                ~Damping() = default;
                virtual double operator()(double input, size_t channel);
            };

            /*
            EQ
            名称:EQ均衡器
            作用:用于播放BGM或空间感渲染.参数以16bit为准.
            */
            class UILIB_API EQ : public OutEffect, public Effect
            {
            private:
                struct Point
                {
                    std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
                    double value = 0.0;
                    bool end = false;
                };
                std::vector<Point> samplePoints;
                std::vector<double> freqs;
            public:
                double add = 0.0;
                struct Factors
                {
                    double facs[10];
                }
                EQFactor;
                //Preset from http://blog.sina.com.cn/s/blog_4cb5837d01018rax.html
                //                     31   62    125  250  500  1K   2K   4K   8K   16K
                const Factors bass = { { 6.0, 4.0, -5.0, 2.0, 3.0, 4.0, 4.0, 5.0, 5.0, 6.0 } };
                const Factors rock = { { 6.0, 4.0, 0.0, -2.0, -6.0, 1.0, 4.0, 6.0, 7.0, 9.0 } };
                const Factors vocal = { { 4.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 4.0, 3.0, 3.0 } };
                //Interval
                const double interval[11][2] =
                {
                    { DBL_MIN, 31.0 },
                    { 31.0, 62.0 },
                    { 125.0, 250.0 },
                    { 250.0, 500.0 },
                    { 500.0, 1000.0 },
                    { 1000.0, 2000.0 },
                    { 2000.0, 4000.0 },
                    { 4000.0, 8000.0 },
                    { 8000.0, 16000.0 },
                    { 16000.0, DBL_MAX }
                };

                EQ() = default;
                ~EQ() = default;
                virtual double operator()(double input, size_t channel);
                virtual double operator()(double input, Source* thisSource);
            };

            /*
            Amplifier
            名称:差异放大器
            作用:放大各声道的差异,提高立体感,作为Stereo的补偿.
            */
            class UILIB_API Amplifier : public OutEffectEx
            {
            public:
                double factor = 1.0;
                Amplifier() = default;
                ~Amplifier() = default;
                virtual void operator()(double* input, size_t channel);
            };

            /*
            Stereo
            名称:3D效果器
            作用:通过声音衰减模型计算音量,是立体感的基础,与Listener的pos2搭配使用.
            */
            class UILIB_API Stereo : public EffectEx
            {
            public:
                Stereo() = default;
                ~Stereo() = default;
                double rolloffFactor = 1.0;
                double referenceDistance = 1.0;
                double maxDistance = 100.0;
                enum Mode
                {
                    INVERSE_DISTANCE,
                    INVERSE_DISTANCE_CLAMPED,
                    LINEAR_DISTANCE,
                    LINEAR_DISTANCE_CLAMPED,
                    EXPONENT_DISTANCE,
                    EXPONENT_DISTANCE_CLAMPED
                } dampMode = EXPONENT_DISTANCE_CLAMPED;
                virtual double operator()(double input, size_t channel, Source* thisSource);
            };

            /*
            Doppler
            名称:多普勒效应
            作用:通过多普勒效应模型计算频率,提高立体感.(要获得更好的体验请更换speedOfSoundSampler函数,并提高采样次数)
            */
            class UILIB_API Doppler : public Effect
            {
            public:
                Doppler() = default;
                ~Doppler() = default;
                virtual double operator()(double input, Source* thisSource);
            };

            /*
            Noise
            名称:噪音生成器
            作用:用于生成噪音并叠加在原波形上,用于播放自然声音.
            */
            class UILIB_API Noise : public Effect
            {
            public:
                Noise();
                ~Noise() = default;
                enum NoiseMode
                {
                    Add,
                    Factor
                } noiseMode = Factor;
                double factor = 0.1;
                virtual double operator()(double input, Source* thisSource);
            };

            /*
            Compressor
            Maxmilian自带效果器.
            */
            class UILIB_API Compressor : public Effect
            {
            public:
                Compressor() = default;
                ~Compressor() = default;
                maxiDyn compressor;
                virtual double operator()(double input, Source* thisSource);
            };
        }
    }
}

#endif
