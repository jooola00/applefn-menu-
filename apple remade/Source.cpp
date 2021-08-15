#include "DirectOverlay.h"
#include <sstream>
#include <string>
#include <algorithm>
#include <list>

#define M_PI 3.14159265358979323846264338327950288419716939937510

#define OFFSET_UWORLD 0x99b5930

//real
bool Menu = true;
bool Aimbot = true;
bool EnemyESP = false;
bool skeleton = false;
bool BoxESP = false;
bool LineESP = false;
bool DistanceESP = false;
bool DrawFov = true;
bool Croshair = true;
bool CornerESP = false;
bool FilledBoxESP = false;
bool CircleESP = false;
bool NameESP = true;
bool EnemyVisible = true;
bool Cursor = true;
bool FilledFOV = true;
bool Head = false;
bool Chest = true;
bool Pelvis = false;
bool VisualTab2 = false;
bool AimTab2 = false;

//fake
bool Memory = true;
bool SilentV1 = false;
bool SilentV2 = false;
bool SpinExtension = false;
bool Spinbot = false;
bool VisibleTargets = true;





















bool CheckBox(int x, int y, bool var, POINT Mouse) {
	DrawBox(x, y, 14, 14, 1, 0.9, 0.9, 0.9, 1, true);
	if (Mouse.x >= x && Mouse.x <= x + 111 && Mouse.y >= y && Mouse.y <= y + 10) {
		if (GetAsyncKeyState(VK_LBUTTON) & 1) {
			var = !var;
			return var;
		}
	}

	if (var) {
		DrawBox(x + 3.5, y + 3.5, 7, 7, 1, 0, 0, 0, 1.f, true);
	}

	return var;
}

bool AimTab(int x, int y, bool var, POINT Mouse) {
	DrawBox(x, y, 90, 30, 1, 0.1, 0.1, 0.1, 1, false);
	DrawBox(x, y, 90, 30, 1, 0.84, 0.84, 0.84, 1, true);
	DrawString(_xor_("Aimbot").c_str(), 14, x + 24, y + 6, 0, 0, 0, 255.f);
	if (Mouse.x >= x && Mouse.x <= x + 90 && Mouse.y >= y && Mouse.y <= y + 30) {
		if (GetAsyncKeyState(VK_LBUTTON) & 1) {
			var = !var;
			return var;
		}
	}

	if (var) {

	}

	return var;
}

bool VisualTab(int x, int y, bool var, POINT Mouse) {
	DrawBox(x, y, 90, 30, 1, 0.1, 0.1, 0.1, 1, false);
	DrawBox(x, y, 90, 30, 1, 0.84, 0.84, 0.84, 1, true);
	DrawString(_xor_("Visual").c_str(), 14, x + 24, y + 6, 0, 0, 0, 255.f);
	if (Mouse.x >= x && Mouse.x <= x + 90 && Mouse.y >= y && Mouse.y <= y + 30) {
		if (GetAsyncKeyState(VK_LBUTTON) & 1) {
			var = !var;
			return var;
		}
	}

	if (var) {

	}

	return var;
}

bool ModsTab(int x, int y, bool var, POINT Mouse) {
	DrawBox(x, y, 40, 23, 1, 0.84, 0.84, 0.84, 1, false);
	DrawBox(x, y, 40, 23, 1, 0.84, 0.84, 0.84, 1, true);
	if (Mouse.x >= x && Mouse.x <= x + 111 && Mouse.y >= y && Mouse.y <= y + 10) {
		if (GetAsyncKeyState(VK_LBUTTON) & 1) {
			var = !var;
			return var;
		}
	}

	if (var) {

	}

	return var;
}

bool MiscTab(int x, int y, bool var, POINT Mouse) {
	DrawBox(x, y, 40, 23, 1, 0.84, 0.84, 0.84, 1, false);
	DrawBox(x, y, 40, 23, 1, 0.84, 0.84, 0.84, 1, true);
	if (Mouse.x >= x && Mouse.x <= x + 111 && Mouse.y >= y && Mouse.y <= y + 10) {
		if (GetAsyncKeyState(VK_LBUTTON) & 1) {
			var = !var;
			return var;
		}
	}

	if (var) {

	}

	return var;
}


DWORD processID;
HWND hwnd = NULL;
 
int width;
int height;
int localplayerID;
int smoothing = 1.0;
int MenuX = 120;
int MenuY = 1;
float FovAngle;

HANDLE DriverHandle;
uint64_t base_address;

DWORD_PTR Uworld;
DWORD_PTR LocalPawn;
DWORD_PTR Localplayer;
DWORD_PTR Rootcomp;
DWORD_PTR PlayerController;
DWORD_PTR Ulevel;

Vector3 localactorpos;
Vector3 Localcam;

float AimFOV = 15; 

bool isaimbotting;
DWORD_PTR entityx;


