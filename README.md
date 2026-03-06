# Ngin

Custom C++ game engine for Windows. Early development stage.

## Architecture

```
Ngin/        - Engine core (shared DLL, NG_API exports)
Sandbox/     - Demo/test client application
```

The engine is built as a DLL. Client apps link against it and implement `CreateApplication()`.

**Build:** Premake5, x64 Windows, C++17, Visual Studio.

---

## Getting Started

### 1. Entry point

Include `<Ngin.h>` and implement `CreateApplication()`:

```cpp
#include <Ngin.h>

class MyApp : public Ngin::Application
{
public:
    MyApp() { /* setup */ }

    void Tick(float deltaTime) override
    {
        // called every frame
    }
};

Ngin::Application* Ngin::CreateApplication()
{
    return new MyApp();
}
```

`Tick(deltaTime)` is called once per frame. `deltaTime` is in seconds.

---

## Subsystems

Subsystems are registered via the `SubsystemManager`. The engine provides built-in subsystems for window, input, and rendering. You can create your own by inheriting from `Subsystem`.

```cpp
class NG_API Subsystem
{
public:
    virtual void OnInit() {}
    virtual void OnShutdown() {}
    virtual void Tick(float deltaTime) {}
    virtual void OnEvent(Event& event) {}
};
```

Access the manager from your application:

```cpp
GetSubsystemManager().Register<MySubsystem>();
GetSubsystemManager().Get<MySubsystem>();
```

The engine automatically calls `OnInit`, `Tick`, `OnEvent`, and `OnShutdown` for all registered subsystems.

---

## Input

`InputSubsystem` tracks keyboard and mouse state, updated from GLFW events.

```cpp
auto* input = GetSubsystemManager().Get<Ngin::InputSubsystem>();

// Keyboard
if (input->IsKeyPressed(Ngin::Key::W)) { ... }

// Mouse buttons
if (input->IsMouseButtonPressed(Ngin::Mouse::Button0)) { ... }

// Mouse position (pixels, top-left origin)
float x = input->GetMouseX();
float y = input->GetMouseY();
auto [x, y] = input->GetMousePos();
```

Key codes are in `Ngin/KeyCodes.h`, mouse codes in `Ngin/MouseCodes.h`.

---

## Renderer

The `RendererSubsystem` handles OpenGL initialization, depth testing, and frame clearing. It also initializes `Renderer` and `Renderer2D` — you do not need to call `Init()`/`Shutdown()` manually.

### Camera

Both renderers accept any camera through the common `Camera` base class:

```cpp
class Camera {
public:
    virtual const float* GetViewProjectionMatrix() const = 0;
};
```

Built-in cameras:

```cpp
// 2D: left/right/bottom/top bounds
Ngin::OrthographicCamera cam2D(-1.6f, 1.6f, -0.9f, 0.9f);
cam2D.SetPosition(0.0f, 0.0f);
cam2D.SetRotation(0.0f); // radians

// 3D: fovY (radians), aspect ratio, near, far
Ngin::PerspectiveCamera cam3D(0.785398f, 1280.0f / 720.0f, 0.1f, 100.0f);
cam3D.SetPosition(0.0f, 0.0f, 3.0f);
cam3D.SetRotation(0.0f, 0.0f); // pitch, yaw in radians
```

Both inherit from `Camera` and can be passed to either renderer.

---

### 3D Renderer

Immediate-mode 3D renderer. Each `Submit` call issues one draw call.

```cpp
void Tick(float deltaTime) override
{
    Ngin::Renderer::BeginScene(m_Camera);

    Ngin::Mat4 model = Ngin::Mat4::RotateY(m_Rotation);
    Ngin::Renderer::Submit(m_VertexArray.get(), m_Shader.get(), model.data);

    Ngin::Renderer::EndScene();
}
```

#### Setting up geometry

```cpp
float vertices[] = {
    // x      y      z       r     g     b     a
    -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 1.0f,
};

uint32_t indices[] = { 0, 1, 2,  2, 3, 0 };

auto va = std::unique_ptr<Ngin::VertexArray>(Ngin::VertexArray::Create());

auto vb = std::shared_ptr<Ngin::VertexBuffer>(
    Ngin::VertexBuffer::Create(vertices, sizeof(vertices))
);
vb->SetLayout({
    { Ngin::ShaderDataType::Float3, "a_Position" },
    { Ngin::ShaderDataType::Float4, "a_Color"    }
});
va->AddVertexBuffer(vb);

auto ib = std::shared_ptr<Ngin::IndexBuffer>(
    Ngin::IndexBuffer::Create(indices, 6)
);
va->SetIndexBuffer(ib);
```

#### Shader

The vertex shader receives `u_ViewProjection` and `u_Model` as `mat4` uniforms:

```glsl
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec4 v_Color;

void main()
{
    v_Color = a_Color;
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}
```

