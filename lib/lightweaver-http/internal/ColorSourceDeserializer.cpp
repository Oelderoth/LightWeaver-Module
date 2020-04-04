#include "ColorSourceDeserializer.h"

/**
 * Validation Macros
 * This file makes liberal use of macros to encourage consistent validation and deserialize patterns
 * These can be used to shorthand common use cases for the Validation Functions defined below
 */
#define Deserializer(type) ColorSource* ColorSourceDeserializer::deserialize##type(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields)
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

        JsonVariant red = obj["red"];
        JsonVariant green = obj["green"];
        JsonVariant blue = obj["blue"];
        JsonVariant alpha = obj["alpha"];

        requiredFieldType(red, uint8_t);
        requiredFieldType(green, uint8_t);
        requiredFieldType(blue, uint8_t);
        optionalFieldType(alpha, uint8_t);

        return RgbaColor(red | 0, green | 0, blue | 0, alpha | 255);
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
                JsonVariant child = obj["child"];
                
                requiredField(child);
                EasingFunction function = deserializeAndValidate(child, deserializeEasingFunction);

                if (function) {
                    return Easing::Mirror(function);
                } else {
                    return Easing::Linear;
                }
            } else if (name == "Reverse") {
                JsonVariant child = obj["child"];
                
                requiredField(child);
                EasingFunction function = deserializeAndValidate(child, deserializeEasingFunction);

                if (function) {
                    return Easing::Reverse(function);
                } else {
                    return Easing::Linear;
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
        return nullptr;
    }
}

/**
 * ColorSource Deserializers
 */
namespace LightWeaver {
    ColorSourceDeserializer::Result ColorSourceDeserializer::deserialize(const JsonVariant& obj) {
        StringListBuilder missingFields(", ","");
        StringListBuilder invalidFields(", ","");
        ColorSource* colorSource = deserializeColorSource(obj, StringListBuilder(".",""), missingFields, invalidFields);
        enforceValidation();
        
        if (!colorSource) {
            delete colorSource;
            return Result::withError("Unknown Error");
        }

        return Result::withSuccess(*colorSource);
    }

    ColorSource* ColorSourceDeserializer::deserializeColorSource(const JsonVariant& obj, const StringListBuilder& fieldName, StringListBuilder& missingFields, StringListBuilder& invalidFields) {
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

        return isValid() ? new SolidColorSource(uid,displayColor) : nullptr;
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
        
        return isValid() ? new FadeColorSource(uid, startColor, endColor, duration, loop | false , easingFunction ? easingFunction : Easing::Linear) : nullptr;
    }

    Deserializer(OverlayColorSource) {
        JsonVariant uid = obj["uid"];
        JsonVariant background = obj["background"];
        JsonVariant overlay = obj["overlay"];

        requiredFieldType(uid, uint32_t);

        ColorSource* backgroundColorSource = deserializeAndValidate(background, deserializeColorSource);
        ColorSource* overlayColorSource = deserializeAndValidate(overlay, deserializeColorSource);
        
        if (isValid() && backgroundColorSource && overlayColorSource) {
            return new OverlayColorSource(uid,*backgroundColorSource,*overlayColorSource);
        } else {
            delete backgroundColorSource;
            delete overlayColorSource;
            return nullptr;
        }
    }
}