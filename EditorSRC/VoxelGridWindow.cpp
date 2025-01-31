#include "stdafx.h"
#include "VoxelGridWindow.h"

using namespace lb::ecs;
using namespace lb::scene;

void VoxelGridWindow::Create(EditorComponent* _editor)
{
	editor = _editor;
	lb::gui::Window::Create(ICON_VOXELGRID " VoxelGrid", lb::gui::Window::WindowControls::COLLAPSE | lb::gui::Window::WindowControls::CLOSE | lb::gui::Window::WindowControls::FIT_ALL_WIDGETS_VERTICAL);
	SetSize(XMFLOAT2(520, 480));

	closeButton.SetTooltip("Delete VoxelGrid");
	OnClose([=](lb::gui::EventArgs args) {

		lb::Archive& archive = editor->AdvanceHistory();
		archive << EditorComponent::HISTORYOP_COMPONENT_DATA;
		editor->RecordEntity(archive, entity);

		editor->GetCurrentScene().voxel_grids.Remove(entity);

		editor->RecordEntity(archive, entity);

		editor->componentsWnd.RefreshEntityTree();
		});

	float x = 80;
	float y = 0;
	float hei = 18;
	float wid = 200;
	float step = hei + 4;

	infoLabel.Create("");
	infoLabel.SetSize(XMFLOAT2(100, 150));
	AddWidget(&infoLabel);

	dimXInput.Create("DimX");
	dimXInput.SetSize(XMFLOAT2(100, hei));
	dimXInput.SetDescription("Dimension X: ");
	dimXInput.OnInputAccepted([=](lb::gui::EventArgs args) {
		Scene& scene = editor->GetCurrentScene();
		lb::VoxelGrid* voxelgrid = scene.voxel_grids.GetComponent(entity);
		if (voxelgrid == nullptr)
			return;
		voxelgrid->init(uint32_t(std::max(1, args.iValue)), voxelgrid->resolution.y, voxelgrid->resolution.z);
	});
	AddWidget(&dimXInput);

	dimYInput.Create("DimY");
	dimYInput.SetSize(XMFLOAT2(100, hei));
	dimYInput.SetDescription("Y: ");
	dimYInput.OnInputAccepted([=](lb::gui::EventArgs args) {
		Scene& scene = editor->GetCurrentScene();
		lb::VoxelGrid* voxelgrid = scene.voxel_grids.GetComponent(entity);
		if (voxelgrid == nullptr)
			return;
		voxelgrid->init(voxelgrid->resolution.x, uint32_t(std::max(1, args.iValue)), voxelgrid->resolution.z);
		});
	AddWidget(&dimYInput);

	dimZInput.Create("DimZ");
	dimZInput.SetSize(XMFLOAT2(100, hei));
	dimZInput.SetDescription("Z: ");
	dimZInput.OnInputAccepted([=](lb::gui::EventArgs args) {
		Scene& scene = editor->GetCurrentScene();
		lb::VoxelGrid* voxelgrid = scene.voxel_grids.GetComponent(entity);
		if (voxelgrid == nullptr)
			return;
		voxelgrid->init(voxelgrid->resolution.x, voxelgrid->resolution.y, uint32_t(std::max(1, args.iValue)));
		});
	AddWidget(&dimZInput);

	clearButton.Create("Clear voxels " ICON_CLEARVOXELS);
	clearButton.SetSize(XMFLOAT2(100, hei));
	clearButton.OnClick([=](lb::gui::EventArgs args) {
		Scene& scene = editor->GetCurrentScene();
		lb::VoxelGrid* voxelgrid = scene.voxel_grids.GetComponent(entity);
		if (voxelgrid == nullptr)
			return;
		voxelgrid->cleardata();
	});
	AddWidget(&clearButton);

	voxelizeObjectsButton.Create("Voxelize objects " ICON_VOXELIZE);
	voxelizeObjectsButton.SetTooltip("Generate navigation grid including all meshes.");
	voxelizeObjectsButton.SetSize(XMFLOAT2(100, hei));
	voxelizeObjectsButton.OnClick([=](lb::gui::EventArgs args) {
		Scene& scene = editor->GetCurrentScene();
		lb::VoxelGrid* voxelgrid = scene.voxel_grids.GetComponent(entity);
		if (voxelgrid == nullptr)
			return;
		scene.VoxelizeScene(*voxelgrid, subtractCheckBox.GetCheck(), lb::enums::FILTER_OBJECT_ALL);
	});
	AddWidget(&voxelizeObjectsButton);

	voxelizeNavigationButton.Create("Voxelize navigation " ICON_VOXELIZE);
	voxelizeNavigationButton.SetTooltip("Generate navigation grid including all navmeshes (object tagged as navmesh).");
	voxelizeNavigationButton.SetSize(XMFLOAT2(100, hei));
	voxelizeNavigationButton.OnClick([=](lb::gui::EventArgs args) {
		Scene& scene = editor->GetCurrentScene();
		lb::VoxelGrid* voxelgrid = scene.voxel_grids.GetComponent(entity);
		if (voxelgrid == nullptr)
			return;
		scene.VoxelizeScene(*voxelgrid, subtractCheckBox.GetCheck(), lb::enums::FILTER_NAVIGATION_MESH);
		});
	AddWidget(&voxelizeNavigationButton);

	voxelizeCollidersButton.Create("Voxelize CPU colliders " ICON_VOXELIZE);
	voxelizeCollidersButton.SetTooltip("Generate navigation grid including all CPU colliders.");
	voxelizeCollidersButton.SetSize(XMFLOAT2(100, hei));
	voxelizeCollidersButton.OnClick([=](lb::gui::EventArgs args) {
		Scene& scene = editor->GetCurrentScene();
		lb::VoxelGrid* voxelgrid = scene.voxel_grids.GetComponent(entity);
		if (voxelgrid == nullptr)
			return;
		scene.VoxelizeScene(*voxelgrid, subtractCheckBox.GetCheck(), lb::enums::FILTER_COLLIDER);
		});
	AddWidget(&voxelizeCollidersButton);

	floodfillButton.Create("Flood fill " ICON_VOXELFILL);
	floodfillButton.SetTooltip("Fill enclosed empty voxel areas to solid.\nThis can take long if there are large enclosed empty areas.");
	floodfillButton.SetSize(XMFLOAT2(100, hei));
	floodfillButton.OnClick([=](lb::gui::EventArgs args) {
		Scene& scene = editor->GetCurrentScene();
		lb::VoxelGrid* voxelgrid = scene.voxel_grids.GetComponent(entity);
		if (voxelgrid == nullptr)
			return;
		lb::Timer tim;
		voxelgrid->flood_fill();
		char text[256] = {};
		snprintf(text, arraysize(text), "Flood filling took %.2f seconds.", tim.elapsed_seconds());
		lb::console::Post(text);
		});
	AddWidget(&floodfillButton);

	fitToSceneButton.Create("Fit bounds to scene " ICON_VOXELBOUNDS);
	fitToSceneButton.SetTooltip("Fit the bounds of the voxel grid onto the whole scene.");
	fitToSceneButton.SetSize(XMFLOAT2(100, hei));
	fitToSceneButton.OnClick([=](lb::gui::EventArgs args) {
		Scene& scene = editor->GetCurrentScene();
		if (scene.bounds.getArea() < 0)
			return;
		lb::VoxelGrid* voxelgrid = scene.voxel_grids.GetComponent(entity);
		if (voxelgrid == nullptr)
			return;
		voxelgrid->from_aabb(scene.bounds);
		TransformComponent* transform = scene.transforms.GetComponent(entity);
		if (transform != nullptr)
		{
			// feed back to transform component if it exists:
			transform->translation_local = voxelgrid->center;
			transform->scale_local = voxelgrid->voxelSize;
			transform->SetDirty();
		}
	});
	AddWidget(&fitToSceneButton);

	generateMeshButton.Create("Generate Mesh " ICON_MESH);
	generateMeshButton.SetTooltip("Generate a mesh from the voxels.");
	generateMeshButton.SetSize(XMFLOAT2(100, hei));
	generateMeshButton.OnClick([=](lb::gui::EventArgs args) {
		Scene& scene = editor->GetCurrentScene();
		lb::VoxelGrid* voxelgrid = scene.voxel_grids.GetComponent(entity);
		if (voxelgrid == nullptr)
			return;
		lb::vector<uint32_t> indices;
		lb::vector<XMFLOAT3> vertices;
		voxelgrid->create_mesh(indices, vertices, false);
		if (vertices.empty())
		{
			lb::console::Post("VoxelGrid.create_mesh() : no voxels were found, so mesh creation is aborted.", lb::console::LogLevel::Warning);
			return;
		}
		scene.Entity_CreateMeshFromData("voxelgrid_to_mesh", indices.size(), indices.data(), vertices.size(), vertices.data());
	});
	AddWidget(&generateMeshButton);

	generateSimplifiedMeshButton.Create("Generate Simplified Mesh " ICON_MESH);
	generateSimplifiedMeshButton.SetTooltip("Generate a simplified mesh from the voxels.");
	generateSimplifiedMeshButton.SetSize(XMFLOAT2(100, hei));
	generateSimplifiedMeshButton.OnClick([=](lb::gui::EventArgs args) {
		Scene& scene = editor->GetCurrentScene();
		lb::VoxelGrid* voxelgrid = scene.voxel_grids.GetComponent(entity);
		if (voxelgrid == nullptr)
			return;
		lb::vector<uint32_t> indices;
		lb::vector<XMFLOAT3> vertices;
		voxelgrid->create_mesh(indices, vertices, true);
		if (vertices.empty())
		{
			lb::console::Post("VoxelGrid.create_mesh() : no voxels were found, so mesh creation is aborted.", lb::console::LogLevel::Warning);
			return;
		}
		scene.Entity_CreateMeshFromData("voxelgrid_to_mesh", indices.size(), indices.data(), vertices.size(), vertices.data());
	});
	AddWidget(&generateSimplifiedMeshButton);

	subtractCheckBox.Create("Subtraction mode: ");
	subtractCheckBox.SetTooltip("If enabled, voxelization will be subtractive, so it will remove voxels instead of add.");
	subtractCheckBox.SetSize(XMFLOAT2(hei, hei));
	AddWidget(&subtractCheckBox);

	debugAllCheckBox.Create("Debug draw all: ");
	debugAllCheckBox.SetTooltip("Draw all voxel grids, whether they are selected or not.");
	debugAllCheckBox.SetSize(XMFLOAT2(hei, hei));
	AddWidget(&debugAllCheckBox);


	SetMinimized(true);
	SetVisible(false);

}

