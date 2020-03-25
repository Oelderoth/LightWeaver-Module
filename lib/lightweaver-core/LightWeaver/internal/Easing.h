#pragma once

/**
 * Easing functions, adapated from
 * http://gizma.com/easing/
 */
namespace LightWeaver {
    typedef std::function<float(float)> EasingFunction;
    namespace Easing {
        inline float Linear(float p) {
            return p;
        }

        inline float QuadraticIn(float p) {
            return p * p;
        }

        inline float QuadraticOut(float p) {
            return p * (2.0f - p);
        }

        inline float QuadraticInOut(float p) {
            p *= 2.0f;
            if (p < 1.0f) {
                return p * p * 0.5f;
            }
            p -= 1.0f;
            return -0.5f * (p * (p-2.0f) - 1.0f);
        }

        inline float CubicIn(float p) {
            return p * p * p;
        }
        
        inline float CubicOut(float p) {
            p -= 1.0f;
            return p * p * p + 1;
        }
        
        inline float CubicInOut(float p) {
            p *= 2.0f;
            if (p < 1.0f) {
                return p * p * p * 0.5f;
            }
            p -= 2.0f;
            return 0.5f * (p * p * p + 2.0f);
        }
        
        inline float QuarticIn(float p) {
            return p * p * p * p;
        }
        
        inline float QuarticOut(float p) {
            p -= 1.0f;
            return -1.0f * (p * p * p * p - 1.0f);
        }
        
        inline float QuarticInOut(float p) {
            p *= 2.0f;
            if (p < 1.0f) {
                return p * p * p * p * 0.5f;
            }
            p -= 2.0f;
            return -0.5f * (p * p * p * p - 2.0f);
        }
        
        inline float QuinticIn(float p) {
            return p * p * p * p * p;
        }
        
        inline float QuinticOut(float p) {
            p -= 1.0f;
            return p * p * p * p * p + 1.0f;
        }
        
        inline float QuinticInOut(float p) {
            p *= 2.0f;
            if (p < 1.0f) {
                return p * p * p * p * p * 0.5f;
            }
            p -= 2.0f;
            return 0.5f * (p * p * p  * p * p + 2.0f);
        }
        
        inline float SinusoidalIn(float p) {
            return -cos(p * HALF_PI) + 1.0f;
        }
        
        inline float SinusoidalOut(float p) {
            return sin(p * HALF_PI);
        }
        
        inline float SinusoidalInOut(float p) {
            return -0.5f * (cos(PI * p) - 1.0f);
        }
        
        inline float ExponentialIn(float p) {
            return pow(2, 10.0f * (p - 1));
        }
        
        inline float ExponentialOut(float p) {
            return -pow(2, -10.0f * p) + 1.0f;
        }
        
        inline float ExponentialInOut(float p) {
            p *= 2.0f;
            if (p < 1.0f) return 0.5 * pow(2, 10 * (p - 1.0f));
            p -= 1.0f;
            return 0.5 * (-pow(2, -10.f * p) + 2);
        }        
    }
}