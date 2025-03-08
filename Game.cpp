#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStructs.h"
#include "SimpleShader.h"
#include "Material.h"


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
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		
		//Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame


		/*Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);*/
		

		//Graphics::Device->CreateBuffer(&cbDesc, 0, cBuffer.GetAddressOf());
		//Graphics::Context->VSSetConstantBuffers(0,1,cBuffer.GetAddressOf()); 
	}

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
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
//void Game::LoadShaders()
//{
//	// BLOBs (or Binary Large OBjects) for reading raw data from external files
//	// - This is a simplified way of handling big chunks of external data
//	// - Literally just a big array of bytes read from a file
//	ID3DBlob* pixelShaderBlob;
//	ID3DBlob* vertexShaderBlob;
//
//	// Loading shaders
//	//  - Visual Studio will compile our shaders at build time
//	//  - They are saved as .cso (Compiled Shader Object) files
//	//  - We need to load them when the application starts
//	{
//		// Read our compiled shader code files into blobs
//		// - Essentially just "open the file and plop its contents here"
//		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
//		// - Note the "L" before the string - this tells the compiler the string uses wide characters
//		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
//		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);
//
//		// Create the actual Direct3D shaders on the GPU
//		Graphics::Device->CreatePixelShader(
//			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
//			pixelShaderBlob->GetBufferSize(),		// How big is that data?
//			0,										// No classes in this shader
//			);			// Address of the ID3D11PixelShader pointer
//
//		Graphics::Device->CreateVertexShader(
//			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
//			vertexShaderBlob->GetBufferSize(),		// How big is that data?
//			0,										// No classes in this shader
//			);			// The address of the ID3D11VertexShader pointer
//	}
//
//	// Create an input layout 
//	//  - This describes the layout of data sent to a vertex shader
//	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
//	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
//	//  - Luckily, we already have that loaded (the vertex shader blob above)
//	{
//		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};
//
//		// Set up the first element - a position, which is 3 float values
//		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
//		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
//		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element
//
//		// Set up the second element - a color, which is 4 more float values
//		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
//		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
//		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element
//
//		// Create the input layout, verifying our description against actual shader code
//		Graphics::Device->CreateInputLayout(
//			inputElements,							// An array of descriptions
//			2,										// How many elements in that array?
//			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
//			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
//			);			// Address of the resulting ID3D11InputLayout pointer
//	}
//}


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

	std::shared_ptr<Material> mat1 = std::make_shared<Material>(XMFLOAT4(1, 0, 0, 0),vs,ps);
	std::shared_ptr<Material> mat2 = std::make_shared<Material>(XMFLOAT4(0, 1, 0, 0),vs,uvPS);
	std::shared_ptr<Material> mat3 = std::make_shared<Material>(XMFLOAT4(0, 0, 1, 0),vs,normalPS);
	std::shared_ptr<Material> mat4 = std::make_shared<Material>(XMFLOAT4(0, 0, 1, 0),vs,customPS);


	std::shared_ptr<GameEntity> e1 = std::make_shared<GameEntity>(cube,mat1);
	std::shared_ptr<GameEntity> e2 = std::make_shared<GameEntity>(cylinder,mat1);
	std::shared_ptr<GameEntity> e3 = std::make_shared<GameEntity>(helix,mat2);
	std::shared_ptr<GameEntity> e4 = std::make_shared<GameEntity>(quad,mat2);
	std::shared_ptr<GameEntity> e5 = std::make_shared<GameEntity>(quad_2F,mat3);
	std::shared_ptr<GameEntity> e6 = std::make_shared<GameEntity>(torus, mat3);
	std::shared_ptr<GameEntity> e7 = std::make_shared<GameEntity>(cube, mat4);
	std::shared_ptr<GameEntity> e8 = std::make_shared<GameEntity>(cylinder, mat4);


	entities.push_back(e1);
	entities.push_back(e2);
	entities.push_back(e3);
	entities.push_back(e4);
	entities.push_back(e5);
	entities.push_back(e6);
	entities.push_back(e7);
	entities.push_back(e8);
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
		}
		float sinWave = (float)sin(totalTime);
		// Move entities
		entities[0].get()->GetTransform()->SetPosition(5, 0, 0);
		entities[1].get()->GetTransform()->SetPosition(0,0, 0);
		entities[2].get()->GetTransform()->SetPosition(-5,0,0);
		entities[3].get()->GetTransform()->SetPosition(-10, 0, 0);
		entities[4].get()->GetTransform()->SetPosition(10, 0, 0);
		entities[5].get()->GetTransform()->SetPosition(15, 0, 0);
		entities[6].get()->GetTransform()->SetPosition(20, 0, 0);
		entities[7].get()->GetTransform()->SetPosition(25, 0, 0);
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
	ImGui::End();
	
}


