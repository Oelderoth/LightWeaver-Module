#include <string>
#include "ColorSourceDeserializer.h"

/**
 * Validation Macros
 * This file makes liberal use of macros to encourage consistent validation and deserialize patterns
 * These can be used to shorthand common use cases for the Validation Functions defined below
 */
#define Deserializer(type) std::unique_ptr<ColorSource> ColorSourceDeserializer::deserialize##type(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields)
#define requiredField(field) validateRequiredField(field, fieldName + #field, missingFields)
#define requiredFieldType(field,type) validateRequiredField(field, fieldName + #field, missingFields); validateFieldType<type>(field, fieldName + #field, invalidFields)
#define optionalFieldType(field,type) if (!field.isNull()) validateFieldType<type>(field, fieldName + #field, invalidFields)
#define deserializeAndValidate(field,deserializeFunction) deserializeFunction(field, fieldName + #field, missingFields, invalidFields)
#define isValid() (missingFields.empty() && invalidFields.empty())
#define enforceValidation(); if (!missingFields.empty()) { return Result::withMissingFields(missingFields); } else if (!invalidFields.empty()) { return Result::withInvalidFields(invalidFields); }

/**
 * Validation Functions
 */
namespace LightWeaver { 
    bool ColorSourceDeserializer::validateRequiredField(const JsonVariant& field, const String& fieldName, StringListBuilder& err) {
        if (field.isNull()) {
            err += fieldName;
            return false;
        }
        return true;
    }

    template<typename T>
    bool ColorSourceDeserializer::validateFieldType(const JsonVariant& field, const String& fieldName, StringListBuilder& err) {
        if (!field.is<T>()) {
            err += fieldName;
            return false;
        }
        return true;
    }
}

/**
 * Deserialization Helpers
 */
namespace LightWeaver {
    RgbaColor ColorSourceDeserializer::deserializeColor(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields) {
        if (!validateRequiredField(obj, fieldName, missingFields)) return RgbaColor(0,0,0,0);

        if (obj.is<String>()) {
            String hexColor = obj.as<String>();
            if (hexColor.charAt(0) != '#') {
                invalidFields += fieldName;
                return RgbaColor();
            }
            hexColor = hexColor.substring(1);
            for (const char& c : hexColor) {
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                    invalidFields += fieldName;
                    return RgbaColor();
                }
            }
            
            if (hexColor.length() == 6) {
                uint8_t r = strtoul(hexColor.substring(0,2).c_str(),nullptr, 16);
                uint8_t g = strtoul(hexColor.substring(2,4).c_str(),nullptr, 16);
                uint8_t b = strtoul(hexColor.substring(4,6).c_str(),nullptr, 16);
                return RgbColor(r,g,b);
            } else if (hexColor.length() == 8) {
                uint8_t r = strtoul(hexColor.substring(0,2).c_str(),nullptr, 16);
                uint8_t g = strtoul(hexColor.substring(2,4).c_str(),nullptr, 16);
                uint8_t b = strtoul(hexColor.substring(4,6).c_str(),nullptr, 16);
                uint8_t a = strtoul(hexColor.substring(6,8).c_str(),nullptr, 16);
                return RgbaColor(r,g,b,a);
            }
            
            invalidFields += fieldName;
            return RgbaColor();
        }

