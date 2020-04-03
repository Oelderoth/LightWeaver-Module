#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LightWeaver/ColorSource.h>
#include <LightWeaver/colorSources/SolidColorSource.h>
#include <LightWeaver/colorSources/FadeColorSource.h>
#include <memory>


namespace LightWeaver {
    class ColorSourceDeserializer {
        static bool validateRequiredField(String& missingField, JsonVariant& field, String name) {
            if (field.isNull()) {
                missingField += missingField.length() ? ", " + name : name;
                return false;
            }
            return true;
        }

        template <typename T>
        static bool validateRequiredFieldOfType(String& missingField, String& err, JsonVariant& field, String name) {
            if (!validateRequiredField(missingField, field, name)) return false;
            if (!field.is<T>()) {
                // Don't overwrite existing errors since we currently only show one
                if (err.length() == 0) err = "Invalid value for " + name;
                return false;
            }
            return true;
        }

        static RgbaColor deserializeColor(const JsonVariant& obj, String& missingFields, String& err, String prefix="") {
            JsonVariant r = obj["red"];
            JsonVariant g = obj["green"];
            JsonVariant b = obj["blue"];
            JsonVariant a = obj["alpha"];

            validateRequiredFieldOfType<uint8_t>(missingFields, err, r, prefix+"red");
            validateRequiredFieldOfType<uint8_t>(missingFields, err, g, prefix+"green");
            validateRequiredFieldOfType<uint8_t>(missingFields, err, b, prefix+"blue");

            return RgbaColor(r,g,b,a);
        }

        static EasingFunction deserializeEasingFunctionFromName(const String& name) {
            if (name == "Linear") return Easing::Linear;
            else if (name == "QuadraticIn") return Easing::QuadraticIn;
            else if (name == "QuadraticOut") return Easing::QuadraticOut;
            else if (name == "QuadraticInOut") return Easing::QuadraticInOut;
            else if (name == "CubicIn") return Easing::CubicIn;
            else if (name == "CubicOut") return Easing::CubicOut;
            else if (name == "CubicInOut") return Easing::CubicInOut;
            else if (name == "QuarticIn") return Easing::QuarticIn;
            else if (name == "QuarticOut") return Easing::QuarticOut;
            else if (name == "QuarticInOut") return Easing::QuarticInOut;
            else if (name == "QuinticIn") return Easing::QuinticIn;
            else if (name == "QuinticOut") return Easing::QuinticOut;
            else if (name == "QuinticInOut") return Easing::QuinticInOut;
            else if (name == "SinusoidalIn") return Easing::SinusoidalIn;
            else if (name == "SinusoidalOut") return Easing::SinusoidalOut;
            else if (name == "SinusoidalInOut") return Easing::SinusoidalInOut;
            else if (name == "ExponentialIn") return Easing::ExponentialIn;
            else if (name == "ExponentialOut") return Easing::ExponentialOut;
            else if (name == "ExponentialInOut") return Easing::ExponentialInOut;
            return nullptr;
        }

        static EasingFunction deserializeEasingFunction(const JsonVariant& obj, String& missingFields, String& err, String prefix = "") {
            if (obj.isNull()) return Easing::Linear;
            if (obj.is<String>()) {
                String name = obj.as<String>();
                EasingFunction easing = deserializeEasingFunctionFromName(name);
                if (easing) {
                    return easing;
                } else {
                    err = "Invalid easing function: " + name;
                    return Easing::Linear;
                }
            }
            if (obj.is<JsonObject>()) {
                JsonVariant name = obj["name"];
                validateRequiredFieldOfType<String>(missingFields, err, name, prefix + "name");
                if (name == "Mirror") {
                    JsonVariant child = obj["child"];
                    validateRequiredField(missingFields, child, prefix + "child");
                    EasingFunction function = deserializeEasingFunction(child, missingFields, err, prefix + "child.");
                    return Easing::Mirror(function);
                } else if (name == "Reverse") {
                    JsonVariant child = obj["child"];
                    validateRequiredField(missingFields, child, prefix + "child");
                    EasingFunction function = deserializeEasingFunction(child, missingFields, err, prefix + "child.");
                    return Easing::Reverse(function);
                } else {
                    return deserializeEasingFunction(name, missingFields, err, prefix);
                }
            }

            err = "Invalid easing function";
            return Easing::Linear;
        }

        static std::unique_ptr<ColorSource> deserializeSolidColorSource(const JsonVariant& obj, String& err) {
            JsonVariant id = obj["id"];

            String missingFields;
            validateRequiredField(missingFields, id, "id");

            RgbaColor color = deserializeColor(obj, missingFields, err);

            if (err.length()) {
                return std::unique_ptr<ColorSource>{};
            } else if (missingFields.length()) {
                err = "Required fields missing: " + missingFields;
                return std::unique_ptr<ColorSource>{};
            }

            return std::unique_ptr<ColorSource>{new SolidColorSource(id,color)};
        }

        static std::unique_ptr<ColorSource> deserializeFadeColorSource(const JsonVariant& obj, String& err) {
            JsonVariant id = obj["id"];
            JsonVariant start = obj["start"];
            JsonVariant end = obj["end"];
            JsonVariant duration = obj["duration"];
            JsonVariant easing = obj["easing"];
            bool loop = obj["loop"] | false;
            RgbaColor startColor;
            RgbaColor endColor;

            String missingFields;
            
            validateRequiredFieldOfType<uint32_t>(missingFields, err, id, "id");
            validateRequiredFieldOfType<uint16_t>(missingFields, err, duration, "duration");
            if (validateRequiredField(missingFields, start, "start")) {
                startColor = deserializeColor(start, missingFields, err, "start.");
            }
            if (validateRequiredField(missingFields, end, "end")) {
                endColor = deserializeColor(end, missingFields, err, "end.");
            }
            EasingFunction easingFunction = deserializeEasingFunction(easing, missingFields, err, "easing.");

            if (err.length()) {
                return std::unique_ptr<ColorSource>{};
            } else if (missingFields.length()) {
                err = "Required fields missing: " + missingFields;
                return std::unique_ptr<ColorSource>{};
            }

            return std::unique_ptr<ColorSource>{new FadeColorSource(id, startColor, endColor, duration, loop, easingFunction)};
        }

        public:
        static std::unique_ptr<ColorSource> deserialize(const JsonVariant& obj, String& err) {
            JsonVariant typeVariant = obj["type"];
            if (!validateRequiredField(err, typeVariant, "type")) {
                err = "Required fields missing: " + err;
                return std::unique_ptr<ColorSource>{};
            }
            
            String type = typeVariant.as<String>();
            if (type == "Solid") {
                return deserializeSolidColorSource(obj, err);
            } else if (type == "Fade") {
                return deserializeFadeColorSource(obj, err);
            } else {
                err = "Unknown type: " + type;
                return std::unique_ptr<ColorSource>{};
            }
        }
    };
}