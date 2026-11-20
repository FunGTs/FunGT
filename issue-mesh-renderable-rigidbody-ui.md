# Add UI controls for `MeshRenderable` rigid-body physics

## Summary

Expose rigid-body setup and inspection for selected `MeshRenderable` objects in the existing scene hierarchy UI.

After rigid-body support is added to `MeshRenderable`, users should be able to select an imported mesh, create or configure a physics proxy, and see the body participate in the existing simulation and debug visualization.

## Dependency

Depends on the `MeshRenderable` rigid-body support issue being completed first.

## Scope

- Extend `SceneHierarchyWindow` with a Physics section for a selected `MeshRenderable`.
- Show whether the selected mesh has an attached rigid body.
- Provide an action to create a rigid body using a bounding-box or bounding-sphere proxy.
- Allow choosing the initial proxy shape: `Box` or `Sphere`.
- Allow configuring mass, friction, and restitution before or during setup.
- Register newly created bodies with the active `CollisionManager`.
- Keep the existing Play/Pause/Stop/Reset controls in `PhysicsControlWindow` working unchanged.
- Make the selected mesh follow its rigid body during simulation.
- Ensure the existing physics debug renderer can display the selected proxy shape.

## UI and ownership design

`SceneHierarchyWindow` currently has access to the `SceneManager` and camera, but not to the physics world or collision manager. The UI construction path must provide access to the active `CollisionManager` so it can register newly created bodies.

The renderable should keep a weak reference to the body. The collision manager should own the registered shared body. The UI must not create bodies implicitly during drawing; creation should happen only after an explicit user action.

## Transform behavior

- Without a rigid body, the current manual transform controls remain active.
- With a rigid body and simulation running, the body drives the mesh transform.
- Manual position and rotation edits should update the body when appropriate, or be disabled while the simulation is active to avoid competing transform authorities.
- Mesh scale should not silently alter an existing rigid body's dimensions unless that behavior is explicitly designed.

## Animation integration

Update the physics recording path if necessary so a physics-enabled `MeshRenderable` can be discovered through the generic `Renderable::getRigidBody()` interface rather than only through `SimpleModel` casts.

## Proposed files

- `GUI/scene_hierarchy_window.hpp`
- The UI construction site that creates `SceneHierarchyWindow`
- `Physics/RigidBody/rigid_body.hpp` if safe mass/inertia setters are needed
- `GUI/animation/animation_control_window.hpp` and/or `Physics/AnimationCreator/animation_controller.hpp` if mesh physics recording is included in this issue

## Acceptance criteria

- Selecting a `MeshRenderable` exposes a Physics section in the scene hierarchy.
- The UI can create and register a Box or Sphere rigid-body proxy from the mesh bounds.
- The selected mesh follows the body when the physics simulation runs.
- Mass, friction, and restitution changes are applied safely.
- The physics debug visualization shows the created proxy.
- Reset restores the body and visual mesh consistently with existing simulation controls.
- Meshes without physics continue to behave exactly as before.
- No arbitrary triangle-mesh collision support is added as part of this issue.
