#include <geo.h>

int map_projection_init_timestamped(struct map_projection_reference_s *ref, double lat_0, double lon_0, uint64_t timestamp) //lat_0, lon_0 are expected to be in correct format: -> 47.1234567 and not 471234567
{

	ref->lat_rad = lat_0 * M_DEG_TO_RAD;
	ref->lon_rad = lon_0 * M_DEG_TO_RAD;
	ref->sin_lat = sin(ref->lat_rad);
	ref->cos_lat = cos(ref->lat_rad);

	ref->timestamp = timestamp;
	ref->init_done = true;

	return 0;
}

int map_projection_init(struct map_projection_reference_s *ref, double lat_0, double lon_0) //lat_0, lon_0 are expected to be in correct format: -> 47.1234567 and not 471234567
{
	return map_projection_init_timestamped(ref, lat_0, lon_0, hrt_absolute_time());
}

int map_projection_project(const struct map_projection_reference_s *ref, double lat, double lon, float *x, float *y)
{
	if (!ref->init_done)
	{
		return -1;
	}
	
	double lat_rad = lat * M_DEG_TO_RAD;
	double lon_rad = lon * M_DEG_TO_RAD;
	
	double sin_lat = sin(lat_rad);
	double cos_lat = cos(lat_rad);
	double cos_d_lon = cos(lon_rad - ref->lon_rad);
	
	double arg = ref->sin_lat * sin_lat + ref->cos_lat * cos_lat * cos_d_lon;
	
	if (arg > 1.0)
	{
		arg = 1.0;
		
	}
	else if (arg < -1.0)
	{
		arg = -1.0;
	}
	
	double c = acos(arg);
	double k = (fabs(c) < DBL_EPSILON) ? 1.0 : (c / sin(c));
	
	*x = k * (ref->cos_lat * sin_lat - ref->sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_OF_EARTH;
	*y = k * cos_lat * sin(lon_rad - ref->lon_rad) * CONSTANTS_RADIUS_OF_EARTH;
	
	return 0;
}

int map_projection_reproject(const struct map_projection_reference_s *ref, float x, float y, double *lat, double *lon)
{
	if (!ref->init_done)
	{
		return -1;
	}

	double x_rad = (double) x / CONSTANTS_RADIUS_OF_EARTH;
	double y_rad = (double) y / CONSTANTS_RADIUS_OF_EARTH;
	double c = sqrtf(x_rad * x_rad + y_rad * y_rad);
	double sin_c = sin(c);
	double cos_c = cos(c);

	double lat_rad;
	double lon_rad;

	if (fabs(c) > DBL_EPSILON)
	{
		lat_rad = asin(cos_c * ref->sin_lat + (x_rad * sin_c * ref->cos_lat) / c);
		lon_rad = (ref->lon_rad + atan2(y_rad * sin_c, c * ref->cos_lat * cos_c - x_rad * ref->sin_lat * sin_c));

	}
	else
	{
		lat_rad = ref->lat_rad;
		lon_rad = ref->lon_rad;
	}

	*lat = lat_rad * 180.0 / M_PI;
	*lon = lon_rad * 180.0 / M_PI;

	return 0;
}

float get_bearing_to_next_waypoint(double lat_now, double lon_now, double lat_next, double lon_next)
{
	double lat_now_rad = lat_now * M_DEG_TO_RAD;
	double lon_now_rad = lon_now * M_DEG_TO_RAD;
	double lat_next_rad = lat_next * M_DEG_TO_RAD;
	double lon_next_rad = lon_next * M_DEG_TO_RAD;

	double d_lon = lon_next_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	float theta = atan2f(sin(d_lon) * cos(lat_next_rad), cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos(lat_next_rad) * cos(d_lon));

	theta = _wrap_pi(theta);

	return theta;
}

float _wrap_pi(float bearing)
{
	/* value is inf or NaN */
	if (!isfinite(bearing))
	{
		return bearing;
	}

	int c = 0;

	while (bearing >= M_PI_F)
	{
		bearing -= M_TWOPI_F;

		if (c++ > 3)
		{
			return NAN;
		}
	}

	c = 0;

	while (bearing < -M_PI_F)
	{
		bearing += M_TWOPI_F;

		if (c++ > 3)
		{
			return NAN;
		}
	}

	return bearing;
}

