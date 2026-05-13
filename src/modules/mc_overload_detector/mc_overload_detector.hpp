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
 * @file mc_nn_control.h
 * Multicopter Neural Network Control module, from position setpoints to control allocator.
 *
 * @author Sindre Meyer Hegre <sindre.hegre@gmail.com>
 * @author Welf Rehberg <welf.rehberg@ntnu.no>
 */
#pragma once

#include <perf/perf_counter.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <px4_platform_common/time.h>
#include <mathlib/mathlib.h>

// Include model
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/SubscriptionCallback.hpp>

// Subscriptions
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/vehicle_thrust_setpoint.h>
#include <uORB/topics/vehicle_overload_status.h>
#include <uORB/topics/register_ext_component_reply.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/vehicle_land_detected.h>

// Publications

using namespace time_literals; // For the 1_s in the subscription interval
class MulticopterOverloadDetector : public ModuleBase, public ModuleParams,
	public px4::ScheduledWorkItem
{
public:

	static Descriptor desc;

	MulticopterOverloadDetector();
	~MulticopterOverloadDetector() override;

	/** @see ModuleBase */
	static int task_spawn(int argc, char *argv[]);

	/** @see ModuleBase */
	static int custom_command(int argc, char *argv[]);

	/** @see ModuleBase */
	static int print_usage(const char *reason = nullptr);

	/** @see ModuleBase */
	int print_status() override;

	bool init();

private:
	void Run() override;
	// function
	void parameters_updated();

	// Subscriptions
	uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};
	uORB::Subscription _position_sub{ORB_ID(vehicle_local_position)};
	uORB::Subscription _thrust_sp_sub{ORB_ID(vehicle_thrust_setpoint)};
	uORB::Subscription _land_detected_sub{ORB_ID(vehicle_land_detected)};


	uORB::Publication<vehicle_overload_status_s> _vehicle_overload_status_pub{ORB_ID(vehicle_overload_status)};


	// Variables
	hrt_abstime _last_run_time{0};
	float _overload_time_s{0.f};
	float _ovld_thr{.0f};
	perf_counter_t _loop_perf; /**< loop duration performance counter */
// 	hrt_abstime _last_run{0};
	bool _overload_detected{false};
	float _thrust_sp{0.f};
	float _vertical_velocity{0.f};

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::MC_OVLD_THR>) _param_ovld_thr
	)
};
