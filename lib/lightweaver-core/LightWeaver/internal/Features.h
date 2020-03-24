#pragma once

/**
 * Flags representing the supported functionality of a given LED Driver
 */

enum SupportedFeature {
    BRIGHTNESS = 1,
    COLOR = 2,
    ANIMATION = 4,
    ADDRESSABLE = 8
};