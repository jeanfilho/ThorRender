#pragma once

#include "Engine/Simulation.h"
#include "DebugTriangle.h"

class DebugTriangleSimulation : public Simulation
{
protected:
    void PopulateCommandList() final;
    void PostInit() final;

private:
    UniquePtr<DebugTriangle> m_Triangle;
};