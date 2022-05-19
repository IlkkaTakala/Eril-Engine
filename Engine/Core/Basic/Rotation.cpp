#include "Rotation.h"

Rotator Rotator::LookAt(const Vector& sourcePoint, const Vector& destPoint, const Vector& front, const Vector& up)
{
    // your code from before
    Vector F = (sourcePoint - destPoint).Normalize();   // lookAt
    Vector R = Vector::Cross(F, {0, 0, 1}).Normalize(); // sideaxis
    Vector U = Vector::Cross(R, F);                  // rotatedup

    // note that R needed to be re-normalized
    // since F and worldUp are not necessary perpendicular
    // so must remove the sin(angle) factor of the cross-product
    // same not true for U because dot(R, F) = 0

    // adapted source
    Rotator q;
    double trace = R.X + U.Y + F.Z;
    if (trace > 0.0) {
        double s = 0.5 / sqrt(trace + 1.0);
        q.W = 0.25 / s;
        q.X = (U.Z - F.Y) * s;
        q.Y = (F.X - R.Z) * s;
        q.Z = (R.Y - U.X) * s;
    }
    else {
        if (R.X > U.Y && R.X > F.Z) {
            double s = 2.0 * sqrt(1.0 + R.X - U.Y - F.Z);
            q.W = (U.Z - F.Y) / s;
            q.X = 0.25 * s;
            q.Y = (U.X + R.Y) / s;
            q.Z = (F.X + R.Z) / s;
        }
        else if (U.Y > F.Z) {
            double s = 2.0 * sqrt(1.0 + U.Y - R.X - F.Z);
            q.W = (F.X - R.Z) / s;
            q.X = (U.X + R.Y) / s;
            q.Y = 0.25 * s;
            q.Z = (F.Y + U.Z) / s;
        }
        else {
            double s = 2.0 * sqrt(1.0 + F.Z - R.X - U.Y);
            q.W = (R.Y - U.X) / s;
            q.X = (F.X + R.Z) / s;
            q.Y = (F.Y + U.Z) / s;
            q.Z = 0.25 * s;
        }
    }
    return q.FastNormalize();
}
