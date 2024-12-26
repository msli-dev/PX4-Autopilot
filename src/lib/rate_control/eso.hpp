/**
 * @file eso.hpp
 *
 * PID 3 axis angular rate / angular velocity control.
 */

#pragma once

#include <matrix/matrix/math.hpp>
#include <mathlib/mathlib.h>
#include <drivers/drv_hrt.h>
#include <uORB/topics/eso.h>

class ExtendedStateObserver
{
public:
	ExtendedStateObserver() {};
	~ExtendedStateObserver() {};

	void set_eso_coef(const float b0, const float beta01, const float beta02, const float beta03);

	void set_eso_gain_cutoff_frequency(const float b0, const float wc);

	void set_disturb_limit(const float dmin, const float dmax);


	void update(const float u, const float y_, const float dt);

	float get_eso_reference(void)
	{
		return y;
	}
	float get_eso_state1(void)
	{
		return z1;
	}
	float get_eso_state2(void)
	{
		return z2;
	}
	float get_eso_state3(void)
	{
		return z3;
	}

	float get_eso_disturb()
	{
		return distrub;
	}

	void reset();

	void record_eso_status(eso_s &rate_status_z);

private:
	float y = 0.0f;
	float z1 = 0.0f;
	float z2 = 0.0f;
	float z3 = 0.0f;
	float distrub = 0.0f;

	float _disturb_min = 0.0f;
	float _disturb_max = 0.0f;

	float _b0 = 1e5f;

	float _beta01 = 0.0f;
	float _beta02 = 0.0f;
	float _beta03 = 0.0f;
};

