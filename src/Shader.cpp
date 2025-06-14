#include "Shader.h"
#include "../Shader/Shader.h"

namespace Mus {
	namespace Shader {
		bool ShaderManager::IsFailedShader(std::string shaderType)
		{
			return Failed.find(lowLetter(shaderType)) != Failed.end();
		}
		bool ShaderManager::SaveCompiledShaderFile(Microsoft::WRL::ComPtr<ID3DBlob>& compiledShader, const std::wstring& shaderFile)
		{
			if (!compiledShader || shaderFile.empty())
				return false;
			std::ofstream file(shaderFile, std::ios::binary);
			if (!file.is_open()) {
				logger::error("Failed to save the compiled shader : {}", wstring2string(shaderFile));
				return false;
			}
			file.clear();
			file.write(static_cast<const char*>(compiledShader->GetBufferPointer()), compiledShader->GetBufferSize());
			file.close();
			logger::info("Save the compiled shader : {}", wstring2string(shaderFile));
			return true;
		}
		bool ShaderManager::LoadComputeShader(const char* shaderCode, ID3D11ComputeShader** computeShader, Microsoft::WRL::ComPtr<ID3DBlob>& shaderData)
		{
			if (!GetDevice() || !shaderCode || std::strlen(shaderCode) == 0)
				return false;
			Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
			HRESULT hr = D3DCompile(shaderCode, std::strlen(shaderCode), nullptr, nullptr, nullptr, "main", "cs_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, shaderData.ReleaseAndGetAddressOf(), &errorBlob);
			if (FAILED(hr)) {
				logger::error("Failed to compile shader {} : {}", hr, errorBlob ? errorBlob->GetBufferPointer() : "Unknown Error");
				return false;
			}
			hr = GetDevice()->CreateComputeShader(shaderData->GetBufferPointer(), shaderData->GetBufferSize(), nullptr, computeShader);
			if (FAILED(hr)) {
				logger::error("Failed to compile shader {}", hr);
				return false;
			}
			return true;
		}
		bool ShaderManager::LoadComputeShaderFile(const std::wstring& shaderFile, ID3D11ComputeShader** computeShader, Microsoft::WRL::ComPtr<ID3DBlob>& shaderData)
		{
			if (!GetDevice() || shaderFile.empty())
				return false;
			Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
			HRESULT hr = D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "main", "cs_5_0", 0, 0, shaderData.ReleaseAndGetAddressOf(), nullptr);
			if (FAILED(hr))
			{
				logger::error("Failed to compile shader {} : {}", hr, errorBlob ? errorBlob->GetBufferPointer() : "Unknown Error");
				return false;
			}
			hr = GetDevice()->CreateComputeShader(shaderData->GetBufferPointer(), shaderData->GetBufferSize(), nullptr, computeShader);
			if (FAILED(hr))
			{
				logger::error("Failed to create shader {}", hr);
				return false;
			}
			return true;
		}
		bool ShaderManager::LoadCompiledComputeShader(const std::wstring& shaderFile, ID3D11ComputeShader** computeShader)
		{
			if (!GetDevice() || shaderFile.empty())
				return false;
			std::ifstream file(shaderFile, std::ios::binary);
			if (!file.is_open()) {
				return false;
			}
			file.seekg(0, std::ios::end);
			size_t fileSize = static_cast<size_t>(file.tellg());
			file.seekg(0, std::ios::beg);
			std::vector<char> shaderData(fileSize);
			file.read(shaderData.data(), fileSize);
			file.close();
			HRESULT hr = GetDevice()->CreateComputeShader(shaderData.data(), fileSize, nullptr, computeShader);
			if (FAILED(hr)) {
				logger::error("Failed to create shader {}", hr);
				return false;
			}
			return true;
		}
		const char* ShaderManager::GetShaderString(std::string shaderType)
		{
			if (IsSameString(shaderType, "Morph"))
			{
				return Morph;
			}
			else
			{
				logger::warn("Invalid shader type {}!", shaderType);
			}
			return "";
		}
		std::wstring ShaderManager::GetShaderFilePath(std::string shaderType, bool compiled)
		{
			std::string file = "Data\\SKSE\\Plugins\\MuFacialExpressionExtended\\Shader\\";
			if (compiled)
				file += "Compiled\\";
			file += shaderType;
			std::string file_ = file;
			if (compiled)
				file_ += ".cso";
			else
				file_ += ".hlsl";
			if (!IsExistFileInStream(file_))
			{
				if (compiled)
				{
					file_ = file;
					if (compiled)
						file_ += ".cso";
					else
						file_ += ".hlsl";
					if (!IsExistFileInStream(file_))
					{
						logger::critical("Failed to get shader file");
						return L"";
					}
				}
			}
			return string2wstring(file_);
		}
		ShaderManager::ComputeShader ShaderManager::CreateComputeShader(std::string shaderType)
		{
			ShaderLockGuard lock;
			std::string shaderType_ = lowLetter(shaderType);
			if (auto found = Shaders.find(shaderType_); found != Shaders.end())
				return found->second;
			if (Failed.find(shaderType_) != Failed.end())
				return nullptr;
			ComputeShader newShader = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> csBlob = nullptr;
			if (LoadComputeShaderFile(GetShaderFilePath(shaderType, false), &newShader, csBlob))
			{
				Shaders[shaderType_] = newShader;
				logger::info("Compile shader done : {}", shaderType);
				SaveCompiledShaderFile(csBlob, GetShaderFilePath(shaderType, true));
				return newShader;
			}
			logger::warn("Failed to compile shader for {}. so re-compile default shader...", shaderType);
			if (LoadComputeShader(GetShaderString(shaderType), &newShader, csBlob))
			{
				Shaders[shaderType_] = newShader;
				logger::info("Compile shader done : {}", shaderType);
				SaveCompiledShaderFile(csBlob, GetShaderFilePath(shaderType, true));
				return newShader;
			}
			logger::warn("Failed to compile shader for {}. so try to load latest compiled shader...", shaderType);
			if (LoadCompiledComputeShader(GetShaderFilePath(shaderType, true), &newShader))
			{
				Shaders[shaderType_] = newShader;
				logger::info("Loaded latest compiled shader : {}", shaderType);
				return newShader;
			}
			logger::error("Failed to load shader ({}), so works into CPU mode...", shaderType);
			std::string FailedShaderCompileName = "$FailedShaderCompile";
			std::string FailedShaderCompile = "";
			SKSE::Translation::Translate(FailedShaderCompileName, FailedShaderCompile);
			if (FailedShaderCompile.length() == 0)
			{
				FailedShaderCompile = "MFEE - Warning!";
				FailedShaderCompile += "\n\nFailed to Compile shader!";
				FailedShaderCompile += "\nPlease update graphic driver";
				FailedShaderCompile += "\nOr set EnableGPUMode to false";
			}
			else
				FailedShaderCompile = ReplaceNewlineEscapes(FailedShaderCompile);
			RE::DebugMessageBox(FailedShaderCompile.c_str());
			Failed.emplace(shaderType);
			return nullptr;
		}
		ShaderManager::ComputeShader ShaderManager::GetShader(std::string shaderType)
		{
			shaderType = lowLetter(shaderType);
			if (auto found = Shaders.find(shaderType); found != Shaders.end())
				return found->second;
			return CreateComputeShader(shaderType);
		}
		ID3D11DeviceContext* ShaderManager::GetContext()
		{
			if (!context)
				context = reinterpret_cast<ID3D11DeviceContext*>(RE::BSGraphics::Renderer::GetSingleton()->GetRuntimeData().context);
			return context;
		}
		ID3D11Device* ShaderManager::GetDevice()
		{
			if (!device)
				GetContext()->GetDevice(&device);
			return device;
		}

