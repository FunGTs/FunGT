#include "particle_rtc_window.hpp"
#include "ParticleSimulation/particle_simulation_rtc.hpp"
#include <imgui.h>
#include <curl/curl.h>
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
ParticleRTCWindow::ParticleRTCWindow(std::shared_ptr<ParticleRTC> particleRTC)
    : m_particleRTC(particleRTC) {



    // Load LLM config
    std::string config_location = getAssetPath(".fungt_config.json");
    std::ifstream config_file(config_location);
    if (config_file.is_open()) {
        std::string line;
        while (std::getline(config_file, line)) {
            auto extractValue = [&](const std::string& field, std::string& dest) {
                size_t pos = line.find("\"" + field + "\"");
                if (pos != std::string::npos) {
                    size_t start = line.find('"', pos + field.size() + 2) + 1;
                    size_t end = line.find('"', start);
                    dest = line.substr(start, end - start);
                }
            };
            extractValue("llm_ax", m_llmAx);
            extractValue("llm_url", m_llmUrl);
        }
        config_file.close();
    }

    if (m_llmAx.empty() || m_llmUrl.empty()) {
        std::cout << "Warning: LLM config incomplete. Check .fungt_config.json.\n";
    }

//     m_initCode = R"(float theta = index * 0.061803f;
// float phi = index * 0.123f;
// float speed = 2.0f + (index % 100) * 0.01f;

// p.position[0] = 0.0f;
// p.position[1] = 0.0f;
// p.position[2] = 0.0f;

// p.velocity[0] = speed * sycl::sin(phi) * sycl::cos(theta);
// p.velocity[1] = speed * sycl::sin(phi) * sycl::sin(theta);
// p.velocity[2] = speed * sycl::cos(phi);)";

//     m_updateCode = R"(constexpr float gravity = -2.0f;
// constexpr float drag = 0.99f;
// p.velocity[2] += gravity * dt;
// p.velocity[0] *= drag;
// p.velocity[1] *= drag;
// p.velocity[2] *= drag;
// p.position[0] += p.velocity[0] * dt;
// p.position[1] += p.velocity[1] * dt;
// p.position[2] += p.velocity[2] * dt;)";

    m_statusMessage = "Ready";
}

int ParticleRTCWindow::InputTextCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        std::string* str = static_cast<std::string*>(data->UserData);
        str->resize(data->BufTextLen);
        data->Buf = const_cast<char*>(str->c_str());
    }
    return 0;
}

void ParticleRTCWindow::compileAsync() {
    if (m_isCompiling) return;

    m_isCompiling = true;
    m_status = CompileStatus::Compiling;
    m_errorMessage.clear();
    m_statusMessage = "Compiling kernels...";

    std::string init_code = m_initCode;
    std::string update_code = m_updateCode;

    m_compileFuture = std::async(std::launch::async,
        [this, init_code, update_code]() -> std::pair<bool, std::string> {

            std::string error;

            std::cout << "Starting init kernel compilation...\n";
            bool init_ok = m_particleRTC->compileInitKernel(init_code, error);
            if (!init_ok) {
                return { false, "Init kernel: " + error };
            }
            std::cout << "Init kernel compiled successfully\n";

            std::cout << "Starting update kernel compilation...\n";
            bool update_ok = m_particleRTC->compileKernel(update_code, error);
            if (!update_ok) {
                return { false, "Update kernel: " + error };
            }
            std::cout << "Update kernel compiled successfully\n";

            // DON'T call init() here - OpenGL not thread-safe!

            return { true, "" };
        });
}

void ParticleRTCWindow::checkCompilationStatus() {
    if (!m_compileFuture.valid()) return;

    if (m_compileFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
        auto [success, error] = m_compileFuture.get();
        m_isCompiling = false;

        if (success) {
            // NOW reinitialize on main thread (OpenGL context lives here)
            std::cout << "Reinitializing particles on main thread...\n";
            m_particleRTC->init();
            std::cout << "Reinitialization complete\n";

            m_status = CompileStatus::Compiled;
            m_statusMessage = "Compilation successful!";
            m_errorMessage.clear();
        }
        else {
            m_status = CompileStatus::Error;
            m_statusMessage = "Compilation failed";
            m_errorMessage = error;
        }
    }
}

