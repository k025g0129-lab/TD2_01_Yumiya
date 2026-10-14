#pragma once

class IEnemiesBehavior {
  public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void UpdateMotion() = 0;

	virtual ~IEnemiesBehavior() = default;
};