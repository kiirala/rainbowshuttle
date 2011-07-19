/*
 * Shuttle.cpp
 *
 *  Created on: 19.7.2011
 *      Author: niko
 */

#include <render/OGLES/OGLES1_Graphics3D.h>
#include <render/OGLES/OGLES2_Graphics3D.h>
#include <slm/vec3.h>
#include <slm/quat.h>

#include "Shuttle.h"

Shuttle::Shuttle(core::Ref<render::Group> model)
: position(0, 0, 0), velocity(0, 0, 0), orientation(0, 0, 0, 1),
  model(model)
{
}

Shuttle::~Shuttle() {
}

void Shuttle::rotate(float roll, float pitch) {
//	core::Debug::printf(core::Debug::APPLICATION, "%.4f %.4f %.4f %.4f, rot. %f %f",
//			orientation.x, orientation.y, orientation.z, orientation.w,
//			roll, pitch);
	orientation *= slm::quat(roll, slm::vec3(0, 1, 0));
	orientation *= slm::quat(pitch, slm::vec3(1, 0, 0));
}

void Shuttle::update(float timeDiff) {
	model->setTransform(slm::translation(this->position) * slm::mat4(this->orientation));
}
