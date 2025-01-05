#include"eso.hpp"

void ExtendedStateObserver::set_eso_coef(const float b0, const float beta01, const float beta02, const float beta03)
{
	_b0 = b0;
	_beta01 = beta01;
	_beta02 = beta02;
	_beta03 = beta03;
}

void ExtendedStateObserver::set_eso_gain_cutoff_frequency(const float b0, const float wc)
{
	_b0 = b0;
	_beta01 = 3.0f * wc;
	_beta02 = 3.0f * wc * wc;
	_beta03 = wc * wc * wc;
}

void ExtendedStateObserver::set_disturb_limit(const float dmin, const float dmax)
{
	_disturb_min = dmin;
	_disturb_max = dmax;
}

void ExtendedStateObserver::set_disturb_gain(const float dis_gain)
{
	_disturb_gain= dis_gain;

}

void ExtendedStateObserver::update(const float u, const float y_, const float dt)
{
	y = y_;
	const float ey = z1 - y;
	z1 += dt * (z2 - _beta01 * ey);
	z2 += dt * (z3 + _b0 * u - _beta02 * ey);
	z3 += dt * (-_beta03 * ey);

	distrub = (-z3 / _b0)*_disturb_gain;
	distrub = math::constrain(distrub, _disturb_min, _disturb_max);
}

void ExtendedStateObserver::reset()
{
	y = 0.0f;
	z1 = z2 = z3 = 0.0f;
	distrub = 0.0f;
}

void ExtendedStateObserver::record_eso_status(eso_s &rate_status_z)
{
	rate_status_z.timestamp = hrt_absolute_time();

	rate_status_z.y = get_eso_reference();
	rate_status_z.z1 = get_eso_state1();
	rate_status_z.z2 = get_eso_state2();
	rate_status_z.z3 = get_eso_state3();
	rate_status_z.disturb = get_eso_disturb();
}
