#pragma once
#include "wiGraphics.h"
#include "wiVector.h"

#include <string>

namespace lb::shadercompiler
{

	enum class Flags
	{
		NONE = 0,
		DISABLE_OPTIMIZATION = 1 << 0,
		STRIP_REFLECTION = 1 << 1,
	};
	struct CompilerInput
	{
		Flags flags = Flags::NONE;
		lb::graphics::ShaderFormat format = lb::graphics::ShaderFormat::NONE;
		lb::graphics::ShaderStage stage = lb::graphics::ShaderStage::Count;
		// if the shader relies on a higher shader model feature, it must be declared here.
		//	But the compiler can also choose a higher one internally, if needed
		lb::graphics::ShaderModel minshadermodel = lb::graphics::ShaderModel::SM_5_0;
		std::string shadersourcefilename;
		std::string entrypoint = "main";
		lb::vector<std::string> include_directories;
		lb::vector<std::string> defines;
	};
	struct CompilerOutput
	{
		std::shared_ptr<void> internal_state;
		inline bool IsValid() const { return internal_state.get() != nullptr; }

		const uint8_t* shaderdata = nullptr;
		size_t shadersize = 0;
		lb::vector<uint8_t> shaderhash;
		std::string error_message;
		lb::vector<std::string> dependencies;
	};
	void Compile(const CompilerInput& input, CompilerOutput& output);

	bool SaveShaderAndMetadata(const std::string& shaderfilename, const CompilerOutput& output);
	bool IsShaderOutdated(const std::string& shaderfilename);

	void RegisterShader(const std::string& shaderfilename);
	size_t GetRegisteredShaderCount();
	bool CheckRegisteredShadersOutdated();
}

template<>
struct enable_bitmask_operators<lb::shadercompiler::Flags> {
	static const bool enable = true;
};
