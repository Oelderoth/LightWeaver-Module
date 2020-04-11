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
        public:

        PixelOffsetConfig(float scale) : type(Type::SCALE), scale(scale), offsetCount(0), offsets(nullptr) {};
        PixelOffsetConfig() : PixelOffsetConfig(0) {};
        PixelOffsetConfig(uint8_t offsetCount, float* offsets):
            type(Type::LIST),
            scale(0),
            offsetCount(offsetCount),
            offsets(std::unique_ptr<float[]>{offsetCount > 0 ? new float[offsetCount] : nullptr}) {
                for (uint8_t i = 0; i < offsetCount; i++) {
                    this->offsets[i] = offsets[i];
                }
            }
        PixelOffsetConfig(const PixelOffsetConfig& other):
            type(other.type),
            scale(other.scale),
            offsetCount(other.offsetCount),
            offsets(std::unique_ptr<float[]>{other.offsetCount > 0 ? new float[offsetCount] : nullptr}) {
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
                    // TODO: Random spread
                    return 0;
                case Type::LIST:
                    if (offsetCount == 0) return 0;
                    if (index > offsetCount) return 0;
                    return offsets[index];
            }
            return 0;
        }
    };
}