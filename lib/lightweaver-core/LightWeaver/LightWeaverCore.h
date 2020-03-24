#pragma once

namespace LightWeaver {
    /**
     * The core class of LightWeaver
     * Responsible for orchestrating the setup and loop steps of all necessary components
     * and delegating color updates to the provided driver class
     */

    template <typename T_DRIVER>
    class LightWeaverCore
    {
    private:
        T_DRIVER driver;
        Animator animator;

    public:
        LightWeaverCore(int pixelCount) : driver(T_DRIVER(pixelCount)){}
        ~LightWeaverCore(){}

        void setup()
        {
            driver.setup();
        }

        void loop()
        {
            driver.loop();
        }
    };
};