FTransform GetBoneIndex(DWORD_PTR mesh, int index)
{
	DWORD_PTR bonearray = read<DWORD_PTR>(DriverHandle, processID, mesh + 0x4A8);  // 4A8  changed often 4u

	if (bonearray == NULL) // added 4u
	{
		bonearray = read<DWORD_PTR>(DriverHandle, processID, mesh + 0x4A8 + 0x10); // added 4u
	}

	return read<FTransform>(DriverHandle, processID, bonearray + (index * 0x30));  // doesn't change
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = read<FTransform>(DriverHandle, processID, mesh + 0x1C0);  // have never seen this change 4u

	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

D3DMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

//4u note:  changes to projectw2s and camera are the most diffucult changes to understand reworking old camloc, be careful blindly making edits

extern Vector3 CameraEXT(0, 0, 0);

Vector3 ProjectWorldToScreen(Vector3 WorldLocation, Vector3 camrot)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);
	Vector3 Camera;

	auto chain69 = read<uintptr_t>(DriverHandle, processID, Localplayer + 0xa8);
	uint64_t chain699 = read<uintptr_t>(DriverHandle, processID, chain69 + 8);

	Camera.x = read<float>(DriverHandle, processID, chain699 + 0x7F8);  //camera pitch  watch out for x and y swapped 4u
	Camera.y = read<float>(DriverHandle, processID, Rootcomp + 0x12C);  //camera yaw

	float test = asin(Camera.x);
	float degrees = test * (180.0 / M_PI);
	Camera.x = degrees;

	if (Camera.y < 0)
		Camera.y = 360 + Camera.y;

	D3DMATRIX tempMatrix = Matrix(Camera);
	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	uint64_t chain = read<uint64_t>(DriverHandle, processID, Localplayer + 0x70);
	uint64_t chain1 = read<uint64_t>(DriverHandle, processID, chain + 0x98);
	uint64_t chain2 = read<uint64_t>(DriverHandle, processID, chain1 + 0x130);

	Vector3 vDelta = WorldLocation - read<Vector3>(DriverHandle, processID, chain2 + 0x10); //camera location credits for Object9999
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float zoom = read<float>(DriverHandle, processID, chain699 + 0x590);

	FovAngle = 80.0f / (zoom / 1.19f);
	float ScreenCenterX = width / 2.0f;
	float ScreenCenterY = height / 2.0f;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	CameraEXT = Camera;

	return Screenlocation;
}


