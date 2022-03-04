{
    const pointsOnCurve = (curve, numberOfPoints) => {
        const pointOnCurve = (curve, perc) => {
            return {
                x: Math.abs(Math.pow((1 - perc), 3)) * curve.start.x + 3 * Math.pow((1 - perc), 2) * perc * curve.cp1.x + 3 * Math.abs((1 - perc)) * Math.pow(perc, 2) * curve.cp2.x + Math.abs(Math.pow(perc, 3)) * curve.end.x,
                y: Math.abs(Math.pow((1 - perc), 3)) * curve.start.y + 3 * Math.pow((1 - perc), 2) * perc * curve.cp1.y + 3 * Math.abs((1 - perc)) * Math.pow(perc, 2) * curve.cp2.y + Math.abs(Math.pow(perc, 3)) * curve.end.y
            };
        };

        const points = [];

        for (let i = 0; i < numberOfPoints; ++i) {
            points.push(pointOnCurve(curve, i / numberOfPoints));
        }

        return points;
    };

    const readFromData = (data, offset = {x: 0, y: 0}) => {
        const curves = [];

        let curve;

        const axis = ['x', 'y'];

        for (const d of data) {
            if (d === "M") {
                curve = [];
            } else if (d === "C") {
                if (curve.length == 8) {
                    curves.push({
                        start: { x: curve[0], y: curve[1] },
                        cp1: { x: curve[2], y: curve[3] },
                        cp2: { x: curve[4], y: curve[5] },
                        end: { x: curve[6], y: curve[7] }
                    });
                    curve = [curve[6], curve[7]];
                }
            } else {
                curve.push(+d + offset[axis[curve.length % 2]]);
            }
        }

        if (curve.length === 8) {
            curves.push({
                start: { x: curve[0], y: curve[1] },
                cp1: { x: curve[2], y: curve[3] },
                cp2: { x: curve[4], y: curve[5] },
                end: { x: curve[6], y: curve[7] }
            });
            curve = [];
        }

        return curves;
    };

    const pointsToSvgCircles = (points) => {
        let message = "";

        for (const p of points) {
            message += "<circle cx=\"" + p.x.toFixed(2) + "\" cy=\"" + p.y.toFixed(2) + "\" r=\"2\"></circle>\n";
        }

        return message;
    };

    const pointsToAnimation = (points) => {
        let message = "";

        for (const i in points) {
            message += "  " + i + ":\n";
            message += "    head:\n";
            message += "      position:\n";
            message += "        x: " + points[i].x.toFixed(2) + "\n";
            message += "        y: " + points[i].y.toFixed(2) + "\n";
            message += "        z: -1\n";
        }

        return message;
    };

    const rawData="M 0 -13 C 0 -19 0 -19 0 -19 C 0 -13 0 -13 0 -13";

    const data = rawData.split(' ');

    const curves = readFromData(data, {x: 0, y: 32});

    const points = [];

    for (const c of curves) {
        const curvePoints = pointsOnCurve(c, 15);

        points.push(...curvePoints);
    }

    let outputMessage = pointsToAnimation(points);

    console.log(outputMessage);
}