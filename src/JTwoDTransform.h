#ifndef J_TWO_D_TRANSFORM_H
#define J_TWO_D_TRANSFORM_H
#include <Arduino.h>
/**
 * @brief  struct for storing a 2 dimensional transformation. Used for telling a drivetrain how to move.
 */
struct JTwoDTransform {
    /**
         * @brief  backwards- | forward+
         */
    float y;
    /**
         * @brief  counterclockwise- | clockwise+
         */
    float rz;
    /**
         * @brief  left- | backwards+
         */
    float x;

    JTwoDTransform operator+(const JTwoDTransform& t)
    {
        this->y = this->y + t.y;
        this->rz = this->rz + t.rz;
        this->x = this->x + t.x;
        return *this;
    }
    JTwoDTransform operator-(const JTwoDTransform& t)
    {
        this->y = this->y - t.y;
        this->rz = this->rz - t.rz;
        this->x = this->x - t.x;
        return *this;
    }
    JTwoDTransform operator*(const JTwoDTransform& t)
    {
        this->y = this->y * t.y;
        this->rz = this->rz * t.rz;
        this->x = this->x * t.x;
        return *this;
    }
    JTwoDTransform operator/(const JTwoDTransform& t)
    {
        this->y = this->y / t.y;
        this->rz = this->rz / t.rz;
        this->x = this->x / t.x;
        return *this;
    }
    JTwoDTransform operator*(const float v)
    {
        this->y = this->y * v;
        this->rz = this->rz * v;
        this->x = this->x * v;
        return *this;
    }
    JTwoDTransform operator/(const float v)
    {
        this->y = this->y / v;
        this->rz = this->rz / v;
        this->x = this->x / v;
        return *this;
    }
    JTwoDTransform operator-()
    {
        this->y = -this->y;
        this->rz = -this->rz;
        this->x = -this->x;
        return *this;
    }
    JTwoDTransform operator+=(const JTwoDTransform& t)
    {
        this->y += t.y;
        this->rz += t.rz;
        this->x += t.x;
        return *this;
    }
    JTwoDTransform operator-=(const JTwoDTransform& t)
    {
        this->y -= t.y;
        this->rz -= t.rz;
        this->x -= t.x;
        return *this;
    }
    JTwoDTransform operator*=(const JTwoDTransform& t)
    {
        this->y *= t.y;
        this->rz *= t.rz;
        this->x *= t.x;
        return *this;
    }
    JTwoDTransform operator/=(const JTwoDTransform& t)
    {
        this->y /= t.y;
        this->rz /= t.rz;
        this->x /= t.x;
        return *this;
    }
    JTwoDTransform operator*=(const float v)
    {
        this->y *= v;
        this->rz *= v;
        this->x *= v;
        return *this;
    }
    JTwoDTransform operator/=(const float v)
    {
        this->y /= v;
        this->rz /= v;
        this->x /= v;
        return *this;
    }
    /**
     * @brief  sum of absolute value of the three values
     */
    float sumAbs()
    {
        return abs(this->y) + abs(this->x) + abs(this->rz);
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
            serial->print("y:");
        serial->print(y, 5);
        serial->print(" ");
        if (labels)
            serial->print("rz:");
        serial->print(rz, 5);
        serial->print(" ");
        if (labels)
            serial->print("x:");
        serial->print(x, 5);
        serial->print(" ");
        if (newline)
            serial->print("\n");
    }
};
#endif