        if (obj.is<JsonObject>()) {
            JsonVariant red = obj["red"];
            JsonVariant green = obj["green"];
            JsonVariant blue = obj["blue"];
            JsonVariant hue = obj["hue"];
            JsonVariant saturation = obj["saturation"];
            JsonVariant value = obj["value"];
            JsonVariant lightness = obj["lightness"];
            JsonVariant alpha = obj["alpha"];

            if (!red.isNull() || !green.isNull() || !blue.isNull()) {
                requiredFieldType(red, uint8_t);
                requiredFieldType(green, uint8_t);
                requiredFieldType(blue, uint8_t);
                optionalFieldType(alpha, uint8_t);

                return RgbaColor(red | 0, green | 0, blue | 0, alpha | 255);
            } else if (!lightness.isNull()) { // Only consider it HSL if lightness is present, otherwise assume HSV
                requiredFieldType(hue, float);
                requiredFieldType(saturation, float);
                requiredFieldType(lightness, float);
                optionalFieldType(alpha, uint8_t);

                return HslaColor(hue | 0.0f, saturation | 0.0f, lightness | 0.0f, alpha | 255);
            } else if (!value.isNull() || !hue.isNull() || !saturation.isNull()) { // Treat HSV as the default over HSL
                requiredFieldType(hue, float);
                requiredFieldType(saturation, float);
                requiredFieldType(value, float);
                optionalFieldType(alpha, uint8_t);

                return HsvaColor(hue | 0.0f, saturation | 0.0f, value | 0.0f, alpha | 255);
            }
        }

        invalidFields += fieldName;
        return RgbaColor();
    }

    EasingFunction ColorSourceDeserializer::deserializeEasingFunction(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields) {
        if (obj.isNull()) return Easing::Linear;
        if (obj.is<String>()) {
            String name = obj.as<String>();
            EasingFunction easing = deserializeEasingFunctionFromName(name);
            if (!easing) {
                invalidFields += fieldName;
                return Easing::Linear;
            }
            return easing;
        }
        if (obj.is<JsonObject>()) {
            JsonVariant name = obj["name"];
            requiredFieldType(name, String);

            if (name == "Mirror") {
                JsonVariant easing = obj["easing"];
                
                EasingFunction function = deserializeAndValidate(easing, deserializeEasingFunction);

                if (function) {
                    return Easing::Mirror(function);
                } else {
                    return Easing::Mirror(Easing::Linear);
                }
            } else if (name == "Reverse") {
                JsonVariant easing = obj["easing"];
                
                EasingFunction function = deserializeAndValidate(easing, deserializeEasingFunction);

                if (function) {
                    return Easing::Reverse(function);
                } else {
                    return Easing::Reverse(Easing::Linear);
                }
            } else {
                return deserializeAndValidate(name, deserializeEasingFunction);
            }
        }

        invalidFields += fieldName;
        return Easing::Linear;
    }

    EasingFunction ColorSourceDeserializer::deserializeEasingFunctionFromName(const String& name) {
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
        else if (name == "Mirror") return Easing::Mirror(Easing::Linear);
        else if (name == "Reverse") return Easing::Reverse(Easing::Linear);
        return nullptr;
    }

    ColorSet ColorSourceDeserializer::deserializeColorSet(const JsonVariant& obj, const StringListBuilder& fieldName,  StringListBuilder& missingFields, StringListBuilder& invalidFields) {
        if (obj.isNull()) {
            invalidFields += fieldName;
            return ColorSet();
        }

        if (obj.is<JsonArray>()) {
            JsonArray arr = obj.as<JsonArray>();
            const uint8_t size = arr.size();
            if (size == 0) {
                invalidFields += fieldName;
                return ColorSet();
            }

            RgbaColor* colors = new RgbaColor[size];

            for (uint8_t i = 0; i < size; i++) {
                JsonVariant color = arr.getElement(i);
                colors[i] = deserializeColor(color, fieldName + String(i), missingFields, invalidFields);
            }
            
            ColorSet colorSet{size, colors};

            delete[] colors;

            return colorSet;
        }

        invalidFields += fieldName;
        return ColorSet();
    }

    GradientColorSource::Gradient ColorSourceDeserializer::deserializeGradient(const JsonVariant& obj, const StringListBuilder& fieldName,  StringListBuilder& missingFields, StringListBuilder& invalidFields) {
        if (!validateRequiredField(obj, fieldName, missingFields) || !validateFieldType<JsonObject>(obj, fieldName, invalidFields)) return GradientColorSource::Gradient(ColorSet());

        JsonVariant colors = obj["colors"];
        JsonVariant easing = obj["easing"];

        requiredField(colors);
        EasingFunction easingFunction = deserializeAndValidate(easing, deserializeEasingFunction);
        if (colors.is<JsonArray>()) {
            // An array signifies that it is an evenly spaced color set w/ no positional data
            ColorSet colorSet = deserializeAndValidate(colors, deserializeColorSet);
            return GradientColorSource::Gradient{colorSet, easingFunction};
        } else if (colors.is<JsonObject>()) {
            // An object signifies that it is an color set w/ positional data
            const uint8_t colorSize = colors.size();
            std::unique_ptr<RgbaColor[]> colorList = std::unique_ptr<RgbaColor[]>{new RgbaColor[colorSize]};
            std::unique_ptr<uint8_t[]> colorPositions = std::unique_ptr<uint8_t[]>{new uint8_t[colorSize]};
            uint8_t i = 0;
            for (JsonPair kv : colors.as<JsonObject>()) {
                colorPositions[i] = String(kv.key().c_str()).toInt();
                colorList[i] = deserializeColor(kv.value(), fieldName + String(kv.key().c_str()), missingFields, invalidFields);
                i++;
            }
            auto gradient = GradientColorSource::Gradient{ColorSet{colorSize, colorList.get()}, colorPositions.get(), easingFunction};
            return gradient;
        } else {
            invalidFields += fieldName + "colors";
        }
        
        return GradientColorSource::Gradient(ColorSet());
    }

    PixelOffsetConfig ColorSourceDeserializer::deserializePixelOffsetConfig(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields) {
        if (obj.isNull()) return PixelOffsetConfig();

        JsonVariant type = obj["type"];
        requiredFieldType(type, String);

        if (type == "Scale") {
            JsonVariant scale = obj["scale"];
            requiredFieldType(scale, float);
            return PixelOffsetConfig(scale | 0.0f); 
        } else if (type == "OffsetList") {
            JsonVariant offsets = obj["offsets"];
            requiredFieldType(offsets, JsonArray);
            
            if (offsets.is<JsonArray>()) {
                uint8_t size = offsets.size();
                float* offsetList = new float[size];
                for (uint8_t i = 0; i < size; i++) {
                    JsonVariant offset = offsets.getElement(i);
                    validateRequiredField(offset, fieldName + String(i), missingFields);
                    validateFieldType<float>(offset, fieldName + String(i), invalidFields);

                    offsetList[i] = offset | 0.0f;
                }
                PixelOffsetConfig config{size, offsetList};
                delete[] offsetList;
                return config;
            }

            return PixelOffsetConfig();
        } else if (type == "Random") {
            // TODO: Support this
            return PixelOffsetConfig();
        }
        
        invalidFields += fieldName;
        return PixelOffsetConfig();
    }

}

