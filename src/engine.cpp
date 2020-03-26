#include <elasty/constraint.hpp>
#include <elasty/engine.hpp>
#include <elasty/particle.hpp>

void elasty::AbstractEngine::stepTime()
{
    // Apply external forces
    setExternalForces();
    for (auto& particle : m_particles)
    {
        particle->v = particle->v + m_delta_time * particle->w * particle->f;
    }

    // Calculate predicted positions
    for (auto& particle : m_particles)
    {
        particle->p = particle->x + m_delta_time * particle->v;
    }

    // Generate collision constraints
    generateCollisionConstraints();

    // Reset Lagrange multipliers (only for XPBD)
    for (auto constraint : m_constraints)
    {
        constraint->m_lagrange_multiplier = 0.0;
    }

    // Solve constraints
    for (unsigned int i = 0; i < m_num_iterations; ++i)
    {
        for (auto constraint : m_constraints)
        {
            constraint->projectParticles(AlgorithmType::Pbd);
        }

        for (auto constraint : m_instant_constraints)
        {
            constraint->projectParticles(AlgorithmType::Pbd);
        }
    }

    // Apply the results
    for (auto& particle : m_particles)
    {
        particle->v = (particle->p - particle->x) * (1.0 / m_delta_time);
        particle->x = particle->p;
    }

    // Update velocities
    updateVelocities();

    // Clear instant constraints
    m_instant_constraints.clear();
}

void elasty::AbstractEngine::addConstraint(std::shared_ptr<AbstractConstraint> constraint)
{
    m_constraints.push_back(constraint);
}

void elasty::AbstractEngine::addInstantConstraint(std::shared_ptr<AbstractConstraint> constraint)
{
    m_instant_constraints.push_back(constraint);
}

void elasty::AbstractEngine::clearScene()
{
    m_particles.clear();
    m_constraints.clear();
    m_instant_constraints.clear();
}
