#ifndef TRIGONOMETRI_H
#define TRIGONOMETRI_H

/* degree to radian */
double deg2rad(double d)
{
	return d * M_PI / 180.0;
}

/* radian to degree */
double rad2deg(double r)
{
	return r * 180.0 / M_PI;
}

/* degree sin */
double dsin(double d)
{
	return sin(deg2rad(d));
}

/* degree cos */
double dcos(double d)
{
	return cos(deg2rad(d));
}

/* degree tan */
double dtan(double d)
{
	return tan(deg2rad(d));
}

/* degree arcsin */
double darcsin(double x)
{
	return rad2deg(asin(x));
}

/* degree arccos */
double darccos(double x)
{
	return rad2deg(acos(x));
}

/* degree arctan */
double darctan(double x)
{
	return rad2deg(atan(x));
}

/* degree arctan2 */
double darctan2(double y, double x)
{
	return rad2deg(atan2(y, x));
}

/* degree arccot */
double darccot(double x)
{
	return rad2deg(atan(1.0 / x));
}


#endif