void menu()
{
	static POINT Mouse;
	GetCursorPos(&Mouse);
	if (Menu)
	{
		if (Mouse.x >= MenuX - 204 && Mouse.x <= MenuX + 204 && Mouse.y >= MenuY - 25 && Mouse.y <= MenuY + 26) {
			if (GetAsyncKeyState(VK_LBUTTON) < 0) {
				MenuX = Mouse.x;
				MenuY = Mouse.y;
			}
			if (GetAsyncKeyState(VK_LBUTTON) == 0) {
			}

		}
		DrawBox(MenuX, MenuY, 540, 18, 2.0f, 0, 0, 0, 1, true);
		DrawBox(MenuX, MenuY + 18, 540, 340, 2.0f, 1, 1, 1, 1, true);
		DrawString(_xor_("APPLE FORTNITE @ May 13 2021 02:22:17 (INSERT) (menu is only useable in game)").c_str(), 12, MenuX + 2, MenuY + 1, 1, 1, 1, 1);

		Aimbot = CheckBox(MenuX + 7, MenuY + 25 + 55, Aimbot, Mouse);
		DrawString(_xor_("Enable").c_str(), 12, MenuX + 25, MenuY + 55 + 23, 0, 0, 0, 255.f);
		DrawString(_xor_("Aimbot Mode (ENTER)").c_str(), 12, MenuX + 8, MenuY + 55 + 25 + 20, 0, 0, 0, 1);
		
		//Memory Aim (Fake)
		Memory = CheckBox(MenuX + 7 + 10, MenuY + 55 + 65, Memory, Mouse);
		DrawString(_xor_("Memory aim").c_str(), 12, MenuX + 35, MenuY + 55 + 63, 0, 0, 0, 255.f);
		//SilentV1 (Fake)
		SilentV1 = CheckBox(MenuX + 7 + 10, MenuY + 55 + 85, SilentV1, Mouse);
		DrawString(_xor_("Silent (V1) (might be unsafe (!!!))").c_str(), 12, MenuX + 35, MenuY + 55 + 83, 0, 0, 0, 255.f);
		//SilentV2 (Fake)
		SilentV2 = CheckBox(MenuX + 7 + 10, MenuY + 55 + 105, SilentV2, Mouse);
		DrawString(_xor_("Silent (V2, sticky) (might be unsafe (!!!))").c_str(), 12, MenuX + 35, MenuY + 55 + 103, 0, 0, 0, 255.f);

		//Spinbot (Fake)
		Spinbot = CheckBox(MenuX + 7, MenuY + 55 + 125, Spinbot, Mouse);
		DrawString(_xor_("Spinbot").c_str(), 12, MenuX + 25, MenuY + 55 + 123, 0, 0, 0, 255.f);
		//Spinbot Extension (Fake)
		SpinExtension = CheckBox(MenuX + 7, MenuY + 55 + 145, SpinExtension, Mouse);
		DrawString(_xor_("Reset rotation after spin").c_str(), 12, MenuX + 25, MenuY + 55 + 143, 0, 0, 0, 255.f);
		//Visible Targets (Fake)
		VisibleTargets = CheckBox(MenuX + 7, MenuY + 55 + 165, VisibleTargets, Mouse);
		DrawString(_xor_("Visible targets only (F4)").c_str(), 12, MenuX + 25, MenuY + 55 + 163, 0, 0, 0, 255.f);
		//AimBone (Real)
		DrawString(_xor_("Aim bone (BACKSPACE)").c_str(), 12, MenuX + 8, MenuY + 55 + 183, 0, 0, 0, 255.f);

		Head = CheckBox(MenuX + 7 + 10, MenuY + 55 + 205, Head, Mouse);
		DrawString(_xor_("Head").c_str(), 12, MenuX + 35, MenuY + 55 + 203, 0, 0, 0, 255.f);
		Chest = CheckBox(MenuX + 7 + 10, MenuY + 55 + 225, Chest, Mouse);
		DrawString(_xor_("Chest").c_str(), 12, MenuX + 35, MenuY + 55 + 223, 0, 0, 0, 255.f);
		Pelvis = CheckBox(MenuX + 7 + 10, MenuY + 55 + 245, Pelvis, Mouse);
		DrawString(_xor_("Pelvis").c_str(), 12, MenuX + 35, MenuY + 55 + 243, 0, 0, 0, 255.f);

		//Slider
		DrawString(_xor_("FOV:").c_str(), 12, MenuX + 17, MenuY + 330 - 13, 0, 0, 0, 255.f);
		DrawString((std::to_string(AimFOV)).c_str(), 12, MenuX + 41, MenuY + 330 - 13, 0, 0, 0, 1);
		DrawBox(MenuX + 63, MenuY + 330 - 13, 49, 14, 2.0f, 1, 1, 1, 1, true);
		DrawString(_xor_("               (deg)").c_str(), 12, MenuX + 17, MenuY + 327 - 11, 0, 0, 0, 255.f);
		DrawBox(MenuX + 17, MenuY + 330 + 5, 187, 3, 2.0f, 0.9, 0.9, 0.9, 1, true);
		DrawBox(MenuX + 17, MenuY + 327 + 5, 4, 8, 2.0f, 0, 0, 0, 1, true);

		AimTab2 = AimTab(MenuX + 22, MenuY + 37, AimTab2, Mouse);
		VisualTab2 = VisualTab(MenuX + 22 + 100, MenuY + 37, VisualTab2, Mouse);

		//DrawString((std::to_string(smoothing)).c_str(), 11, MenuX + 70, MenuY + 32 + 188, 0.f, 255.f, 0.f, 255.f);

		

		//cursor
		DrawBox(Mouse.x - 2, Mouse.y - 2, 5, 5, 3.0f, 0, 0, 0, 0.9, true);

		if (AimTab2)
		{
			static POINT Mouse;
			GetCursorPos(&Mouse);
			if (Menu)
			{
				if (Mouse.x >= MenuX - 204 && Mouse.x <= MenuX + 204 && Mouse.y >= MenuY - 25 && Mouse.y <= MenuY + 26) {
					if (GetAsyncKeyState(VK_LBUTTON) < 0) {
						MenuX = Mouse.x;
						MenuY = Mouse.y;
					}
					if (GetAsyncKeyState(VK_LBUTTON) == 0) {
					}

				}
				DrawBox(MenuX, MenuY, 540, 18, 2.0f, 0, 0, 0, 1, true);
				DrawBox(MenuX, MenuY + 18, 540, 340, 2.0f, 1, 1, 1, 1, true);
				DrawString(_xor_("APPLE FORTNITE @ May 13 2021 02:22:17 (INSERT) (menu is only useable in game)").c_str(), 12, MenuX + 2, MenuY + 1, 1, 1, 1, 1);

				Aimbot = CheckBox(MenuX + 7, MenuY + 25 + 55, Aimbot, Mouse);
				DrawString(_xor_("Enable").c_str(), 12, MenuX + 25, MenuY + 55 + 23, 0, 0, 0, 255.f);
				DrawString(_xor_("Aimbot Mode (ENTER)").c_str(), 12, MenuX + 8, MenuY + 55 + 25 + 20, 0, 0, 0, 1);

				//Memory Aim (Fake)
				Memory = CheckBox(MenuX + 7 + 10, MenuY + 55 + 65, Memory, Mouse);
				DrawString(_xor_("Memory aim").c_str(), 12, MenuX + 35, MenuY + 55 + 63, 0, 0, 0, 255.f);
				//SilentV1 (Fake)
				SilentV1 = CheckBox(MenuX + 7 + 10, MenuY + 55 + 85, SilentV1, Mouse);
				DrawString(_xor_("Silent (V1) (might be unsafe (!!!))").c_str(), 12, MenuX + 35, MenuY + 55 + 83, 0, 0, 0, 255.f);
				//SilentV2 (Fake)
				SilentV2 = CheckBox(MenuX + 7 + 10, MenuY + 55 + 105, SilentV2, Mouse);
				DrawString(_xor_("Silent (V2, sticky) (might be unsafe (!!!))").c_str(), 12, MenuX + 35, MenuY + 55 + 103, 0, 0, 0, 255.f);

				//Spinbot (Fake)
				Spinbot = CheckBox(MenuX + 7, MenuY + 55 + 125, Spinbot, Mouse);
				DrawString(_xor_("Spinbot").c_str(), 12, MenuX + 25, MenuY + 55 + 123, 0, 0, 0, 255.f);
				//Spinbot Extension (Fake)
				SpinExtension = CheckBox(MenuX + 7, MenuY + 55 + 145, SpinExtension, Mouse);
				DrawString(_xor_("Reset rotation after spin").c_str(), 12, MenuX + 25, MenuY + 55 + 143, 0, 0, 0, 255.f);
				//Visible Targets (Fake)
				VisibleTargets = CheckBox(MenuX + 7, MenuY + 55 + 165, VisibleTargets, Mouse);
				DrawString(_xor_("Visible targets only (F4)").c_str(), 12, MenuX + 25, MenuY + 55 + 163, 0, 0, 0, 255.f);
				//AimBone (Real)
				DrawString(_xor_("Aim bone (BACKSPACE)").c_str(), 12, MenuX + 8, MenuY + 55 + 183, 0, 0, 0, 255.f);

				Head = CheckBox(MenuX + 7 + 10, MenuY + 55 + 205, Head, Mouse);
				DrawString(_xor_("Head").c_str(), 12, MenuX + 35, MenuY + 55 + 203, 0, 0, 0, 255.f);
				Chest = CheckBox(MenuX + 7 + 10, MenuY + 55 + 225, Chest, Mouse);
				DrawString(_xor_("Chest").c_str(), 12, MenuX + 35, MenuY + 55 + 223, 0, 0, 0, 255.f);
				Pelvis = CheckBox(MenuX + 7 + 10, MenuY + 55 + 245, Pelvis, Mouse);
				DrawString(_xor_("Pelvis").c_str(), 12, MenuX + 35, MenuY + 55 + 243, 0, 0, 0, 255.f);

				//Slider
				DrawString(_xor_("FOV:").c_str(), 12, MenuX + 17, MenuY + 330 - 13, 0, 0, 0, 255.f);
				DrawString((std::to_string(AimFOV)).c_str(), 12, MenuX + 41, MenuY + 330 - 13, 0, 0, 0, 1);
				DrawBox(MenuX + 63, MenuY + 330 - 13, 49, 14, 2.0f, 1, 1, 1, 1, true);
				DrawString(_xor_("               (deg)").c_str(), 12, MenuX + 17, MenuY + 327 - 11, 0, 0, 0, 255.f);
				DrawBox(MenuX + 17, MenuY + 330 + 5, 187, 3, 2.0f, 0.9, 0.9, 0.9, 1, true);
				DrawBox(MenuX + 17, MenuY + 327 + 5, 4, 8, 2.0f, 0, 0, 0, 1, true);

				AimTab2 = AimTab(MenuX + 22, MenuY + 37, AimTab2, Mouse);
				VisualTab2 = VisualTab(MenuX + 22 + 100, MenuY + 37, VisualTab2, Mouse);

				//DrawString((std::to_string(smoothing)).c_str(), 11, MenuX + 70, MenuY + 32 + 188, 0.f, 255.f, 0.f, 255.f);



				//cursor
				DrawBox(Mouse.x - 2, Mouse.y - 2, 5, 5, 3.0f, 0, 0, 0, 0.9, true);
			}
		}

		if (VisualTab2)
		{
			static POINT Mouse;
			GetCursorPos(&Mouse);
			if (Menu)
			{
				if (Mouse.x >= MenuX - 204 && Mouse.x <= MenuX + 204 && Mouse.y >= MenuY - 25 && Mouse.y <= MenuY + 26) {
					if (GetAsyncKeyState(VK_LBUTTON) < 0) {
						MenuX = Mouse.x;
						MenuY = Mouse.y;
					}
					if (GetAsyncKeyState(VK_LBUTTON) == 0) {
					}

				}
				DrawBox(MenuX, MenuY, 540, 18, 2.0f, 0, 0, 0, 1, true);
				DrawBox(MenuX, MenuY + 18, 540, 340, 2.0f, 1, 1, 1, 1, true);
				DrawString(_xor_("APPLE FORTNITE @ May 13 2021 02:22:17 (INSERT) (menu is only useable in game)").c_str(), 12, MenuX + 2, MenuY + 1, 1, 1, 1, 1);



				AimTab2 = AimTab(MenuX + 22, MenuY + 37, AimTab2, Mouse);
				VisualTab2 = VisualTab(MenuX + 22 + 100, MenuY + 37, VisualTab2, Mouse);
				DrawBox(Mouse.x - 2, Mouse.y - 2, 5, 5, 3.0f, 0, 0, 0, 0.9, true);
			}
		}

	}
}