void ParticleRTCWindow::onImGuiRender() {
    if (!m_isVisible) return;
    ImGui::Begin("Particle RTC Editor", &m_isVisible);

    // Prompt section
    ImGui::Text("Describe the effect:");
    m_userPrompt.reserve(1024);
    ImGui::InputTextMultiline("##prompt",
        const_cast<char*>(m_userPrompt.c_str()),
        m_userPrompt.capacity(),
        ImVec2(-1, 200),
        ImGuiInputTextFlags_CallbackResize,
        InputTextCallback,
        &m_userPrompt);

    if (ImGui::Button("Generate from Prompt", ImVec2(250, 0))) {
        generateFromPrompt();
    }
    ImGui::SameLine();
    if (m_status == CompileStatus::Generating) {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Generating...");
    }

    ImGui::Separator();
    ImGui::Text("Runtime Kernel Compiler", &m_isVisible);

    switch (m_status) {
    case CompileStatus::Idle:
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Status: %s", m_statusMessage.c_str());
        break;
    case CompileStatus::Compiling:
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Status: %s", m_statusMessage.c_str());
        break;
    case CompileStatus::Compiled:
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: %s", m_statusMessage.c_str());
        break;
    case CompileStatus::Error:
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Status: %s", m_statusMessage.c_str());
        break;
    }

    ImGui::Separator();

    ImGui::Text("Initialization Code:");
    ImGui::Text("Lambda: (Particle& p, int index)");

    m_initCode.reserve(8192);
    ImGui::InputTextMultiline("##init",
        const_cast<char*>(m_initCode.c_str()),
        m_initCode.capacity(),
        ImVec2(-1, 200),
        ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_AllowTabInput,
        InputTextCallback,
        &m_initCode);

    ImGui::Separator();

    ImGui::Text("Update Code:");
    ImGui::Text("Lambda: (Particle& p, float dt)");

    m_updateCode.reserve(8192);
    ImGui::InputTextMultiline("##update",
        const_cast<char*>(m_updateCode.c_str()),
        m_updateCode.capacity(),
        ImVec2(-1, 200),
        ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_AllowTabInput,
        InputTextCallback,
        &m_updateCode);

    ImGui::Separator();

    // FIXED: Use single scope for BeginDisabled/EndDisabled
    bool isCompiling = m_isCompiling.load();  // Read once

    if (isCompiling) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button("Compile & Run", ImVec2(170, 0))) {
        compileAsync();
    }

    if (isCompiling) {
        ImGui::EndDisabled();
    }

    // Show wait message outside the disabled scope
    if (isCompiling) {
        ImGui::SameLine();
        ImGui::Text("(Please wait...)");
    }

    if (m_status == CompileStatus::Error && !m_errorMessage.empty()) {
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error:");
        ImGui::TextWrapped("%s", m_errorMessage.c_str());
    }

    if (m_isCompiling) {
        checkCompilationStatus();
    }
    if (m_isGenerating) { 
        checkGenerationStatus();
    }

    ImGui::End();
}
void ParticleRTCWindow::generateFromPrompt() {
    if (m_isGenerating) return;

    m_isGenerating = true;
    m_status = CompileStatus::Generating;
    std::string prompt = m_userPrompt;

    m_generateFuture = std::async(std::launch::async,
        [this, prompt]() -> std::pair<bool, std::string> {

            std::string systemPrompt = R"(
You generate C++ code for particle simulations.

Particle struct: fgt::Particle<float> with position[3], velocity[3], acceleration[3], mass

TOOLS AVAILABLE:
- fungt::RNG rng(index, 0); float r = rng.nextFloat01();
- SYCL math: sycl::sin, sycl::cos, sycl::sqrt, sycl::acos

INIT BODY (vars: Particle& p, int index):
Set initial positions and velocities

UPDATE BODY (vars: Particle& p, float dt):
ALWAYS update ALL position components:
p.position[0] += p.velocity[0] * dt;
p.position[1] += p.velocity[1] * dt;
p.position[2] += p.velocity[2] * dt;
Then add physics (gravity, respawn, etc)

Return PLAIN CODE, NO markdown, NO backticks, NO ```cpp
Format:
INIT:
<code>
UPDATE:
<code>
)";
            try {
                std::string response = reqToLLM(systemPrompt, prompt);
                parseAndSetCode(response);
                return { true, "" };
            }
            catch (std::exception& e) {
                return { false, std::string(e.what()) };
            }
        });
}
std::string ParticleRTCWindow::reqToLLM(const std::string& systemPrompt, const std::string& userPrompt) {
    if (m_llmAx.empty() || m_llmUrl.empty()) {
        throw std::runtime_error("LLM not configured");
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize cURL");
    }

    std::string url = m_llmUrl;
    // Escape JSON strings
    auto escapeJson = [](const std::string& s) {
        std::string escaped = s;
        size_t pos = 0;
        while ((pos = escaped.find('"', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\\"");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped.find('\n', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\n");
            pos += 2;
        }
        return escaped;
        };

    std::string escapedSystem = escapeJson(systemPrompt);
    std::string escapedUser = escapeJson(userPrompt);

    // JSON format
    std::string payload = R"({
        "contents": [{
            "parts": [{
                "text": ")" + escapedSystem + R"(\n\nUser request: )" + escapedUser + R"("
            }]
        }],
        "generationConfig": {
            "temperature": 0.7,
            "maxOutputTokens": 4000
        }
    })";

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("x-goog-api-key: " + m_llmAx).c_str());

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw std::runtime_error("cURL failed: " + std::string(curl_easy_strerror(res)));
    }

    return response;
}
void ParticleRTCWindow::parseAndSetCode(const std::string& response) {
    
    std::cout << "=== LLM RESPONSE ===\n" << response << "\n=== END ===\n";
    size_t text_pos = response.find("\"text\":");
    if (text_pos == std::string::npos) {
        throw std::runtime_error("No text field in LLM response");
    }

    size_t start = response.find('"', text_pos + 7) + 1;
    size_t end = response.find('"', start);
    std::string content = response.substr(start, end - start);

    // Unescape \n
    size_t pos = 0;
    while ((pos = content.find("\\n", pos)) != std::string::npos) {
        content.replace(pos, 2, "\n");
        pos += 1;
    }
    // Unescape \n
    pos = 0;
    while ((pos = content.find("\\n", pos)) != std::string::npos) {
        content.replace(pos, 2, "\n");
        pos += 1;
    }

    // Unescape \u003c → 
    pos = 0;
    while ((pos = content.find("\\u003c", pos)) != std::string::npos) {
        content.replace(pos, 6, "<");
        pos += 1;
    }

    // Unescape \u003e → >
    pos = 0;
    while ((pos = content.find("\\u003e", pos)) != std::string::npos) {
        content.replace(pos, 6, ">");
        pos += 1;
    }
    // Strip markdown code fences
    auto stripMarkdown = [](std::string& code) {
        size_t start = code.find("```cpp");
        if (start != std::string::npos) {
            code.erase(start, 6);
        }
        start = code.find("```");
        if (start != std::string::npos) {
            code.erase(start, 3);
        }
        size_t end = code.rfind("```");
        if (end != std::string::npos) {
            code.erase(end, 3);
        }
        };
    // Extract INIT and UPDATE sections
    size_t init_pos = content.find("INIT:");
    size_t update_pos = content.find("UPDATE:");

    if (init_pos != std::string::npos && update_pos != std::string::npos) {
        m_initCode = content.substr(init_pos + 5, update_pos - init_pos - 5);
        m_updateCode = content.substr(update_pos + 7);

        m_initCode.erase(0, m_initCode.find_first_not_of(" \n\r\t"));
        m_updateCode.erase(0, m_updateCode.find_first_not_of(" \n\r\t"));
    }
    else {
        throw std::runtime_error("Could not find INIT/UPDATE sections in response");
    }
}
void ParticleRTCWindow::checkGenerationStatus() {
    if (!m_generateFuture.valid()) return;

    if (m_generateFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
        auto [success, error] = m_generateFuture.get();
        m_isGenerating = false;

        if (success) {
            m_status = CompileStatus::Idle;
            m_statusMessage = "Code generated! Review and compile.";
        }
        else {
            m_status = CompileStatus::Error;
            m_errorMessage = error;
        }
    }
}