void VoxelGridWindow::SetEntity(Entity entity)
{
	bool changed = this->entity != entity;
	this->entity = entity;

	Scene& scene = editor->GetCurrentScene();
	lb::VoxelGrid* voxelgrid = scene.voxel_grids.GetComponent(entity);

	if (voxelgrid != nullptr)
	{
		std::string infotext = "Voxel grid can be used for navigation. By voxelizing the scene into the grid, path finding can be used on the resulting voxel grid.";
		infotext += "\n\nMemory usage: ";
		infotext += lb::helper::GetMemorySizeText(voxelgrid->get_memory_size());
		infoLabel.SetText(infotext);

		dimXInput.SetValue((int)voxelgrid->resolution.x);
		dimYInput.SetValue((int)voxelgrid->resolution.y);
		dimZInput.SetValue((int)voxelgrid->resolution.z);
	}
}

void VoxelGridWindow::ResizeLayout()
{
	lb::gui::Window::ResizeLayout();
	const float padding = 4;
	const float width = GetWidgetAreaSize().x;
	float y = padding;
	float jump = 20;

	const float margin_left = 90;
	const float margin_right = 40;

	auto add = [&](lb::gui::Widget& widget) {
		if (!widget.IsVisible())
			return;
		widget.SetPos(XMFLOAT2(margin_left, y));
		widget.SetSize(XMFLOAT2(width - margin_left - margin_right, widget.GetScale().y));
		y += widget.GetSize().y;
		y += padding;
	};
	auto add_right = [&](lb::gui::Widget& widget) {
		if (!widget.IsVisible())
			return;
		const float margin_right = padding;
		widget.SetPos(XMFLOAT2(width - margin_right - widget.GetSize().x, y));
		y += widget.GetSize().y;
		y += padding;
	};
	auto add_fullwidth = [&](lb::gui::Widget& widget) {
		if (!widget.IsVisible())
			return;
		const float margin_left = padding;
		const float margin_right = padding;
		widget.SetPos(XMFLOAT2(margin_left, y));
		widget.SetSize(XMFLOAT2(width - margin_left - margin_right, widget.GetScale().y));
		y += widget.GetSize().y;
		y += padding;
	};

	add_fullwidth(infoLabel);

	const float padding2 = 20;
	const float l = 95;
	const float r = width;
	float w = ((r - l) - padding2 * 2) / 3.0f;
	dimXInput.SetSize(XMFLOAT2(w, dimXInput.GetSize().y));
	dimYInput.SetSize(XMFLOAT2(w, dimYInput.GetSize().y));
	dimZInput.SetSize(XMFLOAT2(w, dimZInput.GetSize().y));
	dimXInput.SetPos(XMFLOAT2(margin_left, y));
	dimYInput.SetPos(XMFLOAT2(dimXInput.GetPos().x + w + padding2, y));
	dimZInput.SetPos(XMFLOAT2(dimYInput.GetPos().x + w + padding2, y));
	y += dimZInput.GetSize().y;
	y += padding;

	add_fullwidth(clearButton);
	add_fullwidth(voxelizeObjectsButton);
	add_fullwidth(voxelizeCollidersButton);
	add_fullwidth(voxelizeNavigationButton);
	add_fullwidth(floodfillButton);
	add_fullwidth(fitToSceneButton);
	add_fullwidth(generateMeshButton);
	add_fullwidth(generateSimplifiedMeshButton);
	add_right(subtractCheckBox);
	add_right(debugAllCheckBox);
}
