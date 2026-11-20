# Add rigid-body support to `MeshRenderable`

## Summary

Connect `MeshRenderable` to the existing rigid-body physics path used by `SimpleModel`.

`MeshRenderable` currently exposes rendering and transform operations, but it has no association with a `RigidBody`. As a result, physics can move a body while the corresponding imported mesh remains unchanged.

## Scope

- Add a non-owning `std::weak_ptr<RigidBody>` association to `MeshRenderable`.
- Add an API to attach a rigid body and retrieve it through `Renderable::getRigidBody()`.
- Synchronize the renderable position and rotation from the rigid body during `updateModelMatrix()` when physics following is enabled.
- Preserve the existing manual transform behavior when no rigid body is attached or physics following is disabled.
- Define how physics synchronization interacts with `setModelMatrix()` and imported model matrices.

## Collision shape

This task should use an externally created physics proxy (`Box` or `Sphere`). It should not introduce arbitrary triangle-mesh collision yet. The current collision system does not provide general mesh collision support.

The caller remains responsible for creating the `RigidBody`, choosing its shape and mass, and registering it with `CollisionManager`.

## Proposed files

- `Meshing/mesh_renderable.hpp`
- `Meshing/mesh_renderable.cpp`

No changes to the collision solver or `MeshGeometry` are required for the initial implementation.

## Acceptance criteria

- A `MeshRenderable` can be associated with a shared `RigidBody`.
- `getRigidBody()` returns that body through the existing `Renderable` interface.
- During simulation, rigid-body position and orientation update the rendered mesh.
- Manual position and rotation still work for meshes without physics.
- Imported model matrices do not silently override the active physics transform.
- Existing mesh loading, drawing, bounds calculation, and material behavior remain unchanged.
- The implementation does not create or register physics bodies implicitly during rendering.

## Notes

The first implementation should mirror the ownership model already used by `SimpleModel`: the renderable stores a weak reference, while the physics world or scene setup owns and registers the shared body.
