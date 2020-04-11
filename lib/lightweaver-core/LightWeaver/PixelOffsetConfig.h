#pragma once
#include <memory>
#include <Arduino.h>

namespace LightWeaver {
    class PixelOffsetConfig {
        public:
            enum class Type {
                SCALE,
                RANDOM,
                LIST
            };
        private:
            Type type;
            // For SCALE type
            float scale;
            // For LIST type
            uint8_t offsetCount;
            std::unique_ptr<float[]> offsets;
            // For RANDOM type
            uint16_t factor1;
            uint16_t factor2;
        PixelOffsetConfig(Type type, float scale, uint8_t offsetCount, std::unique_ptr<float[]> offsets, uint16_t factor1, uint16_t factor2) :
            type(type),
            scale(scale),
            offsetCount(offsetCount),
            offsets(std::move(offsets)),
            factor1(factor1),
            factor2(factor2) {};

        uint8_t getRandomIndex(uint8_t seed) const {
            return (seed * factor1) ^ factor2;
        }
        public:

        static PixelOffsetConfig withNone() {
            return PixelOffsetConfig(Type::SCALE,0,0,nullptr,0,0);
        }
        static PixelOffsetConfig withScale(float scale) {
            return PixelOffsetConfig(Type::SCALE,scale,0,nullptr,0,0);
        }
        static PixelOffsetConfig withList(uint8_t count, float* offsets) {
            std::unique_ptr<float[]> offsetList = std::unique_ptr<float[]>(count == 0 ? nullptr : new float[count]);
            if (offsetList && count > 0) {
                memcpy(offsetList.get(), offsets, sizeof(float) * count);
            }
            PixelOffsetConfig config = PixelOffsetConfig(Type::LIST, 0, count, std::move(offsetList), 0, 0);
            return config;

        }
        static PixelOffsetConfig withRandom() {
            return PixelOffsetConfig(Type::RANDOM, 0, 0, nullptr, random(0xFF00,0xFFFF) * 2 + 1, random(0xFF00, 0xFFFF) * 2 + 1);
        }

        PixelOffsetConfig(const PixelOffsetConfig& other):
            type(other.type),
            scale(other.scale),
            offsetCount(other.offsetCount),
            offsets(std::unique_ptr<float[]>{other.offsetCount > 0 ? new float[offsetCount] : nullptr}),
            factor1(other.factor1),
            factor2(other.factor2) {
                for (uint8_t i = 0; i < other.offsetCount; i++) {
                    this->offsets[i] = other.offsets[i];
                }
            }

        const float getOffset(uint8_t index, uint8_t count) const{
            switch (type) {
                case Type::SCALE:
                    if (count <= 1) return 0;
                    return ((float)(index) / (float)(count - 1)) * scale;
                case Type::RANDOM:
                    return (float)getRandomIndex(index) / 255.0f;
                case Type::LIST:
                    if (offsetCount == 0) return 0;
                    if (index > offsetCount) return 0;
                    return offsets[index];
            }
            return 0;
        }
    };
}