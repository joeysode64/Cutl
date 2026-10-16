#pragma once

namespace cu
{
    /** @brief A wrapper for a Cutl C structure. */
    template<typename T>
    class Wrapper {
    protected:
        T me; /**< The internal structure. */
        
    public:
        /** @brief Creates the value initialized to zero. */
        constexpr Wrapper()
            : me{}
        {

        }

        /**
         * @brief Returns the internal value.
         * @return The internal value.
         */
        constexpr T& get()
        {
            return me;
        }

        /**
         * @brief Returns the internal value.
         * @return The internal value.
         */
        constexpr const T& get() const
        {
            return me;
        }
    };
}