```cpp
auto shader = std::unique_ptr<Ngin::Shader>(
    Ngin::Shader::Create(vertSrc, fragSrc)
);
```

---

### 2D Renderer

Batched quad renderer. All quads in a `BeginScene`/`EndScene` block are flushed in a single draw call.

```cpp
void Tick(float deltaTime) override
{
    Ngin::Renderer2D::BeginScene(m_Camera2D);

    // x, y, width, height, r, g, b, a(optional)
    Ngin::Renderer2D::DrawQuad(0.0f, 0.0f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f);
    Ngin::Renderer2D::DrawQuad(0.6f, 0.0f, 0.3f, 0.3f,  0.0f, 0.5f, 1.0f, 0.8f);

    Ngin::Renderer2D::EndScene();
}
```

Max 10 000 quads per batch.

---

## Math

`Mat4` is a column-major 4x4 matrix (OpenGL native format). `data[col*4 + row]`.

```cpp
Ngin::Mat4 m = Ngin::Mat4::Identity();

// Transforms
Ngin::Mat4::Translate(x, y, z)
Ngin::Mat4::RotateX(radians)
Ngin::Mat4::RotateY(radians)
Ngin::Mat4::RotateZ(radians)

// Projections
Ngin::Mat4::Ortho(left, right, bottom, top)
Ngin::Mat4::Perspective(fovY, aspect, nearZ, farZ)

// Combine with operator*
Ngin::Mat4 model = Ngin::Mat4::RotateY(yaw) * Ngin::Mat4::Translate(x, y, z);

// Pass to renderer
Ngin::Renderer::Submit(va, shader, model.data);
```

---

## Full example

```cpp
#include <Ngin.h>
#include <memory>
#include <cmath>

static const char* s_VertSrc = R"(
    #version 330 core
    layout(location = 0) in vec3 a_Position;
    layout(location = 1) in vec4 a_Color;
    uniform mat4 u_ViewProjection;
    uniform mat4 u_Model;
    out vec4 v_Color;
    void main() {
        v_Color = a_Color;
        gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
    }
)";

static const char* s_FragSrc = R"(
    #version 330 core
    in vec4 v_Color;
    out vec4 color;
    void main() { color = v_Color; }
)";

class MyApp : public Ngin::Application
{
public:
    MyApp()
    {
        m_Camera3D = std::make_unique<Ngin::PerspectiveCamera>(
            0.785398f, 1280.0f / 720.0f, 0.1f, 100.0f
        );
        m_Camera3D->SetPosition(0.0f, 0.0f, 3.0f);

        m_Camera2D = std::make_unique<Ngin::OrthographicCamera>(-1.6f, 1.6f, -0.9f, 0.9f);

        float verts[] = {
            -0.5f, -0.5f, 0.0f,  1.0f, 0.3f, 0.3f, 1.0f,
             0.5f, -0.5f, 0.0f,  0.3f, 1.0f, 0.3f, 1.0f,
             0.5f,  0.5f, 0.0f,  0.3f, 0.3f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.3f, 1.0f,
        };
        uint32_t idx[] = { 0, 1, 2,  2, 3, 0 };

        m_VA.reset(Ngin::VertexArray::Create());
        auto vb = std::shared_ptr<Ngin::VertexBuffer>(
            Ngin::VertexBuffer::Create(verts, sizeof(verts))
        );
        vb->SetLayout({
            { Ngin::ShaderDataType::Float3, "a_Position" },
            { Ngin::ShaderDataType::Float4, "a_Color"    }
        });
        m_VA->AddVertexBuffer(vb);
        m_VA->SetIndexBuffer(std::shared_ptr<Ngin::IndexBuffer>(
            Ngin::IndexBuffer::Create(idx, 6)
        ));
        m_Shader.reset(Ngin::Shader::Create(s_VertSrc, s_FragSrc));
    }

    void Tick(float deltaTime) override
    {
        m_Rotation += deltaTime;

        // 3D
        Ngin::Renderer::BeginScene(*m_Camera3D);
        Ngin::Mat4 model = Ngin::Mat4::RotateY(m_Rotation);
        Ngin::Renderer::Submit(m_VA.get(), m_Shader.get(), model.data);
        Ngin::Renderer::EndScene();

        // 2D
        Ngin::Renderer2D::BeginScene(*m_Camera2D);
        Ngin::Renderer2D::DrawQuad(-1.3f, 0.7f, 0.2f, 0.2f,  0.8f, 0.2f, 0.2f);
        Ngin::Renderer2D::EndScene();
    }

private:
    std::unique_ptr<Ngin::PerspectiveCamera>  m_Camera3D;
    std::unique_ptr<Ngin::OrthographicCamera> m_Camera2D;
    std::unique_ptr<Ngin::VertexArray>        m_VA;
    std::unique_ptr<Ngin::Shader>             m_Shader;
    float m_Rotation = 0.0f;
};

Ngin::Application* Ngin::CreateApplication()
{
    return new MyApp();
}
```
