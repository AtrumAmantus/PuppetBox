{
    const distance = (pointA, pointB) => {
        return Math.sqrt(Math.pow(pointA.x - pointB.x, 2) + Math.pow(pointA.y - pointB.y, 2));
    };

    const lawOfCos = (sideA, sideB, sideC) => {
        return Math.acos((Math.pow(sideC, 2) - Math.pow(sideA, 2) - Math.pow(sideB, 2)) / (-2 * sideA * sideB));
    };

    const thetaFromTan = (pointA, pointB) => {
        return Math.atan((pointB.y - pointA.y) / (pointB.x - pointA.x));
    };

    const rad2Deg = (radians) => {
        return (radians * 180) / Math.PI;
    };

    const rectToPolar = (coordA, coordB) => {
        const HALF_PI = (Math.PI / 2);
        const coord = {x: coordA.x, y: coordA.y};

        if (coordB !== undefined) {
            coord.x = coordB.x - coordA.x;
            coord.y = coordB.y - coordA.y;
        }

        const xAxisAngle = Math.atan(coord.y / coord.x);
        const positiveAngle = (xAxisAngle + HALF_PI) % HALF_PI;

        let angle = positiveAngle;

        if (coord.x < 0) {
            angle += HALF_PI;
            if (coord.y < 0) {
                angle += HALF_PI;
            }
        } else if (coord.y < 0) {
            angle += Math.PI + HALF_PI;
        }

        const radius = distance({x: 0, y: 0}, coord);

        return {angle, radius};
    };

    const pointsEqual = (pointA, pointB) => {
        return pointA.x === pointB.x && pointA.y === pointB.y;
    };

    const inverseKinematics = (restingJoints, ikPosition) => {
        // Find resting triangle lengths
        const restingTriangle = {
            a: distance(restingJoints.B, restingJoints.C),
            b: distance(restingJoints.A, restingJoints.C),
            c: distance(restingJoints.A, restingJoints.B)
        };

        if (restingTriangle.b === 0) {
            // Points are on top of eachother, everything is 0 angle
            restingTriangle.A = 0;
            restingTriangle.B = 0;
            restingTriangle.C = 0;
        } else {
            // Find resting angles
            restingTriangle.A = lawOfCos(restingTriangle.c, restingTriangle.b, restingTriangle.a);
            restingTriangle.B = lawOfCos(restingTriangle.c, restingTriangle.a, restingTriangle.b);
            restingTriangle.C = lawOfCos(restingTriangle.a, restingTriangle.b, restingTriangle.c);
        }

        // Find triangle lengths after IK movement
        const ikTriangle = {
            a: restingTriangle.a,
            b: distance(restingJoints.A, ikPosition),
            c: restingTriangle.c
        };

        if (ikTriangle.b === 0) {
            // Points are on top of eachother, everything is 0 angle
            ikTriangle.A = 0;
            ikTriangle.B = 0;
            ikTriangle.C = 0;
        } else if (ikTriangle.b > ikTriangle.a + ikTriangle.c) {
            // ikPos is too far away, stretch out joints to point at it
            ikTriangle.A = 0;
            ikTriangle.B = Math.PI;
            ikTriangle.C = 0;
        } else {
            // Find angles after IK movement
            ikTriangle.A = lawOfCos(ikTriangle.c, ikTriangle.b, ikTriangle.a);
            ikTriangle.B = lawOfCos(ikTriangle.c, ikTriangle.a, ikTriangle.b);
            ikTriangle.C = lawOfCos(ikTriangle.a, ikTriangle.b, ikTriangle.c);
        }

        // Resting angle for joint A, computed relative to joint B
        const restingAngleA = thetaFromTan(restingJoints.A, restingJoints.B);
        const restingDeltaA = restingTriangle.A - ikTriangle.A;

        const restingAngleAC = pointsEqual(restingJoints.A, restingJoints.C) ? restingAngleA : rectToPolar(restingJoints.A, restingJoints.C).angle;
        const ikAngleAC = pointsEqual(restingJoints.A, ikPosition) ? restingAngleA : rectToPolar(restingJoints.A, ikPosition).angle;
        const deltaAngleAC = ikAngleAC - restingAngleAC;

        const ikAngleA = (restingAngleA - restingDeltaA) + deltaAngleAC - restingAngleA;

        const deltaB = ikTriangle.B - restingTriangle.B;

        return {
            A: { radians: ikAngleA, degrees: rad2Deg(ikAngleA) },
            B: { radians: deltaB, degrees: rad2Deg(deltaB) }
        }
    };

    const bones = {
        A: {x: 0, y: 0},
        B: {x: 0, y: -16},
        C: {x: 0, y: 0}
    };

    const listOfPoints = [
        {x: 0, y: 0},
        {x: -1.39, y: 1.6},
        {x: -2.96, y: 3.35},
        {x: -4.25, y: 7.58},
        {x: -4.77, y: 12.62},
        {x: -4.05, y: 17.79},
        {x: -1.60, y: 22.42},
        {x: 3.05, y: 25.84},
        {x: 7.91, y: 27.96},
        {x: 10.79, y: 29.14},
        {x: 11.85, y: 29.34},
        {x: 11.25, y: 28.51},
        {x: 9.17, y: 26.64},
        {x: 5.75, y: 23.68},
        {x: 1.18, y: 19.61},
        {x: -2.57, y: 16.59},
        {x: -3.81, y: 16.35},
        {x: -2.89, y: 18.19},
        {x: -0.16, y: 21.39},
        {x: 4.05, y: 25.25},
        {x: 9.38, y: 29.05},
        {x: 15.52, y: 32.07},
        {x: 18.58, y: 33.65},
        {x: 16.35, y: 33.45},
        {x: 10.96, y: 31.31},
        {x: 4.51, y: 27.07},
        {x: -0.88, y: 20.55},
        {x: -3.09, y: 21.58}
    ];

    let message = "";
    let counter = 0;

    for (const ikPos of listOfPoints) {
        const result = inverseKinematics(bones, ikPos);

        message += "  " + (counter++) + ":\n" +
            "    right_shoulder:\n" +
            "      rotation:\n" +
            "        z: " + (-result.A.degrees.toFixed(2)) + "\n" +
            "    right_elbow:\n" +
            "      rotation:\n" +
            "        z: " + (-result.B.degrees.toFixed(2)) + "\n";
    }

    console.log(message);
}