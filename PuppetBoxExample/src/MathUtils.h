#pragma once

#include <math.h>

#include <puppetbox/DataStructures.h>

#define HALF_PI         1.5707963267f
#define PI              3.1415926535f
#define PI_AND_HALF     4.7123889803f
#define RADS_PER_DEG    0.0174532925f

#define FLOAT_EQUALITY_THRESHOLD 0.0001

namespace MathUtils
{
    /**
     * \brief Finds the magnitude of the given vector.
     *
     * \param vector    The vector to get the magnitude of.
     *
     * \return The magnitude of the given vector.
     */
    inline float Magnitude(PB::vec2 vector)
    {
        return sqrt(pow(vector.x, 2) + pow(vector.y, 2));
    }

    /**
     * \brief Calculates and returns the vector normal.
     *
     * \param vector    The vector to normalize.
     *
     * \return A new normalized vector of the given vector.
     */
    inline PB::vec2 Normalize(PB::vec2 vector)
    {
        return vector / Magnitude(vector);
    }

    /**
     * \brief Law of cosines equation for finding angle 'C' of a triangle, given lengths 'a', 'b', 'c'.
     *
     * \param sideA The length of the triangle's side 'a'
     * \param sideB The length of the triangle's side 'b'
     * \param sideC The length of the triangle's side 'c'
     *
     * \return The angle of angle 'C' in radians.
     */
    inline float LawOfCos(float sideA, float sideB, float sideC)
    {
        return acos(
                (pow(sideC, 2) - pow(sideA, 2) - pow(sideB, 2))
                / (-2 * sideA * sideB));
    }

    /**
     * \brief Finds the angle of a triangle with hypotenuse between origin pointA and pointB.
     *
     * \param pointA    The origin coord of the triangle.
     * \param pointB    The opposite end of the hypotenuse.
     *
     * \return The angle of the triangle at pointA.
     */
    inline float ThetaFromTan(PB::vec2 pointA, PB::vec2 pointB)
    {
        return atan((pointB.y - pointA.y) / (pointB.x - pointA.x));
    }

    /**
     * \brief Converts the given vector to polar coordinates.
     *
     * \param vector    The vector to convert.
     *
     * \return The polar coordinates of the given vector as a {\link PB::vec2} with
     * x being the angle and y the radius.
     */
    inline PB::vec2 RectToPolar(PB::vec2 vector)
    {
        float xAxisAngle = atan(vector.y / vector.x);
        float angle = fmod((xAxisAngle + HALF_PI), HALF_PI);

        if (vector.x < 0)
        {
            angle += HALF_PI;

            if (vector.y < 0)
            {
                angle += HALF_PI;
            }
        }
        else if (vector.y < 0)
        {
            angle += PI_AND_HALF;
        }

        float radius = Magnitude(vector);

        return {angle, radius};
    }

