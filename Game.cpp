#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStructs.h"
#include "SimpleShader.h"
#include "Material.h"
#include "WICTextureLoader.h"


#include <DirectXMath.h>
// This code assumes files are in "ImGui" subfolder!
// Adjust as necessary for your own folder structure and project setup
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// Anonymous namespace for private variables
namespace {
	float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	bool showDemoWindow = false;
	bool showTriangle = true;
	XMFLOAT4 colorTint = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	int currentCamIndex = 0;
	std::string windowName = "Debug Inspector";
	XMFLOAT3 ambientColor = XMFLOAT3(0.2f, 0.2f, 0.2f);
}

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	//LoadShaders();
	CreateGeometry();

	// Set initial graphics API state
	Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	directionalLight = {};
	directionalLight.Type = LIGHT_DIRECTIONAL_TYPE;
	directionalLight.Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	directionalLight.Color = XMFLOAT3(0.0f, 1.0f, 0.0f);
	directionalLight.Intensity = 1.0f;

	dirLight2 = {};
	dirLight2.Type = LIGHT_DIRECTIONAL_TYPE;
	dirLight2.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	dirLight2.Color = XMFLOAT3(1.0f, 1.0f, 0.0f);
	dirLight2.Intensity = 1.0f;

	dirLight3 = {};
	dirLight3.Type = LIGHT_DIRECTIONAL_TYPE;
	dirLight3.Direction = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	dirLight3.Color = XMFLOAT3(0.0f, 1.0f, 1.0f);
	dirLight3.Intensity = 1.0f;

	spotLight = {};
	spotLight.Type = LIGHT_TYPE_SPOT;
	spotLight.Position = XMFLOAT3(10.0f, 1.5f, 0.0f);
	spotLight.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	spotLight.Color = XMFLOAT3(1.0f, 0.0f, 0.0f);
	spotLight.Intensity = 1.0f;
	spotLight.SpotInnerAngle = XMConvertToRadians(20.0f);
	spotLight.SpotOuterAngle = XMConvertToRadians(40.0f);
	spotLight.Range = 15.0f;

	pointLight = {};
	pointLight.Type = LIGHT_TYPE_POINT;
	pointLight.Position = XMFLOAT3(0.0f, 0.0f, -2.0f);
	pointLight.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	pointLight.Intensity = 1.0f;
	pointLight.Range = 10.0f;

	

	lights.push_back(directionalLight);
	lights.push_back(dirLight2);
	lights.push_back(dirLight3);
	lights.push_back(spotLight);
	lights.push_back(pointLight);

	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	ImGui::StyleColorsClassic();

	mainCam = std::make_shared<Camera>(
		XMFLOAT3(-1.0f,0.0f,-2.0f),		// Init position
		Window::AspectRatio(),	// Aspect ratio
		(3.1415962f/4.0f),		// FOV
		0.01f,					// Near clip
		1000.0f,				// Far clip
		10.0f,						// Move speed
		0.001f);						// Look speed
	secondCam = std::make_shared<Camera>(
		XMFLOAT3(1.0f, 0.0f, -4.0f),		// Init position
		Window::AspectRatio(),	// Aspect ratio
		(3.1415962f / 2.0f),		// FOV
		0.01f,					// Near clip
		1000.0f,				// Far clip
		15.0f,						// Move speed
		0.001f);				// Look speed
	cameras.push_back(mainCam);
	cameras.push_back(secondCam);
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Initialize mesh objects
	cube = std::make_shared<Mesh>(FixPath("../../Assets/cube.obj").c_str());
	meshes.push_back(cube);
	// Square
	cylinder = std::make_shared<Mesh>(FixPath("../../Assets/cylinder.obj").c_str());
	meshes.push_back(cylinder);
	// Octagon
	helix = std::make_shared<Mesh>(FixPath("../../Assets/helix.obj").c_str());
	meshes.push_back(helix);

	quad = std::make_shared<Mesh>(FixPath("../../Assets/quad.obj").c_str());
	meshes.push_back(quad);

	quad_2F = std::make_shared<Mesh>(FixPath("../../Assets/quad_double_sided.obj").c_str());
	meshes.push_back(quad_2F);

	torus = std::make_shared<Mesh>(FixPath("../../Assets/torus.obj").c_str());
	meshes.push_back(torus);

	sphere = std::make_shared<Mesh>(FixPath("../../Assets/sphere.obj").c_str());
	meshes.push_back(sphere);

	std::shared_ptr<SimpleVertexShader> vs = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> ps = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> uvPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"DebugUVsPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> normalPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"DebugNormalsPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> customPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"CustomPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> doubleTexPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"TwoTexturePS.cso").c_str());

	// Sky related shaders
	std::shared_ptr<SimpleVertexShader> skyVS = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"SkyVertexShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> skyPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"SkyPixelShader.cso").c_str());

	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> graniteSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> fabricSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobbleSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobbleNormalSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rockSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rockNormalSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cushionSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cushionNormalSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> flatNormalSRV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	//CreateWICTextureFromFile(Graphics::Device.Get(),Graphics::Context.Get(),FixPath(L"../../Assets/granite_tile_diff_1k.png").c_str(), 0, graniteSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(),Graphics::Context.Get(),FixPath(L"../../Assets/fabric_pattern_07_col_1_1k.png").c_str(), 0, fabricSRV.GetAddressOf());

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/cobblestone.png").c_str(), 0, cobbleSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/cobblestone_normals.png").c_str(), 0, cobbleNormalSRV.GetAddressOf());

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/cushion.png").c_str(), 0, cushionSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/cushion_normals.png").c_str(), 0, cushionNormalSRV.GetAddressOf());

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/rock.png").c_str(), 0, rockSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/rock_normals.png").c_str(), 0, rockNormalSRV.GetAddressOf());

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/flat_normals.png").c_str(), 0, flatNormalSRV.GetAddressOf());

	D3D11_SAMPLER_DESC sampleDesc = {};
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampleDesc.MaxAnisotropy = 10;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT res = Graphics::Device->CreateSamplerState(&sampleDesc, &samplerState);

	std::shared_ptr<Material> mat1 = std::make_shared<Material>(XMFLOAT4(0.936f, 0.702f, 0.799f, 1.000f),vs,ps,XMFLOAT2(1,1),XMFLOAT2(0,0),0.0f);
	mat1->AddSampler("BasicSampler", samplerState);
	mat1->AddTextureSRV("SurfaceTexture",rockSRV);
	mat1->AddTextureSRV("NormalMap", rockNormalSRV);
	std::shared_ptr<Material> mat2 = std::make_shared<Material>(XMFLOAT4(0.936f, 0.702f, 0.799f, 1.000f),vs,ps,XMFLOAT2(1,1),XMFLOAT2(0,0),-0.2f);
	mat2->AddSampler("BasicSampler", samplerState);
	mat2->AddTextureSRV("SurfaceTexture", cushionSRV);
	mat2->AddTextureSRV("NormalMap", cushionNormalSRV);
	std::shared_ptr<Material> mat3 = std::make_shared<Material>(XMFLOAT4(0.936f, 0.702f, 0.799f, 1.000f),vs,ps,XMFLOAT2(1,1),XMFLOAT2(0,0),-0.4f);
	mat3->AddSampler("BasicSampler", samplerState);
	mat3->AddTextureSRV("SurfaceTexture", fabricSRV);
	mat3->AddTextureSRV("NormalMap", flatNormalSRV);
	std::shared_ptr<Material> mat4 = std::make_shared<Material>(XMFLOAT4(0.936f, 0.702f, 0.799f, 1.000f),vs,ps,XMFLOAT2(1,1),XMFLOAT2(0,0),-0.6f);
	mat4->AddSampler("BasicSampler", samplerState);
	mat4->AddTextureSRV("SurfaceTexture", cobbleSRV);
	mat4->AddTextureSRV("NormalMap", cobbleNormalSRV);
	std::shared_ptr<Material> mat5 = std::make_shared<Material>(XMFLOAT4(0.936f, 0.702f, 0.799f, 1.000f),vs,ps,XMFLOAT2(5,5),XMFLOAT2(0,0),-0.8f);
	mat5->AddSampler("BasicSampler", samplerState);
	mat5->AddTextureSRV("SurfaceTexture", cushionSRV);
	mat5->AddTextureSRV("NormalMap", cushionNormalSRV);
	//std::shared_ptr<Material> mat6 = std::make_shared<Material>(XMFLOAT4(1, 1, 1, 0),vs,doubleTexPS,XMFLOAT2(1,1),XMFLOAT2(0,0),-1.0f);
	//mat6->AddSampler("BasicSampler", samplerState);
	//mat6->AddTextureSRV("SurfaceTexture", fabricSRV);
	//mat6->AddTextureSRV("SecondTexture",graniteSRV);


	std::shared_ptr<GameEntity> e1 = std::make_shared<GameEntity>(cube,mat1);
	std::shared_ptr<GameEntity> e2 = std::make_shared<GameEntity>(sphere,mat2);
	std::shared_ptr<GameEntity> e3 = std::make_shared<GameEntity>(helix,mat3);
	//std::shared_ptr<GameEntity> e4 = std::make_shared<GameEntity>(quad,mat2);
	//std::shared_ptr<GameEntity> e5 = std::make_shared<GameEntity>(quad_2F,mat3);
	std::shared_ptr<GameEntity> e6 = std::make_shared<GameEntity>(torus, mat4);
	std::shared_ptr<GameEntity> e7 = std::make_shared<GameEntity>(sphere, mat5);
	std::shared_ptr<GameEntity> e8 = std::make_shared<GameEntity>(cylinder, mat5);


	entities.push_back(e1);
	entities.push_back(e2);
	entities.push_back(e3);
	//entities.push_back(e4);
	//entities.push_back(e5);
	entities.push_back(e6);
	entities.push_back(e7);
	entities.push_back(e8);


	sky = std::make_shared<Sky>(
		FixPath(L"../../Assets/right.png").c_str(),
		FixPath(L"../../Assets/left.png").c_str(),
		FixPath(L"../../Assets/up.png").c_str(),
		FixPath(L"../../Assets/down.png").c_str(),
		FixPath(L"../../Assets/front.png").c_str(),
		FixPath(L"../../Assets/back.png").c_str(),
		cube,
		samplerState,
		skyPS,
		skyVS);
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	for (int i = 0; i < cameras.size(); i++)
	{
		cameras[i]->UpdateProjectionMatrix(Window::AspectRatio());
	}
}



// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
	ResetUI(deltaTime);
	cameras[currentCamIndex]->Update(deltaTime);
	BuildUI();
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	if (showTriangle)
	{
		for (int i=0;i<entities.size(); i++) {
			entities[i]->Draw(cameras[currentCamIndex]);
			entities[i]->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambientColor);
			entities[i]->GetMaterial()->GetPixelShader()->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
		}
		float sinWave = (float)sin(totalTime);
		// Move entities
		entities[0].get()->GetTransform()->SetPosition(5, 0, 0);
		entities[1].get()->GetTransform()->SetPosition(0,0, 0);
		entities[2].get()->GetTransform()->SetPosition(-5,0,0);
		entities[3].get()->GetTransform()->SetPosition(-10, 0, 0);
		entities[4].get()->GetTransform()->SetPosition(10, 0, 0);
		entities[5].get()->GetTransform()->SetPosition(15, 0, 0);
		//entities[6].get()->GetTransform()->SetPosition(20, 0, 0);
		//entities[7].get()->GetTransform()->SetPosition(25, 0, 0);

		sky->Draw(cameras[currentCamIndex]);
	}

	ImGui::Render(); // Turns this frame’s UI into renderable triangles
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen
	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}

void Game::ResetUI(float deltaTime) {
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);
	// Show the demo window
	//ImGui::ShowDemoWindow();
}

