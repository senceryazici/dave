/*
 * Copyright 2020 Naval Postgraduate School
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef GAZEBO_UUV_MATING_HH_
#define GAZEBO_UUV_MATING_HH_

#ifndef DEBUG
#define DEBUG 0
#endif

#include <sstream>
#include <string>
#include <vector>

#include <gazebo/common/common.hh>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector3.hh>

namespace gazebo
{
  class PlugAndSocketMatingPlugin : public ModelPlugin
  {
    /// \brief Pointer to the Gazebo world.
    private: physics::WorldPtr world;

    /// \brief Name of the socket model
    private: std::string socketModelName;

    /// \brief Pointer to the socket model.
    private: physics::ModelPtr socketModel;

    /// \brief Name of the socket tube link
    private: std::string tubeLinkName;

    /// \brief Pointer to the hollow tube like structure that receives the plug
    private: physics::LinkPtr tubeLink;

    /// \brief Name of the sensor plate link name
    private: std::string sensorPlateName;

    /// \brief Pointer to the sensor plate in the socket model that senses when
    /// the plug is inserter.
    private: physics::LinkPtr sensorPlate;

    /// \brief Model name of the plub model
    private: std::string plugModelName;

    /// \brief Pointer to the plug model.
    private: physics::ModelPtr plugModel;

    /// \brief Name of the plug link
    private: std::string plugLinkName;

    /// \brief Pointer to the plug link.
    private: physics::LinkPtr plugLink;

    /// \brief Pinter to the prismatic joint formed between the plug and the
    /// socket.
    private: physics::JointPtr prismaticJoint;

    /// \brief Boolean that indicates weather the prismatic joint is created.
    private: bool joined = false;

    /// \brief Boolean that indicates weather the plug and socket are fixed.
    private: bool locked = false;

    /// \brief Pointer to the update event connection.
    private: gazebo::event::ConnectionPtr updateConnection;

    /// \brief Vector that contains forces (For the purpose of averaging).
    private: std::vector<double> forcesBuffer;

    /// \brief Time stamps associated with forces in the above vector.
    private: std::vector<common::Time> timeStamps;

    /// \brief Time the plug and socket has been in alignment.
    private: common::Time alignmentTime = 0;

    /// \brief Time the plug and the socket has been freed.
    /// Used to put some buffer between unfreezing and another possible mating.
    private: common::Time unfreezeTimeBuffer = 0;

    /// \brief Roll alignment tolerance.
    private: double rollAlignmentTolerance;

    /// \brief pitch alignment tolerance.
    private: double pitchAlignmentTolerance;

    /// \brief Yaw alignment tolerance.
    private: double yawAlignmentTolerance;

    /// \brief force required to mate the plug & socket (lock joint).
    private: double matingForce;

    /// \brief force required to unmate the plug & socket (unlock joint).
    private: double unmatingForce;

    // Some private counter variables ISO "throttled" logging/messages

    /// \brief alignment of plug & socket INFO message
    private: int alignLogThrottle = 0;

    /// \brief proximity of plug & socket INFO message
    private: int proximityLogThrottle = 0;

    /// \brief rotational alignment of plug & socket INFO message
    private: int linksInContactLogThrottle = 0;

    /// \brief Concatenates/trims forcesBuffer and timeStamps vectors to
    /// include only the last trimDuration.
    /// \param[in] trimDuration Duration over which to trim the vectors.
    void trimForceVector(double trimDuration);

    /// \brief Calculates the average of the forcesBuffer vector.
    /// \return Average of the forcesBuffer vector.
    double movingTimedAverage();

    /// \brief Appends another force reading to the forcesBuffer vector.
    void addForce(double force);

    /// \brief Utility function to normalize error angles to (-pi, pi]
    /// \return normalized angle
    private: double normalizeError(double error);

    /// \brief Constructor.
    public: PlugAndSocketMatingPlugin();

    /// Documentation inherited.
    public: void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf);

    /// \brief Locks the prismatic joint connecting plug & socket.
    public: void lockJoint();

    /// \brief Release the plug from the joint connecting plug & socket.
    public: void unlockJoint();

    /// \brief Check if plug and socket have the same orientation and altitude.
    /// \return Return true if same r,p,y and z.
    private: bool isAligned();

    /// \brief Measure Euclidean distance between plug an socket.
    /// \return Return true if plug is close to the socket.
    private: bool checkProximity();

    /// \brief Creates the prismatic joint between the socket and plug.
    private: void constructJoint();

    /// \brief Distroys the prismatic joint between the socket and the plug.
    private: void removeJoint();

    /// \brief Calculates the average force exerted by contact2 on contact 1.
    /// \return Average force exerted by contact2 on contact1.
    public: bool averageForceOnLink(std::string contact1, std::string contact2);

    /// \brief Determine if Electrical Plug is pushing against electrical
    /// socket.
    /// \return boolean indicating whether the plug is pushing against the
    /// socket.
    public: bool isPlugPushingSensorPlate(int numberOfDatapointsThresh = 10);

    /// \brief Determine if Electrical Plug is pushing against electrical
    /// socket.
    /// \return boolean indicating whether the plug is pushing against the
    /// socket.
    public: bool isEndEffectorPushingPlug(int numberOfDatapointsThresh = 10);

    /// \brief Gets the collision index between two links.
    /// \return Collision index between contact1 and contact2.
    public: int getCollisionBetween(std::string contact1, std::string contact2);

    /// \brief Callback executed at every physics update.
    public: void Update();
  };
  GZ_REGISTER_MODEL_PLUGIN(PlugAndSocketMatingPlugin)
}
#endif
