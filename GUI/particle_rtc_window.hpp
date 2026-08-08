#ifndef PARTICLE_RTC_WINDOW_HPP
#define PARTICLE_RTC_WINDOW_HPP

#include "GUI/imgui_window.hpp"
#include "ParticleSimulation/particle_simulation_rtc.hpp"
#include <string>
#include <future>
#include <atomic>
#include <memory>



class ParticleRTCWindow : public ImGuiWindow {
private:
    std::shared_ptr<ParticleRTC> m_particleRTC;

    std::string m_initCode;
    std::string m_updateCode;
    std::string m_statusMessage;
    std::string m_errorMessage;
    std::string m_userPrompt;
    bool m_isVisible = true;
    std::string m_llmAx;
    std::string m_llmUrl;
    // Threading
    std::atomic<bool> m_isCompiling{ false };
    std::atomic<bool> m_isGenerating{ false };
    std::future<std::pair<bool, std::string>> m_generateFuture;
    std::future<std::pair<bool, std::string>> m_compileFuture; ;
    enum class CompileStatus {
        Idle,
        Compiling,
        Generating,
        Compiled,
        Generated,
        Error
    };
    
    CompileStatus m_status = CompileStatus::Idle;

    static int InputTextCallback(ImGuiInputTextCallbackData* data);
    void compileAsync();
    void checkCompilationStatus();
    void generateFromPrompt();  
    void checkGenerationStatus(); 
    std::string reqToLLM(const std::string& systemPrompt, const std::string& userPrompt);
    void parseAndSetCode(const std::string& response);

public:
    ParticleRTCWindow(std::shared_ptr<ParticleRTC> particleRTC);
    void onImGuiRender() override;
};

#endif