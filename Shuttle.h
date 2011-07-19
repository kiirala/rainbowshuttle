/*
 * Shuttle.h
 *
 *  Created on: 19.7.2011
 *      Author: niko
 */

#ifndef SHUTTLE_H_
#define SHUTTLE_H_

#include <core/Ref.h>
#include <slm/vec3.h>
#include <slm/quat.h>
#include <render/World.h>

class Shuttle : public core::Object
{
public:
	slm::vec3 position;
	slm::vec3 velocity;
	slm::quat orientation;
	core::Ref<render::Group> model;

	Shuttle(core::Ref<render::Group> model);
	virtual ~Shuttle();

	void rotate(float roll, float pitch);
	void update(float timeDiff);
};

#endif /* SHUTTLE_H_ */