DWORD Menuthread(LPVOID in)
{
	while (1)
	{
		HWND test = FindWindowA(0, _xor_("Fortnite  ").c_str());

		if (test == NULL)
		{
			ExitProcess(0);
		}

		if (GetAsyncKeyState(VK_INSERT) & 1) {
			Menu = !Menu;
		}

		if (Menu)
		{
			if (smoothing) {
				if (GetAsyncKeyState(VK_RIGHT) & 1)
					smoothing += 1;
				if (GetAsyncKeyState(VK_LEFT) & 1)
					smoothing -= 1;
				if (smoothing < 1)
					smoothing = 1;
				if (smoothing > 20)
					smoothing = 20;
			}

			if (GetAsyncKeyState(VK_F4) & 1)
			{
				VisibleTargets = !VisibleTargets;
			}

			if (AimFOV) {
				if (GetAsyncKeyState(VK_UP) & 1)
					AimFOV += 1;
				if (GetAsyncKeyState(VK_DOWN) & 1)
					AimFOV -= 1;
				if (AimFOV < 1)
					AimFOV = 1;
				if (AimFOV > 360)
					AimFOV = 360;
			}

		}
	}
}

bool GetAimKey()
{
	return (GetAsyncKeyState(VK_CONTROL));
}

void TriggerBot(float x, float y)
{

}

