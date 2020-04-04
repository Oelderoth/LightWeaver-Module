#pragma once
#include <memory>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LightWeaver/ColorSource.h>
#include <LightWeaver/colorSources/SolidColorSource.h>
#include <LightWeaver/colorSources/FadeColorSource.h>
#include <LightWeaver/colorSources/OverlayColorSource.h>

namespace LightWeaver {
    class ColorSourceDeserializer {
        struct StringListBuilder {
            private:
            const String separator;
            public:
            String value;
            StringListBuilder(const String& separator) : separator(separator), value("") {}
            StringListBuilder(const String& separator, const String& value) : separator(separator), value(value) {};

            bool empty() {
                return value.length() == 0;
            }

            friend const StringListBuilder operator+(const StringListBuilder& lhs, const String& rhs) {
                return StringListBuilder(lhs.separator, lhs.value.length() ? lhs.value + lhs.separator + rhs : rhs);
            }

            const StringListBuilder& operator+=(const String& rhs) {
                value = value.length() ? value + separator + rhs : rhs;
                return *this;
            }

            operator String() const{
                return value;
            }
        };

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
                    static Result withSuccess(const ColorSource& colorSource) {
                        return Result(std::unique_ptr<ColorSource>{colorSource.clone()});
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
        static bool validateRequiredField(const JsonVariant& field, const String& fieldName, StringListBuilder& err);
        template<typename T> static bool validateFieldType(const JsonVariant& field, const String& fieldName, StringListBuilder& err);

        static RgbaColor deserializeColor(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static EasingFunction deserializeEasingFunction(const JsonVariant& obj, const StringListBuilder& fieldName,  StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static EasingFunction deserializeEasingFunctionFromName(const String& name);

        static Result deserialize(const JsonVariant& obj, const StringListBuilder& fieldName);
        static ColorSource* deserializeColorSource(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static ColorSource* deserializeSolidColorSource(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static ColorSource* deserializeFadeColorSource(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        static ColorSource* deserializeOverlayColorSource(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields);
        public:
        static Result deserialize(const JsonVariant& obj);
    };
}