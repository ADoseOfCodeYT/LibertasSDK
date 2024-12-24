#pragma once

class Translator
{
private:
	bool dragging = false;
	bool dragStarted = false;
	bool dragEnded = false;
	XMFLOAT3 intersection_start = XMFLOAT3(0, 0, 0);
	XMFLOAT3 axis = XMFLOAT3(1, 0, 0);
	float angle = 0;
	float angle_start = 0;
	bool has_selected_transform = false;
	lb::vector<uint64_t> temp_filters;
public:

	void Update(const lb::scene::CameraComponent& camera, const XMFLOAT4& currentMouse, const lb::Canvas& canvas);
	void Draw(const lb::scene::CameraComponent& camera, const XMFLOAT4& currentMouse, lb::graphics::CommandList cmd) const;

	// Attach selection to translator temporarily
	void PreTranslate();
	// Apply translator to selection
	void PostTranslate();

	lb::scene::Scene* scene = nullptr;
	lb::scene::TransformComponent transform;
	lb::vector<lb::scene::PickResult> selected; // all the selected picks
	lb::unordered_set<lb::ecs::Entity> selectedEntitiesLookup; // fast lookup for selected entities
	lb::vector<lb::ecs::Entity> selectedEntitiesNonRecursive; // selected entities that don't contain entities that would be included in recursive iterations

	float scale_snap = 1;
	float rotate_snap = XM_PIDIV4;
	float translate_snap = 1;
	float opacity = 1;

	enum TRANSLATOR_STATE
	{
		TRANSLATOR_IDLE,
		TRANSLATOR_X,
		TRANSLATOR_Y,
		TRANSLATOR_Z,
		TRANSLATOR_XY,
		TRANSLATOR_XZ,
		TRANSLATOR_YZ,
		TRANSLATOR_XYZ,
	} state = TRANSLATOR_IDLE;

	XMMATRIX GetMirrorMatrix(TRANSLATOR_STATE state, const lb::scene::CameraComponent& camera) const;
	void WriteAxisText(TRANSLATOR_STATE axis, const lb::scene::CameraComponent& camera, char* text) const;

	float dist = 1;

	bool isTranslator = true;
	bool isScalator = false;
	bool isRotator = false;
	bool IsEnabled() const { return isTranslator || isRotator || isScalator; }
	void SetEnabled(bool value)
	{
		if (value && !IsEnabled())
		{
			isTranslator = true;
		}
		else if (!value && IsEnabled())
		{
			isTranslator = false;
			isScalator = false;
			isRotator = false;
		}
	}


	// Check if the drag started in this exact frame
	bool IsDragStarted() const { return dragStarted; };
	// Check if the drag ended in this exact frame
	bool IsDragEnded() const { return dragEnded; };

	bool IsInteracting() const { return state != TRANSLATOR_IDLE; }

	lb::scene::TransformComponent transform_start;
	lb::vector<XMFLOAT4X4> matrices_start;
	lb::vector<XMFLOAT4X4> matrices_current;
};

