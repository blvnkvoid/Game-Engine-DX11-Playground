    #pragma once
    #include <string>
    #include <vector>
    #include <optional>

    class GraphicsEngine;
    class PhysicsEngine;

        struct ResolutionRequest
    {
        int width;
        int height;
    };

    class DevConsole
    {
    public:
        void Draw();
        void ExecuteCommand(GraphicsEngine& engine, PhysicsEngine& physics);
        bool ConsumeResolutionRequest(int &width, int &height);

    private:
        bool m_open = false;
        char m_input[256] = {};
        std::vector<std::string> m_log;
        std::string m_pendingCommand;
        void SubmitCommand(const char* command);
        std::optional<ResolutionRequest> m_resolutionRequest;
    };


