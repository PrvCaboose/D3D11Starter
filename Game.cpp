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
	int blurRadius = 0;
	int ppIsEnabled = 0; // change to 1 to disable custom post process
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
	directionalLight.Direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
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
	CustomPPPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"CustomPPPS.cso").c_str());

	// Sky related shaders
	std::shared_ptr<SimpleVertexShader> skyVS = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"SkyVertexShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> skyPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"SkyPixelShader.cso").c_str());

	// Shadow shader
	shadowVS = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"ShadowVertexShader.cso").c_str());

	ppPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PostProcess.cso").c_str());
	ppVS = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"FullScreenVS.cso").c_str());


	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> graniteSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> fabricSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobbleAlbedoSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobbleNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobbleMetalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobbleRoughnessSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorAlbedoSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorMetalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorRoughnessSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodAlbedoSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodMetalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodRoughnessSRV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/cobblestone_albedo.png").c_str(), 0, cobbleAlbedoSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/cobblestone_normals.png").c_str(), 0, cobbleNormalSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/cobblestone_metal.png").c_str(), 0, cobbleMetalSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/cobblestone_roughness.png").c_str(), 0, cobbleRoughnessSRV.GetAddressOf());

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/floor_albedo.png").c_str(), 0, floorAlbedoSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/floor_normals.png").c_str(), 0, floorNormalSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/floor_metal.png").c_str(), 0, floorMetalSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/floor_roughness.png").c_str(), 0, floorRoughnessSRV.GetAddressOf());

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/wood_albedo.png").c_str(), 0, woodAlbedoSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/wood_normals.png").c_str(), 0, woodNormalSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/wood_metal.png").c_str(), 0, woodMetalSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/wood_roughness.png").c_str(), 0, woodRoughnessSRV.GetAddressOf());

	D3D11_SAMPLER_DESC sampleDesc = {};
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampleDesc.MaxAnisotropy = 10;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT res = Graphics::Device->CreateSamplerState(&sampleDesc, &samplerState);

	// Shadows stuff
	// Create the actual texture that will be the shadow map
	
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = 1024; // Ideally a power of 2 (like 1024)
	shadowDesc.Height = 1024; // Ideally a power of 2 (like 1024)
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
	Graphics::Device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

	// Create the depth/stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	Graphics::Device->CreateDepthStencilView(
		shadowTexture.Get(),
		&shadowDSDesc,
		shadowDSV.GetAddressOf());

	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Graphics::Device->CreateShaderResourceView(
		shadowTexture.Get(),
		&srvDesc,
		shadowSRV.GetAddressOf());

	// Directional light projection matrix
	// Needs a orthogonal projection matrix
	DirectX::XMVECTOR directLightDir = DirectX::XMVECTOR{0, 30, 0, 0.0f};

	DirectX::XMMATRIX lightView = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(0, 30, -30, 0),
		DirectX::XMVectorSet(0, 0, 0, 0),
		DirectX::XMVectorSet(0, 1, 0, 0));
	DirectX::XMStoreFloat4x4(&lightViewMatrix, lightView);

	// Projection matrix
	float lightProjSize = 50.0f;
	DirectX::XMMATRIX shadowProjMat = DirectX::XMMatrixOrthographicLH(
		lightProjSize,
		lightProjSize,
		1.0f,
		100.0f);
	DirectX::XMStoreFloat4x4(&lightProjectionMatrix, shadowProjMat);

	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Min. precision units, not world units!
	shadowRastDesc.SlopeScaledDepthBias = 1.0f; // Bias more based on slope
	Graphics::Device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f; // Only need the first component
	Graphics::Device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	// Sampler state for post processing
	D3D11_SAMPLER_DESC ppSampDesc = {};
	ppSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ppSampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::Device->CreateSamplerState(&ppSampDesc, ppSampler.GetAddressOf());

	// Describe the texture we're creating
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = Window::Width();
	textureDesc.Height = Window::Height();
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	// Create the resource (no need to track it after the views are created below)
	Microsoft::WRL::ComPtr<ID3D11Texture2D> ppTexture;
	Graphics::Device->CreateTexture2D(&textureDesc, 0, ppTexture.GetAddressOf());

	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	Graphics::Device->CreateRenderTargetView(
		ppTexture.Get(),
		&rtvDesc,
		ppRTV.ReleaseAndGetAddressOf());
	// Create the Shader Resource View
	// By passing it a null description for the SRV, we
	// get a "default" SRV that has access to the entire resource
	Graphics::Device->CreateShaderResourceView(
		ppTexture.Get(),
		0,
		ppSRV.ReleaseAndGetAddressOf());
	

	std::shared_ptr<Material> mat1 = std::make_shared<Material>(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.000f),vs,ps,XMFLOAT2(1,1),XMFLOAT2(0,0),0.0f);
	mat1->AddSampler("BasicSampler", samplerState);
	mat1->AddTextureSRV("Albedo", cobbleAlbedoSRV);
	mat1->AddTextureSRV("NormalMap", cobbleNormalSRV);
	mat1->AddTextureSRV("RoughnessMap", cobbleRoughnessSRV);
	mat1->AddTextureSRV("MetalnessMap", cobbleMetalSRV);
	std::shared_ptr<Material> mat2 = std::make_shared<Material>(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.000f),vs,ps,XMFLOAT2(1,1),XMFLOAT2(0,0),-0.2f);
	mat2->AddSampler("BasicSampler", samplerState);
	mat2->AddTextureSRV("Albedo", floorAlbedoSRV);
	mat2->AddTextureSRV("NormalMap", floorNormalSRV);
	mat2->AddTextureSRV("RoughnessMap", floorRoughnessSRV);
	mat2->AddTextureSRV("MetalnessMap", floorMetalSRV);
	std::shared_ptr<Material> mat3 = std::make_shared<Material>(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.000f),vs,ps,XMFLOAT2(1,1),XMFLOAT2(0,0),-0.4f);
	mat3->AddSampler("BasicSampler", samplerState);
	mat3->AddTextureSRV("Albedo", woodAlbedoSRV);
	mat3->AddTextureSRV("NormalMap", woodNormalSRV);
	mat3->AddTextureSRV("RoughnessMap", woodRoughnessSRV);
	mat3->AddTextureSRV("MetalnessMap", woodMetalSRV);

	std::shared_ptr<GameEntity> e1 = std::make_shared<GameEntity>(cube,mat1);
	std::shared_ptr<GameEntity> e2 = std::make_shared<GameEntity>(sphere,mat2);
	std::shared_ptr<GameEntity> e3 = std::make_shared<GameEntity>(helix,mat3);
	//std::shared_ptr<GameEntity> e4 = std::make_shared<GameEntity>(quad,mat2);
	//std::shared_ptr<GameEntity> e5 = std::make_shared<GameEntity>(quad_2F,mat3);
	std::shared_ptr<GameEntity> e6 = std::make_shared<GameEntity>(torus, mat1);
	std::shared_ptr<GameEntity> e7 = std::make_shared<GameEntity>(sphere, mat2);
	std::shared_ptr<GameEntity> e8 = std::make_shared<GameEntity>(cylinder, mat3);


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

	// Move entities
	entities[0].get()->GetTransform()->SetPosition(5, 0, 0);
	entities[1].get()->GetTransform()->SetPosition(0, 0, 0);
	entities[2].get()->GetTransform()->SetPosition(-5, 0, 0);
	entities[3].get()->GetTransform()->SetPosition(-10, 0, 0);
	entities[4].get()->GetTransform()->SetPosition(10, 0, 0);
	entities[5].get()->GetTransform()->SetPosition(0, -5, 0);
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
	
	// Shadow stuff needs to happen BEFORE the frame starts to render
	Graphics::Context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	ID3D11RenderTargetView* nullRTV{};
	Graphics::Context->OMSetRenderTargets(1, &nullRTV, shadowDSV.Get());
	Graphics::Context->RSSetState(shadowRasterizer.Get());


	D3D11_VIEWPORT viewport = {};
	viewport.Width = 1024.0f;
	viewport.Height = 1024.0f;
	viewport.MaxDepth = 1.0f;
	Graphics::Context->RSSetViewports(1, &viewport);

	shadowVS->SetShader();
	shadowVS->SetMatrix4x4("view", lightViewMatrix);
	shadowVS->SetMatrix4x4("projection", lightProjectionMatrix);
	Graphics::Context->PSSetShader(0, 0, 0);
	// Loop and draw all entities
	for (auto& e : entities)
	{
		shadowVS->SetMatrix4x4("world", e->GetTransform()->GetWorldMatrix());
		shadowVS->CopyAllBufferData();
		// Draw the mesh directly to avoid the entity's material
		e->GetMesh()->Draw();
	}

	viewport.Width = (float)Window::Width();
	viewport.Height = (float)Window::Height();
	Graphics::Context->RSSetViewports(1, &viewport);
	Graphics::Context->OMSetRenderTargets(
		1,
		Graphics::BackBufferRTV.GetAddressOf(),
		Graphics::DepthBufferDSV.Get());
	Graphics::Context->RSSetState(0);

	// Post process
	const float num[4] = { 0.0f,0.0f,0.0f,1.0f };
	Graphics::Context->ClearRenderTargetView(ppRTV.Get(), num);
	Graphics::Context->OMSetRenderTargets(1, ppRTV.GetAddressOf(), Graphics::DepthBufferDSV.Get());

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	if (showTriangle)
	{
		for (int i=0;i<entities.size(); i++) {

			entities[i]->GetMaterial()->GetVertexShader()->SetMatrix4x4("lightView", lightViewMatrix);
			entities[i]->GetMaterial()->GetVertexShader()->SetMatrix4x4("lightProjection", lightProjectionMatrix);

			entities[i]->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambientColor);

			entities[i]->GetMaterial()->GetPixelShader()->SetShaderResourceView("ShadowMap", shadowSRV);
			entities[i]->GetMaterial()->GetPixelShader()->SetSamplerState("ShadowSampler", shadowSampler);

			entities[i]->GetMaterial()->GetPixelShader()->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());

			entities[i]->Draw(cameras[currentCamIndex]);
		}
		float sinWave = (float)sin(totalTime);


		entities[5].get()->GetTransform()->SetScale(50, 1, 50);

		entities[2].get()->GetTransform()->Rotate(deltaTime, -deltaTime, 0);

		sky->Draw(cameras[currentCamIndex]);
	}

	// Back to the screen (no depth buffer necessary at this point)
	Graphics::Context->OMSetRenderTargets(1, Graphics::BackBufferRTV.GetAddressOf(), 0);

	// Activate shaders and bind resources
	// Also set any required cbuffer data (not shown)
	ppVS->SetShader();
	ppPS->SetShader();
	ppPS->SetShaderResourceView("Pixels", ppSRV);
	ppPS->SetSamplerState("ClampSampler", ppSampler);

	ppPS->SetInt("blurRadius",blurRadius);
	ppPS->SetFloat("pixelWidth", 1.0f / (float)Window::Width());
	ppPS->SetFloat("pixelHeight", 1.0f / (float)Window::Height());
	ppPS->CopyAllBufferData();




	// Use ppSRV to get pixels of screen
	// use that as input to another customPS, and set that shader
	CustomPPPS->SetShader();
	CustomPPPS->SetShaderResourceView("Pixels", ppSRV);
	CustomPPPS->SetSamplerState("BasicSampler", ppSampler);


	XMFLOAT2 camPos = XMFLOAT2(cameras[currentCamIndex]->GetPosition().x, cameras[currentCamIndex]->GetPosition().y);
	
	CustomPPPS->SetFloat2("focusPoint", camPos);
	CustomPPPS->SetInt("enabled",ppIsEnabled);
	CustomPPPS->CopyAllBufferData();

	Graphics::Context->Draw(3, 0); // Draw exactly 3 vertices (one triangle)

	ID3D11ShaderResourceView* nullSRVs[128] = {};
	Graphics::Context->PSSetShaderResources(0, 128, nullSRVs);


	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		ImGui::Render(); // Turns this frame’s UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen
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
	if (ImGui::TreeNode("Shadow Map")) {
		ImGui::Image((ImTextureID)shadowSRV.Get(), ImVec2(256, 256));
	}
	if (ImGui::TreeNode("Post Process")) {
		if (ImGui::TreeNode("Blur")) {
			ImGui::DragInt("Blur Radius",&blurRadius,0.5,0,50);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Chromatic Abberation")) {
			ImGui::Checkbox("Enabled",(bool*)& ppIsEnabled);
			ImGui::Text("%d",ppIsEnabled);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::End();
	
}


