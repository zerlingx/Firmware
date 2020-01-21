/****************************************************************************
 *
 *   Copyright (c) 2019 PX4 Development Team. All rights reserved.
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

#pragma once

#include <lib/ecl/validation/data_validator_group.h>
#include <lib/mathlib/math/Limits.hpp>
#include <lib/matrix/matrix/math.hpp>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/SubscriptionCallback.hpp>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_correction.h>

#include <uORB/topics/sensor_baro.h>
#include <uORB/topics/vehicle_air_data.h>

class VehicleAirData : public ModuleParams, public px4::ScheduledWorkItem
{
public:

	VehicleAirData();
	~VehicleAirData() override;

	void	Run() override;

	bool	Start();
	void	Stop();

	void	PrintStatus();

private:
	void ParametersUpdate(bool force = false);
	void SensorCorrectionsUpdate(bool force = false);

	static constexpr int MAX_SENSOR_COUNT = 3;

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::SENS_BARO_QNH>) _param_sens_baro_qnh
	)

	uORB::Publication<vehicle_air_data_s> _vehicle_air_data_pub{ORB_ID(vehicle_air_data)};

	uORB::Subscription			_params_sub{ORB_ID(parameter_update)};			/**< parameter updates subscription */
	uORB::Subscription			_sensor_correction_sub{ORB_ID(sensor_correction)};	/**< sensor thermal correction subscription */

	uORB::SubscriptionCallbackWorkItem	_sensor_sub[MAX_SENSOR_COUNT] {				/**< sensor data subscription */
		{this, ORB_ID(sensor_baro), 0},
		{this, ORB_ID(sensor_baro), 1},
		{this, ORB_ID(sensor_baro), 2}
	};

	float			_offset{0.f};
	float			_scale{1.f};

	uint32_t				_selected_sensor_device_id{0};
	uint8_t					_selected_sensor_sub_index{0};
	int8_t					_corrections_selected_instance{-1};


	uint8_t _priority[MAX_SENSOR_COUNT]; /**< sensor priority */
	uint8_t _last_best_vote{0}; /**< index of the latest best vote */
	int _subscription_count{0};
	DataValidatorGroup _voter{1};
	unsigned int last_failover_count;

};
