#pragma once
#include <memory>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LightWeaver/ColorSource.h>
#include <LightWeaver/ColorSet.h>
#include <LightWeaver/Gradient.h>
#include <LightWeaver/PixelOffsetConfig.h>
#include <LightWeaver/util/StringListBuilder.h>

namespace LightWeaver {
    class ColorSourceDeserializer {
        public:
            struct Result {
                public:
                    bool success;
                    std::unique_ptr<ColorSource> value;
                    String error;
                private:
                    Result(std::unique_ptr<ColorSource> value):
                        success(true),
                        value(std::move(value)) {};

                    Result(String error):
                        success(false),
                        value(std::unique_ptr<ColorSource>{}),
                        error(error) {};
                public:
                    static Result withSuccess(const std::unique_ptr<ColorSource>& colorSource) {
                        return Result(std::unique_ptr<ColorSource>{colorSource->clone()});
                    }

                    static Result withError(const String error) {
                        return Result(error);
                    }

                    static Result withMissingFields(const String missingFields) {
                        return withError("Required fields missing: " + missingFields);
                    }

                    static Result withInvalidFields(const String invalidFields) {
                        return withError("Invalid value for fields: " + invalidFields);
                    }

                    operator bool () const {
                        return success;
                    }
            };

        private:

        static RgbaColor deserializeColor(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static EasingFunction deserializeEasingFunction(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static EasingFunction deserializeEasingFunctionFromName(const String& name);
        static ColorSet deserializeColorSet(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static Gradient deserializeGradient(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static PixelOffsetConfig deserializePixelOffsetConfig(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);

        static Result deserialize(const JsonVariant& obj, const StringListBuilder& fieldName);
        static std::unique_ptr<ColorSource> deserializeColorSource(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static std::unique_ptr<ColorSource> deserializeSolidColorSource(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static std::unique_ptr<ColorSource> deserializeFadeColorSource(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static std::unique_ptr<ColorSource> deserializeOverlayColorSource(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static std::unique_ptr<ColorSource> deserializeGradientColorSource(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static std::unique_ptr<ColorSource> deserializeHsvMeanderColorSource(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        public:
        static Result deserialize(const JsonVariant& obj);
        static bool validateRequiredField(const JsonVariant& field, const String& fieldName, StringListBuilder& err);
        template<typename T> static bool validateFieldType(const JsonVariant& field, const String& fieldName, StringListBuilder& err);
    };
}