    /**
     * \brief Calculates the rotational values of a 3 joint structure given a matrix containing
     * resting position data for the 3 joints and a target IK position (what to "reach" for).
     *
     * mat3[0] is the origin joint (shoulder)
     * mat3[1] is the secondary joint (elbow)
     * mat3[2] is the reaching joint (hand)
     *
     * \param jointPositions    The resting jointPositions of the three joints, each column containing
     * the x,y,z coords.
     * \param ikPosition        The target 2D position to "reach" for with the 3 joint system.
     *
     * \return A {\link PB::vec3} containing the rotational values for joints A ([0]) and B ([1]) of the
     * 3 joint system.
     */
    inline PB::mat3 InverseKinematics3DF(PB::mat3 jointPositions, PB::vec2 ikPosition)
    {
        // First calculates all the triangle side lengths for resting positions
        PB::vec3 restingMagnitudes = {
                Magnitude({jointPositions[2].x - jointPositions[1].x, jointPositions[2].y - jointPositions[1].y}),
                Magnitude({jointPositions[2].x - jointPositions[0].x, jointPositions[2].y - jointPositions[0].y}),
                Magnitude({jointPositions[1].x - jointPositions[0].x, jointPositions[1].y - jointPositions[0].y})
        };

        // Then calculate all the triangle resting angles
        PB::vec3 restingAngles{};

        if (restingMagnitudes[1] > FLOAT_EQUALITY_THRESHOLD)
        {
            restingAngles[0] = LawOfCos(restingMagnitudes[2], restingMagnitudes[1], restingMagnitudes[0]);
            restingAngles[1] = LawOfCos(restingMagnitudes[2], restingMagnitudes[0], restingMagnitudes[1]);
            // Resting angle of "hand" (restingAngles[2]) doesn't matter.
        }

        // Now calculate the triangle lengths if pointC was on the ikPosition
        PB::vec3 ikMagnitudes = {
                restingMagnitudes[0],
                Magnitude({ikPosition.x - jointPositions[0].x, ikPosition.y - jointPositions[0].y}),
                restingMagnitudes[2]
        };

        PB::vec3 ikAngles{};

        // If the distance between Point A and Point C is greater than length AB + BC, the system is
        // fully stretched with the middle joint at 180 degrees (adjusting for resting angle).
        if (ikMagnitudes[1] > ikMagnitudes[0] + ikMagnitudes[2])
        {
            ikAngles[1] = PI - restingAngles[1];
        }
        else if (ikMagnitudes[1] > FLOAT_EQUALITY_THRESHOLD)
        {
            // If Point A and Point C are NOT right on top of each other, calculate angles using
            // law of cosines.
            ikAngles[0] = LawOfCos(ikMagnitudes[2], ikMagnitudes[1], ikMagnitudes[0]);
            ikAngles[1] = LawOfCos(ikMagnitudes[2], ikMagnitudes[0], ikMagnitudes[1]);
            // IK Angle of "hand" (ikAngles[2]) doesn't matter.
        }
        else
        {
            // If Point A and Point C ARE right on top of each other, set the known values
            //TODO: This assumes resting position has Joint C on Joint A, fix
            //TODO: Implement logic
        }

        // Find the grid angle between Joint A (assumed at [0,0]) and Joint B.  This gives the absolute
        // angle of the shoulder, where if the arm pointed straight down, this would be 270 degrees.
        float restingAngleA = ThetaFromTan({jointPositions[0].x, jointPositions[0].y}, {jointPositions[1].x, jointPositions[1].y});
        // Find the difference between the grid angle and the C-A-B system angle in resting
        // pose and in ikPosition pose.  This gives the change in relative difference between
        // the "shoulder" and "elbow" needed to get the "hand" the right distance away from the "shoulder".
        float restingDeltaA = restingAngles[0] - ikAngles[0];

        // This will be the absolute angle of Point C (the "hand") relative to Point A (the "shoulder")
        // where if the arm was stretched straight away from the body, parallel to the floor, it would be
        // 180 degrees.
        float restingAngleAC;

        if (restingMagnitudes[1] < FLOAT_EQUALITY_THRESHOLD)
        {
            // If the points are right on top of each other
            //TODO: This assumes resting position has Joint C on Joint A, fix
            restingAngleAC = restingAngleA;
        }
        else
        {
            // Otherwise, calculate the angle between the points
            restingAngleAC = RectToPolar(
                    {jointPositions[2].x - jointPositions[0].x, jointPositions[2].y - jointPositions[0].y}).x;
        }

        // This will be the absolute angle of the IK Position (the "target") relative to Point A (the "Shoulder")
        // where if the target was straight out from the "shoulder" in a line parallel to the floor, it would
        // be 180 degrees.
        float ikAngleAC;

        if (ikMagnitudes[1] < FLOAT_EQUALITY_THRESHOLD)
        {
            // If the points are right on top of each other
            //TODO: This assumes resting position has Joint C on Joint A, fix
            ikAngleAC = restingAngleA;
        }
        else
        {
            // Otherwise, calculate the angle between the points
            ikAngleAC = RectToPolar({ikPosition.x - jointPositions[0].x, ikPosition.y - jointPositions[0].y}).x;
        }

        // This is the difference of the relative angle to where the "hand" is, to where it needs
        // to be (i.e., the "target").
        float deltaAngleAC = ikAngleAC - restingAngleAC;

        // Rotate by the difference of where the "hand" was and where it needs to be, offset
        // by the angle the "hand" was moved to get it the right distance from the "shoulder"
        float ikAngleA = deltaAngleAC - restingDeltaA;

        // This gives the delta angle applied to the "elbow" to get the "hand" the right
        // distance from the "shoulder".
        float deltaB = ikAngles[1] - restingAngles[1];

        //TODO: Only doing 2d animations atm, so only Z axis rotations are being calculated
        return {
                {0, 0, ikAngleA},
                {0, 0, deltaB},
                {0, 0, 0}
        };
    }
}