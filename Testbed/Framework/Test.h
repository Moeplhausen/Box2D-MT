/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
* Copyright (c) 2015 Justin Hoffman https://github.com/jhoffman0x/Box2D-MT
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef TEST_H
#define TEST_H

#include "Box2D/Box2D.h"
#include "DebugDraw.h"

#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include "Testbed/glad/glad.h"
#endif
#include "Testbed/glfw/glfw3.h"

#include <stdlib.h>

class Test;
struct Settings;

typedef Test* TestCreateFcn();

#define	RAND_LIMIT	32767
#define DRAW_STRING_NEW_LINE 16

/// Random number in range [-1,1]
inline float32 RandomFloat()
{
	float32 r = (float32)(rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = 2.0f * r - 1.0f;
	return r;
}

/// Random floating point number in range [lo, hi]
inline float32 RandomFloat(float32 lo, float32 hi)
{
	float32 r = (float32)(rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = (hi - lo) * r + lo;
	return r;
}

/// Test settings. Some can be controlled in the GUI.
struct Settings
{
	Settings()
	{
		hz = 60.0f;
		velocityIterations = 8;
		positionIterations = 3;
		threadCount = 1;
		stepsPerProfileUpdate = 4;
		mtProfileIterations = 4;
		mtConsistencyIterations = 2;
		mtCurrentTestOnly = false;
		drawSubTrees = true;
		drawShapes = true;
		drawJoints = true;
		drawAABBs = false;
		drawContactPoints = false;
		drawContactNormals = false;
		drawContactImpulse = false;
		drawFrictionImpulse = false;
		drawCOMs = false;
		drawStats = false;
		drawProfile = true;
		enableWarmStarting = true;
		enableContinuous = true;
		enableSubStepping = false;
		enableSleep = true;
		pause = false;
		singleStep = false;
	}

	float32 hz;
	int32 velocityIterations;
	int32 positionIterations;
	int32 threadCount;
	int32 stepsPerProfileUpdate;
	int32 mtProfileIterations;
	int32 mtConsistencyIterations;
	bool mtCurrentTestOnly;
	bool drawSubTrees;
	bool drawShapes;
	bool drawJoints;
	bool drawAABBs;
	bool drawContactPoints;
	bool drawContactNormals;
	bool drawContactImpulse;
	bool drawFrictionImpulse;
	bool drawCOMs;
	bool drawStats;
	bool drawProfile;
	bool enableWarmStarting;
	bool enableContinuous;
	bool enableSubStepping;
	bool enableSleep;
	bool pause;
	bool singleStep;
};

struct TestEntry
{
	const char *name;
	TestCreateFcn *createFcn;
	int32 mtStepCount;
};

extern TestEntry g_testEntries[];
// This is called when a joint in the world is implicitly destroyed
// because an attached body is destroyed. This gives us a chance to
// nullify the mouse joint.
class DestructionListener : public b2DestructionListener
{
public:
	void SayGoodbye(b2Fixture* fixture) override { B2_NOT_USED(fixture); }
	void SayGoodbye(b2Joint* joint) override;

	Test* test;
};

const int32 k_maxContactPoints = 8192;

struct ContactPoint
{
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	b2Vec2 position;
	b2PointState state;
	float32 normalImpulse;
	float32 tangentImpulse;
	float32 separation;
};

enum class TestResult
{
	NONE = 0,
	PASS,
	FAIL
};

inline TestResult operator&=(TestResult& a, TestResult b)
{
	if ((int)a < (int)b)
	{
		a = b;
	}
	return a;
}

inline const char* TestResultString(TestResult a)
{
	switch(a)
	{
		case TestResult::NONE:	return "None";
		case TestResult::PASS:	return "Pass";
		case TestResult::FAIL:	return "FAIL";
	}
	b2Assert(false);
	return "";
}

class Test : public b2ContactListener
{
public:

	Test();
	virtual ~Test();

	void DrawTitle(const char *string);
	virtual void Step(Settings* settings);
	virtual void Keyboard(int key) { B2_NOT_USED(key); }
	virtual void KeyboardUp(int key) { B2_NOT_USED(key); }
	void ShiftMouseDown(const b2Vec2& p);
	virtual void MouseDown(const b2Vec2& p);
	virtual void MouseUp(const b2Vec2& p);
	void MouseMove(const b2Vec2& p);
	void LaunchBomb();
	void LaunchBomb(const b2Vec2& position, const b2Vec2& velocity);

	void SpawnBomb(const b2Vec2& worldPt);
	void CompleteBombSpawn(const b2Vec2& p);

	// Let derived tests know that a joint was destroyed.
	virtual void JointDestroyed(b2Joint* joint) { B2_NOT_USED(joint); }

	// Contact listener interface.
	// Derived classes must override the immedaite functions if they need a deferred callback.
	virtual bool BeginContactImmediate(b2Contact* contact, uint32 threadId) override
	{
		B2_NOT_USED(contact);
		B2_NOT_USED(threadId);
		return false;
	}
	virtual bool EndContactImmediate(b2Contact* contact, uint32 threadId) override
	{
		B2_NOT_USED(contact);
		B2_NOT_USED(threadId);
		return false;
	}
	virtual bool PreSolveImmediate(b2Contact* contact, const b2Manifold* oldManifold, uint32 threadId) override;
	virtual bool PostSolveImmediate(b2Contact* contact, const b2ContactImpulse* impulse, uint32 threadId) override
	{
		B2_NOT_USED(contact);
		B2_NOT_USED(impulse);
		B2_NOT_USED(threadId);
		return false;
	}
	virtual void BeginContact(b2Contact* contact)  override
	{
		B2_NOT_USED(contact);
	}
	virtual void EndContact(b2Contact* contact)  override
	{
		B2_NOT_USED(contact);
	}
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override
	{
		B2_NOT_USED(contact);
		B2_NOT_USED(oldManifold);
	}
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override
	{
		B2_NOT_USED(contact);
		B2_NOT_USED(impulse);
	}
	virtual TestResult TestPassed() const { return TestResult::NONE; }

	void ShiftOrigin(const b2Vec2& newOrigin);

	b2ThreadPoolTaskExecutor* GetExecutor();
	b2World* GetWorld();
	const b2Profile& GetTotalProfile() const;
	void SetVisible(bool flag);

protected:
	friend class DestructionListener;
	friend class BoundaryListener;
	friend class ContactListener;

	b2Body* m_groundBody;
	b2AABB m_worldAABB;
	ContactPoint m_points[b2_maxThreads][k_maxContactPoints];
	int32 m_pointCount[b2_maxThreads];
	DestructionListener m_destructionListener;
	int32 m_textLine;
	b2World* m_world;
	b2Body* m_bomb;
	b2MouseJoint* m_mouseJoint;
	b2Vec2 m_bombSpawnPoint;
	bool m_bombSpawning;
	bool m_visible;
	b2Vec2 m_mouseWorld;
	float32 m_timeStep;
	int32 m_stepCount;
	int32 m_smoothProfileStepCount;

	b2Profile m_maxProfile;
	b2Profile m_totalProfile;
	b2Profile m_smoothProfile[2];

	b2ThreadPoolTaskExecutor m_threadPoolExec;
};

inline b2ThreadPoolTaskExecutor* Test::GetExecutor()
{
	return &m_threadPoolExec;
}

inline b2World* Test::GetWorld()
{
	return m_world;
}

inline const b2Profile& Test::GetTotalProfile() const
{
	return m_totalProfile;
}

inline void Test::SetVisible(bool flag)
{
	m_visible = flag;
}

#endif
