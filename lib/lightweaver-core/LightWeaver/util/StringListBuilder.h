#pragma once
#include <Arduino.h>

namespace LightWeaver {
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

        const char* c_str() const {
            return value.c_str();
        }
    };
}