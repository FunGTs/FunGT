#if !defined(_IMODE_H_)
#define _IMODE_H_

#include <string>

// Interface for different workflow modes (Game, Render, Animation)
class IMode {
public:
    virtual ~IMode() = default;

    // Called when this mode becomes active
    virtual void onEnter() = 0;

    // Called when switching away from this mode
    virtual void onExit() = 0;

    // Called every frame - update logic
    virtual void onUpdate(float deltaTime) = 0;

    // Called every frame - rendering
    virtual void onRender() = 0;

    // Called every frame - GUI rendering (ImGui panels, etc.)
    virtual void onImGuiRender() = 0;

    // Get mode name for display
    virtual std::string getName() const = 0;
};

#endif // _IMODE_H_