void aimbot(float x, float y)
{
	float ScreenCenterX = (width / 2);
	float ScreenCenterY = (height / 2);
	int AimSpeed = smoothing;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);

	return;
}

double GetCrossDistance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

bool GetClosestPlayerToCrossHair(Vector3 Pos, float& max, float aimfov, DWORD_PTR entity)
{
	if (!GetAimKey() || !isaimbotting)
	{
		if (entity)
		{
			float Dist = GetCrossDistance(Pos.x, Pos.y, width / 2, height / 2);

			if (Dist < max)
			{
				max = Dist;
				entityx = entity;
				AimFOV = aimfov;
			}
		}
	}
	return false;
}

void AimAt2(DWORD_PTR entity, Vector3 Localcam)
{
	uint64_t currentactormesh = read<uint64_t>(DriverHandle, processID, entity + 0x280);

	auto rootHead = GetBoneWithRotation(currentactormesh, 2);
	Vector3 rootHeadOut = ProjectWorldToScreen(rootHead, Vector3(Localcam.y, Localcam.x, Localcam.z));

	if (rootHeadOut.x != 0 || rootHeadOut.y != 0)
	{
		if ((GetCrossDistance(rootHeadOut.x, rootHeadOut.y, width / 2, height / 2) <= AimFOV * 8) || isaimbotting)
		{
			aimbot(rootHeadOut.x, rootHeadOut.y);
		}
	}
}


void AimAt(DWORD_PTR entity, Vector3 Localcam)
{
	uint64_t currentactormesh = read<uint64_t>(DriverHandle, processID, entity + 0x280); // changes often 4u
	auto rootHead = GetBoneWithRotation(currentactormesh, 66);
	Vector3 rootHeadOut = ProjectWorldToScreen(rootHead, Vector3(Localcam.y, Localcam.x, Localcam.z));

	if (rootHeadOut.x != 0 || rootHeadOut.y != 0)  // these are both y in all updates, fixed 4u, is old bug never fixed
	{
		if ((GetCrossDistance(rootHeadOut.x, rootHeadOut.y, width / 2, height / 2) <= AimFOV * 8) || isaimbotting)
		{
			aimbot(rootHeadOut.x, rootHeadOut.y);
		}
	}
}

void aimbot(Vector3 Localcam)
{
	if (entityx != 0)
	{
		if (GetAimKey())
		{
			AimAt(entityx, Localcam);;;
		}
		else
		{
			isaimbotting = false;
		}
	}
}

void AIms(DWORD_PTR entity, Vector3 Localcam)
{
	float max = 100.f;

	uint64_t currentactormesh = read<uint64_t>(DriverHandle, processID, entity + 0x280);  // changed often 4u

	Vector3 rootHead = GetBoneWithRotation(currentactormesh, 66);
	Vector3 rootHeadOut = ProjectWorldToScreen(rootHead, Vector3(Localcam.y, Localcam.x, Localcam.z));

	if (GetClosestPlayerToCrossHair(rootHeadOut, max, AimFOV, entity))
		entityx = entity;
}

