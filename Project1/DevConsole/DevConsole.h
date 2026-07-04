    #pragma once
    #include <string>
    #include <vector>

    class GraphicsEngine;
    class PhysicsEngine;

    class DevConsole
    {
    public:
        void Draw();
        void ExecuteCommand(GraphicsEngine& engine, PhysicsEngine& physics);

    private:
        bool m_open = false;
        char m_input[256] = {};
        std::vector<std::string> m_log;
        std::string m_pendingCommand;

        void SubmitCommand(const char* command);
    };