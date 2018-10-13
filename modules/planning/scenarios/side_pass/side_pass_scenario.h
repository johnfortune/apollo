/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file
 **/

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "modules/common/proto/pnc_point.pb.h"
#include "modules/common/status/status.h"
#include "modules/common/util/factory.h"
#include "modules/planning/common/reference_line_info.h"
#include "modules/planning/common/speed_profile_generator.h"
#include "modules/planning/proto/planning.pb.h"
#include "modules/planning/reference_line/reference_line.h"
#include "modules/planning/reference_line/reference_point.h"
#include "modules/planning/scenarios/scenario.h"
#include "modules/planning/toolkits/task.h"

namespace apollo {
namespace planning {

class SidePassScenario : public Scenario {
 public:
  SidePassScenario() : Scenario(ScenarioConfig::SIDE_PASS) {}
  virtual ~SidePassScenario() = default;

  bool Init() override;

  common::Status Process(const common::TrajectoryPoint& planning_init_point,
                         Frame* frame) override;

  bool IsTransferable(const Scenario& current_scenario,
                      const common::TrajectoryPoint& ego_point,
                      const Frame& frame) const override;

 private:
  enum class SidePassStage {
    UNKNOWN,
    OBSTACLE_APPROACH,
    PATH_GENERATION,
    WAITPOINT_STOP,
    SAFETY_DETECTION,
    OBSTACLE_PASS,
  };

  void RegisterTasks();

  int StageIndexInConf(const SidePassStage& stage);

  SidePassStage GetNextStage(const SidePassStage& current_stage);

  common::Status ApproachObstacle(
      const common::TrajectoryPoint& planning_start_point, Frame* frame);

  common::Status GeneratePath(
      const common::TrajectoryPoint& planning_start_point, Frame* frame);

  common::Status StopOnWaitPoint(
      const common::TrajectoryPoint& planning_start_point, Frame* frame);

  common::Status DetectSafety(
      const common::TrajectoryPoint& planning_start_point, Frame* frame);

  common::Status PassObstacle(
      const common::TrajectoryPoint& planning_start_point, Frame* frame);

  bool IsSidePassScenario(const common::TrajectoryPoint& planning_start_point,
                          const Frame& frame) const;

  common::Status RunPlanOnReferenceLine(
      const common::TrajectoryPoint& planning_start_point, Frame* frame);

  bool IsFarFromIntersection(const Frame& frame);

  bool HasBlockingObstacle(const SLBoundary& adc_sl_boundary,
                           const PathDecision& path_decision) const;

 private:
  std::vector<std::unique_ptr<Task>> tasks_;
  ScenarioConfig config_;
  SidePassStage stage_ = SidePassStage::OBSTACLE_APPROACH;
  bool stage_init_ = false;
  SpeedProfileGenerator speed_profile_generator_;
  PathData path_;
  double wait_point_s = 0;
  PathData path_data_;
};

}  // namespace planning
}  // namespace apollo