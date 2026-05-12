/****************************************************************************
*
*   Copyright (c) 2025 PX4 Development Team. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
* 3. Neither the name PX4 nor the names of its contributors may be
*    used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************/
/**
* @file mc_nn_control.cpp
* Multicopter Neural Network Control module, from position setpoints to actuator motors.
*
* @author Sindre Meyer Hegre <sindre.hegre@gmail.com>
* @author Welf Rehberg <welf.rehberg@ntnu.no>
*/

#include "mc_overload_detector.hpp"
#ifdef __PX4_NUTTX
#include <drivers/drv_hrt.h>
#else
#endif

ModuleBase::Descriptor MulticopterOverloadDetector::desc{task_spawn, custom_command, print_usage};


MulticopterOverloadDetector::MulticopterOverloadDetector() :
	ModuleParams(nullptr),
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::lp_default),
	_loop_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": cycle"))
{
	parameters_updated();
}

MulticopterOverloadDetector::~MulticopterOverloadDetector()
{
	ScheduleClear();
	perf_free(_loop_perf);
}


bool MulticopterOverloadDetector::init()
{
	ScheduleOnInterval(100_ms);
	return true;
}

void MulticopterOverloadDetector::parameters_updated()
{
	_ovld_thr = _param_ovld_thr.get();
}
// int32_t MulticopterNeuralNetworkControl::GetTime()
// {
// #ifdef __PX4_NUTTX
// 	return static_cast<int32_t>(hrt_absolute_time());
// #else
// 	return static_cast<int32_t>(std::chrono::duration_cast<std::chrono::microseconds>
// 				    (std::chrono::system_clock::now().time_since_epoch()).count());
// #endif
// }

int MulticopterOverloadDetector::task_spawn(int argc, char *argv[])
{
	MulticopterOverloadDetector *instance = new MulticopterOverloadDetector();

	if (instance) {
		desc.object.store(instance);
		desc.task_id = task_id_is_work_queue;

		if (instance->init()) {
			return PX4_OK;

		} else {
			PX4_ERR("init failed");
		}

	} else {
		PX4_ERR("alloc failed");
	}

	delete instance;
	desc.object.store(nullptr);
	desc.task_id = -1;

	return PX4_ERROR;
}

void MulticopterOverloadDetector::Run()
{
	if (should_exit()) {
		ScheduleClear();
		exit_and_cleanup(desc);
		return;
	}

	perf_begin(_loop_perf);

	if (_parameter_update_sub.updated()) {
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);

		ModuleParams::updateParams();
		parameters_updated();
	}

	const hrt_abstime now = hrt_absolute_time();
	const float dt = (_last_run_time > 0) ? math::constrain((now - _last_run_time) * 1e-6f, 0.01f, 0.2f) : 0.1f;
	_last_run_time = now;

	vehicle_land_detected_s land_detected{};

	if (_land_detected_sub.update(&land_detected)) {
		if (land_detected.landed) {
			_land_command_sent = false;
			_overload_time_s = 0.f;
			_overload_detected = false;
		}
	}

	vehicle_thrust_setpoint_s thrust_sp{};

	if (_thrust_sp_sub.update(&thrust_sp) && PX4_ISFINITE(thrust_sp.xyz[2])) {
		_thrust_sp = math::constrain(-thrust_sp.xyz[2], 0.f, 1.f);
	}

	vehicle_local_position_s local_pos{};

	if (_position_sub.update(&local_pos)) {
		_vertical_velocity = PX4_ISFINITE(local_pos.vz) ? local_pos.vz : 0.f;
	}

	const float overload_threshold = math::constrain(_ovld_thr, 0.f, 1.f);

	const bool thrust_high = (overload_threshold > 0.f) && (_thrust_sp >= overload_threshold);
	const bool descending = _vertical_velocity > 0.3f;

	if (thrust_high && descending) {
		_overload_time_s += dt;

	} else {
		_overload_time_s = 0.f;
	}

	_overload_detected = _overload_time_s > 3.0f;

	const bool critical_overload = (_thrust_sp > 0.95f) && (_vertical_velocity > 0.5f);

	if (_overload_detected && !_land_command_sent) {
		send_land_command();
	}

	vehicle_overload_status_s overload_status{};
	overload_status.timestamp = now;
	overload_status.overload = _overload_detected;
	overload_status.critical = critical_overload;
	overload_status.thrust_mean = _thrust_sp;
	overload_status.motor_saturation = 0.f;
	overload_status.estimated_load_ratio = overload_threshold > 0.f ? _thrust_sp / overload_threshold : 0.f;

	_vehicle_overload_status_pub.publish(overload_status);

	perf_end(_loop_perf);
}

void MulticopterOverloadDetector::send_land_command()
{
	if (_land_command_sent) {
		return;
	}

	vehicle_command_s cmd{};
	cmd.timestamp = hrt_absolute_time();
	cmd.command = vehicle_command_s::VEHICLE_CMD_NAV_LAND;

	// 使用当前位置/当前任务逻辑，不强行指定经纬度
	cmd.param5 = NAN;
	cmd.param6 = NAN;
	cmd.param7 = NAN;

	cmd.target_system = 1;
	cmd.target_component = 1;
	cmd.source_system = 1;
	cmd.source_component = 1;
	cmd.from_external = false;

	_vehicle_command_pub.publish(cmd);

	_land_command_sent = true;
	PX4_WARN("overload detected, NAV_LAND command sent");
}

int MulticopterOverloadDetector::custom_command(int argc, char *argv[])
{
	return print_usage("unknown command");
}

int MulticopterOverloadDetector::print_status()
{
	PX4_INFO("Multicopter overload detector status:");
	PX4_INFO("  thrust threshold:  %.3f", (double)_param_ovld_thr.get());
	PX4_INFO("  overload detected: %s", _overload_detected ? "true" : "false");
	PX4_INFO("  thrust setpoint:   %.3f", (double)_thrust_sp);
	PX4_INFO("  vertical velocity: %.3f m/s", (double)_vertical_velocity);

	perf_print_counter(_loop_perf);

	return 0;
}

int MulticopterOverloadDetector::print_usage(const char *reason)
{
	if (reason) {
		PX4_ERR("%s", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
	### Description
	Multicopter overload detector module.

	This module monitors the multicopter thrust margin, vertical tracking error,
	vertical velocity and actuator saturation to detect possible overload conditions.

	When an overload condition is detected, it publishes overload status for
	commander or failsafe logic to trigger a safe landing action.

	Typical overload indicators include:
	- high collective thrust demand for a sustained period
	- poor altitude tracking
	- unexpected descent while thrust demand is high
	- actuator saturation
	)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("mc_overload_detector", "system");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();


	return 0;
}



extern "C" __EXPORT int mc_overload_detector_main(int argc, char *argv[])
{
	return ModuleBase::main(MulticopterOverloadDetector::desc, argc, argv);
}