void DrawSkeleton(DWORD_PTR mesh)
{
	Vector3 vHeadBone = GetBoneWithRotation(mesh, 96);
	Vector3 vHip = GetBoneWithRotation(mesh, 2);
	Vector3 vNeck = GetBoneWithRotation(mesh, 65);
	Vector3 vUpperArmLeft = GetBoneWithRotation(mesh, 34);
	Vector3 vUpperArmRight = GetBoneWithRotation(mesh, 91);
	Vector3 vLeftHand = GetBoneWithRotation(mesh, 35);
	Vector3 vRightHand = GetBoneWithRotation(mesh, 63);
	Vector3 vLeftHand1 = GetBoneWithRotation(mesh, 33);
	Vector3 vRightHand1 = GetBoneWithRotation(mesh, 60);
	Vector3 vRightThigh = GetBoneWithRotation(mesh, 74);
	Vector3 vLeftThigh = GetBoneWithRotation(mesh, 67);
	Vector3 vRightCalf = GetBoneWithRotation(mesh, 75);
	Vector3 vLeftCalf = GetBoneWithRotation(mesh, 68);
	Vector3 vLeftFoot = GetBoneWithRotation(mesh, 69);
	Vector3 vRightFoot = GetBoneWithRotation(mesh, 76);

	Vector3 vHeadBoneOut = ProjectWorldToScreen(vHeadBone, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vHipOut = ProjectWorldToScreen(vHip, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vNeckOut = ProjectWorldToScreen(vNeck, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vUpperArmLeftOut = ProjectWorldToScreen(vUpperArmLeft, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vUpperArmRightOut = ProjectWorldToScreen(vUpperArmRight, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vLeftHandOut = ProjectWorldToScreen(vLeftHand, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vRightHandOut = ProjectWorldToScreen(vRightHand, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vLeftHandOut1 = ProjectWorldToScreen(vLeftHand1, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vRightHandOut1 = ProjectWorldToScreen(vRightHand1, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vRightThighOut = ProjectWorldToScreen(vRightThigh, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vLeftThighOut = ProjectWorldToScreen(vLeftThigh, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vRightCalfOut = ProjectWorldToScreen(vRightCalf, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vLeftCalfOut = ProjectWorldToScreen(vLeftCalf, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vLeftFootOut = ProjectWorldToScreen(vLeftFoot, Vector3(Localcam.y, Localcam.x, Localcam.z));
	Vector3 vRightFootOut = ProjectWorldToScreen(vRightFoot, Vector3(Localcam.y, Localcam.x, Localcam.z));

	DrawLine(vHipOut.x, vHipOut.y, vNeckOut.x, vNeckOut.y, 2.f, 255.f, 0.f, 0.f, 255.f);

	DrawLine(vUpperArmLeftOut.x, vUpperArmLeftOut.y, vNeckOut.x, vNeckOut.y, 2.f, 255.f, 0.f, 0.f, 255.f);
	DrawLine(vUpperArmRightOut.x, vUpperArmRightOut.y, vNeckOut.x, vNeckOut.y, 2.f, 255.f, 0.f, 0.f, 255.f);

	DrawLine(vLeftHandOut.x, vLeftHandOut.y, vUpperArmLeftOut.x, vUpperArmLeftOut.y, 2.f, 255.f, 0.f, 0.f, 255.f);
	DrawLine(vRightHandOut.x, vRightHandOut.y, vUpperArmRightOut.x, vUpperArmRightOut.y, 2.f, 255.f, 0.f, 0.f, 255.f);

	DrawLine(vLeftHandOut.x, vLeftHandOut.y, vLeftHandOut1.x, vLeftHandOut1.y, 2.f, 255.f, 0.f, 0.f, 255.f);
	DrawLine(vRightHandOut.x, vRightHandOut.y, vRightHandOut1.x, vRightHandOut1.y, 2.f, 255.f, 0.f, 0.f, 255.f);

	DrawLine(vLeftThighOut.x, vLeftThighOut.y, vHipOut.x, vHipOut.y, 2.f, 255.f, 0.f, 0.f, 255.f);
	DrawLine(vRightThighOut.x, vRightThighOut.y, vHipOut.x, vHipOut.y, 2.f, 255.f, 0.f, 0.f, 255.f);

	DrawLine(vLeftCalfOut.x, vLeftCalfOut.y, vLeftThighOut.x, vLeftThighOut.y, 2.f, 255.f, 0.f, 0.f, 255.f);
	DrawLine(vRightCalfOut.x, vRightCalfOut.y, vRightThighOut.x, vRightThighOut.y, 2.f, 255.f, 0.f, 0.f, 255.f);

	DrawLine(vLeftFootOut.x, vLeftFootOut.y, vLeftCalfOut.x, vLeftCalfOut.y, 2.f, 255.f, 0.f, 0.f, 255.f);
	DrawLine(vRightFootOut.x, vRightFootOut.y, vRightCalfOut.x, vRightCalfOut.y, 2.f, 255.f, 0.f, 0.f, 255.f);
}

void drawLoop(int width, int height) {
	menu();

	float radiusx = AimFOV * (width / 2 / 100);
	float radiusy = AimFOV * (height / 2 / 100);

	float calcradius = (radiusx + radiusy) / 2;

	if (Croshair) {
		DrawLine((float)(width / 2), (float)(height / 2) - 8.f, (float)(width / 2), (float)(height / 2) + 8.f, 1.2f, 0.f, 0.f, 0.f, 1);
		DrawLine((float)(width / 2) - 8.f, (float)(height / 2), (float)(width / 2) + 8.f, (float)(height / 2), 1.2f, 0.f, 0.f, 0.f, 1);
	}

	if (FilledFOV)
	{
		DrawEllipse(width / 2, height / 2, 2, 2, AimFOV + AimFOV + AimFOV + AimFOV + AimFOV + AimFOV + AimFOV + AimFOV + AimFOV + AimFOV + AimFOV + AimFOV + AimFOV + AimFOV, 0.f, 0.f, 0.f, 0.2, false);
	}

	if (DrawFov) {
		DrawCircle(width / 2, height / 2, calcradius, 2.f, 1.f, 1.f, 1.f, 255.f, false);
	}

	Uworld = read<DWORD_PTR>(DriverHandle, processID, base_address + OFFSET_UWORLD);
	//printf(_xor_("Uworld: %p.\n").c_str(), Uworld);

	DWORD_PTR Gameinstance = read<DWORD_PTR>(DriverHandle, processID, Uworld + 0x180); // changes sometimes 4u

	if (Gameinstance == (DWORD_PTR)nullptr)
		return;

	//printf(_xor_("Gameinstance: %p.\n").c_str(), Gameinstance);

	DWORD_PTR LocalPlayers = read<DWORD_PTR>(DriverHandle, processID, Gameinstance + 0x38);

	if (LocalPlayers == (DWORD_PTR)nullptr)
		return;

	//printf(_xor_("LocalPlayers: %p.\n").c_str(), LocalPlayers);

	Localplayer = read<DWORD_PTR>(DriverHandle, processID, LocalPlayers);

	if (Localplayer == (DWORD_PTR)nullptr)
		return;

	//printf(_xor_("LocalPlayer: %p.\n").c_str(), Localplayer);

	PlayerController = read<DWORD_PTR>(DriverHandle, processID, Localplayer + 0x30);

	if (PlayerController == (DWORD_PTR)nullptr)
		return;

	//printf(_xor_("playercontroller: %p.\n").c_str(), PlayerController);

	LocalPawn = read<uint64_t>(DriverHandle, processID, PlayerController + 0x2A0);  // changed often 4u sometimes called AcknowledgedPawn

	if (LocalPawn == (DWORD_PTR)nullptr)
		return;

	//printf(_xor_("Pawn: %p.\n").c_str(), LocalPawn);

	Rootcomp = read<uint64_t>(DriverHandle, processID, LocalPawn + 0x130);  

	if (Rootcomp == (DWORD_PTR)nullptr)
		return;

	//printf(_xor_("Rootcomp: %p.\n").c_str(), Rootcomp);

	if (LocalPawn != 0) {
		localplayerID = read<int>(DriverHandle, processID, LocalPawn + 0x18);
	}

	Ulevel = read<DWORD_PTR>(DriverHandle, processID, Uworld + 0x30);
	//printf(_xor_("Ulevel: %p.\n").c_str(), Ulevel);

	if (Ulevel == (DWORD_PTR)nullptr)
		return;

	DWORD64 PlayerState = read<DWORD64>(DriverHandle, processID, LocalPawn + 0x240);  //changes often 4u

	if (PlayerState == (DWORD_PTR)nullptr)
		return;

	DWORD ActorCount = read<DWORD>(DriverHandle, processID, Ulevel + 0xA0);

	DWORD_PTR AActors = read<DWORD_PTR>(DriverHandle, processID, Ulevel + 0x98);
	//printf(_xor_("AActors: %p.\n").c_str(), AActors);

	if (AActors == (DWORD_PTR)nullptr)
		return;

	for (int i = 0; i < ActorCount; i++)
	{
		uint64_t CurrentActor = read<uint64_t>(DriverHandle, processID, AActors + i * 0x8);

		int curactorid = read<int>(DriverHandle, processID, CurrentActor + 0x18);

		if (curactorid == localplayerID || curactorid == 9885539 || curactorid == 21510299 || curactorid == 21610299)
		{
			if (CurrentActor == (uint64_t)nullptr || CurrentActor == -1 || CurrentActor == NULL)
				continue;

			uint64_t CurrentActorRootComponent = read<uint64_t>(DriverHandle, processID, CurrentActor + 0x130);

			if (CurrentActorRootComponent == (uint64_t)nullptr || CurrentActorRootComponent == -1 || CurrentActorRootComponent == NULL)
				continue;

			uint64_t currentactormesh = read<uint64_t>(DriverHandle, processID, CurrentActor + 0x280); // change as needed 4u

			if (currentactormesh == (uint64_t)nullptr || currentactormesh == -1 || currentactormesh == NULL)
				continue;

			int MyTeamId = read<int>(DriverHandle, processID, PlayerState + 0xED0);  //changes often 4u

			DWORD64 otherPlayerState = read<uint64_t>(DriverHandle, processID, CurrentActor + 0x240); //changes often 4u

			if (otherPlayerState == (uint64_t)nullptr || otherPlayerState == -1 || otherPlayerState == NULL)
				continue;

			int ActorTeamId = read<int>(DriverHandle, processID, otherPlayerState + 0xED0); //changes often 4u

			Vector3 CirclePOS = GetBoneWithRotation(currentactormesh, 2);

			Vector3 Headpos = GetBoneWithRotation(currentactormesh, 66);
			Localcam = CameraEXT;
			localactorpos = read<Vector3>(DriverHandle, processID, Rootcomp + 0x11C);

			float distance = localactorpos.Distance(Headpos) / 100.f;

			if (distance < 1.5f)
				continue;

			//W2S
			Vector3 HeadposW2s = ProjectWorldToScreen(Headpos, Vector3(Localcam.y, Localcam.x, Localcam.z));
			Vector3 bone0 = GetBoneWithRotation(currentactormesh, 0);
			Vector3 bottom = ProjectWorldToScreen(bone0, Vector3(Localcam.y, Localcam.x, Localcam.z));
			Vector3 Headbox = ProjectWorldToScreen(Vector3(Headpos.x, Headpos.y, Headpos.z + 15), Vector3(Localcam.y, Localcam.x, Localcam.z));
			Vector3 Aimpos = ProjectWorldToScreen(Vector3(Headpos.x, Headpos.y, Headpos.z + 10), Vector3(Localcam.y, Localcam.x, Localcam.z));
			Vector3 CircleESPPosition = ProjectWorldToScreen(Vector3(CirclePOS.x, CirclePOS.y, CirclePOS.z + 15), Vector3(Localcam.y, Localcam.x, Localcam.z));;;

			float Height1 = abs(Headbox.y - bottom.y);
			float Width1 = Height1 * 0.65;

			if (MyTeamId != ActorTeamId) 
			{
				//Fake Features

				if (Memory)
				{

				}
				if (SilentV1)
				{

				}
				if (SilentV2)
				{

				}
				if (SpinExtension)
				{

				}
				if (Spinbot)
				{

				}
				if (VisibleTargets)
				{

				}


				// Real Features

				if (Head)
				{

				}
				
				if (Chest)
				{

				}
				
				if (Pelvis)
				{

				}



				if (skeleton)
					DrawSkeleton(currentactormesh);

				if (BoxESP)
					DrawBox(Headbox.x - (Width1 / 2), Headbox.y, Width1, Height1, 2.f, 0.f, 0.f, 0.f, 255.f, false);

				if (EnemyESP)
					DrawString(_xor_("").c_str(), 13, HeadposW2s.x, HeadposW2s.y - 25, 0, 1, 1);

				if (FilledBoxESP)
					DrawBox(Headbox.x - (Width1 / 2), Headbox.y, Width1, Height1, 0.f, 0.f, 0.f, 0.f, 0.2f, true);

				if (EnemyVisible)
				{
					DrawBox(Headbox.x - (Width1 / 2), Headbox.y, Width1, Height1, 0.f, 0.f, 0.f, 0.f, 0.0f, true);
					DrawString(_xor_("                                                                                                                                                                Enemy Visible").c_str(), 20, 0, 50, 0.f, 255.f, 0.f, 0.9);
				}

				if (CornerESP) {
					DrawLine(Headbox.x - (Width1 / 2), Headbox.y, Headbox.x - (Width1 / 2) + (Width1 / 4), Headbox.y, 1.f, 0.f, 255.f, 0.f, 100);
					DrawLine(Headbox.x - (Width1 / 2), Headbox.y, Headbox.x - (Width1 / 2), Headbox.y + (Width1 / 4), 1.f, 0.f, 255.f, 0.f, 100);
					DrawLine(Headbox.x - (Width1 / 2) + Width1, Headbox.y, Headbox.x + (Width1 / 2) - (Width1 / 4), Headbox.y, 1.f, 0.f, 255.f, 0.f, 100);
					DrawLine(Headbox.x - (Width1 / 2) + Width1, Headbox.y, Headbox.x - (Width1 / 2) + Width1, Headbox.y + (Width1 / 4), 1.f, 0.f, 255.f, 0.f, 100);
					DrawLine(Headbox.x - (Width1 / 2), Headbox.y + Height1, Headbox.x - (Width1 / 2) + (Width1 / 4), Headbox.y + Height1, 1.f, 0.f, 255.f, 0.f, 100);
					DrawLine(Headbox.x - (Width1 / 2), Headbox.y - (Width1 / 4) + Height1, Headbox.x - (Width1 / 2), Headbox.y + Height1, 1.f, 0.f, 255.f, 0.f, 100);
					DrawLine(Headbox.x - (Width1 / 2) + Width1, Headbox.y + Height1, Headbox.x + (Width1 / 2) - (Width1 / 4), Headbox.y + Height1, 1.f, 0.f, 255.f, 0.f, 100);
					DrawLine(Headbox.x - (Width1 / 2) + Width1, Headbox.y - (Width1 / 4) + Height1, Headbox.x - (Width1 / 2) + Width1, Headbox.y + Height1, 1.f, 0.f, 255.f, 0.f, 100);
				}

				if (CircleESP)
					DrawEllipse(CircleESPPosition.x, CircleESPPosition.y, 17.5, 17.5, 1.f, 0.f, 0.f, 0.f, 100.0f, false);

				if (DistanceESP)
				{
					CHAR dist[50];
					sprintf_s(dist, _xor_("%.fM").c_str(), distance);

					DrawString(dist, 13, HeadposW2s.x + 40, HeadposW2s.y - 25, 0, 0, 1);
				}

				if (LineESP)
					DrawLine(width / 2, height, bottom.x, bottom.y, 2.f, 0.f, 0.f, 0.f, 255.f);

				if (Aimbot)
				{
					AIms(CurrentActor, Localcam);
				}
				
				if (NameESP) {
					char name[64];
					sprintf_s(name, "Player - %2.fm", distance);
					DrawString(name, 13, HeadposW2s.x - 40, HeadposW2s.y - 25, 0, 0, 0, 0.7);
				}


			}
		}
	}

	if (Aimbot)
	{
		aimbot(Localcam);
	}
}

void main()
{
	DriverHandle = CreateFileW(_xor_(L"\\\\.\\injdrv").c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);  // this needs to match your driver

	if (DriverHandle == INVALID_HANDLE_VALUE)
	{
		printf(_xor_("Load Driver first, exiting...\n").c_str());
		Sleep(2000);
		exit(0);
	}

	info_t Input_Output_Data1;
	unsigned long int Readed_Bytes_Amount1;
	DeviceIoControl(DriverHandle, ctl_clear, &Input_Output_Data1, sizeof Input_Output_Data1, &Input_Output_Data1, sizeof Input_Output_Data1, &Readed_Bytes_Amount1, nullptr);

	while (hwnd == NULL)
	{
		hwnd = FindWindowA(0, _xor_("Fortnite  ").c_str());

		printf(_xor_("Looking for process...\n").c_str());
		Sleep(750);
		system("cls");
	}
	GetWindowThreadProcessId(hwnd, &processID);

	RECT rect;
	if(GetWindowRect(hwnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	info_t Input_Output_Data;
	Input_Output_Data.pid = processID;
	unsigned long int Readed_Bytes_Amount;

	DeviceIoControl(DriverHandle, ctl_base, &Input_Output_Data, sizeof Input_Output_Data, &Input_Output_Data, sizeof Input_Output_Data, &Readed_Bytes_Amount, nullptr);
	base_address = (unsigned long long int)Input_Output_Data.data;
	std::printf(_xor_("Process base address: %p.\n").c_str(), (void*)base_address);

	CreateThread(NULL, NULL, Menuthread, NULL, NULL, NULL);
	
	DirectOverlaySetOption(D2DOV_DRAW_FPS | D2DOV_FONT_COURIER); 
	DirectOverlaySetup(drawLoop, hwnd);
	getchar();
}
