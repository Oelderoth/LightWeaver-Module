#pragma once

#include <LightWeaver/ColorSource.h>
#include <LightWeaver/animation/Animator.h>

namespace LightWeaver {
    class OverlayColorSource : public ColorSource {
        private:
            Animator animator{2,Animator::AnimatorTimescale::MILLISECOND};
            ColorSource* backgroundColorSource;
            ColorSource* overlayColorSource;
            Animation animation{0,true,std::bind(&OverlayColorSource::onAnimationTick, this, std::placeholders::_1)};

            void onAnimationTick(const AnimationParam& param) {
                if (param.state == AnimationState::Started) {
                    animator.playAnimation(backgroundColorSource->getAnimation());
                    animator.playAnimation(overlayColorSource->getAnimation());
                }
                animator.loop();
                if (!animator.isAnimating()) {
                    // TODO: Stop my animation if the two children are done
                }
            }

        public:
            OverlayColorSource(uint32_t uid, ColorSource& backgroundColorSource, ColorSource& overlayColorSource) : 
                ColorSource(uid),
                backgroundColorSource(backgroundColorSource.clone()),
                overlayColorSource(overlayColorSource.clone()) {
                    animator.setup();
                }
            ~OverlayColorSource() {
                delete backgroundColorSource;
                delete overlayColorSource;
                backgroundColorSource = nullptr;
                overlayColorSource = nullptr;

                animator.stop();
            }

            virtual RgbaColor getColor() const {
                LightWeaver::RgbColor backgroundColor = backgroundColorSource->getColor();
                LightWeaver::RgbaColor overlayColor = overlayColorSource->getColor();
                uint16_t R = backgroundColor.R + (float)(overlayColor.A)/255.0f * overlayColor.R;
                uint16_t G = backgroundColor.G + (float)(overlayColor.A)/255.0f * overlayColor.G;
                uint16_t B = backgroundColor.B + (float)(overlayColor.A)/255.0f * overlayColor.B;
                return LightWeaver::RgbColor(R > 255 ? 255 : R, G > 255 ? 255 : G, B > 255 ? 255 : B);
            }

            virtual ColorSource* clone() const {
                return new OverlayColorSource(uid, *backgroundColorSource, *overlayColorSource);
            }

            virtual const Animation* getAnimation() const {
                return &animation;
            }
    };
}