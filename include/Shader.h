#pragma once

namespace Mus {
	namespace Shader {
		class ShaderManager {
		public:
			[[nodiscard]] static ShaderManager& GetSingleton() {
				static ShaderManager instance;
				return instance;
			}

			ShaderManager() {};
			~ShaderManager() {};

			typedef Microsoft::WRL::ComPtr<ID3D11ComputeShader> ComputeShader;
			typedef Microsoft::WRL::ComPtr<ID3D11Device> Device;

			ComputeShader GetShader(std::string shaderType);
			ID3D11DeviceContext* GetContext();
			ID3D11Device* GetDevice();
			inline void ShaderContextLock() { EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&RE::BSGraphics::Renderer::GetSingleton()->GetLock())); };
			inline void ShaderContextUnlock() { LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&RE::BSGraphics::Renderer::GetSingleton()->GetLock())); };

			class ShaderLockGuard {
			public:
				ShaderLockGuard() { EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&RE::BSGraphics::Renderer::GetSingleton()->GetLock())); };
				~ShaderLockGuard() { LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&RE::BSGraphics::Renderer::GetSingleton()->GetLock())); };

				ShaderLockGuard(const ShaderLockGuard&) = delete;
				ShaderLockGuard& operator=(const ShaderLockGuard&) = delete;
			};

			bool IsFailedShader(std::string shaderType);

			void ResetShader() { Shaders.clear(); Failed.clear(); };
		private:
			bool SaveCompiledShaderFile(Microsoft::WRL::ComPtr<ID3DBlob>& compiledShader, const std::wstring& shaderFile);
			bool LoadComputeShader(const char* shaderCode, ID3D11ComputeShader** computeShader, Microsoft::WRL::ComPtr<ID3DBlob>& shaderData);
			bool LoadComputeShaderFile(const std::wstring& shaderFile, ID3D11ComputeShader** computeShader, Microsoft::WRL::ComPtr<ID3DBlob>& shaderData);
			bool LoadCompiledComputeShader(const std::wstring& shaderFile, ID3D11ComputeShader** computeShader);
			const char* GetShaderString(std::string shaderType);
			std::wstring GetShaderFilePath(std::string shaderType, bool compiled);
			ShaderManager::ComputeShader CreateComputeShader(std::string shaderType);

			ID3D11DeviceContext* context;
			ID3D11Device* device;

			concurrency::concurrent_unordered_map<std::string, ComputeShader> Shaders;
			std::unordered_set<std::string> Failed;
		};

		class BackupShader {
		public:
			void Backup(ID3D11DeviceContext* context);
			void Revert(ID3D11DeviceContext* context);
		private:
			Microsoft::WRL::ComPtr<ID3D11ComputeShader> oldComputeShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer> oldConstantBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> oldShaderResourceView = nullptr;
			Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> oldUnorderedAccessView = nullptr;
		};
	}

	class MorphProcess {
	public:
		MorphProcess() {};
		~MorphProcess() {};

		bool Initial(DirectX::XMFLOAT4* dynamicVertices, const std::vector<DirectX::XMFLOAT4>& morph, const float multiplier);
		bool Run();

	private:
		struct Constants {
			float multiplier;
			UINT vertexCount;

			float _padding[2];
		};
		Constants constants;

		D3D11_BUFFER_DESC morphBufferDesc;
		D3D11_BUFFER_DESC dynamicBufferDesc;
		D3D11_BUFFER_DESC constantsBufferDesc;

		D3D11_SUBRESOURCE_DATA morphData = {};
		D3D11_SUBRESOURCE_DATA dynamicData = {};
		D3D11_SUBRESOURCE_DATA constantsData = {};

		Microsoft::WRL::ComPtr<ID3D11Buffer> morphBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> dynamicBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> constantsBuffer = nullptr;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> moprhResourceView = nullptr;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> dynamicAccessView = nullptr;
	};
}