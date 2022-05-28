#ifndef J_TWO_D_TRANSFORM_H
#define J_TWO_D_TRANSFORM_H
#include <Arduino.h>
/**
 * @brief  struct for storing a 2 dimensional transformation. Used for telling a drivetrain how to move.
 * Follows ROS conventions for coordinate system: forward is positive, rotation CCW is positive, left is positive
 */
struct JTwoDTransform {
    /**
     * @brief  backwards- | forward+
     */
    float x;
    /**
     * @brief  right- | left+
     */
    float y;
    /**
     * @brief  clockwise- | counterclockwise+
     */
    float theta;

    /**
     * @brief converts from a coordinate system that might be more intuitive for some people
     * @param  fb: backwards- | forward+
     * @param  sideways:  left- | right+
     * @param  turn: CCW- | CW+ in DEGREES
     * @retval converted JTwoDTransform
     */
    static JTwoDTransform fromNice(float fb, float sideways, float turn)
    {
        return { fb, -sideways, radians(-turn) };
    }

    static float getForwards(JTwoDTransform t)
    {
        return t.x;
    }

    static float getRight(JTwoDTransform t)
    {
        return -t.y;
    }

    static float getTurnDegreesCW(JTwoDTransform t)
    {
        return -degrees(t.theta);
    }

    /**
     * @brief adds the second transform after the first, assuming movement, then rotation
     * @note assumes radians
     * @retval combined transform
     */
    JTwoDTransform operator&(const JTwoDTransform& t)
    {

        this->y = this->y + cos(this->theta) * t.y + sin(this->theta) * t.x;
        this->x = this->x + cos(this->theta) * t.x + sin(this->theta) * t.y;
        this->theta = this->theta + t.theta;
        return *this;
    }

    JTwoDTransform operator+(const JTwoDTransform& t)
    {
        this->y = this->y + t.y;
        this->theta = this->theta + t.theta;
        this->x = this->x + t.x;
        return *this;
    }
    JTwoDTransform operator-(const JTwoDTransform& t)
    {
        this->y = this->y - t.y;
        this->theta = this->theta - t.theta;
        this->x = this->x - t.x;
        return *this;
    }
    JTwoDTransform operator*(const JTwoDTransform& t)
    {
        this->y = this->y * t.y;
        this->theta = this->theta * t.theta;
        this->x = this->x * t.x;
        return *this;
    }
    JTwoDTransform operator/(const JTwoDTransform& t)
    {
        this->y = this->y / t.y;
        this->theta = this->theta / t.theta;
        this->x = this->x / t.x;
        return *this;
    }
    JTwoDTransform operator*(const float v)
    {
        this->y = this->y * v;
        this->theta = this->theta * v;
        this->x = this->x * v;
        return *this;
    }
    JTwoDTransform operator/(const float v)
    {
        this->y = this->y / v;
        this->theta = this->theta / v;
        this->x = this->x / v;
        return *this;
    }
    JTwoDTransform operator-()
    {
        this->y = -this->y;
        this->theta = -this->theta;
        this->x = -this->x;
        return *this;
    }
    JTwoDTransform operator+=(const JTwoDTransform& t)
    {
        this->y += t.y;
        this->theta += t.theta;
        this->x += t.x;
        return *this;
    }
    JTwoDTransform operator-=(const JTwoDTransform& t)
    {
        this->y -= t.y;
        this->theta -= t.theta;
        this->x -= t.x;
        return *this;
    }
    JTwoDTransform operator*=(const JTwoDTransform& t)
    {
        this->y *= t.y;
        this->theta *= t.theta;
        this->x *= t.x;
        return *this;
    }
    JTwoDTransform operator/=(const JTwoDTransform& t)
    {
        this->y /= t.y;
        this->theta /= t.theta;
        this->x /= t.x;
        return *this;
    }
    JTwoDTransform operator*=(const float v)
    {
        this->y *= v;
        this->theta *= v;
        this->x *= v;
        return *this;
    }
    JTwoDTransform operator/=(const float v)
    {
        this->y /= v;
        this->theta /= v;
        this->x /= v;
        return *this;
    }
    /**
     * @brief  sum of absolute value of the three values
     */
    float sumAbs()
    {
        return abs(this->y) + abs(this->x) + abs(this->theta);
    }
    /**
     * @brief  use to print values of transform to serial port
     * @param  labels: (bool, default=true) label what axis each value corresponds to
     * @param  newline: (bool, default=false) send a newline character at the end
     * @param  serial: (Stream*, default=Serial) what serial port to print to
     */
    void print(bool labels = true, bool newline = false, Stream* serial = &Serial)
    {
        if (labels)
            serial->print("x_fb:");
        serial->print(x, 5);
        serial->print(" ");
        if (labels)
            serial->print("y_lr:");
        serial->print(y, 5);
        serial->print(" ");
        if (labels)
            serial->print("rz_ccw:");
        serial->print(theta, 5);
        serial->print(" ");
        if (newline)
            serial->print("\n");
    }
};
#endif
