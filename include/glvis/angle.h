#pragma once

namespace glvis {

    class Angle {
    public:
        Angle();
        float asDegrees() const;
        float asRadians() const;

    private:
        friend Angle degrees(float degrees);
        friend Angle radians(float radians);

        float radians = 0.0f;

    };

    Angle degrees(float degrees);
    Angle radians(float radians);

}