void Game::BuildUI() {
	ImGui::Begin(windowName.c_str());
	ImGui::Text("Framerate: %f fps", ImGui::GetIO().Framerate);
	ImGui::Text("Window Resolution: %dx%d", Window::Width(), Window::Height());
	ImGui::ColorEdit4("Background Color", &color[0]);
	if (ImGui::Button("Show Demo Window")) {
		showDemoWindow = !showDemoWindow;
	}
	if (showDemoWindow) {
		ImGui::ShowDemoWindow();
	}
	ImGui::Text("Select Camera: ");
	ImGui::SameLine();
	static int active = 0;
	if (ImGui::RadioButton("Camera 1", &active, 0)) {
		currentCamIndex = 0;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Camera 2", &active, 1)) {
		currentCamIndex = 1;
	}
	if (ImGui::TreeNode("Active Camera"))
	{
		if (ImGui::TreeNode("Controls"))
		{
			ImGui::Text("W: Forward");
			ImGui::Text("A: Left");
			ImGui::Text("S: Back");
			ImGui::Text("D: Right");
			ImGui::Text("Shift: Speed Up");
			ImGui::Text("Control: Slow Down");
			ImGui::Text("Space: Up");
			ImGui::Text("X: Down");
			ImGui::Text("Hold Left Mouse Button: Look Around");
			ImGui::TreePop();
		}
		XMFLOAT3 pos = cameras[currentCamIndex]->GetPosition();
		ImGui::Text("Camera Position: X:%.2f Y:%.2f Z:%.2f",pos.x,pos.y,pos.z);
		ImGui::Text("Field of View: %.1f Degrees", cameras[currentCamIndex]->GetFov() * (180/3.141598623));
		ImGui::TreePop();
	}
	static bool checked = true;
	if (ImGui::Checkbox("Show Triangle",&checked)) {
		showTriangle = !showTriangle;
	}

	ImGui::ColorEdit4("Mesh Color Tint",&colorTint.x);
	// Entity UI
	if (ImGui::TreeNode("Entities"))
	{
		for (int i = 0; i < entities.size();i++) {
			ImGui::PushID(i);
			if (ImGui::TreeNode("","Entity %d ",i))
			{
				std::shared_ptr<Transform> eTransform = entities[i]->GetTransform();
				XMFLOAT3 pos = eTransform->GetPosition();
				XMFLOAT3 rot = eTransform->GetPitchYawRoll();
				XMFLOAT3 scale = eTransform->GetScale();
				
				
				if (ImGui::DragFloat3("Position", &pos.x,0.01f)) {
					eTransform->SetPosition(pos);
				}
				if (ImGui::DragFloat3("Rotation", &rot.x,0.01f)) {
					eTransform->SetRotation(rot);
				}
				if (ImGui::DragFloat3("Scale", &scale.x,0.01f)) {
					eTransform->SetScale(scale);
				}
				ImGui::Text("Mesh Indices: %d", entities[i]->GetMesh()->GetIndexCount());
				ImGui::PushID(i);
				if (ImGui::TreeNode("","Entity %d material",i)) {
					XMFLOAT2 uvScale = entities[i]->GetMaterial()->GetUVScale();
					XMFLOAT2 uvOffset = entities[i]->GetMaterial()->GetUVOffset();
					XMFLOAT4 colorTint = entities[i]->GetMaterial()->GetColorTint();
					std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureMap = entities[i]->GetMaterial()->GetTextureMap();

					if (ImGui::DragFloat2("UV Scale",&uvScale.x,0.01f)) {
						entities[i]->GetMaterial()->SetUVScale(uvScale);
					}
					if (ImGui::DragFloat2("UV Offset", &uvOffset.x, 0.01f)) {
						entities[i]->GetMaterial()->SetUVOffset(uvOffset);
					}
					if (ImGui::DragFloat4("Color Tint", &colorTint.x, 0.01f)) {
						entities[i]->GetMaterial()->SetColorTint(colorTint);
					}
					for (auto& t : textureMap) {
						ImGui::Image((ImTextureID)t.second.Get(),ImVec2(125,125));
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
	// Mesh UI
	if (ImGui::TreeNode("Meshes")) {
		for (int i = 0; i < meshes.size();i++) {
			ImGui::PushID(i);
			if (ImGui::TreeNode("", "Mesh %d ",i)) {
				ImGui::Text("Triangles: %d", meshes[i]->GetIndexCount() / 3);
				ImGui::Text("Vertices: %d", meshes[i]->GetVertexCount());
				ImGui::Text("Indices: %d", meshes[i]->GetIndexCount());
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Lights")) {
		for (int i = 0; i < lights.size(); i++)
		{
			ImGui::PushID(i);
			if (ImGui::TreeNode("", "Light %d",i))
			{
				ImGui::DragFloat3("color", & lights[i].Color.x, 0.01f, 0.0f, 1.0f);
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::DragFloat3("Ambient color", &ambientColor.x,0.01f,0.0f,1.0f);
		ImGui::TreePop();
	}
	ImGui::End();
	
}