		void BackupShader::Backup(ID3D11DeviceContext* context)
		{
			// backup current CS state
			context->CSGetShader(oldComputeShader.ReleaseAndGetAddressOf(), nullptr, nullptr);
			context->CSGetUnorderedAccessViews(0, 1, oldUnorderedAccessView.ReleaseAndGetAddressOf());
			context->CSGetShaderResources(0, 1, oldShaderResourceView.ReleaseAndGetAddressOf());
			context->CSGetConstantBuffers(0, 1, oldConstantBuffer.ReleaseAndGetAddressOf());
		}
		void BackupShader::Revert(ID3D11DeviceContext* context)
		{
			// revert cs state
			context->CSSetShader(oldComputeShader.Get(), nullptr, 0);
			context->CSSetUnorderedAccessViews(0, 1, oldUnorderedAccessView.GetAddressOf(), nullptr);
			context->CSSetShaderResources(0, 1, oldShaderResourceView.GetAddressOf());
			context->CSSetConstantBuffers(0, 1, oldConstantBuffer.GetAddressOf());
		}
	}

	bool MorphProcess::Initial(DirectX::XMFLOAT4* dynamicVertices, const std::vector<DirectX::XMFLOAT4>& morph, const float multiplier)
	{
		auto device = Shader::ShaderManager::GetSingleton().GetDevice();
		if (!device)
		{
			logger::error("Failed to get device");
			return false;
		}

		if (morph.size() == 0)
		{
			logger::error("Vertex count is invalid");
			return false;
		}

		morphBufferDesc = D3D11_BUFFER_DESC();
		morphBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		morphBufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * morph.size();
		morphBufferDesc.StructureByteStride = sizeof(DirectX::XMFLOAT4);
		morphBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		morphBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		morphBufferDesc.CPUAccessFlags = 0;

		morphData.pSysMem = morph.data();

		HRESULT hr = device->CreateBuffer(&morphBufferDesc, &morphData, &morphBuffer);
		if (FAILED(hr))
		{
			logger::error("Failed to create morphBuffer");
			return false;
		}

		dynamicBufferDesc = D3D11_BUFFER_DESC();
		dynamicBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		dynamicBufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * morph.size();
		dynamicBufferDesc.StructureByteStride = sizeof(DirectX::XMFLOAT4);
		dynamicBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		dynamicBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		dynamicBufferDesc.CPUAccessFlags = 0;

		dynamicData.pSysMem = dynamicVertices;

		hr = device->CreateBuffer(&dynamicBufferDesc, &dynamicData, &dynamicBuffer);
		if (FAILED(hr))
		{
			logger::error("Failed to create dynamicBuffer");
			return false;
		}

		constants = Constants{ multiplier, (UINT)morph.size() };

		constantsBufferDesc = D3D11_BUFFER_DESC();
		constantsBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		constantsBufferDesc.ByteWidth = sizeof(Constants);
		constantsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantsBufferDesc.CPUAccessFlags = 0;

		constantsData.pSysMem = &constants;
		hr = device->CreateBuffer(&constantsBufferDesc, &constantsData, &constantsBuffer);
		if (FAILED(hr))
		{
			logger::error("Failed to create constantsBuffer");
			return false;
		}

		hr = device->CreateShaderResourceView(morphBuffer.Get(), nullptr, &moprhResourceView);
		if (FAILED(hr))
		{
			logger::error("Failed to create moprhResourceView");
			return false;
		}

		hr = device->CreateUnorderedAccessView(dynamicBuffer.Get(), nullptr, &dynamicAccessView);
		if (FAILED(hr))
		{
			logger::error("Failed to create dynamicAccessView");
			return false;
		}

		return true;
	}

	bool MorphProcess::Run()
	{
		using namespace Shader;

		auto device = ShaderManager::GetSingleton().GetDevice();
		auto context = ShaderManager::GetSingleton().GetContext();
		if (!device || !context)
		{
			logger::error("Failed to get {}", !device ? "device" : "context");
			return false;
		}

		// get compute shader
		ShaderManager::ComputeShader computeShader = ShaderManager::GetSingleton().GetShader("Morph");
		if (!computeShader)
		{
			logger::error("Failed to get shader");
			return false;
		}

		ShaderManager::GetSingleton().ShaderContextLock();
		BackupShader bs = BackupShader();
		bs.Backup(context);
		context->CSSetShader(computeShader.Get(), nullptr, 0);
		context->CSSetUnorderedAccessViews(0, 1, dynamicAccessView.GetAddressOf(), nullptr);
		context->CSSetShaderResources(0, 1, moprhResourceView.GetAddressOf());
		context->CSSetConstantBuffers(0, 1, constantsBuffer.GetAddressOf());
		context->Dispatch((UINT)((constants.vertexCount + 63) / 64), 1, 1);
		bs.Revert(context);
		ShaderManager::GetSingleton().ShaderContextUnlock();
		return true;
	}
}