/**
 * ColorSource Deserializers
 */
namespace LightWeaver {
    ColorSourceDeserializer::Result ColorSourceDeserializer::deserialize(const JsonVariant& obj) {
        StringListBuilder missingFields(", ","");
        StringListBuilder invalidFields(", ","");
        std::unique_ptr<ColorSource> colorSource = deserializeColorSource(obj, StringListBuilder(".",""), missingFields, invalidFields);
        enforceValidation();
        
        if (!colorSource) {
            return Result::withError("Unknown Error");
        }

        return Result::withSuccess(colorSource);
    }

    Deserializer(ColorSource) {
        if (!validateRequiredField(obj, fieldName, missingFields)
            || !validateRequiredField(obj["type"], fieldName + "type", missingFields)) {
            return nullptr;
        }

        String type = obj["type"].as<String>();
        if (type == "Solid") {
            return deserializeSolidColorSource(obj, fieldName, missingFields, invalidFields);
        } else if (type == "Fade") {
            return deserializeFadeColorSource(obj, fieldName, missingFields, invalidFields);
        } else if (type == "Overlay") {
            return deserializeOverlayColorSource(obj, fieldName, missingFields, invalidFields);
        } else if (type == "Gradient") {
            return deserializeGradientColorSource(obj, fieldName, missingFields, invalidFields);
        } else if (type == "HueMeander") {
            return deserializeHueMeanderColorSource(obj, fieldName, missingFields, invalidFields);
        } else {
            invalidFields += (fieldName + "type");
            return nullptr;
        }
    }
    
