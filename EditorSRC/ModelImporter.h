#pragma once

void ImportModel_OBJ(const std::string& fileName, lb::scene::Scene& scene);
void ImportModel_GLTF(const std::string& fileName, lb::scene::Scene& scene);
void ExportModel_GLTF(const std::string& filename, lb::scene::Scene& scene);
void ImportModel_FBX(const std::string& fileName, lb::scene::Scene& scene);