    Deserializer(SolidColorSource) {
        JsonVariant uid = obj["uid"];
        JsonVariant color = obj["color"];

        requiredFieldType(uid, uint32_t);
        RgbaColor displayColor = deserializeAndValidate(color,deserializeColor);

        return isValid() ? std::unique_ptr<ColorSource>{new SolidColorSource(uid,displayColor)} : nullptr;
    }

    Deserializer(FadeColorSource) {
        JsonVariant uid = obj["uid"];
        JsonVariant duration = obj["duration"];
        JsonVariant loop = obj["loop"];
        JsonVariant start = obj["start"];
        JsonVariant end = obj["end"];
        JsonVariant easing = obj["easing"];

        requiredFieldType(uid, uint32_t);
        requiredFieldType(duration, uint16_t);        
        optionalFieldType(loop, bool);
        
        RgbaColor startColor = deserializeAndValidate(start, deserializeColor);
        RgbaColor endColor = deserializeAndValidate(end, deserializeColor);
        EasingFunction easingFunction = deserializeAndValidate(easing, deserializeEasingFunction);
        
        return isValid() 
            ? std::unique_ptr<ColorSource>{new FadeColorSource(uid, startColor, endColor, duration, loop | false , easingFunction)} 
            : nullptr;
    }

    Deserializer(OverlayColorSource) {
        JsonVariant uid = obj["uid"];
        JsonVariant background = obj["background"];
        JsonVariant overlay = obj["overlay"];

        requiredFieldType(uid, uint32_t);

        std::unique_ptr<ColorSource> backgroundColorSource = deserializeAndValidate(background, deserializeColorSource);
        std::unique_ptr<ColorSource> overlayColorSource = deserializeAndValidate(overlay, deserializeColorSource);
        
        if (isValid() && backgroundColorSource && overlayColorSource) {
            return std::unique_ptr<ColorSource>{new OverlayColorSource(uid,*backgroundColorSource,*overlayColorSource)};
        } else {
            return nullptr;
        }
    }

    Deserializer(GradientColorSource) {
        JsonVariant uid = obj["uid"];
        JsonVariant duration = obj["duration"];
        JsonVariant loop = obj["loop"];
        JsonVariant gradient = obj["gradient"];
        JsonVariant easing = obj["easing"];
        JsonVariant pixelOffsets = obj["pixelOffsets"];

        requiredFieldType(uid, uint32_t);
        requiredFieldType(duration, uint16_t);
        optionalFieldType(loop, bool);
        GradientColorSource::Gradient gradientData = deserializeAndValidate(gradient, deserializeGradient);
        EasingFunction easingFunction = deserializeAndValidate(easing, deserializeEasingFunction);
        PixelOffsetConfig pixelOffsetConfig = deserializeAndValidate(pixelOffsets, deserializePixelOffsetConfig);

        return isValid() 
            ? std::unique_ptr<ColorSource>{new GradientColorSource(uid, gradientData, duration, loop | false, easingFunction, pixelOffsetConfig)} 
            : nullptr;
        return nullptr;
    }

    Deserializer(HueMeanderColorSource) {
        JsonVariant uid = obj["uid"];
        JsonVariant color = obj["color"];
        JsonVariant maxDistance = obj["maxDistance"];
        JsonVariant maxDuration = obj["maxDuration"];
        JsonVariant easing = obj["easing"];

        requiredFieldType(uid, uint32_t);
        requiredFieldType(maxDistance, float);
        requiredFieldType(maxDuration, uint16_t);
        RgbaColor baseColor = deserializeAndValidate(color, deserializeColor);
        EasingFunction easingFunction = deserializeAndValidate(easing, deserializeEasingFunction);

        return isValid() 
            ? std::unique_ptr<ColorSource>{new HueMeanderColorSource(uid, baseColor, maxDistance, maxDuration, easingFunction)} 
            : nullptr;
        return nullptr;
    }
}